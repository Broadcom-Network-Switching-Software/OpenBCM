#include <shared/bsl.h>

#include <soc/mcm/memregs.h>
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_INGRESS


#include <soc/mem.h>
#include <soc/register.h>

#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/TMC/tmc_api_ingress_scheduler.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>
#include <soc/dpp/ARAD/arad_ingress_scheduler.h>
#include <soc/mcm/memregs.h>
#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/drv.h>




#define JER_IPT_MC_SLOW_START_TIMER_MAX    (17)


















STATIC uint32
  jer_ingress_scheduler_regs_init(
    SOC_SAND_IN  int                 unit
  )
{   
  SOCDNX_INIT_FUNC_DEFS;

  

  
  SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IPT_IQM_SLOW_START_CFG_TIMER_PERIODr, REG_PORT_ANY, 0, SHAPER_9_SLOW_START_ENABLE_Nf,  0 ));
  SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IPT_IQM_SLOW_START_CFG_TIMER_PERIODr, REG_PORT_ANY, 0, SHAPER_10_SLOW_START_ENABLE_Nf,  0 ));
  SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IPT_IQM_SLOW_START_CFG_TIMER_PERIODr, REG_PORT_ANY, 0, SHAPER_4_SLOW_START_ENABLE_Nf,  0 ));
  
  SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IPT_IQM_SLOW_START_CFG_TIMER_PERIODr, REG_PORT_ANY, 1, SHAPER_9_SLOW_START_ENABLE_Nf,  0 ));
  SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IPT_IQM_SLOW_START_CFG_TIMER_PERIODr, REG_PORT_ANY, 1, SHAPER_10_SLOW_START_ENABLE_Nf,  0 ));
  SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IPT_IQM_SLOW_START_CFG_TIMER_PERIODr, REG_PORT_ANY, 1, SHAPER_4_SLOW_START_ENABLE_Nf,  0 ));

  
  SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IPT_IQM_SLOW_START_CFG_TIMER_PERIODr, REG_PORT_ANY, 0, SHAPER_9_SLOW_START_CFG_N_TIMER_PERIOD_0f,  JER_IPT_MC_SLOW_START_TIMER_MAX ));
  SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IPT_IQM_SLOW_START_CFG_TIMER_PERIODr, REG_PORT_ANY, 0, SHAPER_9_SLOW_START_CFG_N_TIMER_PERIOD_1f,  JER_IPT_MC_SLOW_START_TIMER_MAX ));
  SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IPT_IQM_SLOW_START_CFG_TIMER_PERIODr, REG_PORT_ANY, 0, SHAPER_10_SLOW_START_CFG_N_TIMER_PERIOD_0f,  JER_IPT_MC_SLOW_START_TIMER_MAX ));
  SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IPT_IQM_SLOW_START_CFG_TIMER_PERIODr, REG_PORT_ANY, 0, SHAPER_10_SLOW_START_CFG_N_TIMER_PERIOD_1f,  JER_IPT_MC_SLOW_START_TIMER_MAX ));
  SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IPT_IQM_SLOW_START_CFG_TIMER_PERIODr, REG_PORT_ANY, 0, SHAPER_4_SLOW_START_CFG_N_TIMER_PERIOD_0f,  JER_IPT_MC_SLOW_START_TIMER_MAX ));
  SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IPT_IQM_SLOW_START_CFG_TIMER_PERIODr, REG_PORT_ANY, 0, SHAPER_4_SLOW_START_CFG_N_TIMER_PERIOD_1f,  JER_IPT_MC_SLOW_START_TIMER_MAX ));
  
  SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IPT_IQM_SLOW_START_CFG_TIMER_PERIODr, REG_PORT_ANY, 1, SHAPER_9_SLOW_START_CFG_N_TIMER_PERIOD_0f,  JER_IPT_MC_SLOW_START_TIMER_MAX ));
  SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IPT_IQM_SLOW_START_CFG_TIMER_PERIODr, REG_PORT_ANY, 1, SHAPER_9_SLOW_START_CFG_N_TIMER_PERIOD_1f,  JER_IPT_MC_SLOW_START_TIMER_MAX ));
  SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IPT_IQM_SLOW_START_CFG_TIMER_PERIODr, REG_PORT_ANY, 1, SHAPER_10_SLOW_START_CFG_N_TIMER_PERIOD_0f,  JER_IPT_MC_SLOW_START_TIMER_MAX ));
  SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IPT_IQM_SLOW_START_CFG_TIMER_PERIODr, REG_PORT_ANY, 1, SHAPER_10_SLOW_START_CFG_N_TIMER_PERIOD_1f,  JER_IPT_MC_SLOW_START_TIMER_MAX ));
  SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IPT_IQM_SLOW_START_CFG_TIMER_PERIODr, REG_PORT_ANY, 1, SHAPER_4_SLOW_START_CFG_N_TIMER_PERIOD_0f,  JER_IPT_MC_SLOW_START_TIMER_MAX ));
  SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IPT_IQM_SLOW_START_CFG_TIMER_PERIODr, REG_PORT_ANY, 1, SHAPER_4_SLOW_START_CFG_N_TIMER_PERIOD_1f,  JER_IPT_MC_SLOW_START_TIMER_MAX ));


exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t
  jer_ingress_scheduler_init(
    SOC_SAND_IN  int                 unit
  )
{
  soc_error_t
    res = SOC_SAND_OK;

  SOCDNX_INIT_FUNC_DEFS;

  res = jer_ingress_scheduler_regs_init(
          unit
        );
  SOCDNX_IF_ERR_EXIT(res);

exit:    
    SOCDNX_FUNC_RETURN;
}






