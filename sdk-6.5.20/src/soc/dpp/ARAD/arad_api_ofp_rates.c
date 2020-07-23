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
#include <soc/error.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/drv.h>
#include <soc/dpp/ARAD/arad_api_ofp_rates.h>
#include <soc/dpp/ARAD/arad_ofp_rates.h>
#include <soc/dpp/ARAD/arad_scheduler_ports.h>




























uint32
    arad_ofp_rates_port2chan_arb(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                fap_port,
    SOC_SAND_OUT  ARAD_OFP_RATES_EGQ_CHAN_ARB_ID *chan_arb_id
    )
{
   uint32
    res = SOC_SAND_OK;

  SOCDNX_INIT_FUNC_DEFS
  ;
  

  SOCDNX_NULL_CHECK(chan_arb_id);

  res = arad_ofp_rates_port2chan_arb_unsafe(unit,
                                            fap_port,
                                            chan_arb_id);
  SOCDNX_IF_ERR_EXIT(res);

  exit:
    SOCDNX_FUNC_RETURN
}

int
  arad_ofp_rates_egq_single_port_rate_sw_set(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                    core,
    SOC_SAND_IN  uint32                 tm_port,
    SOC_SAND_IN  uint32                 rate
  )
{
  int
    res = SOC_SAND_OK;

  SOCDNX_INIT_FUNC_DEFS
  ;
  

  res = arad_ofp_rates_single_port_verify(
          unit,
          &rate
        );
  SOCDNX_IF_ERR_EXIT(res);

  res = arad_ofp_rates_egq_single_port_rate_sw_set_unsafe(
          unit,
          core,
          tm_port,
          rate
        );
  SOCDNX_IF_ERR_EXIT(res);


  
exit:
  SOCDNX_FUNC_RETURN
}

int
  arad_ofp_rates_egq_single_port_rate_hw_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32              tm_port,
    SOC_SAND_IN  uint32              is_alt_calc

  )
{
  uint32
    res = SOC_SAND_OK;

  SOCDNX_INIT_FUNC_DEFS
  


  res = arad_ofp_rates_egq_single_port_rate_hw_set_unsafe(
          unit,
          is_alt_calc
        );
  SOCDNX_IF_ERR_EXIT(res);


  
exit:
  SOCDNX_FUNC_RETURN
}

int
  arad_ofp_rates_sch_single_port_rate_hw_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32              tm_port,
    SOC_SAND_IN  uint32              is_alt_calc

  )
{
  uint32
    res = SOC_SAND_OK;

  SOCDNX_INIT_FUNC_DEFS
  


  res = arad_ofp_rates_sch_single_port_rate_hw_set_unsafe(
          unit,
          is_alt_calc
        );
  SOCDNX_IF_ERR_EXIT(res);


  
exit:
  SOCDNX_FUNC_RETURN
}

int
  arad_ofp_rates_single_port_max_burst_set(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                    core,
    SOC_SAND_IN  uint32                 tm_port,
    SOC_SAND_IN  uint32                 max_burst
  )
{
  int
    res = SOC_SAND_OK;

  SOCDNX_INIT_FUNC_DEFS
  ;

  

  res = arad_ofp_rates_single_port_verify(
          unit,
          &max_burst
        );
  SOCDNX_IF_ERR_EXIT(res);

  res = arad_ofp_rates_single_port_max_burst_set_unsafe(
          unit,
          core,
          tm_port,
          max_burst
        );
  SOCDNX_IF_ERR_EXIT(res);


  
exit:
  SOCDNX_FUNC_RETURN
}

int
  arad_ofp_rates_egq_single_port_rate_hw_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32              tm_port,
    SOC_SAND_OUT uint32              *rate
  )
{
  uint32
    res = SOC_SAND_OK;

  SOCDNX_INIT_FUNC_DEFS

  ;

  SOCDNX_NULL_CHECK(rate);

  

  res = arad_ofp_rates_egq_single_port_rate_hw_get_unsafe(
          unit,
          core,
          tm_port,
          rate
        );
  SOCDNX_IF_ERR_EXIT(res);


  
exit:
  SOCDNX_FUNC_RETURN
}

