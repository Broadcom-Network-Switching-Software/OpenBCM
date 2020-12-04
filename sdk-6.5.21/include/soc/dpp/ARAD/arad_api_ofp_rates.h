/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __ARAD_API_OFP_RATES_INCLUDED__

#define __ARAD_API_OFP_RATES_INCLUDED__



#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/ARAD/arad_api_general.h>
#include <soc/dpp/TMC/tmc_api_ofp_rates.h>
#include <soc/dpp/port_sw_db.h>





#define ARAD_OFP_RATES_SCH_BURST_LIMIT_MAX          (SOC_TMC_OFP_RATES_SCH_BURST_LIMIT_MAX)
#define ARAD_OFP_RATES_BURST_LIMIT_MAX              (SOC_TMC_OFP_RATES_BURST_LIMIT_MAX)
#define ARAD_OFP_RATES_BURST_DEAULT                 (SOC_TMC_OFP_RATES_BURST_DEFAULT)
#define ARAD_OFP_RATES_EMPTY_Q_BURST_LIMIT_MAX      (SOC_TMC_OFP_RATES_BURST_EMPTY_Q_LIMIT_MAX)
#define ARAD_OFP_RATES_FC_Q_BURST_LIMIT_MAX         (SOC_TMC_OFP_RATES_BURST_FC_Q_LIMIT_MAX)     
#define ARAD_OFP_RATES_CHNIF_BURST_LIMIT_MAX        (0xFFC0)

#define ARAD_OFP_RATES_ILLEGAL_PORT_ID              (ARAD_NOF_FAP_PORTS)











#define ARAD_OFP_RATES_CAL_SET_A                          SOC_TMC_OFP_RATES_CAL_SET_A
#define ARAD_OFP_RATES_CAL_SET_B                          SOC_TMC_OFP_RATES_CAL_SET_B
#define ARAD_OFP_NOF_RATES_CAL_SETS                       SOC_TMC_OFP_NOF_RATES_CAL_SETS
typedef SOC_TMC_OFP_RATES_CAL_SET                         ARAD_OFP_RATES_CAL_SET;
typedef SOC_TMC_OFP_RATES_EGQ_CHAN_ARB_FIELD_TYPE         ARAD_OFP_RATES_EGQ_CHAN_ARB_FIELD_TYPE;

#define ARAD_OFP_RATES_EGQ_CAL_CHAN_ARB                   SOC_TMC_OFP_RATES_EGQ_CAL_CHAN_ARB
#define ARAD_OFP_RATES_EGQ_CAL_TCG                        SOC_TMC_OFP_RATES_EGQ_CAL_TCG
#define ARAD_OFP_RATES_EGQ_CAL_PORT_PRIORITY              SOC_TMC_OFP_RATES_EGQ_CAL_PORT_PRIORITY
typedef SOC_TMC_OFP_RATES_EGQ_CAL_TYPE                    ARAD_OFP_RATES_EGQ_CAL_TYPE;
typedef SOC_TMC_OFP_RATES_CAL_INFO                        ARAD_OFP_RATES_CAL_INFO;

#define ARAD_OFP_SHPR_UPDATE_MODE_SUM_OF_PORTS            SOC_TMC_OFP_SHPR_UPDATE_MODE_SUM_OF_PORTS
#define ARAD_OFP_SHPR_UPDATE_MODE_OVERRIDE                SOC_TMC_OFP_SHPR_UPDATE_MODE_OVERRIDE
#define ARAD_OFP_SHPR_UPDATE_MODE_DONT_TUCH               SOC_TMC_OFP_SHPR_UPDATE_MODE_DONT_TUCH
#define ARAD_OFP_NOF_SHPR_UPDATE_MODES                    SOC_TMC_OFP_NOF_SHPR_UPDATE_MODES
typedef SOC_TMC_OFP_SHPR_UPDATE_MODE                           ARAD_OFP_SHPR_UPDATE_MODE;

typedef SOC_TMC_OFP_RATES_INTERFACE_SHPR_INFO                  ARAD_OFP_RATES_INTERFACE_SHPR_INFO;
typedef SOC_TMC_OFP_RATE_INFO                                  ARAD_OFP_RATE_INFO;