soc_error_t
jer_ingress_scheduler_clos_slow_start_get (
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_OUT SOC_TMC_ING_SCH_CLOS_INFO   *clos_info
    )
{
  uint32
    res;

  reg_field max_credit, delay, cal;
  ARAD_ING_SCH_SHAPER shaper_temp;
  uint32 slow_start_enable;

  SOCDNX_INIT_FUNC_DEFS;

  
  max_credit.reg = IPT_IQM_SHAPERS_MAX_CREDITr;
  max_credit.field = SHAPER_9_MAX_CREDITf;
  max_credit.index = core;
  delay.reg = IPT_IQM_SHAPERS_DELAYr;
  delay.field = SHAPER_9_DELAYf;
  delay.index = core;
  cal.reg = IPT_IQM_SHAPERS_CALr;
  cal.field = SHAPER_9_CALf;
  cal.index = core;

  res = arad_ingress_scheduler_shaper_values_get(
          unit,
          TRUE,
          &max_credit,
          &delay,
          &cal,
          &(clos_info->shapers.hp.fabric_multicast)
        );
  SOCDNX_SAND_IF_ERR_EXIT(res);


  
  
  SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_read(unit, IPT_IQM_SLOW_START_CFG_TIMER_PERIODr, REG_PORT_ANY, core, SHAPER_9_SLOW_START_ENABLE_Nf, &slow_start_enable));
  clos_info->shapers.hp.fabric_multicast.slow_start_enable = (slow_start_enable == 0) ? 0 : 1;
  
  max_credit.reg = IPT_IQM_SHAPERS_MAX_CREDITr;
  max_credit.field = SHAPER_9_MAX_CREDITf;
  max_credit.index = core;
  delay.reg = IPT_IQM_SHAPER_9_SLOW_START_DELAYr;
  delay.field = SHAPER_9_SLOW_START_N_DELAY_0f;
  delay.index = core;
  cal.reg = IPT_IQM_SHAPER_9_SLOW_START_CALr;
  cal.field = SHAPER_9_SLOW_START_N_CAL_0f;
  cal.index = core;

  SOC_TMC_ING_SCH_SHAPER_clear(&shaper_temp);
  res = arad_ingress_scheduler_shaper_values_get(
    unit,
    TRUE,
    &max_credit,
    &delay,
    &cal,
    &shaper_temp
  );
  SOCDNX_SAND_IF_ERR_EXIT(res);
  if (clos_info->shapers.hp.fabric_multicast.max_rate != 0)
  {
      clos_info->shapers.hp.fabric_multicast.slow_start_rate_phase_0 = shaper_temp.max_rate * 100 / clos_info->shapers.hp.fabric_multicast.max_rate;
  } else {
      clos_info->shapers.hp.fabric_multicast.slow_start_rate_phase_0 = 0;
  }

  
  max_credit.reg = IPT_IQM_SHAPERS_MAX_CREDITr;
  max_credit.field = SHAPER_9_MAX_CREDITf;
  max_credit.index = core;
  delay.reg = IPT_IQM_SHAPER_9_SLOW_START_DELAYr;
  delay.field = SHAPER_9_SLOW_START_N_DELAY_1f;
  delay.index = core;
  cal.reg = IPT_IQM_SHAPER_9_SLOW_START_CALr;
  cal.field = SHAPER_9_SLOW_START_N_CAL_1f;
  cal.index = core;

  SOC_TMC_ING_SCH_SHAPER_clear(&shaper_temp);
  res = arad_ingress_scheduler_shaper_values_get(
    unit,
    TRUE,
    &max_credit,
    &delay,
    &cal,
    &shaper_temp
  );
  SOCDNX_SAND_IF_ERR_EXIT(res);
  if (clos_info->shapers.hp.fabric_multicast.max_rate != 0)
  {
      clos_info->shapers.hp.fabric_multicast.slow_start_rate_phase_1 = shaper_temp.max_rate * 100 / clos_info->shapers.hp.fabric_multicast.max_rate;
  } else {
      clos_info->shapers.hp.fabric_multicast.slow_start_rate_phase_1 = 0;
  }



  
  max_credit.reg = IPT_IQM_SHAPERS_MAX_CREDITr;
  max_credit.field = SHAPER_10_MAX_CREDITf;
  max_credit.index = core;
  delay.reg = IPT_IQM_SHAPERS_DELAYr;
  delay.field = SHAPER_10_DELAYf;
  delay.index = core;
  cal.reg = IPT_IQM_SHAPERS_CALr;
  cal.field = SHAPER_10_CALf;
  cal.index = core;

  res = arad_ingress_scheduler_shaper_values_get(
          unit,
          TRUE,
          &max_credit,
          &delay,
          &cal,
          &(clos_info->shapers.lp.fabric_multicast)
        );
  SOCDNX_SAND_IF_ERR_EXIT(res);

  
  
  SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_read(unit, IPT_IQM_SLOW_START_CFG_TIMER_PERIODr, REG_PORT_ANY, core, SHAPER_10_SLOW_START_ENABLE_Nf, &slow_start_enable));
  clos_info->shapers.lp.fabric_multicast.slow_start_enable = (slow_start_enable == 0) ? 0 : 1;
  
  max_credit.reg = IPT_IQM_SHAPERS_MAX_CREDITr;
  max_credit.field = SHAPER_10_MAX_CREDITf;
  max_credit.index = core;
  delay.reg = IPT_IQM_SHAPER_10_SLOW_START_DELAYr;
  delay.field = SHAPER_10_SLOW_START_N_DELAY_0f;
  delay.index = core;
  cal.reg = IPT_IQM_SHAPER_10_SLOW_START_CALr;
  cal.field = SHAPER_10_SLOW_START_N_CAL_0f;
  cal.index = core;

  SOC_TMC_ING_SCH_SHAPER_clear(&shaper_temp);
  res = arad_ingress_scheduler_shaper_values_get(
        unit,
        TRUE,
        &max_credit,
        &delay,
        &cal,
        &shaper_temp
      );
  SOCDNX_SAND_IF_ERR_EXIT(res);
  if (clos_info->shapers.lp.fabric_multicast.max_rate != 0)
  {
      clos_info->shapers.lp.fabric_multicast.slow_start_rate_phase_0 = shaper_temp.max_rate * 100 / clos_info->shapers.lp.fabric_multicast.max_rate;
  } else {
      clos_info->shapers.lp.fabric_multicast.slow_start_rate_phase_0 = 0;
  }

  
  max_credit.reg = IPT_IQM_SHAPERS_MAX_CREDITr;
  max_credit.field = SHAPER_10_MAX_CREDITf;
  max_credit.index = core;
  delay.reg = IPT_IQM_SHAPER_10_SLOW_START_DELAYr;
  delay.field = SHAPER_10_SLOW_START_N_DELAY_1f;
  delay.index = core;
  cal.reg = IPT_IQM_SHAPER_10_SLOW_START_CALr;
  cal.field = SHAPER_10_SLOW_START_N_CAL_1f;
  cal.index = core;

  SOC_TMC_ING_SCH_SHAPER_clear(&shaper_temp);
  res = arad_ingress_scheduler_shaper_values_get(
        unit,
        TRUE,
        &max_credit,
        &delay,
        &cal,
        &shaper_temp
      );
  SOCDNX_SAND_IF_ERR_EXIT(res);
  if (clos_info->shapers.lp.fabric_multicast.max_rate != 0)
  {
      clos_info->shapers.lp.fabric_multicast.slow_start_rate_phase_1 = shaper_temp.max_rate * 100 / clos_info->shapers.lp.fabric_multicast.max_rate;
  } else {
      clos_info->shapers.lp.fabric_multicast.slow_start_rate_phase_1 = 0;
  }

exit:
    SOCDNX_FUNC_RETURN;
}