int
  arad_ofp_rates_single_port_max_burst_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                    core,
    SOC_SAND_IN  uint32                 tm_port,
    SOC_SAND_OUT uint32                 *max_burst
  )
{
  int
    res = SOC_SAND_OK;

  SOCDNX_INIT_FUNC_DEFS

  ;

  SOCDNX_NULL_CHECK(max_burst);

  

  res = arad_ofp_rates_single_port_max_burst_get_unsafe(
          unit,
          core,
          tm_port,
          max_burst
        );
  SOCDNX_IF_ERR_EXIT(res);


  
exit:
  SOCDNX_FUNC_RETURN
}

int
  arad_ofp_rates_egq_interface_shaper_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  int                      core,
    SOC_SAND_IN  uint32                   tm_port,
    SOC_SAND_IN SOC_TMC_OFP_SHPR_UPDATE_MODE rate_update_mode,
    SOC_SAND_IN  uint32                      if_shaper_rate
  )
{
  uint32
    res = SOC_SAND_OK;

  SOCDNX_INIT_FUNC_DEFS
  ;

  res = arad_ofp_rates_interface_shaper_verify(
          unit,
          core,
          tm_port
        );
  SOCDNX_IF_ERR_EXIT(res);

  

  res = arad_ofp_rates_egq_interface_shaper_set_unsafe(
          unit,
          core,
          tm_port,
          rate_update_mode,
          if_shaper_rate
        );
  SOCDNX_IF_ERR_EXIT(res);


  
exit:
  SOCDNX_FUNC_RETURN
}

int
  arad_ofp_rates_egq_interface_shaper_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32              tm_port,
    SOC_SAND_OUT uint32             *if_shaper_rate
  )
{
  uint32
    res = SOC_SAND_OK;

  SOCDNX_INIT_FUNC_DEFS
  ;

  SOCDNX_NULL_CHECK(if_shaper_rate);

  

  res = arad_ofp_rates_egq_interface_shaper_get_unsafe(
          unit,
          core,
          tm_port,
          if_shaper_rate
        );
  SOCDNX_IF_ERR_EXIT(res);


  
exit:
  SOCDNX_FUNC_RETURN
}


uint32
    arad_ofp_rates_port_priority_max_burst_for_fc_queues_set(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint32 max_burst_fc_queues
    )
{
  uint32
    res = SOC_SAND_OK;

  SOCDNX_INIT_FUNC_DEFS
  ;

  res = arad_ofp_rates_port_priority_max_burst_for_fc_queues_verify(
          unit,
          max_burst_fc_queues
          );
  SOCDNX_IF_ERR_EXIT(res);

  

  res = arad_ofp_rates_port_priority_max_burst_for_fc_queues_set_unsafe(
          unit,
          max_burst_fc_queues
        );
  SOCDNX_IF_ERR_EXIT(res);


  
exit:
  SOCDNX_FUNC_RETURN
}

uint32
    arad_ofp_rates_port_priority_max_burst_for_empty_queues_set(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint32 max_burst_empty_queues
    )
{
  uint32
    res = SOC_SAND_OK;

  SOCDNX_INIT_FUNC_DEFS
  ;

  res = arad_ofp_rates_port_priority_max_burst_for_empty_queues_verify(
          unit,
          max_burst_empty_queues
          );
  SOCDNX_IF_ERR_EXIT(res);

  

  res = arad_ofp_rates_port_priority_max_burst_for_empty_queues_set_unsafe(
          unit,
          max_burst_empty_queues
        );
  SOCDNX_IF_ERR_EXIT(res);


  
exit:
  SOCDNX_FUNC_RETURN
}