typedef enum
{
  ARAD_OFP_RATES_EGQ_CHAN_ARB_00   =  0,
  ARAD_OFP_RATES_EGQ_CHAN_ARB_01   =  1,
  ARAD_OFP_RATES_EGQ_CHAN_ARB_02   =  2,
  ARAD_OFP_RATES_EGQ_CHAN_ARB_03   =  3,
  ARAD_OFP_RATES_EGQ_CHAN_ARB_04   =  4,
  ARAD_OFP_RATES_EGQ_CHAN_ARB_05   =  5,
  ARAD_OFP_RATES_EGQ_CHAN_ARB_06   =  6,
  ARAD_OFP_RATES_EGQ_CHAN_ARB_07   =  7,
  ARAD_OFP_RATES_EGQ_CHAN_ARB_CPU  = 8,
  ARAD_OFP_RATES_EGQ_CHAN_ARB_08 = 8,
  ARAD_OFP_RATES_EGQ_CHAN_ARB_RCY  = 9,
  ARAD_OFP_RATES_EGQ_CHAN_ARB_09 = 9,
  ARAD_OFP_RATES_EGQ_CHAN_ARB_NON_CHAN = 10,
  ARAD_OFP_RATES_EGQ_NOF_CHAN_ARB = 11,
  ARAD_OFP_RATES_EGQ_CHAN_ARB_INVALID = INVALID_CALENDAR
}ARAD_OFP_RATES_EGQ_CHAN_ARB_ID;












uint32
    arad_ofp_rates_port2chan_arb(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                fap_port,
    SOC_SAND_OUT  ARAD_OFP_RATES_EGQ_CHAN_ARB_ID *chan_arb_id
    );

uint32
    arad_ofp_rates_port_priority_max_burst_for_fc_queues_set(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32 max_burst_fc_queues
    );
uint32
    arad_ofp_rates_port_priority_max_burst_for_fc_queues_get(
    SOC_SAND_IN  int    unit,
    SOC_SAND_OUT  uint32 *max_burst_fc_queues
    );
uint32
    arad_ofp_rates_port_priority_max_burst_for_empty_queues_set(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32 max_burst_empty_queues
    );
uint32
    arad_ofp_rates_port_priority_max_burst_for_empty_queues_get(
    SOC_SAND_IN  int    unit,
    SOC_SAND_OUT  uint32 *max_burst_empty_queues
    );

int
  arad_ofp_rates_egq_single_port_rate_sw_set(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                    core,
    SOC_SAND_IN  uint32                 tm_port,
    SOC_SAND_IN  uint32                 rate
  );
int
  arad_ofp_rates_egq_single_port_rate_hw_set(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                    core,
    SOC_SAND_IN  uint32                 tm_port,
    SOC_SAND_IN  uint32                 is_alt_calc
  );
int
  arad_ofp_rates_sch_single_port_rate_hw_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32              tm_port,
    SOC_SAND_IN  uint32              is_alt_calc

  );
int
  arad_ofp_rates_egq_single_port_rate_hw_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                    core,
    SOC_SAND_IN  uint32                 tm_port,
    SOC_SAND_OUT uint32                 *rate
  );

int
  arad_ofp_rates_single_port_max_burst_set(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                    core,
    SOC_SAND_IN  uint32                 tm_port,
    SOC_SAND_IN  uint32                 max_burst
  );
int
  arad_ofp_rates_single_port_max_burst_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                    core,
    SOC_SAND_IN  uint32                 tm_port,
    SOC_SAND_OUT uint32                 *max_burst
  );

int
  arad_ofp_rates_egq_interface_shaper_set(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                    core,
    SOC_SAND_IN  uint32                 ofp_ndx,
    SOC_SAND_IN SOC_TMC_OFP_SHPR_UPDATE_MODE rate_update_mode,
    SOC_SAND_IN  uint32                 if_shaper_rate
  );
int
  arad_ofp_rates_egq_interface_shaper_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                    core,
    SOC_SAND_IN  uint32                 tm_port,
    SOC_SAND_OUT  uint32                *if_shaper_rate
  );

uint32
    arad_ofp_rates_egq_tcg_max_burst_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core,
    SOC_SAND_IN  uint32                tm_port,   
    SOC_SAND_IN  uint32                tcg_ndx, 
    SOC_SAND_IN  uint32                max_burst
    );
uint32
    arad_ofp_rates_egq_tcg_max_burst_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core,
    SOC_SAND_IN  uint32                tm_port,   
    SOC_SAND_IN  uint32                tcg_ndx, 
    SOC_SAND_OUT  uint32               *max_burst
    );
uint32
    arad_ofp_rates_sch_tcg_max_burst_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core,
    SOC_SAND_IN  uint32                tm_port,   
    SOC_SAND_IN  uint32                tcg_ndx, 
    SOC_SAND_IN  uint32                max_burst
    );
uint32
    arad_ofp_rates_sch_tcg_max_burst_get(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  int                core,
    SOC_SAND_IN  uint32             tm_port,   
    SOC_SAND_IN  uint32             tcg_ndx, 
    SOC_SAND_OUT uint32             *max_burst
    );

uint32
    arad_ofp_rates_egq_tcg_rate_sw_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core,
    SOC_SAND_IN  uint32                tm_port,   
    SOC_SAND_IN  uint32                tcg_ndx, 
    SOC_SAND_IN  uint32                rate
    );
