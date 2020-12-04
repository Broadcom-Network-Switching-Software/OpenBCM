/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __ARAD_OFP_RATES_INCLUDED__

#define __ARAD_OFP_RATES_INCLUDED__



#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/ARAD/arad_nif.h>
#include <soc/dpp/ARAD/arad_api_ofp_rates.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>






#define ARAD_OFP_RATES_CAL_LEN_EGQ_SMALL_MAX    SOC_DPP_DEFS_MAX(SMALL_CHANNELIZED_CAL_SIZE)


#define ARAD_OFP_RATES_CAL_LEN_EGQ_BIG_MAX      SOC_DPP_DEFS_MAX(BIG_CHANNELIZED_CAL_SIZE)

#define ARAD_OFP_RATES_CAL_LEN_EGQ_MAX          (SOC_SAND_MAX(ARAD_OFP_RATES_CAL_LEN_EGQ_SMALL_MAX,ARAD_OFP_RATES_CAL_LEN_EGQ_BIG_MAX))

#define ARAD_OFP_RATES_CAL_LEN_EGQ_PORT_PRIO_MAX 0x0100
#define ARAD_OFP_RATES_CAL_LEN_EGQ_TCG_MAX       0x0100

#define ARAD_OFP_RATES_CALCAL_LEN_EGQ_MAX        0x0080
#define ARAD_OFP_RATES_CAL_LEN_SCH_MAX           0x0400

#define ARAD_OFP_RATES_EGRESS_SHAPER_MAX_INTERNAL_CAL_BURST      0x1fff
#define ARAD_OFP_RATES_EGRESS_SHAPER_MAX_INTERNAL_IF_RATE        0xffff
#define ARAD_OFP_RATES_EGRESS_SHAPER_MAX_INTERNAL_IF_BURST       0x4000 

#define ARAD_OFP_RATES_CHAN_ARB_NOF_BITS  (4)
#define ARAD_OFP_RATES_IFC_NOF_BITS (5)

#define ARAD_OFP_RATES_DEFAULT_PACKET_SIZE (64)












typedef struct  {
  uint32 base_q_pair;
  uint32 credit;
}ARAD_OFP_EGQ_RATES_CAL_ENTRY;


typedef uint32 ARAD_OFP_SCH_RATES_CAL_ENTRY;


typedef struct
{
  ARAD_OFP_EGQ_RATES_CAL_ENTRY slots[ARAD_OFP_RATES_CAL_LEN_EGQ_MAX];
}ARAD_OFP_RATES_CAL_EGQ;


typedef struct
{
  ARAD_OFP_SCH_RATES_CAL_ENTRY slots[ARAD_OFP_RATES_CAL_LEN_SCH_MAX];
}ARAD_OFP_RATES_CAL_SCH;

typedef enum
{
    ARAD_OFP_RATES_GENERIC_FUNC_STATE_TCG_SCH_RATE = 0,
    ARAD_OFP_RATES_GENERIC_FUNC_STATE_PTC_SCH_RATE = 1,
    ARAD_OFP_RATES_GENERIC_FUNC_STATE_TCG_SCH_BURST = 2,
    ARAD_OFP_RATES_GENERIC_FUNC_STATE_PTC_SCH_BURST = 3,
    ARAD_OFP_RATES_GENERIC_FUNC_STATE_TCG_EGQ_RATE = 4,
    ARAD_OFP_RATES_GENERIC_FUNC_STATE_PTC_EGQ_RATE = 5,
    ARAD_OFP_RATES_GENERIC_FUNC_STATE_TCG_EGQ_BURST = 6,
    ARAD_OFP_RATES_GENERIC_FUNC_STATE_PTC_EGQ_BURST = 7,
    ARAD_OFP_RATES_GENERIC_FUNC_STATE_PTC_EGQ_EMPTY_Q_BURST = 8,
    ARAD_OFP_RATES_GENERIC_FUNC_STATE_PTC_EGQ_FC_Q_BURST = 9

}ARAD_OFP_RATES_GENERIC_FUNC_STATE;











uint32
  arad_ofp_rates_init(
    SOC_SAND_IN  int                    unit
  );


