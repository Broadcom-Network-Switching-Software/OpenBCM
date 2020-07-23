/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shared/bsl.h>
#include <soc/mcm/memregs.h>

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_INGRESS



#include <soc/mem.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/TMC/tmc_api_ingress_scheduler.h>
#include <soc/dpp/ARAD/arad_ingress_scheduler.h>
#include <soc/dpp/QAX/qax_ingress_scheduler.h>



typedef struct qax_ingress_scheduler_mem_field_s {
    soc_mem_t   delay_cal_mem;
    soc_field_t delay_field;
    soc_field_t cal_field;
    int         delay_cal_index;
} qax_ingress_scheduler_mem_field_t;




STATIC soc_error_t
  qax_ingress_scheduler_shaper_values_get(
    SOC_SAND_IN   int                       unit,
    SOC_SAND_IN   int                       is_delay_2_clocks,
    SOC_SAND_IN   qax_ingress_scheduler_mem_field_t *shaper_mem,
    SOC_SAND_OUT  ARAD_ING_SCH_SHAPER       *shaper
  )
{
  uint32 res, shaper_delay_2_clocks, shaper_cal;
  uint32 shaper_table_entry[4] = {0};
  SOCDNX_INIT_FUNC_DEFS;

  SOCDNX_NULL_CHECK(shaper_mem);
  SOCDNX_NULL_CHECK(shaper);

  
  SOCDNX_IF_ERR_EXIT(soc_mem_read(unit, shaper_mem->delay_cal_mem, MEM_BLOCK_ANY, shaper_mem->delay_cal_index, shaper_table_entry));
  shaper_delay_2_clocks = soc_mem_field32_get(unit, shaper_mem->delay_cal_mem, shaper_table_entry, shaper_mem->delay_field);
  shaper_cal = soc_mem_field32_get(unit, shaper_mem->delay_cal_mem, shaper_table_entry, shaper_mem->cal_field);

  
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
  SOCDNX_SAND_IF_ERR_EXIT(res);

exit:
  SOCDNX_FUNC_RETURN;
}



STATIC soc_error_t
  qax_ingress_scheduler_shaper_values_set(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  int                  is_delay_2_clocks,
    SOC_SAND_IN  qax_ingress_scheduler_mem_field_t *shaper_mem,
    SOC_SAND_IN  ARAD_ING_SCH_SHAPER     *shaper
  )
{
    uint32 res, shaper_delay_2_clocks, shaper_cal, dummy, shaper_table_entry[4] = {0};
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(shaper_mem);
    SOCDNX_NULL_CHECK(shaper);

    

    
    res = arad_ingress_scheduler_rate_to_delay_cal_form(
            unit,
            shaper->max_rate,
            is_delay_2_clocks,
            &shaper_delay_2_clocks,
            &shaper_cal,
            &dummy
    );
    SOCDNX_SAND_IF_ERR_EXIT(res);

    
    SOCDNX_IF_ERR_EXIT(soc_mem_read(unit, shaper_mem->delay_cal_mem, MEM_BLOCK_ANY, shaper_mem->delay_cal_index, shaper_table_entry));
    soc_mem_field32_set(unit, shaper_mem->delay_cal_mem, shaper_table_entry, shaper_mem->delay_field, shaper_delay_2_clocks);
    soc_mem_field32_set(unit, shaper_mem->delay_cal_mem, shaper_table_entry, shaper_mem->cal_field, shaper_cal);
    SOCDNX_IF_ERR_EXIT(soc_mem_write(unit, shaper_mem->delay_cal_mem, MEM_BLOCK_ANY, shaper_mem->delay_cal_index, shaper_table_entry));

exit:
  SOCDNX_FUNC_RETURN;
}