soc_error_t
jer_ingress_scheduler_clos_slow_start_set (
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN SOC_TMC_ING_SCH_CLOS_INFO   *clos_info
    )
{
  uint32
    res,
    exact_fabm_max_rate;
  
  reg_field max_credit, delay, cal;
  ARAD_ING_SCH_SHAPER shaper_temp, out_shaper_struct;

  SOCDNX_INIT_FUNC_DEFS;


  
  max_credit.reg = IPT_IQM_SHAPERS_MAX_CREDITr;
  max_credit.field = SHAPER_9_MAX_CREDITf;
  max_credit.index = core;
  delay.reg = IPT_IQM_SHAPERS_DELAYr;
  delay.field = SHAPER_9_DELAYf;
  delay.index = core;
  cal.reg = IPT_IQM_SHAPERS_CALr;
  cal.field = SHAPER_9_CALf;
  cal.index = core;

  res = arad_ingress_scheduler_shaper_values_get(
          unit,
          TRUE,
          &max_credit,
          &delay,
          &cal,
          &out_shaper_struct
        );
  SOCDNX_SAND_IF_ERR_EXIT(res);

  
  
  if (clos_info->shapers.hp.fabric_multicast.slow_start_enable != SOC_TMC_ING_SCH_DONT_TOUCH)
  {
      SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IPT_IQM_SLOW_START_CFG_TIMER_PERIODr, REG_PORT_ANY, core, SHAPER_9_SLOW_START_ENABLE_Nf,  clos_info->shapers.hp.fabric_multicast.slow_start_enable ? 1  : 0 ));
  }
  
  max_credit.reg = IPT_IQM_SHAPERS_MAX_CREDITr;
  max_credit.field = SHAPER_9_MAX_CREDITf;
  max_credit.index = core;
  delay.reg = IPT_IQM_SHAPER_9_SLOW_START_DELAYr;
  delay.field = SHAPER_9_SLOW_START_N_DELAY_0f;
  delay.index = core;
  cal.reg = IPT_IQM_SHAPER_9_SLOW_START_CALr;
  cal.field = SHAPER_9_SLOW_START_N_CAL_0f;
  cal.index = core;

  SOC_TMC_ING_SCH_SHAPER_clear(&shaper_temp);
  if (clos_info->shapers.hp.fabric_multicast.slow_start_rate_phase_0  == SOC_TMC_ING_SCH_DONT_TOUCH) {
      shaper_temp.max_rate = SOC_TMC_ING_SCH_DONT_TOUCH;
      shaper_temp.max_burst = out_shaper_struct.max_burst;
  } else {
      shaper_temp.max_rate = out_shaper_struct.max_rate * clos_info->shapers.hp.fabric_multicast.slow_start_rate_phase_0 / 100;
      shaper_temp.max_burst = out_shaper_struct.max_burst;
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
  SOCDNX_SAND_IF_ERR_EXIT(res);

  
  max_credit.reg = IPT_IQM_SHAPERS_MAX_CREDITr;
  max_credit.field = SHAPER_9_MAX_CREDITf;
  max_credit.index = core;
  delay.reg = IPT_IQM_SHAPER_9_SLOW_START_DELAYr;
  delay.field = SHAPER_9_SLOW_START_N_DELAY_1f;
  delay.index = core;
  cal.reg = IPT_IQM_SHAPER_9_SLOW_START_CALr;
  cal.field = SHAPER_9_SLOW_START_N_CAL_1f;
  cal.index = core;

  SOC_TMC_ING_SCH_SHAPER_clear(&shaper_temp);
  if (clos_info->shapers.hp.fabric_multicast.slow_start_rate_phase_1  == SOC_TMC_ING_SCH_DONT_TOUCH) {
      shaper_temp.max_rate = SOC_TMC_ING_SCH_DONT_TOUCH;
      shaper_temp.max_burst = out_shaper_struct.max_burst;
  } else {
      shaper_temp.max_rate = out_shaper_struct.max_rate * clos_info->shapers.hp.fabric_multicast.slow_start_rate_phase_1 / 100;
      shaper_temp.max_burst = out_shaper_struct.max_burst;
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
  SOCDNX_SAND_IF_ERR_EXIT(res);


  
  max_credit.reg = IPT_IQM_SHAPERS_MAX_CREDITr;
  max_credit.field = SHAPER_10_MAX_CREDITf;
  max_credit.index = core;
  delay.reg = IPT_IQM_SHAPERS_DELAYr;
  delay.field = SHAPER_10_DELAYf;
  delay.index = core;
  cal.reg = IPT_IQM_SHAPERS_CALr;
  cal.field = SHAPER_10_CALf;
  cal.index = core;

  res = arad_ingress_scheduler_shaper_values_get(
          unit,
          TRUE,
          &max_credit,
          &delay,
          &cal,
          &out_shaper_struct
        );
  SOCDNX_SAND_IF_ERR_EXIT(res);

  
  
  if (clos_info->shapers.lp.fabric_multicast.slow_start_enable != SOC_TMC_ING_SCH_DONT_TOUCH)
  {
      SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IPT_IQM_SLOW_START_CFG_TIMER_PERIODr, REG_PORT_ANY, core, SHAPER_10_SLOW_START_ENABLE_Nf,  clos_info->shapers.lp.fabric_multicast.slow_start_enable ? 1  : 0 ));
  }
  
  max_credit.reg = IPT_IQM_SHAPERS_MAX_CREDITr;
  max_credit.field = SHAPER_10_MAX_CREDITf;
  max_credit.index = core;
  delay.reg = IPT_IQM_SHAPER_10_SLOW_START_DELAYr;
  delay.field = SHAPER_10_SLOW_START_N_DELAY_0f;
  delay.index = core;
  cal.reg = IPT_IQM_SHAPER_10_SLOW_START_CALr;
  cal.field = SHAPER_10_SLOW_START_N_CAL_0f;
  cal.index = core;

  SOC_TMC_ING_SCH_SHAPER_clear(&shaper_temp);
  if (clos_info->shapers.lp.fabric_multicast.slow_start_rate_phase_0  == SOC_TMC_ING_SCH_DONT_TOUCH) {
      shaper_temp.max_rate = SOC_TMC_ING_SCH_DONT_TOUCH;
      shaper_temp.max_burst = out_shaper_struct.max_burst;
  } else {
      shaper_temp.max_rate = out_shaper_struct.max_rate * clos_info->shapers.lp.fabric_multicast.slow_start_rate_phase_0 / 100;
      shaper_temp.max_burst = out_shaper_struct.max_burst;
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
  SOCDNX_SAND_IF_ERR_EXIT(res);

  
  max_credit.reg = IPT_IQM_SHAPERS_MAX_CREDITr;
  max_credit.field = SHAPER_10_MAX_CREDITf;
  max_credit.index = core;
  delay.reg = IPT_IQM_SHAPER_10_SLOW_START_DELAYr;
  delay.field = SHAPER_10_SLOW_START_N_DELAY_1f;
  delay.index = core;
  cal.reg = IPT_IQM_SHAPER_10_SLOW_START_CALr;
  cal.field = SHAPER_10_SLOW_START_N_CAL_1f;
  cal.index = core;

  SOC_TMC_ING_SCH_SHAPER_clear(&shaper_temp);
  if (clos_info->shapers.lp.fabric_multicast.slow_start_rate_phase_1  == SOC_TMC_ING_SCH_DONT_TOUCH) {
      shaper_temp.max_rate = SOC_TMC_ING_SCH_DONT_TOUCH;
      shaper_temp.max_burst = out_shaper_struct.max_burst;
  } else {
      shaper_temp.max_rate = out_shaper_struct.max_rate * clos_info->shapers.lp.fabric_multicast.slow_start_rate_phase_1 / 100;
      shaper_temp.max_burst = out_shaper_struct.max_burst;
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
  SOCDNX_SAND_IF_ERR_EXIT(res);

exit:
    SOCDNX_FUNC_RETURN;
}



soc_error_t
jer_ingress_scheduler_mesh_slow_start_get (
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_OUT SOC_TMC_ING_SCH_SHAPER   *shaper_info
    )
{
  uint32
    res;

  reg_field max_credit, delay, cal;
  ARAD_ING_SCH_SHAPER shaper_temp;
  ARAD_ING_SCH_SHAPER mesh_mc_shaper;
  uint32 slow_start_enable;

  SOCDNX_INIT_FUNC_DEFS;

  
  max_credit.reg = IPT_IQM_SHAPERS_MAX_CREDITr;
  max_credit.field = SHAPER_4_MAX_CREDITf;
  max_credit.index = core;
  delay.reg = IPT_IQM_SHAPERS_DELAYr;
  delay.field = SHAPER_4_DELAYf;
  delay.index = core;
  cal.reg = IPT_IQM_SHAPERS_CALr;
  cal.field = SHAPER_4_CALf;
  cal.index = core;

  res = arad_ingress_scheduler_shaper_values_get(
          unit,
          TRUE,
          &max_credit,
          &delay,
          &cal,
          &mesh_mc_shaper
        );
  SOCDNX_SAND_IF_ERR_EXIT(res);


  
  
  SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_read(unit, IPT_IQM_SLOW_START_CFG_TIMER_PERIODr, REG_PORT_ANY, core, SHAPER_4_SLOW_START_ENABLE_Nf, &slow_start_enable));
  shaper_info->slow_start_enable = (slow_start_enable == 0) ? 0 : 1;
  
  max_credit.reg = IPT_IQM_SHAPERS_MAX_CREDITr;
  max_credit.field = SHAPER_4_MAX_CREDITf;
  max_credit.index = core;
  delay.reg = IPT_IQM_SHAPER_4_SLOW_START_DELAYr;
  delay.field = SHAPER_4_SLOW_START_N_DELAY_0f;
  delay.index = core;
  cal.reg = IPT_IQM_SHAPER_4_SLOW_START_CALr;
  cal.field = SHAPER_4_SLOW_START_N_CAL_0f;
  cal.index = core;

  SOC_TMC_ING_SCH_SHAPER_clear(&shaper_temp);
  res = arad_ingress_scheduler_shaper_values_get(
    unit,
    TRUE,
    &max_credit,
    &delay,
    &cal,
    &shaper_temp
  );
  SOCDNX_SAND_IF_ERR_EXIT(res);
  if (mesh_mc_shaper.max_rate != 0)
  {
      shaper_info->slow_start_rate_phase_0 = shaper_temp.max_rate * 100 / mesh_mc_shaper.max_rate;
  } else {
      shaper_info->slow_start_rate_phase_0 = 0;
  }

  
  max_credit.reg = IPT_IQM_SHAPERS_MAX_CREDITr;
  max_credit.field = SHAPER_4_MAX_CREDITf;
  max_credit.index = core;
  delay.reg = IPT_IQM_SHAPER_4_SLOW_START_DELAYr;
  delay.field = SHAPER_4_SLOW_START_N_DELAY_1f;
  delay.index = core;
  cal.reg = IPT_IQM_SHAPER_4_SLOW_START_CALr;
  cal.field = SHAPER_4_SLOW_START_N_CAL_1f;
  cal.index = core;

  SOC_TMC_ING_SCH_SHAPER_clear(&shaper_temp);
  res = arad_ingress_scheduler_shaper_values_get(
    unit,
    TRUE,
    &max_credit,
    &delay,
    &cal,
    &shaper_temp
  );
  SOCDNX_SAND_IF_ERR_EXIT(res);
  if (mesh_mc_shaper.max_rate != 0)
  {
      shaper_info->slow_start_rate_phase_1 = shaper_temp.max_rate * 100 / mesh_mc_shaper.max_rate;
  } else {
      shaper_info->slow_start_rate_phase_1 = 0;
  }


exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t
jer_ingress_scheduler_mesh_slow_start_set (
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN ARAD_ING_SCH_SHAPER   *shaper_info
    )
{
  uint32
    res,
    exact_fabm_max_rate;
  
  reg_field max_credit, delay, cal;
  ARAD_ING_SCH_SHAPER shaper_temp, out_shaper_struct;

  SOCDNX_INIT_FUNC_DEFS;
  
  max_credit.reg = IPT_IQM_SHAPERS_MAX_CREDITr;
  max_credit.field = SHAPER_4_MAX_CREDITf;
  max_credit.index = core;
  delay.reg = IPT_IQM_SHAPERS_DELAYr;
  delay.field = SHAPER_4_DELAYf;
  delay.index = core;
  cal.reg = IPT_IQM_SHAPERS_CALr;
  cal.field = SHAPER_4_CALf;
  cal.index = core;

  res = arad_ingress_scheduler_shaper_values_get(
          unit,
          TRUE,
          &max_credit,
          &delay,
          &cal,
          &out_shaper_struct
        );
  SOCDNX_SAND_IF_ERR_EXIT(res);

  
  
  if (shaper_info->slow_start_enable != SOC_TMC_ING_SCH_DONT_TOUCH)
  {
      SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IPT_IQM_SLOW_START_CFG_TIMER_PERIODr, REG_PORT_ANY, core, SHAPER_4_SLOW_START_ENABLE_Nf,  shaper_info->slow_start_enable ? 1  : 0 ));
  }
  
  max_credit.reg = IPT_IQM_SHAPERS_MAX_CREDITr;
  max_credit.field = SHAPER_4_MAX_CREDITf;
  max_credit.index = core;
  delay.reg = IPT_IQM_SHAPER_4_SLOW_START_DELAYr;
  delay.field = SHAPER_4_SLOW_START_N_DELAY_0f;
  delay.index = core;
  cal.reg = IPT_IQM_SHAPER_4_SLOW_START_CALr;
  cal.field = SHAPER_4_SLOW_START_N_CAL_0f;
  cal.index = core;

  SOC_TMC_ING_SCH_SHAPER_clear(&shaper_temp);
  if (shaper_info->slow_start_rate_phase_0  == SOC_TMC_ING_SCH_DONT_TOUCH) {
      shaper_temp.max_rate = SOC_TMC_ING_SCH_DONT_TOUCH;
      shaper_temp.max_burst = out_shaper_struct.max_burst;
  } else {
      shaper_temp.max_rate = out_shaper_struct.max_rate * shaper_info->slow_start_rate_phase_0 / 100;
      shaper_temp.max_burst = out_shaper_struct.max_burst;
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
  SOCDNX_SAND_IF_ERR_EXIT(res);

  
  max_credit.reg = IPT_IQM_SHAPERS_MAX_CREDITr;
  max_credit.field = SHAPER_4_MAX_CREDITf;
  max_credit.index = core;
  delay.reg = IPT_IQM_SHAPER_4_SLOW_START_DELAYr;
  delay.field = SHAPER_4_SLOW_START_N_DELAY_1f;
  delay.index = core;
  cal.reg = IPT_IQM_SHAPER_4_SLOW_START_CALr;
  cal.field = SHAPER_4_SLOW_START_N_CAL_1f;
  cal.index = core;

  SOC_TMC_ING_SCH_SHAPER_clear(&shaper_temp);
  if (shaper_info->slow_start_rate_phase_1  == SOC_TMC_ING_SCH_DONT_TOUCH) {
      shaper_temp.max_rate = SOC_TMC_ING_SCH_DONT_TOUCH;
      shaper_temp.max_burst = out_shaper_struct.max_burst;
  } else {
      shaper_temp.max_rate = out_shaper_struct.max_rate * shaper_info->slow_start_rate_phase_1 / 100;
      shaper_temp.max_burst = out_shaper_struct.max_burst;
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

  SOCDNX_SAND_IF_ERR_EXIT(res);

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC soc_error_t
  jer_ingress_scheduler_clos_shaper_gport_to_field(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  soc_gport_t            gport,
    SOC_SAND_IN  int                    core,
    SOC_SAND_OUT reg_field              *max_credit,
    SOC_SAND_OUT reg_field              *delay,
    SOC_SAND_OUT reg_field              *cal,
    SOC_SAND_OUT int                    *is_delay_2_clocks
  )
{
    SOCDNX_INIT_FUNC_DEFS;
   
    if (_SHR_GPORT_IS_FABRIC_CLOS_COMMON_LOCAL0(gport)){
       max_credit->reg = IPT_COMMON_SHAPERS_MAX_CREDITr;
       max_credit->field = SHAPER_0_COMMON_MAX_CREDITf;
       max_credit->index = 0;
       delay->reg = IPT_COMMON_SHAPERS_DELAYr;
       delay->field = SHAPER_0_COMMON_DELAYf;
       delay->index = 0;
       cal->reg = IPT_COMMON_SHAPERS_CALr;
       cal->field = SHAPER_0_COMMON_CALf;
       cal->index = 0;
       *is_delay_2_clocks = FALSE;                
    }
    else if (_SHR_GPORT_IS_FABRIC_CLOS_COMMON_LOCAL1(gport)){
       max_credit->reg = IPT_COMMON_SHAPERS_MAX_CREDITr;
       max_credit->field = SHAPER_1_COMMON_MAX_CREDITf;
       max_credit->index = 0;
       delay->reg = IPT_COMMON_SHAPERS_DELAYr;
       delay->field = SHAPER_1_COMMON_DELAYf;
       delay->index = 0;
       cal->reg = IPT_COMMON_SHAPERS_CALr;
       cal->field = SHAPER_1_COMMON_CALf;
       cal->index = 0;
       *is_delay_2_clocks = FALSE;                
    }
    else if (_SHR_GPORT_IS_FABRIC_CLOS_COMMON_FABRIC(gport)){
       max_credit->reg = IPT_COMMON_SHAPERS_MAX_CREDITr;
       max_credit->field = SHAPER_2_COMMON_MAX_CREDITf;
       max_credit->index = 0;
       delay->reg = IPT_COMMON_SHAPERS_DELAYr;
       delay->field = SHAPER_2_COMMON_DELAYf;
       delay->index = 0;
       cal->reg = IPT_COMMON_SHAPERS_CALr;
       cal->field = SHAPER_2_COMMON_CALf;
       cal->index = 0;
       *is_delay_2_clocks = FALSE;                
    }
    else if (_SHR_GPORT_IS_FABRIC_CLOS_COMMON_UNICAST_FABRIC(gport)){
       max_credit->reg = IPT_COMMON_SHAPERS_MAX_CREDITr;
       max_credit->field = SHAPER_3_COMMON_MAX_CREDITf;
       max_credit->index = 0;
       delay->reg = IPT_COMMON_SHAPERS_DELAYr;
       delay->field = SHAPER_3_COMMON_DELAYf;
       delay->index = 0;
       cal->reg = IPT_COMMON_SHAPERS_CALr;
       cal->field = SHAPER_3_COMMON_CALf;
       cal->index = 0;
       *is_delay_2_clocks = FALSE;                
    }
    else if (_SHR_GPORT_IS_FABRIC_CLOS_COMMON_MULTICAST_FABRIC(gport)){
       max_credit->reg = IPT_COMMON_SHAPERS_MAX_CREDITr;
       max_credit->field = SHAPER_4_COMMON_MAX_CREDITf;
       max_credit->index = 0;
       delay->reg = IPT_COMMON_SHAPERS_DELAYr;
       delay->field = SHAPER_4_COMMON_DELAYf;
       delay->index = 0;
       cal->reg = IPT_COMMON_SHAPERS_CALr;
       cal->field = SHAPER_4_COMMON_CALf;
       cal->index = 0;
       *is_delay_2_clocks = FALSE;                
    }      
    else if (_SHR_GPORT_IS_FABRIC_CLOS_LOCAL0(gport)){
       max_credit->reg = IPT_IQM_SHAPERS_MAX_CREDITr;
       max_credit->field = SHAPER_0_MAX_CREDITf;
       max_credit->index = core;
       delay->reg = IPT_IQM_SHAPERS_DELAYr;
       delay->field = SHAPER_0_DELAYf;
       delay->index = core;
       cal->reg = IPT_IQM_SHAPERS_CALr;
       cal->field = SHAPER_0_CALf;
       cal->index = core;
       *is_delay_2_clocks = TRUE;         
    }
    else if (_SHR_GPORT_IS_FABRIC_CLOS_LOCAL1(gport)){
       max_credit->reg = IPT_IQM_SHAPERS_MAX_CREDITr;
       max_credit->field = SHAPER_1_MAX_CREDITf;
       max_credit->index = core;
       delay->reg = IPT_IQM_SHAPERS_DELAYr;
       delay->field = SHAPER_1_DELAYf;
       delay->index = core;
       cal->reg = IPT_IQM_SHAPERS_CALr;
       cal->field = SHAPER_1_CALf;
       cal->index = core;
       *is_delay_2_clocks = TRUE;         
    }
    else if (_SHR_GPORT_IS_FABRIC_CLOS_FABRIC(gport)){
       max_credit->reg = IPT_IQM_SHAPERS_MAX_CREDITr;
       max_credit->field = SHAPER_2_MAX_CREDITf;
       max_credit->index = core;
       delay->reg = IPT_IQM_SHAPERS_DELAYr;
       delay->field = SHAPER_2_DELAYf;
       delay->index = core;
       cal->reg = IPT_IQM_SHAPERS_CALr;
       cal->field = SHAPER_2_CALf;
       cal->index = core;
       *is_delay_2_clocks = TRUE;         
    }
    else if (_SHR_GPORT_IS_FABRIC_CLOS_LOCAL0_HIGH(gport)){
       max_credit->reg = IPT_IQM_SHAPERS_MAX_CREDITr;
       max_credit->field = SHAPER_3_MAX_CREDITf;
       max_credit->index = core;
       delay->reg = IPT_IQM_SHAPERS_DELAYr;
       delay->field = SHAPER_3_DELAYf;
       delay->index = core;
       cal->reg = IPT_IQM_SHAPERS_CALr;
       cal->field = SHAPER_3_CALf;
       cal->index = core;
       *is_delay_2_clocks = TRUE;         
    }
    else if (_SHR_GPORT_IS_FABRIC_CLOS_LOCAL0_LOW(gport)){
       max_credit->reg = IPT_IQM_SHAPERS_MAX_CREDITr;
       max_credit->field = SHAPER_4_MAX_CREDITf;
       max_credit->index = core;
       delay->reg = IPT_IQM_SHAPERS_DELAYr;
       delay->field = SHAPER_4_DELAYf;
       delay->index = core;
       cal->reg = IPT_IQM_SHAPERS_CALr;
       cal->field = SHAPER_4_CALf;
       cal->index = core;
       *is_delay_2_clocks = TRUE;         
    }
    else if (_SHR_GPORT_IS_FABRIC_CLOS_LOCAL1_HIGH(gport)){
       max_credit->reg = IPT_IQM_SHAPERS_MAX_CREDITr;
       max_credit->field = SHAPER_5_MAX_CREDITf;
       max_credit->index = core;
       delay->reg = IPT_IQM_SHAPERS_DELAYr;
       delay->field = SHAPER_5_DELAYf;
       delay->index = core;
       cal->reg = IPT_IQM_SHAPERS_CALr;
       cal->field = SHAPER_5_CALf;
       cal->index = core;
       *is_delay_2_clocks = TRUE;         
    }
    else if (_SHR_GPORT_IS_FABRIC_CLOS_LOCAL1_LOW(gport)){
       max_credit->reg = IPT_IQM_SHAPERS_MAX_CREDITr;
       max_credit->field = SHAPER_6_MAX_CREDITf;
       max_credit->index = core;
       delay->reg = IPT_IQM_SHAPERS_DELAYr;
       delay->field = SHAPER_6_DELAYf;
       delay->index = core;
       cal->reg = IPT_IQM_SHAPERS_CALr;
       cal->field = SHAPER_6_CALf;
       cal->index = core;
       *is_delay_2_clocks = TRUE;         
    }
    else if (_SHR_GPORT_IS_FABRIC_CLOS_UNICAST_FABRIC_HIGH(gport)){
       max_credit->reg = IPT_IQM_SHAPERS_MAX_CREDITr;
       max_credit->field = SHAPER_7_MAX_CREDITf;
       max_credit->index = core;
       delay->reg = IPT_IQM_SHAPERS_DELAYr;
       delay->field = SHAPER_7_DELAYf;
       delay->index = core;
       cal->reg = IPT_IQM_SHAPERS_CALr;
       cal->field = SHAPER_7_CALf;
       cal->index = core;
       *is_delay_2_clocks = TRUE;         
    }
    else if (_SHR_GPORT_IS_FABRIC_CLOS_UNICAST_FABRIC_LOW(gport)){
       max_credit->reg = IPT_IQM_SHAPERS_MAX_CREDITr;
       max_credit->field = SHAPER_8_MAX_CREDITf;
       max_credit->index = core;
       delay->reg = IPT_IQM_SHAPERS_DELAYr;
       delay->field = SHAPER_8_DELAYf;
       delay->index = core;
       cal->reg = IPT_IQM_SHAPERS_CALr;
       cal->field = SHAPER_8_CALf;
       cal->index = core;
       *is_delay_2_clocks = TRUE;         
    }
    else if (_SHR_GPORT_IS_FABRIC_CLOS_FMQ_GUARANTEED(gport)){
       max_credit->reg = IPT_IQM_SHAPERS_MAX_CREDITr;
       max_credit->field = SHAPER_9_MAX_CREDITf;
       max_credit->index = core;
       delay->reg = IPT_IQM_SHAPERS_DELAYr;
       delay->field = SHAPER_9_DELAYf;
       delay->index = core;
       cal->reg = IPT_IQM_SHAPERS_CALr;
       cal->field = SHAPER_9_CALf;
       cal->index = core;
       *is_delay_2_clocks = TRUE;         
    }
    else if (_SHR_GPORT_IS_FABRIC_CLOS_FMQ_BESTEFFORT(gport)){
       max_credit->reg = IPT_IQM_SHAPERS_MAX_CREDITr;
       max_credit->field = SHAPER_10_MAX_CREDITf;
       max_credit->index = core;
       delay->reg = IPT_IQM_SHAPERS_DELAYr;
       delay->field = SHAPER_10_DELAYf;
       delay->index = core;
       cal->reg = IPT_IQM_SHAPERS_CALr;
       cal->field = SHAPER_10_CALf;
       cal->index = core;
       *is_delay_2_clocks = TRUE;         
    }
    else if (_SHR_GPORT_IS_FABRIC_CLOS_SCOPE(gport)){
       max_credit->reg = IPT_IQM_SHAPERS_MAX_CREDITr;
       max_credit->field = SHAPER_11_MAX_CREDITf;
       max_credit->index = core;
       delay->reg = IPT_IQM_SHAPERS_DELAYr;
       delay->field = SHAPER_11_DELAYf;
       delay->index = core;
       cal->reg = IPT_IQM_SHAPERS_CALr;
       cal->field = SHAPER_11_CALf;
       cal->index = core;
       *is_delay_2_clocks = TRUE;         
    }
    else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PORT,(_BSL_SOCDNX_MSG("gport type is not matched to fabric clos type\n")));
    }

exit:    
    SOCDNX_FUNC_RETURN;
}