uint32
    arad_ofp_rates_port_priority_max_burst_for_fc_queues_get(
    SOC_SAND_IN  int unit,
    SOC_SAND_OUT  uint32 *max_burst_fc_queues
    )
{
  uint32
    res = SOC_SAND_OK;

  SOCDNX_INIT_FUNC_DEFS
  ;

  SOCDNX_NULL_CHECK(max_burst_fc_queues);  

  

  res = arad_ofp_rates_port_priority_max_burst_for_fc_queues_get_unsafe(
          unit,
          max_burst_fc_queues
        );
  SOCDNX_IF_ERR_EXIT(res);


  
exit:
  SOCDNX_FUNC_RETURN
}

uint32
    arad_ofp_rates_port_priority_max_burst_for_empty_queues_get(
    SOC_SAND_IN  int unit,
    SOC_SAND_OUT  uint32 *max_burst_empty_queues
    )
{
  uint32
    res = SOC_SAND_OK;

  SOCDNX_INIT_FUNC_DEFS
  ;

  SOCDNX_NULL_CHECK(max_burst_empty_queues);  

  

  res = arad_ofp_rates_port_priority_max_burst_for_empty_queues_get_unsafe(
          unit,
          max_burst_empty_queues
        );
  SOCDNX_IF_ERR_EXIT(res);


  
exit:
  SOCDNX_FUNC_RETURN
}

uint32
    arad_ofp_rates_egq_tcg_max_burst_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32              tm_port,   
    SOC_SAND_IN  uint32              tcg_ndx, 
    SOC_SAND_IN  uint32              max_burst
    )
{
  uint32
    res = SOC_SAND_OK;

  SOCDNX_INIT_FUNC_DEFS
  ;

  res = arad_ofp_rates_tcg_shaper_verify(unit,
                                         core,
                                         tm_port,
                                         tcg_ndx,
                                         max_burst,
                                         ARAD_OFP_RATES_GENERIC_FUNC_STATE_TCG_EGQ_BURST);
  SOCDNX_IF_ERR_EXIT(res);

  

  res = arad_ofp_rates_egq_tcg_max_burst_set_unsafe(unit,
                                                    core, 
                                                    tm_port,  
                                                    tcg_ndx,   
                                                    max_burst);

  SOCDNX_IF_ERR_EXIT(res);


  
exit:
  SOCDNX_FUNC_RETURN
}

uint32
    arad_ofp_rates_sch_tcg_max_burst_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core,
    SOC_SAND_IN  uint32                tm_port,   
    SOC_SAND_IN  uint32                tcg_ndx, 
    SOC_SAND_IN  uint32                max_burst
    )
{
  uint32
    res = SOC_SAND_OK;

  SOCDNX_INIT_FUNC_DEFS
  ;

  res = arad_ofp_rates_tcg_shaper_verify(unit,
                                         core,
                                         tm_port,
                                         tcg_ndx,
                                         max_burst,
                                         ARAD_OFP_RATES_GENERIC_FUNC_STATE_TCG_SCH_BURST);
  SOCDNX_IF_ERR_EXIT(res);

  

  res = arad_ofp_rates_sch_tcg_max_burst_set_unsafe(unit,
                                                    core,
                                                    tm_port,  
                                                    tcg_ndx,   
                                                    max_burst);

  SOCDNX_IF_ERR_EXIT(res);


  
exit:
  SOCDNX_FUNC_RETURN
}

uint32
    arad_ofp_rates_egq_tcg_rate_sw_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core,
    SOC_SAND_IN  uint32                tm_port,   
    SOC_SAND_IN  uint32                tcg_ndx, 
    SOC_SAND_IN  uint32                rate
    )
{
  uint32
    res = SOC_SAND_OK;

  SOCDNX_INIT_FUNC_DEFS
  ;

  res = arad_ofp_rates_tcg_shaper_verify(unit,
                                         core,
                                         tm_port,
                                         tcg_ndx,
                                         rate,
                                         ARAD_OFP_RATES_GENERIC_FUNC_STATE_TCG_EGQ_RATE);
  SOCDNX_IF_ERR_EXIT(res);

  

  res = arad_ofp_rates_egq_tcg_rate_sw_set_unsafe(unit,
                                                  core, 
                                                  tm_port,  
                                                  tcg_ndx,   
                                                  rate);

  SOCDNX_IF_ERR_EXIT(res);


  
exit:
  SOCDNX_FUNC_RETURN
}