STATIC soc_error_t
qax_ingress_scheduler_slow_start_phase_set (
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ING_SCH_SHAPER *shaper_info,
    SOC_SAND_IN  ARAD_ING_SCH_SHAPER *full_rate_shaper,
    SOC_SAND_IN int                  is_hp,
    SOC_SAND_IN int                  is_phase_1
    )
{
    qax_ingress_scheduler_mem_field_t shaper_mem;
    ARAD_ING_SCH_SHAPER shaper_slow_phase_config;
    soc_error_t rv = SOC_E_NONE;
    uint32 factor;
    SOCDNX_INIT_FUNC_DEFS;

    shaper_mem.delay_cal_mem = PTS_SHAPER_FMC_CFGm;
    shaper_mem.delay_field = is_phase_1? SLOW_START_DELAY_1f : SLOW_START_DELAY_0f;
    shaper_mem.cal_field = is_phase_1? SLOW_START_CAL_1f : SLOW_START_CAL_0f;
    shaper_mem.delay_cal_index = is_hp? QAX_INGRESS_SCHEDULER_HP_MC_SHAPER_SLOW_START_CONFIG_INDEX : QAX_INGRESS_SCHEDULER_LP_MC_SHAPER_SLOW_START_CONFIG_INDEX;

    
    SOC_TMC_ING_SCH_SHAPER_clear(&shaper_slow_phase_config);
    factor = is_phase_1? shaper_info->slow_start_rate_phase_1 : shaper_info->slow_start_rate_phase_0;
    shaper_slow_phase_config.max_rate = full_rate_shaper->max_rate * factor / 100;
    
    rv = qax_ingress_scheduler_shaper_values_set(
            unit,
            TRUE,
            &shaper_mem,
            &shaper_slow_phase_config
    );
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC soc_error_t
qax_ingress_scheduler_slow_start_configuration_set (
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ING_SCH_SHAPER *shaper_info,
    SOC_SAND_IN int                  is_hp
    )
{
    qax_ingress_scheduler_mem_field_t shaper_mem;
    ARAD_ING_SCH_SHAPER full_rate_shaper;
    uint32 shaper_table_entry[4] = {0};
    soc_error_t rv = SOC_E_NONE;
    SOCDNX_INIT_FUNC_DEFS;

    shaper_mem.delay_cal_mem = PTS_PER_SHAPER_CFGm;
    shaper_mem.delay_field = SHAPER_DELAYf;
    shaper_mem.cal_field = SHAPER_CALf;
    shaper_mem.delay_cal_index = is_hp? QAX_INGRESS_SCHEDULER_HP_MC_SHAPER_INDEX : QAX_INGRESS_SCHEDULER_LP_MC_SHAPER_INDEX;

    
    rv = qax_ingress_scheduler_shaper_values_get(
            unit,
            TRUE,
            &shaper_mem,
            &full_rate_shaper
    );
    SOCDNX_IF_ERR_EXIT(rv);

    

    
    if (shaper_info->slow_start_enable != SOC_TMC_ING_SCH_DONT_TOUCH)
    {
        int index = is_hp? QAX_INGRESS_SCHEDULER_HP_MC_SHAPER_SLOW_START_CONFIG_INDEX : QAX_INGRESS_SCHEDULER_LP_MC_SHAPER_SLOW_START_CONFIG_INDEX;
        int val = (shaper_info->slow_start_enable)? 1 : 0;
        SOCDNX_IF_ERR_EXIT(soc_mem_read(unit, PTS_SHAPER_FMC_CFGm, MEM_BLOCK_ANY, index, shaper_table_entry));
        soc_mem_field32_set(unit, PTS_SHAPER_FMC_CFGm, shaper_table_entry, SLOW_START_ENf, val);
        SOCDNX_IF_ERR_EXIT(soc_mem_write(unit, PTS_SHAPER_FMC_CFGm, MEM_BLOCK_ANY, index, shaper_table_entry));
    }

    
    if (shaper_info->slow_start_rate_phase_0 != SOC_TMC_ING_SCH_DONT_TOUCH)
    {
        rv = qax_ingress_scheduler_slow_start_phase_set (
                unit,
                shaper_info,
                &full_rate_shaper, 
                is_hp,
                0
         );
        SOCDNX_IF_ERR_EXIT(rv);
    }

    
    if (shaper_info->slow_start_rate_phase_1 != SOC_TMC_ING_SCH_DONT_TOUCH)
    {
        rv = qax_ingress_scheduler_slow_start_phase_set (
                unit,
                shaper_info,
                &full_rate_shaper, 
                is_hp,
                1
        );
        SOCDNX_IF_ERR_EXIT(rv);
    }

exit:
    SOCDNX_FUNC_RETURN;
}



STATIC soc_error_t
qax_ingress_scheduler_slow_start_phase_get (
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT  ARAD_ING_SCH_SHAPER *shaper_info,
    SOC_SAND_IN int                  is_hp,
    SOC_SAND_IN int                  is_phase_1
    )
{
    qax_ingress_scheduler_mem_field_t shaper_mem;
    ARAD_ING_SCH_SHAPER shaper_slow_phase_config;
    soc_error_t rv = SOC_E_NONE;
    uint32 val = 0;
    SOCDNX_INIT_FUNC_DEFS;

    
    shaper_mem.delay_cal_mem = PTS_SHAPER_FMC_CFGm;
    shaper_mem.delay_field = is_phase_1? SLOW_START_DELAY_1f : SLOW_START_DELAY_0f;
    shaper_mem.cal_field = is_phase_1? SLOW_START_CAL_1f : SLOW_START_CAL_0f;
    shaper_mem.delay_cal_index = is_hp? QAX_INGRESS_SCHEDULER_HP_MC_SHAPER_SLOW_START_CONFIG_INDEX : QAX_INGRESS_SCHEDULER_LP_MC_SHAPER_SLOW_START_CONFIG_INDEX;

    
    SOC_TMC_ING_SCH_SHAPER_clear(&shaper_slow_phase_config);
    rv = qax_ingress_scheduler_shaper_values_get(
            unit,
            TRUE,
            &shaper_mem,
            &shaper_slow_phase_config
    );
    SOCDNX_IF_ERR_EXIT(rv);

    if (shaper_info->max_rate != 0) 
    {
        val = shaper_slow_phase_config.max_rate * 100 / shaper_info->max_rate;
    }
    if (is_phase_1)
    {
        shaper_info->slow_start_rate_phase_1 = val;
    } else
    {
        shaper_info->slow_start_rate_phase_0 = val;
    }

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC soc_error_t
qax_ingress_scheduler_slow_start_configuration_get (
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT  ARAD_ING_SCH_SHAPER *shaper_info,
    SOC_SAND_IN int                  is_hp
    )
{
    qax_ingress_scheduler_mem_field_t shaper_mem;
    uint32 shaper_table_entry[4] = {0};
    soc_error_t rv = SOC_E_NONE;
    int index;
    SOCDNX_INIT_FUNC_DEFS;

    
    shaper_mem.delay_cal_mem = PTS_PER_SHAPER_CFGm;
    shaper_mem.delay_field = SHAPER_DELAYf;
    shaper_mem.cal_field = SHAPER_CALf;
    shaper_mem.delay_cal_index = is_hp? QAX_INGRESS_SCHEDULER_HP_MC_SHAPER_INDEX : QAX_INGRESS_SCHEDULER_LP_MC_SHAPER_INDEX;

    
    rv = qax_ingress_scheduler_shaper_values_get(
            unit,
            TRUE,
            &shaper_mem,
            shaper_info
    );
    SOCDNX_IF_ERR_EXIT(rv);

    
    
    index = is_hp? QAX_INGRESS_SCHEDULER_HP_MC_SHAPER_SLOW_START_CONFIG_INDEX : QAX_INGRESS_SCHEDULER_LP_MC_SHAPER_SLOW_START_CONFIG_INDEX;
    SOCDNX_IF_ERR_EXIT(soc_mem_read(unit, PTS_SHAPER_FMC_CFGm, MEM_BLOCK_ANY, index, shaper_table_entry));
    shaper_info->slow_start_enable = soc_mem_field32_get(unit, PTS_SHAPER_FMC_CFGm, shaper_table_entry, SLOW_START_ENf);

    
    rv = qax_ingress_scheduler_slow_start_phase_get (
            unit,
            shaper_info,
            is_hp,
            0
    );
    SOCDNX_IF_ERR_EXIT(rv);

    
    rv = qax_ingress_scheduler_slow_start_phase_get (
            unit,
            shaper_info,
            is_hp,
            1
    );
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t
qax_ingress_scheduler_clos_slow_start_get (
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_OUT SOC_TMC_ING_SCH_CLOS_INFO   *clos_info
    )
{
    soc_error_t rv = SOC_E_NONE;
    SOCDNX_INIT_FUNC_DEFS;

    
    rv = qax_ingress_scheduler_slow_start_configuration_get (
            unit,
            &clos_info->shapers.hp.fabric_multicast,
            1
    );
    SOCDNX_IF_ERR_EXIT(rv);

    
    rv = qax_ingress_scheduler_slow_start_configuration_get (
            unit,
            &clos_info->shapers.lp.fabric_multicast,
            0
    );
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}



soc_error_t
qax_ingress_scheduler_clos_slow_start_set (
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN SOC_TMC_ING_SCH_CLOS_INFO   *clos_info
    )
{
    soc_error_t rv = SOC_E_NONE;
    SOCDNX_INIT_FUNC_DEFS;

    
    rv = qax_ingress_scheduler_slow_start_configuration_set (
        unit,
        &clos_info->shapers.hp.fabric_multicast,
        1
    );
    SOCDNX_IF_ERR_EXIT(rv);

    
    rv = qax_ingress_scheduler_slow_start_configuration_set (
        unit,
        &clos_info->shapers.lp.fabric_multicast,
        0
    );
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}



soc_error_t
qax_ingress_scheduler_mesh_slow_start_get (
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_OUT SOC_TMC_ING_SCH_SHAPER   *shaper_info
    )
{
    soc_error_t rv = SOC_E_NONE;
    SOCDNX_INIT_FUNC_DEFS;

    
    rv = qax_ingress_scheduler_slow_start_configuration_get (
            unit,
            shaper_info,
            1
    );
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t
qax_ingress_scheduler_mesh_slow_start_set (
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN SOC_TMC_ING_SCH_SHAPER   *shaper_info
    )
{
    soc_error_t rv = SOC_E_NONE;
    SOCDNX_INIT_FUNC_DEFS;

    
    
    rv = qax_ingress_scheduler_slow_start_configuration_set (
            unit,
            shaper_info,
            1
    );
    SOCDNX_IF_ERR_EXIT(rv);

    
    rv = qax_ingress_scheduler_slow_start_configuration_set (
            unit,
            shaper_info,
            0
    );
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}