STATIC soc_error_t
  jer_ingress_scheduler_mesh_shaper_gport_to_field(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  soc_gport_t            gport,
    SOC_SAND_IN  int                    core,
    SOC_SAND_OUT reg_field              *max_credit,
    SOC_SAND_OUT reg_field              *delay,
    SOC_SAND_OUT reg_field              *cal,
    SOC_SAND_OUT int                    *is_delay_2_clocks
  )
{
    SOCDNX_INIT_FUNC_DEFS;

    
    if (_SHR_GPORT_IS_FABRIC_MESH_COMMON_LOCAL0(gport)){
       max_credit->reg = IPT_COMMON_SHAPERS_MAX_CREDITr;
       max_credit->field = SHAPER_0_COMMON_MAX_CREDITf;
       max_credit->index = 0;
       delay->reg = IPT_COMMON_SHAPERS_DELAYr;
       delay->field = SHAPER_0_COMMON_DELAYf;
       delay->index = 0;
       cal->reg = IPT_COMMON_SHAPERS_CALr;
       cal->field = SHAPER_0_COMMON_CALf;
       cal->index = 0;
       *is_delay_2_clocks = FALSE;                
    }
    else if (_SHR_GPORT_IS_FABRIC_MESH_COMMON_LOCAL1(gport)){
       max_credit->reg = IPT_COMMON_SHAPERS_MAX_CREDITr;
       max_credit->field = SHAPER_1_COMMON_MAX_CREDITf;
       max_credit->index = 0;
       delay->reg = IPT_COMMON_SHAPERS_DELAYr;
       delay->field = SHAPER_1_COMMON_DELAYf;
       delay->index = 0;
       cal->reg = IPT_COMMON_SHAPERS_CALr;
       cal->field = SHAPER_1_COMMON_CALf;
       cal->index = 0;
       *is_delay_2_clocks = FALSE;                
    }
    else if (_SHR_GPORT_IS_FABRIC_MESH_COMMON_DEV1(gport)){
       max_credit->reg = IPT_COMMON_SHAPERS_MAX_CREDITr;
       max_credit->field = SHAPER_2_COMMON_MAX_CREDITf;
       max_credit->index = 0;
       delay->reg = IPT_COMMON_SHAPERS_DELAYr;
       delay->field = SHAPER_2_COMMON_DELAYf;
       delay->index = 0;
       cal->reg = IPT_COMMON_SHAPERS_CALr;
       cal->field = SHAPER_2_COMMON_CALf;
       cal->index = 0;
       *is_delay_2_clocks = FALSE;                
    }
    else if (_SHR_GPORT_IS_FABRIC_MESH_COMMON_DEV2(gport)){
       max_credit->reg = IPT_COMMON_SHAPERS_MAX_CREDITr;
       max_credit->field = SHAPER_3_COMMON_MAX_CREDITf;
       max_credit->index = 0;
       delay->reg = IPT_COMMON_SHAPERS_DELAYr;
       delay->field = SHAPER_3_COMMON_DELAYf;
       delay->index = 0;
       cal->reg = IPT_COMMON_SHAPERS_CALr;
       cal->field = SHAPER_3_COMMON_CALf;
       cal->index = 0;
       *is_delay_2_clocks = FALSE;                
    }
    else if (_SHR_GPORT_IS_FABRIC_MESH_COMMON_DEV3(gport) || _SHR_GPORT_IS_FABRIC_MESH_COMMON_MC(gport)){
       max_credit->reg = IPT_COMMON_SHAPERS_MAX_CREDITr;
       max_credit->field = SHAPER_4_COMMON_MAX_CREDITf;
       max_credit->index = 0;
       delay->reg = IPT_COMMON_SHAPERS_DELAYr;
       delay->field = SHAPER_4_COMMON_DELAYf;
       delay->index = 0;
       cal->reg = IPT_COMMON_SHAPERS_CALr;
       cal->field = SHAPER_4_COMMON_CALf;
       cal->index = 0;
       *is_delay_2_clocks = FALSE;                
    }      
    else if (_SHR_GPORT_IS_FABRIC_MESH_LOCAL0(gport)){
       max_credit->reg = IPT_IQM_SHAPERS_MAX_CREDITr;
       max_credit->field = SHAPER_0_MAX_CREDITf;
       max_credit->index = core;
       delay->reg = IPT_IQM_SHAPERS_DELAYr;
       delay->field = SHAPER_0_DELAYf;
       delay->index = core;
       cal->reg = IPT_IQM_SHAPERS_CALr;
       cal->field = SHAPER_0_CALf;
       cal->index = core;
       *is_delay_2_clocks = TRUE;         
    }
    else if (_SHR_GPORT_IS_FABRIC_MESH_LOCAL1(gport)){
       max_credit->reg = IPT_IQM_SHAPERS_MAX_CREDITr;
       max_credit->field = SHAPER_1_MAX_CREDITf;
       max_credit->index = core;
       delay->reg = IPT_IQM_SHAPERS_DELAYr;
       delay->field = SHAPER_1_DELAYf;
       delay->index = core;
       cal->reg = IPT_IQM_SHAPERS_CALr;
       cal->field = SHAPER_1_CALf;
       cal->index = core;
       *is_delay_2_clocks = TRUE;         
    }
    else if (_SHR_GPORT_IS_FABRIC_MESH_DEV1(gport)){
       max_credit->reg = IPT_IQM_SHAPERS_MAX_CREDITr;
       max_credit->field = SHAPER_2_MAX_CREDITf;
       max_credit->index = core;
       delay->reg = IPT_IQM_SHAPERS_DELAYr;
       delay->field = SHAPER_2_DELAYf;
       delay->index = core;
       cal->reg = IPT_IQM_SHAPERS_CALr;
       cal->field = SHAPER_2_CALf;
       cal->index = core;
       *is_delay_2_clocks = TRUE;         
    }
    else if (_SHR_GPORT_IS_FABRIC_MESH_DEV2(gport)){
       max_credit->reg = IPT_IQM_SHAPERS_MAX_CREDITr;
       max_credit->field = SHAPER_3_MAX_CREDITf;
       max_credit->index = core;
       delay->reg = IPT_IQM_SHAPERS_DELAYr;
       delay->field = SHAPER_3_DELAYf;
       delay->index = core;
       cal->reg = IPT_IQM_SHAPERS_CALr;
       cal->field = SHAPER_3_CALf;
       cal->index = core;
       *is_delay_2_clocks = TRUE;         
    }
    else if (_SHR_GPORT_IS_FABRIC_MESH_DEV3(gport) || _SHR_GPORT_IS_FABRIC_MESH_MC(gport)){
       max_credit->reg = IPT_IQM_SHAPERS_MAX_CREDITr;
       max_credit->field = SHAPER_4_MAX_CREDITf;
       max_credit->index = core;
       delay->reg = IPT_IQM_SHAPERS_DELAYr;
       delay->field = SHAPER_4_DELAYf;
       delay->index = core;
       cal->reg = IPT_IQM_SHAPERS_CALr;
       cal->field = SHAPER_4_CALf;
       cal->index = core;
       *is_delay_2_clocks = TRUE;         
    }
    else if (_SHR_GPORT_IS_FABRIC_MESH_SCOPE(gport)){
       max_credit->reg = IPT_IQM_SHAPERS_MAX_CREDITr;
       max_credit->field = SHAPER_5_MAX_CREDITf;
       max_credit->index = core;
       delay->reg = IPT_IQM_SHAPERS_DELAYr;
       delay->field = SHAPER_5_DELAYf;
       delay->index = core;
       cal->reg = IPT_IQM_SHAPERS_CALr;
       cal->field = SHAPER_5_CALf;
       cal->index = core;
       *is_delay_2_clocks = TRUE;         
    }
    else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PORT,(_BSL_SOCDNX_MSG("gport type is not matched to fabric clos type\n")));
    }