uint32
    arad_ofp_rates_egq_tcg_rate_hw_set(
    SOC_SAND_IN  int                   unit
    )
{
  uint32
    res = SOC_SAND_OK;
  int
    core;

  SOCDNX_INIT_FUNC_DEFS
  ;

  
  SOC_DPP_CORES_ITER(SOC_CORE_ALL, core) {
      res = arad_ofp_rates_egq_tcg_rate_hw_set_unsafe(unit, core);
  }

  SOCDNX_IF_ERR_EXIT(res);


  
exit:
  SOCDNX_FUNC_RETURN
}

uint32
    arad_ofp_rates_sch_tcg_rate_set(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  int                core,
    SOC_SAND_IN  uint32             tm_port,   
    SOC_SAND_IN  uint32             tcg_ndx, 
    SOC_SAND_IN  uint32             rate
    )
{
  uint32
    res = SOC_SAND_OK;

  SOCDNX_INIT_FUNC_DEFS
  ;

  res = arad_ofp_rates_tcg_shaper_verify(unit,
                                         core,
                                         tm_port,
                                         tcg_ndx,
                                         rate,
                                         ARAD_OFP_RATES_GENERIC_FUNC_STATE_TCG_SCH_RATE);
  SOCDNX_IF_ERR_EXIT(res);

  

  res = arad_ofp_rates_sch_tcg_rate_set_unsafe(unit,
                                               core, 
                                               tm_port,  
                                               tcg_ndx,   
                                               rate);

  SOCDNX_IF_ERR_EXIT(res);


  
exit:
  SOCDNX_FUNC_RETURN
}

uint32
    arad_ofp_rates_egq_port_priority_max_burst_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core,
    SOC_SAND_IN  uint32                tm_port,    
    SOC_SAND_IN  uint32                port_priority_ndx, 
    SOC_SAND_IN  uint32                max_burst
    )
{
  uint32
    res = SOC_SAND_OK;

  SOCDNX_INIT_FUNC_DEFS
  ;

  res = arad_ofp_rates_port_priority_shaper_verify(
                                         unit,
                                         core,
                                         tm_port,
                                         port_priority_ndx,
                                         max_burst,
                                         ARAD_OFP_RATES_GENERIC_FUNC_STATE_PTC_EGQ_BURST);
  SOCDNX_IF_ERR_EXIT(res);

  

  res = arad_ofp_rates_egq_port_priority_max_burst_set_unsafe(
                                                    unit,
                                                    core, 
                                                    tm_port,  
                                                    port_priority_ndx,   
                                                    max_burst);

  SOCDNX_IF_ERR_EXIT(res);


  
exit:
  SOCDNX_FUNC_RETURN
}

uint32
    arad_ofp_rates_sch_port_priority_max_burst_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core,
    SOC_SAND_IN  uint32                tm_port,   
    SOC_SAND_IN  uint32                port_priority_ndx, 
    SOC_SAND_IN  uint32                max_burst
    )
{
  uint32
    res = SOC_SAND_OK;

  SOCDNX_INIT_FUNC_DEFS
  ;

  res = arad_ofp_rates_port_priority_shaper_verify(unit,
                                         core,
                                         tm_port,
                                         port_priority_ndx,
                                         max_burst,
                                         ARAD_OFP_RATES_GENERIC_FUNC_STATE_PTC_SCH_BURST);
  SOCDNX_IF_ERR_EXIT(res);

  

  res = arad_ofp_rates_sch_port_priority_max_burst_set_unsafe(unit,
                                                    core,           
                                                    tm_port,  
                                                    port_priority_ndx,   
                                                    max_burst);

  SOCDNX_IF_ERR_EXIT(res);


  
exit:
  SOCDNX_FUNC_RETURN
}