uint32
  arad_ofp_rates_port_priority_max_burst_for_fc_queues_verify(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                 rate
  );
uint32
  arad_ofp_rates_port_priority_max_burst_for_empty_queues_verify(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                 rate
  );
uint32
  arad_ofp_rates_tcg_shaper_verify(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                    core,
    SOC_SAND_IN  uint32                 tm_port,
    SOC_SAND_IN  ARAD_TCG_NDX           tcg_ndx,
    SOC_SAND_IN  uint32                 rate,
    SOC_SAND_IN  ARAD_OFP_RATES_GENERIC_FUNC_STATE set_state
  );
uint32
  arad_ofp_rates_port_priority_shaper_verify(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                    core,
    SOC_SAND_IN  uint32                 tm_port,
    SOC_SAND_IN  uint32                 priority_ndx,
    SOC_SAND_IN  uint32                 rate,
    SOC_SAND_IN  ARAD_OFP_RATES_GENERIC_FUNC_STATE set_state
  );
int
  arad_ofp_rates_single_port_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                    *rate
  );
uint32
  arad_ofp_rates_interface_shaper_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  int                       core,
    SOC_SAND_IN  uint32                    tm_port

  );

int
    arad_ofp_rates_port2chan_arb_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                fap_port,
    SOC_SAND_OUT  ARAD_OFP_RATES_EGQ_CHAN_ARB_ID *chan_arb_id
    );
int
    arad_ofp_rates_port2chan_cal_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core,
    SOC_SAND_IN  uint32                tm_port,
    SOC_SAND_OUT uint32                *calendar
    );

uint32
    arad_ofp_rates_port_priority_max_burst_for_fc_queues_set_unsafe(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32 max_burst_fc_queues
    );
uint32
    arad_ofp_rates_port_priority_max_burst_for_fc_queues_get_unsafe(
    SOC_SAND_IN  int    unit,
    SOC_SAND_OUT  uint32 *max_burst_fc_queues
    );
uint32
    arad_ofp_rates_port_priority_max_burst_for_empty_queues_set_unsafe(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32 max_burst_empty_queues
    );
uint32
    arad_ofp_rates_port_priority_max_burst_for_empty_queues_get_unsafe(
    SOC_SAND_IN  int    unit,
    SOC_SAND_OUT  uint32 *max_burst_empty_queues
    );

int
    arad_ofp_rates_is_channalized(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core, 
    SOC_SAND_IN  uint32                tm_port, 
    SOC_SAND_OUT uint32                *is_channalzied
    );


int
  arad_ofp_rates_sch_single_port_rate_sw_set(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                    core,
    SOC_SAND_IN  uint32                 tm_port,
    SOC_SAND_IN  uint32                 rate
  );

int
  arad_ofp_rates_sch_single_port_rate_hw_set_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                 is_alt_calc
  );
int
  arad_ofp_rates_sch_single_port_rate_hw_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                    core,
    SOC_SAND_IN  uint32                 tm_port,
    SOC_SAND_OUT uint32                 *rate
  );
int
  arad_ofp_rates_egq_single_port_rate_sw_set_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                    core,
    SOC_SAND_IN  uint32                 tm_port,
    SOC_SAND_IN  uint32                 rate
  );
int
    arad_ofp_rates_egq_single_port_rate_sw_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core,
    SOC_SAND_IN  uint32                tm_port,
    SOC_SAND_OUT uint32                *rate
    );
int
  arad_ofp_rates_egq_single_port_rate_hw_set_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                 is_alt_calc
  );
uint32
  arad_ofp_rates_egq_single_port_rate_hw_get_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                    core,
    SOC_SAND_IN  uint32                 tm_port,
    SOC_SAND_OUT uint32                 *rate
  );

int
  arad_ofp_rates_single_port_max_burst_set_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                    core,
    SOC_SAND_IN  uint32                 tm_port,
    SOC_SAND_IN  uint32                 max_burst
  );
int
  arad_ofp_rates_single_port_max_burst_get_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                    core,
    SOC_SAND_IN  uint32                 tm_port,
    SOC_SAND_OUT uint32                 *max_burst
  );