exit:    
    SOCDNX_FUNC_RETURN;
}


STATIC soc_error_t
  jer_ingress_scheduler_clos_sched_gport_to_field(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  soc_gport_t            gport,
    SOC_SAND_OUT reg_field              *weight_reg
  )
{
    SOCDNX_INIT_FUNC_DEFS;

    weight_reg->reg = IPT_IQM_WFQ_WEIGHTSr;

    if (_SHR_GPORT_IS_FABRIC_CLOS_LOCAL0_OCB_HIGH(gport)){
       weight_reg->field = WFQ_WEIGHT_0f;     
    }
    else if (_SHR_GPORT_IS_FABRIC_CLOS_LOCAL1_OCB_HIGH(gport)){
       weight_reg->field = WFQ_WEIGHT_1f;     
    }
    else if (_SHR_GPORT_IS_FABRIC_CLOS_UNICAST_FABRIC_OCB_HIGH(gport)){
       weight_reg->field = WFQ_WEIGHT_2f;     
    }
    else if (_SHR_GPORT_IS_FABRIC_CLOS_FMQ_GUARANTEED_OCB(gport)){
       weight_reg->field = WFQ_WEIGHT_3f;     
    }
    else if (_SHR_GPORT_IS_FABRIC_CLOS_LOCAL0_MIX_HIGH(gport)){
       weight_reg->field = WFQ_WEIGHT_4f;     
    }
    else if (_SHR_GPORT_IS_FABRIC_CLOS_LOCAL1_MIX_HIGH(gport)){
       weight_reg->field = WFQ_WEIGHT_5f;     
    }
    else if (_SHR_GPORT_IS_FABRIC_CLOS_UNICAST_FABRIC_MIX_HIGH(gport)){
       weight_reg->field = WFQ_WEIGHT_6f;     
    }
    else if (_SHR_GPORT_IS_FABRIC_CLOS_FMQ_GUARANTEED_MIX(gport)){
       weight_reg->field = WFQ_WEIGHT_7f;     
    }
    else if (_SHR_GPORT_IS_FABRIC_CLOS_LOCAL0_OCB_LOW(gport)){
       weight_reg->field = WFQ_WEIGHT_8f;     
    }
    else if (_SHR_GPORT_IS_FABRIC_CLOS_LOCAL1_OCB_LOW(gport)){
       weight_reg->field = WFQ_WEIGHT_9f;     
    }
    else if (_SHR_GPORT_IS_FABRIC_CLOS_UNICAST_FABRIC_OCB_LOW(gport)){
       weight_reg->field = WFQ_WEIGHT_10f;     
    }
    else if (_SHR_GPORT_IS_FABRIC_CLOS_FMQ_BEST_EFFORT_OCB(gport)){
       weight_reg->field = WFQ_WEIGHT_11f;     
    }
    else if (_SHR_GPORT_IS_FABRIC_CLOS_LOCAL0_MIX_LOW(gport)){
       weight_reg->field = WFQ_WEIGHT_12f;     
    }
    else if (_SHR_GPORT_IS_FABRIC_CLOS_LOCAL1_MIX_LOW(gport)){
       weight_reg->field = WFQ_WEIGHT_13f;     
    }
    else if (_SHR_GPORT_IS_FABRIC_CLOS_UNICAST_FABRIC_MIX_LOW(gport)){
       weight_reg->field = WFQ_WEIGHT_14f;     
    }
    else if (_SHR_GPORT_IS_FABRIC_CLOS_FMQ_BEST_EFFORT_MIX(gport)){
       weight_reg->field = WFQ_WEIGHT_15f;     
    }
    else if (_SHR_GPORT_IS_FABRIC_CLOS_OCB_HIGH(gport)){
       weight_reg->field = WFQ_WEIGHT_16f;     
    }
    else if (_SHR_GPORT_IS_FABRIC_CLOS_MIX_HIGH(gport)){
       weight_reg->field = WFQ_WEIGHT_17f;     
    }
    else if (_SHR_GPORT_IS_FABRIC_CLOS_OCB_LOW(gport)){
       weight_reg->field = WFQ_WEIGHT_18f;     
    }
    else if (_SHR_GPORT_IS_FABRIC_CLOS_MIX_LOW(gport)){
       weight_reg->field = WFQ_WEIGHT_19f;     
    }
    else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PORT,(_BSL_SOCDNX_MSG("gport type is not matched to fabric clos type\n")));
    }