uint32
    arad_ofp_rates_egq_port_priority_rate_sw_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core,
    SOC_SAND_IN  uint32                tm_port,   
    SOC_SAND_IN  uint32                port_priority_ndx, 
    SOC_SAND_IN  uint32                rate
    )
{
  uint32
    res = SOC_SAND_OK;

  SOCDNX_INIT_FUNC_DEFS
  ;

  res = arad_ofp_rates_port_priority_shaper_verify(unit,
                                         core,
                                         tm_port,
                                         port_priority_ndx,
                                         rate,
                                         ARAD_OFP_RATES_GENERIC_FUNC_STATE_PTC_EGQ_RATE);
  SOCDNX_IF_ERR_EXIT(res);

  

  res = arad_ofp_rates_egq_port_priority_rate_sw_set_unsafe(unit,
                                                            core, 
                                                            tm_port,  
                                                            port_priority_ndx,   
                                                            rate);

  SOCDNX_IF_ERR_EXIT(res);


  
exit:
  SOCDNX_FUNC_RETURN
}


uint32
    arad_ofp_rates_egq_port_priority_rate_hw_set(
    SOC_SAND_IN  int                   unit
    )
{
  uint32
    res = SOC_SAND_OK;
  int core;

  SOCDNX_INIT_FUNC_DEFS
  ;

  
  SOC_DPP_CORES_ITER(SOC_CORE_ALL, core) {
      res = arad_ofp_rates_egq_port_priority_rate_hw_set_unsafe(unit, core);
      SOCDNX_IF_ERR_EXIT(res);
  }


  
exit:
  SOCDNX_FUNC_RETURN
}


uint32
    arad_ofp_rates_sch_port_priority_rate_set(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  int                core,
    SOC_SAND_IN  uint32             tm_port,   
    SOC_SAND_IN  uint32             port_priority_ndx, 
    SOC_SAND_IN  uint32             rate
    )
{
  uint32
    res = SOC_SAND_OK;

  SOCDNX_INIT_FUNC_DEFS
  ;

  res = arad_ofp_rates_port_priority_shaper_verify(unit,
                                         core,
                                         tm_port,
                                         port_priority_ndx,
                                         rate,
                                         ARAD_OFP_RATES_GENERIC_FUNC_STATE_PTC_SCH_RATE);
  SOCDNX_IF_ERR_EXIT(res);

  

  res = arad_ofp_rates_sch_port_priority_rate_set_unsafe(unit, 
                                                         core,
                                                         tm_port,  
                                                         port_priority_ndx,   
                                                         rate);

  SOCDNX_IF_ERR_EXIT(res);


  
exit:
  SOCDNX_FUNC_RETURN
}

uint32
    arad_ofp_rates_sch_port_priority_rate_sw_set(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  int                core,
    SOC_SAND_IN  uint32             tm_port,   
    SOC_SAND_IN  uint32             port_priority_ndx, 
    SOC_SAND_IN  uint32             rate
    )
{
  uint32
    res = SOC_SAND_OK;

  SOCDNX_INIT_FUNC_DEFS
  ;

  res = arad_ofp_rates_port_priority_shaper_verify(unit,
                                         core,
                                         tm_port,
                                         port_priority_ndx,
                                         rate,
                                         ARAD_OFP_RATES_GENERIC_FUNC_STATE_PTC_SCH_RATE);
  SOCDNX_IF_ERR_EXIT(res);

  

  res = arad_ofp_rates_sch_port_priority_rate_sw_set_unsafe(unit, 
                                                         core,
                                                         tm_port,  
                                                         port_priority_ndx,   
                                                         rate);

  SOCDNX_IF_ERR_EXIT(res);


  
exit:
  SOCDNX_FUNC_RETURN
}