uint32
  arad_ofp_rates_egq_interface_shaper_set_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                    core,
    SOC_SAND_IN  uint32                 tm_port,
    SOC_SAND_IN SOC_TMC_OFP_SHPR_UPDATE_MODE rate_update_mode,
    SOC_SAND_IN  uint32                 if_shaper_rate
  );
uint32
  arad_ofp_rates_egq_interface_shaper_get_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                    core,
    SOC_SAND_IN  uint32                 tm_port,
    SOC_SAND_OUT  uint32                *if_shaper_rate
  );

uint32
    arad_ofp_rates_egq_tcg_max_burst_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core,
    SOC_SAND_IN  uint32                tm_port,   
    SOC_SAND_IN  uint32                tcg_ndx, 
    SOC_SAND_IN  uint32                max_burst
    );
uint32
    arad_ofp_rates_egq_tcg_max_burst_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core,
    SOC_SAND_IN  uint32                tm_port,     
    SOC_SAND_IN  uint32                tcg_ndx, 
    SOC_SAND_OUT  uint32               *max_burst
    );
uint32
    arad_ofp_rates_sch_tcg_max_burst_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core,
    SOC_SAND_IN  uint32                tm_port,   
    SOC_SAND_IN  uint32                tcg_ndx, 
    SOC_SAND_OUT uint32                max_burst
    );
uint32
    arad_ofp_rates_sch_tcg_max_burst_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core, 
    SOC_SAND_IN  uint32                tm_port, 
    SOC_SAND_IN  ARAD_TCG_NDX          tcg_ndx,   
    SOC_SAND_OUT uint32                *max_burst
    );

int
    arad_ofp_rates_egq_tcg_rate_sw_set_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                    core, 
    SOC_SAND_IN  uint32                 tm_port,  
    SOC_SAND_IN  ARAD_TCG_NDX           tcg_ndx,   
    SOC_SAND_IN  uint32                 tcg_rate  
    );
int
    arad_ofp_rates_egq_tcg_rate_hw_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core
    );
int
    arad_ofp_rates_egq_tcg_rate_hw_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core,
    SOC_SAND_IN  ARAD_FAP_PORT_ID      tm_port, 
    SOC_SAND_IN  ARAD_TCG_NDX          tcg_ndx,   
    SOC_SAND_OUT  uint32               *tcg_rate
    );
uint32
    arad_ofp_rates_sch_tcg_rate_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core,
    SOC_SAND_IN  uint32                tm_port,   
    SOC_SAND_IN  uint32                tcg_ndx, 
    SOC_SAND_IN  uint32                rate
    );
uint32
    arad_ofp_rates_sch_tcg_rate_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core,
    SOC_SAND_IN  uint32                tm_port,   
    SOC_SAND_IN  uint32                tcg_ndx, 
    SOC_SAND_OUT  uint32               *rate
    );

int
arad_ofp_rates_sch_tcg_shaper_disable(
    SOC_SAND_IN     int               unit,
    SOC_SAND_IN     int               core,
    SOC_SAND_IN     uint32            base_qpair,
    SOC_SAND_IN     uint32            nof_q_pairs);



uint32
    arad_ofp_rates_egq_port_priority_rate_sw_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core,  
    SOC_SAND_IN  uint32                tm_port, 
    SOC_SAND_IN  uint32                prio_ndx,   
    SOC_SAND_IN  uint32                ptc_rate
    );
uint32
    arad_ofp_rates_egq_port_priority_rate_hw_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core
    );
uint32
    arad_ofp_rates_egq_port_priority_rate_hw_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core, 
    SOC_SAND_IN  uint32                tm_port, 
    SOC_SAND_IN  ARAD_TCG_NDX          ptc_ndx,   
    SOC_SAND_OUT uint32                *ptc_rate
    );

uint32
    arad_ofp_rates_sch_port_priority_rate_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core,
    SOC_SAND_IN  uint32                tm_port,   
    SOC_SAND_IN  uint32                port_priority_ndx, 
    SOC_SAND_IN  uint32                rate
    );