exit:    
    SOCDNX_FUNC_RETURN;
}


STATIC soc_error_t
  jer_ingress_scheduler_mesh_sched_gport_to_field(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  soc_gport_t            gport,
    SOC_SAND_OUT reg_field              *weight_reg
  )
{
    SOCDNX_INIT_FUNC_DEFS;

    weight_reg->reg = IPT_IQM_WFQ_WEIGHTSr;

    if (_SHR_GPORT_IS_FABRIC_MESH_DEV1(gport)){
       weight_reg->field = WFQ_WEIGHT_22f;     
    }
    else if (_SHR_GPORT_IS_FABRIC_MESH_DEV2(gport)){
       weight_reg->field = WFQ_WEIGHT_23f;     
    }
    else if (_SHR_GPORT_IS_FABRIC_MESH_DEV2(gport) || _SHR_GPORT_IS_FABRIC_MESH_MC(gport)){
       weight_reg->field = WFQ_WEIGHT_24f;     
    }
    else if (_SHR_GPORT_IS_FABRIC_MESH_LOCAL0(gport)){
       weight_reg->field = WFQ_WEIGHT_20f;     
    }
    else if (_SHR_GPORT_IS_FABRIC_MESH_LOCAL1(gport)){
       weight_reg->field = WFQ_WEIGHT_21f;     
    }
    else if (_SHR_GPORT_IS_FABRIC_MESH_LOCAL0_OCB_HIGH(gport)){
       weight_reg->field = WFQ_WEIGHT_0f;     
    }
    else if (_SHR_GPORT_IS_FABRIC_MESH_LOCAL0_OCB_LOW(gport)){
       weight_reg->field = WFQ_WEIGHT_2f;     
    }
    else if (_SHR_GPORT_IS_FABRIC_MESH_LOCAL0_MIX_HIGH(gport)){
       weight_reg->field = WFQ_WEIGHT_1f;     
    }
    else if (_SHR_GPORT_IS_FABRIC_MESH_LOCAL0_MIX_LOW(gport)){
       weight_reg->field = WFQ_WEIGHT_3f;     
    }
    else if (_SHR_GPORT_IS_FABRIC_MESH_LOCAL1_OCB_HIGH(gport)){
       weight_reg->field = WFQ_WEIGHT_4f;     
    }
    else if (_SHR_GPORT_IS_FABRIC_MESH_LOCAL1_OCB_LOW(gport)){
       weight_reg->field = WFQ_WEIGHT_6f;     
    }
    else if (_SHR_GPORT_IS_FABRIC_MESH_LOCAL1_MIX_HIGH(gport)){
       weight_reg->field = WFQ_WEIGHT_5f;     
    }
    else if (_SHR_GPORT_IS_FABRIC_MESH_LOCAL1_MIX_LOW(gport)){
       weight_reg->field = WFQ_WEIGHT_7f;     
    }
    else if (_SHR_GPORT_IS_FABRIC_MESH_DEV1_OCB_HIGH(gport)){
       weight_reg->field = WFQ_WEIGHT_8f;     
    }
    else if (_SHR_GPORT_IS_FABRIC_MESH_DEV1_OCB_LOW(gport)){
       weight_reg->field = WFQ_WEIGHT_10f;     
    }
    else if (_SHR_GPORT_IS_FABRIC_MESH_DEV1_MIX_HIGH(gport)){
       weight_reg->field = WFQ_WEIGHT_9f;     
    }
    else if (_SHR_GPORT_IS_FABRIC_MESH_DEV1_MIX_LOW(gport)){
       weight_reg->field = WFQ_WEIGHT_11f;     
    }
    else if (_SHR_GPORT_IS_FABRIC_MESH_DEV2_OCB_HIGH(gport)){
       weight_reg->field = WFQ_WEIGHT_12f;     
    }
    else if (_SHR_GPORT_IS_FABRIC_MESH_DEV2_OCB_LOW(gport)){
       weight_reg->field = WFQ_WEIGHT_14f;     
    }
    else if (_SHR_GPORT_IS_FABRIC_MESH_DEV2_MIX_HIGH(gport)){
       weight_reg->field = WFQ_WEIGHT_13f;     
    }
    else if (_SHR_GPORT_IS_FABRIC_MESH_DEV2_MIX_LOW(gport)){
       weight_reg->field = WFQ_WEIGHT_15f;     
    }
    else if (_SHR_GPORT_IS_FABRIC_MESH_DEV3_OCB_HIGH(gport) || _SHR_GPORT_IS_FABRIC_MESH_MC_OCB_HIGH(gport)){
       weight_reg->field = WFQ_WEIGHT_16f;     
    }
    else if (_SHR_GPORT_IS_FABRIC_MESH_DEV3_OCB_LOW(gport) || _SHR_GPORT_IS_FABRIC_MESH_MC_OCB_LOW(gport)){
       weight_reg->field = WFQ_WEIGHT_18f;     
    }
    else if (_SHR_GPORT_IS_FABRIC_MESH_DEV3_MIX_HIGH(gport) || _SHR_GPORT_IS_FABRIC_MESH_MC_MIX_HIGH(gport)){
       weight_reg->field = WFQ_WEIGHT_17f;     
    }
    else if (_SHR_GPORT_IS_FABRIC_MESH_DEV3_MIX_LOW(gport) || _SHR_GPORT_IS_FABRIC_MESH_MC_MIX_LOW(gport)){
       weight_reg->field = WFQ_WEIGHT_19f;     
    }
    else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PORT,(_BSL_SOCDNX_MSG("gport type is not matched to fabric mesh type\n")));
    }