uint32
    arad_ofp_rates_egq_tcg_rate_hw_set(
    SOC_SAND_IN  int                   unit
    );
uint32
    arad_ofp_rates_egq_tcg_rate_hw_get(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  int                core,
    SOC_SAND_IN  uint32             tm_port,   
    SOC_SAND_IN  uint32             tcg_ndx, 
    SOC_SAND_OUT uint32             *rate
    );

uint32
    arad_ofp_rates_sch_tcg_rate_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core,
    SOC_SAND_IN  uint32                tm_port,   
    SOC_SAND_IN  uint32                tcg_ndx, 
    SOC_SAND_IN  uint32                rate
    );
uint32
    arad_ofp_rates_sch_tcg_rate_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core,
    SOC_SAND_IN  uint32                tm_port,   
    SOC_SAND_IN  uint32                tcg_ndx, 
    SOC_SAND_OUT uint32                *rate
    );


uint32
    arad_ofp_rates_egq_port_priority_rate_sw_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core,
    SOC_SAND_IN  uint32                tm_port,   
    SOC_SAND_IN  uint32                port_priority_ndx, 
    SOC_SAND_IN  uint32                rate
    );
uint32
    arad_ofp_rates_egq_port_priority_rate_hw_set(
    SOC_SAND_IN  int                   unit
    );
uint32
    arad_ofp_rates_egq_port_priority_rate_hw_get(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  int                core,
    SOC_SAND_IN  uint32             tm_port,   
    SOC_SAND_IN  uint32             port_priority_ndx, 
    SOC_SAND_OUT uint32             *rate
    );
uint32
    arad_ofp_rates_sch_port_priority_rate_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core,
    SOC_SAND_IN  uint32                tm_port,   
    SOC_SAND_IN  uint32                port_priority_ndx, 
    SOC_SAND_IN  uint32                rate
    );

uint32
    arad_ofp_rates_sch_port_priority_rate_sw_set(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  int                core,
    SOC_SAND_IN  uint32             tm_port,   
    SOC_SAND_IN  uint32             port_priority_ndx, 
    SOC_SAND_IN  uint32             rate
    );

uint32
    arad_ofp_rates_sch_port_priority_hw_set(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  int                core
    );

uint32
    arad_ofp_rates_sch_port_priority_rate_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core,
    SOC_SAND_IN  uint32                tm_port,   
    SOC_SAND_IN  uint32                port_priority_ndx, 
    SOC_SAND_OUT uint32                *rate
    );

uint32
    arad_ofp_rates_egq_port_priority_max_burst_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core,
    SOC_SAND_IN  uint32                tm_port,   
    SOC_SAND_IN  uint32                port_priority_ndx, 
    SOC_SAND_IN  uint32                max_burst
    );
uint32
    arad_ofp_rates_egq_port_priority_max_burst_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core,
    SOC_SAND_IN  uint32                tm_port, 
    SOC_SAND_IN  uint32                port_priority_ndx, 
    SOC_SAND_OUT  uint32               *max_burst
    );
uint32
    arad_ofp_rates_sch_port_priority_max_burst_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core,
    SOC_SAND_IN  uint32                tm_port,   
    SOC_SAND_IN  uint32                port_priority_ndx, 
    SOC_SAND_IN  uint32                max_burst
    );
uint32
    arad_ofp_rates_sch_port_priority_max_burst_get(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  int                core,
    SOC_SAND_IN  uint32             tm_port,   
    SOC_SAND_IN  uint32             port_priority_ndx, 
    SOC_SAND_OUT uint32             *max_burst
    );

void
  arad_ARAD_OFP_RATES_INTERFACE_SHPR_INFO_clear(
    SOC_SAND_OUT ARAD_OFP_RATES_INTERFACE_SHPR_INFO *info
  );

void
  arad_ARAD_OFP_RATE_INFO_clear(
    SOC_SAND_OUT ARAD_OFP_RATE_INFO *info
  );

#if ARAD_DEBUG_IS_LVL1

const char*
  arad_ARAD_OFP_RATES_CAL_SET_to_string(
    SOC_SAND_IN ARAD_OFP_RATES_CAL_SET enum_val
  );

const char*
  arad_ARAD_OFP_SHPR_UPDATE_MODE_to_string(
    SOC_SAND_IN ARAD_OFP_SHPR_UPDATE_MODE enum_val
  );

void
  arad_ARAD_OFP_RATES_INTERFACE_SHPR_INFO_print(
    SOC_SAND_IN ARAD_OFP_RATES_INTERFACE_SHPR_INFO *info
  );

void
  arad_ARAD_OFP_RATE_INFO_print(
    SOC_SAND_IN ARAD_OFP_RATE_INFO *info
  );

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif


