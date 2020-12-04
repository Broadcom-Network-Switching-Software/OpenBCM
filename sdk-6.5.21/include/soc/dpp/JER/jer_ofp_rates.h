/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: jer_ofp_rates.h
 */

#ifndef __JER_OFP_RATES_INCLUDED__

#define __JER_OFP_RATES_INCLUDED__


#include <soc/dpp/ARAD/arad_api_ofp_rates.h>




#define JERICHO_OFP_RATES_EGRESS_SHAPER_CONFIG_CAL_ID_NON_CHN     32      
#define JERICHO_OFP_RATES_EGRESS_SHAPER_CONFIG_CAL_ID_QUEUE_PAIR  33
#define JERICHO_OFP_RATES_EGRESS_SHAPER_CONFIG_CAL_ID_TCG         34
#define JERICHO_OFP_RATES_EGRESS_SHAPER_CONFIG_NUM_OF_CALS        35           




int
soc_jer_ofp_rates_init(
    SOC_SAND_IN  int                         unit
  );

int
soc_jer_ofp_rates_egq_interface_shaper_set(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  int                          core,
    SOC_SAND_IN  uint32                       tm_port,
    SOC_SAND_IN  SOC_TMC_OFP_SHPR_UPDATE_MODE rate_update_mode,
    SOC_SAND_IN  uint32                       if_shaper_rate
  );


int
soc_jer_ofp_rates_egq_interface_shaper_get(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  int                       core,
    SOC_SAND_IN  uint32                    tm_port,
    SOC_SAND_OUT uint32                    *if_shaper_rate
  );

int
soc_jer_ofp_rates_interface_internal_rate_get(
    SOC_SAND_IN   int                   unit, 
    SOC_SAND_IN   int                   core, 
    SOC_SAND_IN   uint32                egr_if_id, 
    SOC_SAND_OUT  uint32                *internal_rate);

int
soc_jer_ofp_rates_egq_single_port_rate_sw_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core,
    SOC_SAND_IN  uint32                tm_port,
    SOC_SAND_IN  uint32                rate
    );

int soc_jer_ofp_rates_calendar_allocate(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  int             core,
    SOC_SAND_IN  uint32          tm_port,
    SOC_SAND_IN  uint32          num_required_slots,
    SOC_SAND_IN  uint32          old_cal,
    SOC_SAND_OUT uint32          *new_cal);

int
soc_jer_ofp_rates_calendar_deallocate(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  int             core,
    SOC_SAND_IN  uint32          cal_id);

int
soc_jer_ofp_rates_port2chan_cal_get(
    SOC_SAND_IN  int        unit, 
    SOC_SAND_IN  int        core, 
    SOC_SAND_IN  uint32     tm_port, 
    SOC_SAND_OUT uint32     *calendar);

int
soc_jer_ofp_rates_retrieve_egress_shaper_reg_field_names(
    SOC_SAND_IN  int                                      unit,
    SOC_SAND_IN  ARAD_OFP_RATES_CAL_INFO                  *cal_info,
    SOC_SAND_IN  ARAD_OFP_RATES_CAL_SET                   cal2set,    
    SOC_SAND_IN  ARAD_OFP_RATES_EGQ_CHAN_ARB_FIELD_TYPE   field_type,
    SOC_SAND_OUT soc_mem_t                                *memory_name,
    SOC_SAND_OUT soc_field_t                              *field_name
  );

int 
soc_jer_ofp_rates_egress_shaper_mem_field_read (
    SOC_SAND_IN  int                                      unit,
    SOC_SAND_IN  int                                      core,
    SOC_SAND_IN  ARAD_OFP_RATES_CAL_INFO                  *cal_info,   
    SOC_SAND_IN  soc_reg_t                                register_name,
    SOC_SAND_IN  soc_field_t                              field_name,
    SOC_SAND_OUT uint32                                   *data
    );

int
soc_jer_ofp_rates_egress_shaper_mem_field_write (
    SOC_SAND_IN  int                                      unit,
    SOC_SAND_IN  int                                      core,
    SOC_SAND_IN  ARAD_OFP_RATES_CAL_INFO                  *cal_info,   
    SOC_SAND_IN  soc_mem_t                                mem_name,
    SOC_SAND_IN  soc_field_t                              field_name,
    SOC_SAND_OUT uint32                                   data
    );

soc_mem_t
  soc_jer_ofp_rates_egq_scm_chan_arb_id2scm_id(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint32 chan_arb_id
  );


int
soc_jer_ofp_rates_egress_shaper_cal_write (
    SOC_SAND_IN  int                                      unit,
    SOC_SAND_IN  int                                      core,
    SOC_SAND_IN  SOC_TMC_OFP_RATES_CAL_INFO              *cal_info,
    SOC_SAND_IN  SOC_TMC_OFP_RATES_CAL_SET                cal2set,    
    SOC_SAND_IN  SOC_TMC_OFP_RATES_EGQ_CHAN_ARB_FIELD_TYPE field_type,
    SOC_SAND_IN  uint32                                   data
    );

int
soc_jer_ofp_rates_egress_shaper_cal_read (
    SOC_SAND_IN  int                                      unit,
    SOC_SAND_IN  int                                      core,
    SOC_SAND_IN  SOC_TMC_OFP_RATES_CAL_INFO              *cal_info,
    SOC_SAND_IN  SOC_TMC_OFP_RATES_CAL_SET                cal2set,    
    SOC_SAND_IN  SOC_TMC_OFP_RATES_EGQ_CHAN_ARB_FIELD_TYPE field_type,
    SOC_SAND_OUT uint32                                   *data
    );
#endif 