exit:    
    SOCDNX_FUNC_RETURN;
}


STATIC soc_error_t
  jer_ingress_scheduler_clos_shaper_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  soc_gport_t            gport,
    SOC_SAND_IN  int                    core, 
    SOC_SAND_OUT SOC_TMC_ING_SCH_SHAPER *shaper
  )
{

    uint32  res;
    reg_field max_credit, delay, cal;
    int is_delay_2_clocks;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(jer_ingress_scheduler_clos_shaper_gport_to_field(unit, gport, core, &max_credit, &delay, &cal, &is_delay_2_clocks));

    res = arad_ingress_scheduler_shaper_values_get(
          unit,
          is_delay_2_clocks,
          &max_credit,
          &delay,
          &cal,
          shaper
        );

    SOCDNX_SAND_IF_ERR_EXIT(res);

exit:    
    SOCDNX_FUNC_RETURN;
}


STATIC soc_error_t
  jer_ingress_scheduler_clos_shaper_set(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  soc_gport_t            gport,
    SOC_SAND_IN  int                    core,
    SOC_SAND_IN  SOC_TMC_ING_SCH_SHAPER shaper
  )
{
    int is_delay_2_clocks;
    uint32 exact_fabm_max_rate;
    reg_field max_credit, delay, cal;
    uint32  res;

    SOCDNX_INIT_FUNC_DEFS;

   
    SOCDNX_IF_ERR_EXIT(jer_ingress_scheduler_clos_shaper_gport_to_field(unit, gport, core, &max_credit, &delay, &cal, &is_delay_2_clocks));

    
    res = arad_ingress_scheduler_shaper_values_set(
          unit,
          is_delay_2_clocks,
          &shaper,
          &max_credit,
          &delay,
          &cal,
          &exact_fabm_max_rate
    );

    SOCDNX_SAND_IF_ERR_EXIT(res);


exit:    
    SOCDNX_FUNC_RETURN;
}



STATIC soc_error_t
  jer_ingress_scheduler_mesh_shaper_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  soc_gport_t            gport,
    SOC_SAND_IN  int                    core, 
    SOC_SAND_OUT SOC_TMC_ING_SCH_SHAPER *shaper
  )
{

    uint32  res;
    reg_field max_credit, delay, cal;
    int is_delay_2_clocks;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(jer_ingress_scheduler_mesh_shaper_gport_to_field(unit, gport, core, &max_credit, &delay, &cal, &is_delay_2_clocks));

    
    res = arad_ingress_scheduler_shaper_values_get(
          unit,
          is_delay_2_clocks,
          &max_credit,
          &delay,
          &cal,
          shaper
        );

    SOCDNX_SAND_IF_ERR_EXIT(res);

exit:    
    SOCDNX_FUNC_RETURN;
}


STATIC soc_error_t
  jer_ingress_scheduler_mesh_shaper_set(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  soc_gport_t            gport,
    SOC_SAND_IN  int                    core, 
    SOC_SAND_IN  SOC_TMC_ING_SCH_SHAPER shaper
  )
{
    int is_delay_2_clocks;
    uint32 exact_fabm_max_rate;
    reg_field max_credit, delay, cal;
    uint32  res;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(jer_ingress_scheduler_mesh_shaper_gport_to_field(unit, gport, core, &max_credit, &delay, &cal, &is_delay_2_clocks));

    
    res = arad_ingress_scheduler_shaper_values_set(
          unit,
          is_delay_2_clocks,
          &shaper,
          &max_credit,
          &delay,
          &cal,
          &exact_fabm_max_rate
    );

    SOCDNX_SAND_IF_ERR_EXIT(res);

exit:    
    SOCDNX_FUNC_RETURN;
}



soc_error_t
  jer_ingress_scheduler_clos_bandwidth_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  soc_gport_t         gport, 
    SOC_SAND_OUT uint32              *rate
  )
{
    SOC_TMC_ING_SCH_SHAPER shaper;
    int                    core;

    SOCDNX_INIT_FUNC_DEFS;

    core =  SOC_GPORT_SCHEDULER_CORE_GET(gport);
    if (core == SOC_CORE_ALL) 
    {
        core = 0;
    }

    SOC_TMC_ING_SCH_SHAPER_clear(&shaper);

    SOCDNX_IF_ERR_EXIT(jer_ingress_scheduler_clos_shaper_get(unit, gport, core, &shaper)); 
  
    *rate = shaper.max_rate; 

exit:    
    SOCDNX_FUNC_RETURN;
}




soc_error_t
  jer_ingress_scheduler_clos_bandwidth_set(
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  soc_gport_t         gport, 
    SOC_SAND_IN  uint32              rate
    )
{
    SOC_TMC_ING_SCH_SHAPER shaper;
    int index, core;

    SOCDNX_INIT_FUNC_DEFS;

    core = SOC_GPORT_SCHEDULER_CORE_GET(gport);

    SOC_DPP_CORES_ITER(core, index)
    {
        SOC_TMC_ING_SCH_SHAPER_clear(&shaper);

        SOCDNX_IF_ERR_EXIT(jer_ingress_scheduler_clos_shaper_get(unit, gport, index, &shaper));

        shaper.max_rate = rate;

        SOCDNX_IF_ERR_EXIT(jer_ingress_scheduler_clos_shaper_set(unit, gport, index, shaper)); 
    }
        
exit:    
    SOCDNX_FUNC_RETURN;
}


soc_error_t
  jer_ingress_scheduler_mesh_bandwidth_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  soc_gport_t         gport, 
    SOC_SAND_OUT uint32              *rate
  )
{
    SOC_TMC_ING_SCH_SHAPER shaper;
    int                    core;

    SOCDNX_INIT_FUNC_DEFS;

    core =  SOC_GPORT_SCHEDULER_CORE_GET(gport);
    if (core == SOC_CORE_ALL) 
    {
        core = 0;
    }

    SOC_TMC_ING_SCH_SHAPER_clear(&shaper);

    SOCDNX_IF_ERR_EXIT(jer_ingress_scheduler_mesh_shaper_get(unit, gport, core, &shaper)); 
  
    *rate = shaper.max_rate; 

exit:    
    SOCDNX_FUNC_RETURN;
}




soc_error_t
  jer_ingress_scheduler_mesh_bandwidth_set(
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  soc_gport_t         gport, 
    SOC_SAND_IN  uint32              rate
    )
{
    SOC_TMC_ING_SCH_SHAPER shaper;
    int core, index;

    SOCDNX_INIT_FUNC_DEFS;

    core = SOC_GPORT_SCHEDULER_CORE_GET(gport);

    SOC_DPP_CORES_ITER(core, index)
    {
        SOC_TMC_ING_SCH_SHAPER_clear(&shaper);

        SOCDNX_IF_ERR_EXIT(jer_ingress_scheduler_mesh_shaper_get(unit, gport, index, &shaper));

        shaper.max_rate = rate;

        SOCDNX_IF_ERR_EXIT(jer_ingress_scheduler_mesh_shaper_set(unit, gport, index, shaper)); 
    }
        
exit:    
    SOCDNX_FUNC_RETURN;
}



soc_error_t
  jer_ingress_scheduler_clos_burst_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  soc_gport_t         gport, 
    SOC_SAND_OUT int                 *burst
  )
{
    SOC_TMC_ING_SCH_SHAPER shaper;
    int                    core;

    SOCDNX_INIT_FUNC_DEFS;

    core =  SOC_GPORT_SCHEDULER_CORE_GET(gport);
    if (core == SOC_CORE_ALL) 
    {
        core = 0;
    }

    SOC_TMC_ING_SCH_SHAPER_clear(&shaper);

    SOCDNX_IF_ERR_EXIT(jer_ingress_scheduler_clos_shaper_get(unit, gport, core, &shaper)); 
  
    *burst = shaper.max_burst; 

exit:    
    SOCDNX_FUNC_RETURN;
}




soc_error_t
  jer_ingress_scheduler_clos_burst_set(
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  soc_gport_t         gport, 
    SOC_SAND_IN  int                 burst
    )
{
    SOC_TMC_ING_SCH_SHAPER shaper;
    int                    core, index;

    SOCDNX_INIT_FUNC_DEFS;

    
    if (burst > ARAD_ING_SCH_MAX_MAX_CREDIT_VALUE)
    {
       
       SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid burst size")));
    }
    core = SOC_GPORT_SCHEDULER_CORE_GET(gport);

    SOC_DPP_CORES_ITER(core, index)
    {

        SOC_TMC_ING_SCH_SHAPER_clear(&shaper);

        SOCDNX_IF_ERR_EXIT(jer_ingress_scheduler_clos_shaper_get(unit, gport, index, &shaper));

        shaper.max_rate = SOC_TMC_ING_SCH_DONT_TOUCH;
        shaper.max_burst = burst;

        SOCDNX_IF_ERR_EXIT(jer_ingress_scheduler_clos_shaper_set(unit, gport, index, shaper)); 
    }
        
exit:    
    SOCDNX_FUNC_RETURN;
}


soc_error_t
  jer_ingress_scheduler_mesh_burst_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  soc_gport_t         gport, 
    SOC_SAND_OUT int                 *burst
  )
{
    SOC_TMC_ING_SCH_SHAPER shaper;
    int                    core;

    SOCDNX_INIT_FUNC_DEFS;

    core =  SOC_GPORT_SCHEDULER_CORE_GET(gport);
    if (core == SOC_CORE_ALL) 
    {
        core = 0;
    }

    SOC_TMC_ING_SCH_SHAPER_clear(&shaper);

    SOCDNX_IF_ERR_EXIT(jer_ingress_scheduler_mesh_shaper_get(unit, gport, core, &shaper)); 
  
    *burst = shaper.max_burst; 

exit:    
    SOCDNX_FUNC_RETURN;
}




soc_error_t
  jer_ingress_scheduler_mesh_burst_set(
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  soc_gport_t         gport, 
    SOC_SAND_IN  int                 burst
    )
{
    SOC_TMC_ING_SCH_SHAPER shaper;
    int core, index;

    SOCDNX_INIT_FUNC_DEFS;

    
    if (burst > ARAD_ING_SCH_MAX_MAX_CREDIT_VALUE)
    {
       
       SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid burst size")));
    }

    core = SOC_GPORT_SCHEDULER_CORE_GET(gport);

    SOC_DPP_CORES_ITER(core, index)
    {
        SOC_TMC_ING_SCH_SHAPER_clear(&shaper);

        SOCDNX_IF_ERR_EXIT(jer_ingress_scheduler_mesh_shaper_get(unit, gport, index, &shaper));

        shaper.max_rate = SOC_TMC_ING_SCH_DONT_TOUCH;
        shaper.max_burst = burst;

        SOCDNX_IF_ERR_EXIT(jer_ingress_scheduler_mesh_shaper_set(unit, gport, index, shaper)); 
    }
        
exit:    
    SOCDNX_FUNC_RETURN;
}





soc_error_t
  jer_ingress_scheduler_clos_sched_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  soc_gport_t            gport, 
    SOC_SAND_OUT int                    *weight
  )
{
    int       core;
    reg_field weight_reg;
    uint32    reg_data;

    SOCDNX_INIT_FUNC_DEFS;

    core =  SOC_GPORT_SCHEDULER_CORE_GET(gport);
    if (core == SOC_CORE_ALL) 
    {
        core = 0;
    }        

    SOCDNX_IF_ERR_EXIT(jer_ingress_scheduler_clos_sched_gport_to_field(unit, gport, &weight_reg));

    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_read(unit, weight_reg.reg, REG_PORT_ANY, core, weight_reg.field, &reg_data));

    *weight = reg_data;

exit:    
    SOCDNX_FUNC_RETURN;
}


soc_error_t
  jer_ingress_scheduler_clos_sched_set(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  soc_gport_t            gport, 
    SOC_SAND_IN  int                    weight
  )
{
    int core, index;
    reg_field weight_reg;

    SOCDNX_INIT_FUNC_DEFS;

    
    if (weight > ARAD_ING_SCH_MAX_WEIGHT_VALUE) 
    {
       
       SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid weight")));
    }

    SOCDNX_IF_ERR_EXIT(jer_ingress_scheduler_clos_sched_gport_to_field(unit, gport, &weight_reg));
              
    core = SOC_GPORT_SCHEDULER_CORE_GET(gport);

    SOC_DPP_CORES_ITER(core, index)
    { 
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, weight_reg.reg, REG_PORT_ANY, index, weight_reg.field,  weight));
    }

exit:    
    SOCDNX_FUNC_RETURN;
}



soc_error_t
  jer_ingress_scheduler_mesh_sched_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  soc_gport_t            gport, 
    SOC_SAND_OUT int                    *weight
  )
{

    int core;
    uint32    reg_data;
    reg_field weight_reg;

    SOCDNX_INIT_FUNC_DEFS;

    core =  SOC_GPORT_SCHEDULER_CORE_GET(gport);
    if (core == SOC_CORE_ALL) 
    {
        core = 0;
    }

    SOCDNX_IF_ERR_EXIT(jer_ingress_scheduler_mesh_sched_gport_to_field(unit, gport, &weight_reg));
            
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_read(unit, weight_reg.reg, REG_PORT_ANY, core, weight_reg.field, &reg_data));

    *weight = reg_data;

exit:    
    SOCDNX_FUNC_RETURN;
}


soc_error_t
  jer_ingress_scheduler_mesh_sched_set(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  soc_gport_t            gport, 
    SOC_SAND_IN  int                    weight
  )
{
    int core, index;
    reg_field weight_reg;

    SOCDNX_INIT_FUNC_DEFS;

    
    if (weight > ARAD_ING_SCH_MAX_WEIGHT_VALUE) 
    {
       
       SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid weight")));
    }

    SOCDNX_IF_ERR_EXIT(jer_ingress_scheduler_mesh_sched_gport_to_field(unit, gport, &weight_reg));
           
    core = SOC_GPORT_SCHEDULER_CORE_GET(gport);

    SOC_DPP_CORES_ITER(core, index)
    {
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, weight_reg.reg, REG_PORT_ANY, index, weight_reg.field,  weight));
    }

exit:    
    SOCDNX_FUNC_RETURN;
}


#undef _ERR_MSG_MODULE_NAME

#include <soc/dpp/SAND/Utils/sand_footer.h>