uint32
    arad_ofp_rates_sch_port_priority_hw_set(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  int                core
    )
{
  uint32
    res = SOC_SAND_OK;
  SOCDNX_INIT_FUNC_DEFS;


  res = arad_sch_port_priority_shaper_hw_set_unsafe(unit, core);
  SOCDNX_IF_ERR_EXIT(res);

exit:
  SOCDNX_FUNC_RETURN
}




uint32
    arad_ofp_rates_egq_tcg_max_burst_get(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  int                core,
    SOC_SAND_IN  uint32             tm_port,  
    SOC_SAND_IN  uint32             tcg_ndx, 
    SOC_SAND_OUT  uint32            *max_burst
    )
{
  uint32
    res = SOC_SAND_OK;

  SOCDNX_INIT_FUNC_DEFS
  ;

  SOCDNX_NULL_CHECK(max_burst);

  

  res = arad_ofp_rates_egq_tcg_max_burst_get_unsafe(unit,
                                                    core, 
                                                    tm_port,  
                                                    tcg_ndx,   
                                                    max_burst);

  SOCDNX_IF_ERR_EXIT(res);


  
exit:
  SOCDNX_FUNC_RETURN
}

uint32
    arad_ofp_rates_sch_tcg_max_burst_get(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  int                core,
    SOC_SAND_IN  uint32             tm_port,   
    SOC_SAND_IN  uint32             tcg_ndx, 
    SOC_SAND_OUT uint32             *max_burst
    )
{
  uint32
    res = SOC_SAND_OK;

  SOCDNX_INIT_FUNC_DEFS
  ;

  SOCDNX_NULL_CHECK(max_burst);

  

  res = arad_ofp_rates_sch_tcg_max_burst_get_unsafe(unit,
                                                    core, 
                                                    tm_port,  
                                                    tcg_ndx,   
                                                    max_burst);

  SOCDNX_IF_ERR_EXIT(res);


  
exit:
  SOCDNX_FUNC_RETURN
}

uint32
    arad_ofp_rates_egq_tcg_rate_hw_get(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  int                core,
    SOC_SAND_IN  uint32             tm_port,   
    SOC_SAND_IN  uint32             tcg_ndx, 
    SOC_SAND_OUT uint32             *rate
    )
{
  uint32
    res = SOC_SAND_OK;

  SOCDNX_INIT_FUNC_DEFS
  ;

  SOCDNX_NULL_CHECK(rate);

  

  res = arad_ofp_rates_egq_tcg_rate_hw_get_unsafe(unit,
                                                  core, 
                                                  tm_port,  
                                                  tcg_ndx,   
                                                  rate);

  SOCDNX_IF_ERR_EXIT(res);


  
exit:
  SOCDNX_FUNC_RETURN
}

uint32
    arad_ofp_rates_sch_tcg_rate_get(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  int                core,
    SOC_SAND_IN  uint32             tm_port,   
    SOC_SAND_IN  uint32             tcg_ndx, 
    SOC_SAND_OUT uint32             *rate
    )
{
  uint32
    res = SOC_SAND_OK;

  SOCDNX_INIT_FUNC_DEFS
  ;

  SOCDNX_NULL_CHECK(rate);

  SOCDNX_IF_ERR_EXIT(res);

  

  res = arad_ofp_rates_sch_tcg_rate_get_unsafe(unit,
                                               core,
                                               tm_port,  
                                               tcg_ndx,   
                                               rate);

  SOCDNX_IF_ERR_EXIT(res);


  
exit:
  SOCDNX_FUNC_RETURN
}

uint32
    arad_ofp_rates_egq_port_priority_max_burst_get(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  int                core,
    SOC_SAND_IN  uint32             tm_port,   
    SOC_SAND_IN  uint32             port_priority_ndx, 
    SOC_SAND_OUT  uint32            *max_burst
    )
{
  uint32
    res = SOC_SAND_OK;

  SOCDNX_INIT_FUNC_DEFS
  ;

  SOCDNX_NULL_CHECK(max_burst);

  

  res = arad_ofp_rates_egq_port_priority_max_burst_get_unsafe(unit,
                                                    core,
                                                    tm_port,  
                                                    port_priority_ndx,   
                                                    max_burst);

  SOCDNX_IF_ERR_EXIT(res);


  
exit:
  SOCDNX_FUNC_RETURN
}



uint32
    arad_ofp_rates_sch_port_priority_max_burst_get(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  int                core,
    SOC_SAND_IN  uint32             tm_port,   
    SOC_SAND_IN  uint32             port_priority_ndx, 
    SOC_SAND_OUT uint32             *max_burst
    )
{
  uint32
    res = SOC_SAND_OK;

  SOCDNX_INIT_FUNC_DEFS
  ;
  SOCDNX_NULL_CHECK(max_burst);
  
  

  res = arad_ofp_rates_sch_port_priority_max_burst_get_unsafe(unit,
                                                    core, 
                                                    tm_port,  
                                                    port_priority_ndx,   
                                                    max_burst);

  SOCDNX_IF_ERR_EXIT(res);


  
exit:
  SOCDNX_FUNC_RETURN
}

uint32
    arad_ofp_rates_egq_port_priority_rate_hw_get(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  int                core,
    SOC_SAND_IN  uint32             tm_port,   
    SOC_SAND_IN  uint32             port_priority_ndx, 
    SOC_SAND_OUT uint32             *rate
    )
{
  uint32
    res = SOC_SAND_OK;

  SOCDNX_INIT_FUNC_DEFS
  ;

  SOCDNX_NULL_CHECK(rate);

  

  res = arad_ofp_rates_egq_port_priority_rate_hw_get_unsafe(unit,
                                                    core, 
                                                    tm_port,  
                                                    port_priority_ndx,   
                                                    rate);

  SOCDNX_IF_ERR_EXIT(res);


  
exit:
  SOCDNX_FUNC_RETURN
}



uint32
    arad_ofp_rates_sch_port_priority_rate_get(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  int                core,
    SOC_SAND_IN  uint32             tm_port,   
    SOC_SAND_IN  uint32             port_priority_ndx, 
    SOC_SAND_OUT uint32             *rate
    )
{
  uint32
    res = SOC_SAND_OK;

  SOCDNX_INIT_FUNC_DEFS
  ;
  SOCDNX_NULL_CHECK(rate);

  

  res = arad_ofp_rates_sch_port_priority_rate_get_unsafe(unit, 
                                                         core,
                                                         tm_port,  
                                                         port_priority_ndx,   
                                                         rate);

  SOCDNX_IF_ERR_EXIT(res);


  
exit:
  SOCDNX_FUNC_RETURN
}


void
  arad_ARAD_OFP_RATES_INTERFACE_SHPR_INFO_clear(
    SOC_SAND_OUT ARAD_OFP_RATES_INTERFACE_SHPR_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_OFP_RATES_INTERFACE_SHPR_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_OFP_RATE_INFO_clear(
    SOC_SAND_OUT ARAD_OFP_RATE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_OFP_RATE_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if ARAD_DEBUG_IS_LVL1

const char*
  arad_ARAD_OFP_RATES_CAL_SET_to_string(
    SOC_SAND_IN  ARAD_OFP_RATES_CAL_SET enum_val
  )
{
  return SOC_TMC_OFP_RATES_CAL_SET_to_string(enum_val);
}

const char*
  arad_ARAD_OFP_SHPR_UPDATE_MODE_to_string(
    SOC_SAND_IN  ARAD_OFP_SHPR_UPDATE_MODE enum_val
  )
{
  return SOC_TMC_OFP_SHPR_UPDATE_MODE_to_string(enum_val);
}

void
  arad_ARAD_OFP_RATES_INTERFACE_SHPR_INFO_print(
    SOC_SAND_IN  ARAD_OFP_RATES_INTERFACE_SHPR_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_OFP_RATES_INTERFACE_SHPR_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_OFP_RATE_INFO_print(
    SOC_SAND_IN  ARAD_OFP_RATE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_OFP_RATE_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif 