uint32
    arad_ofp_rates_sch_port_priority_rate_sw_set_unsafe(
    SOC_SAND_IN  int                   unit,  
    SOC_SAND_IN  int                   core,  
    SOC_SAND_IN  uint32                tm_port, 
    SOC_SAND_IN  uint32                prio_ndx,   
    SOC_SAND_IN  uint32                rate
    );

uint32
    arad_ofp_rates_sch_port_priority_rate_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core,
    SOC_SAND_IN  uint32                tm_port,   
    SOC_SAND_IN  uint32                port_priority_ndx, 
    SOC_SAND_OUT uint32                *rate
    );

uint32
    arad_ofp_rates_egq_port_priority_max_burst_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core,
    SOC_SAND_IN  uint32                tm_port,   
    SOC_SAND_IN  uint32                port_priority_ndx, 
    SOC_SAND_IN  uint32                max_burst
    );
uint32
    arad_ofp_rates_egq_port_priority_max_burst_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core,
    SOC_SAND_IN  uint32                tm_port,   
    SOC_SAND_IN  uint32                port_priority_ndx, 
    SOC_SAND_OUT  uint32               *max_burst
    );
uint32
    arad_ofp_rates_sch_port_priority_max_burst_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core, 
    SOC_SAND_IN  uint32                tm_port, 
    SOC_SAND_IN  uint32                prio_ndx,   
    SOC_SAND_IN  uint32                max_burst
    );
uint32
    arad_ofp_rates_sch_port_priority_max_burst_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core, 
    SOC_SAND_IN  ARAD_FAP_PORT_ID      tm_port, 
    SOC_SAND_IN  uint32                prio_ndx,   
    SOC_SAND_OUT uint32                *max_burst
    );


uint32
  arad_ofp_rates_egq_chnif_shaper_set_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                    core,
    SOC_SAND_IN  uint32                 tm_port,
    SOC_SAND_IN  uint32                  rate_kbps,
    SOC_SAND_IN  uint32                  max_burst
  );


uint32
  arad_ofp_if_spr_rate_by_reg_val_set(
     SOC_SAND_IN int                   unit,
     SOC_SAND_IN uint32                port,                            
     SOC_SAND_IN uint32                shpr_rate_reg_val
     );

uint32
  arad_ofp_if_shaper_disable(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  soc_port_t            port,
    SOC_SAND_OUT uint32*               shpr_rate_reg_val
  );

uint32
  arad_ofp_otm_shapers_disable(
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  soc_port_t port,
    SOC_SAND_IN  uint32     queue_rates_size,
    SOC_SAND_OUT uint32*    queue_rates
  );

uint32
  arad_ofp_otm_shapers_set(
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  soc_port_t port,
    SOC_SAND_IN  uint32     queue_rates_size,
    SOC_SAND_IN  uint32*    queue_rates
  );

uint32
  arad_ofp_tcg_shapers_enable(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  soc_port_t               port,
    SOC_SAND_IN  uint32                   enable
  );
uint32
  jericho_ofp_otm_shapers_enable(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  soc_port_t               port,
    SOC_SAND_IN  uint32                   enable
  );
uint32
  arad_ofp_q_pair_shapers_enable(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  soc_port_t               port,
    SOC_SAND_IN  uint32                   enable
  );
uint32
  arad_ofp_rates_max_credit_empty_port_set(
     SOC_SAND_IN int                                    unit,
     SOC_SAND_IN int                                    arg
     );
uint32
  arad_ofp_rates_max_credit_empty_port_get(
     SOC_SAND_IN int                                    unit,
     SOC_SAND_OUT int*                                    arg
     );

uint32
  arad_ofp_rates_egq_shaper_rate_from_internal(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  ARAD_OFP_RATES_EGQ_CAL_TYPE cal_type,
    SOC_SAND_IN  uint32                      rate_internal,
    SOC_SAND_OUT uint32                     *rate_kbps
  );

uint32
  arad_ofp_rates_egq_shaper_rate_to_internal(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  ARAD_OFP_RATES_EGQ_CAL_TYPE cal_type,
    SOC_SAND_IN  uint32                      rate_kbps,
    SOC_SAND_IN  int                         round_up_flag,    
    SOC_SAND_OUT uint32                     *rate_internal
  );

int
  arad_ofp_rates_retrieve_egress_shaper_reg_field_names(
    SOC_SAND_IN  int                                   unit,
    SOC_SAND_IN  ARAD_OFP_RATES_CAL_INFO              *cal_info,
    SOC_SAND_IN  ARAD_OFP_RATES_CAL_SET                   cal2set,    
    SOC_SAND_IN  ARAD_OFP_RATES_EGQ_CHAN_ARB_FIELD_TYPE   field_type,
    SOC_SAND_OUT soc_reg_t                                *register_name,
    SOC_SAND_OUT soc_field_t                              *field_name
  );

int
  arad_ofp_rates_egress_shaper_reg_field_read (
    SOC_SAND_IN  int                                      unit,
    SOC_SAND_IN  int                                      core,
    SOC_SAND_IN  ARAD_OFP_RATES_CAL_INFO                  *cal_info,   
    SOC_SAND_IN  soc_reg_t                                register_name,
    SOC_SAND_IN  soc_field_t                              field_name,
    SOC_SAND_OUT uint32                                   *data
    );

int 
arad_ofp_rates_egress_shaper_reg_field_write (
    SOC_SAND_IN  int                                      unit,
    SOC_SAND_IN  int                                      core,
    SOC_SAND_IN  ARAD_OFP_RATES_CAL_INFO                  *cal_info,   
    SOC_SAND_IN  soc_reg_t                                register_name,
    SOC_SAND_IN  soc_field_t                              field_name,
    SOC_SAND_OUT uint32                                   data
    );

int
    arad_ofp_rates_packet_mode_packet_size_get (
        SOC_SAND_IN   int                   unit, 
        SOC_SAND_IN   int                   core,  
        SOC_SAND_OUT  uint32                *num_of_bytes
        );

int
arad_ofp_rates_egress_shaper_cal_write (
    SOC_SAND_IN  int                                      unit,
    SOC_SAND_IN  int                                      core,
    SOC_SAND_IN  SOC_TMC_OFP_RATES_CAL_INFO              *cal_info,
    SOC_SAND_IN  SOC_TMC_OFP_RATES_CAL_SET                cal2set,    
    SOC_SAND_IN  SOC_TMC_OFP_RATES_EGQ_CHAN_ARB_FIELD_TYPE field_type,
    SOC_SAND_IN  uint32                                   data
    );

int
arad_ofp_rates_egress_shaper_cal_read (
    SOC_SAND_IN  int                                      unit,
    SOC_SAND_IN  int                                      core,
    SOC_SAND_IN  SOC_TMC_OFP_RATES_CAL_INFO              *cal_info,
    SOC_SAND_IN  SOC_TMC_OFP_RATES_CAL_SET                cal2set,    
    SOC_SAND_IN  SOC_TMC_OFP_RATES_EGQ_CHAN_ARB_FIELD_TYPE field_type,
    SOC_SAND_OUT uint32                                   *data
    );

int
  arad_ofp_rates_from_rates_to_calendar(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                    *ports_rates,
    SOC_SAND_IN  uint32                    nof_ports,
    SOC_SAND_IN  uint32                    total_credit_bandwidth,
    SOC_SAND_IN  uint32                    max_calendar_len,
    SOC_SAND_OUT ARAD_OFP_RATES_CAL_SCH    *calendar,
    SOC_SAND_OUT uint32                    *calendar_len,
    SOC_SAND_IN  uint32                    is_alt_calc
  );

uint32
  arad_ofp_rates_fixed_len_cal_build(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                   *port_nof_slots,
    SOC_SAND_IN  uint32                   nof_ports,
    SOC_SAND_IN  uint32                    calendar_len,
    SOC_SAND_IN  uint32                    max_calendar_len,
    SOC_SAND_IN  uint32                    is_fqp_pqp,
    SOC_SAND_IN  uint32                    is_alt_calc,
    SOC_SAND_OUT uint32                   *calendar
  );


#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif



