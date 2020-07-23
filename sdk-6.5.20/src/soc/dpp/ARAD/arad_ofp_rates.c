#include <shared/bsl.h>

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



#include <shared/swstate/access/sw_state_access.h>
#include <soc/mem.h>
#include <soc/dcmn/error.h>
#include <bcm_int/dpp/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>


#include <soc/error.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/mbcm.h>
#include <soc/dpp/ARAD/arad_ofp_rates.h>
#include <soc/dpp/JER/jer_ofp_rates.h>
#include <soc/dpp/ARAD/arad_scheduler_device.h>
#include <soc/dpp/ARAD/arad_scheduler_ports.h>
#include <soc/dpp/ARAD/arad_ports.h>
#include <soc/dpp/ARAD/arad_general.h>
#include <soc/dpp/ARAD/arad_sw_db.h>
#include <soc/dpp/ARAD/arad_reg_access.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/arad_general.h>
#include <soc/dpp/QAX/qax_link_bonding.h>

#include <soc/dpp/port_sw_db.h>

#include <soc/dpp/SAND/Utils/sand_u64.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>
#include <soc/dpp/SAND/Utils/sand_conv.h>





#define ARAD_OFP_RATES_OLP_GUARANTEED_KBPS             (1 * 1024 * 1024)
#define ARAD_OFP_RATES_OAMP_GUARANTEED_KBPS             (1 * 1024 * 1024)

#define ARAD_OFP_RATES_QPAIR_INITIAL_KBPS              (800 * 1024 * 1024)
#define ARAD_OFP_RATES_TCG_INITIAL_KBPS                (800 * 1024 * 1024)


#define ARAD_OFP_RATES_CAL_RATE_DEVIATION              (100000000)


#define ARAD_EGQ_CAL_SLOT_TRAVERSE_IN_CLOCKS           (2)


#define ARAD_EGQ_UNITS_VAL_IN_BITS_CHAN_ARB            (256 / 8)


#define ARAD_EGQ_UNITS_VAL_IN_BITS_QPAIR_TCG           (128 / 8)


#define ARAD_EGQ_NIF_MAL_SCM_TBL_ENTRY_PORT_CR_TO_ADD_NOF_BITS              (18)


#define ARAD_OFP_RATES_ILLEGAL_SCHEDULER_ID            (0xfff)




#define ARAD_OFP_RATES_UNMAP_BASE_Q_PAIR               (ARAD_NOF_FAP_PORTS)

#define ARAD_OFP_RATES_DUMMY_PORT_ID                   (ARAD_MAX_FAP_PORT_ID)


#define ARAD_OFP_RATES_NOF_ITERATIONS                  (4)


#define ARAD_OFP_RATES_NOF_PRIORITIES_TCG_SUPPORT (8)







#define SOC_REG64_SET(reg_desc, index,reg_value) \
{\
    int res = SOC_E_NONE;\
    res = soc_reg64_set(unit, (reg_desc), (REG_PORT_ANY), (index), (reg_value));\
    SOCDNX_SAND_IF_ERR_EXIT(res);\
}
#define SOC_REG64_GET(reg_desc, index, reg_value)  \
{ \
    int res = SOC_E_NONE;\
    COMPILER_64_ZERO(reg_value); \
    res = soc_reg64_get(unit, reg_desc, REG_PORT_ANY, index, &(reg_value));    \
    SOCDNX_SAND_IF_ERR_EXIT(res);\
}

#define SOC_FIELD_GET(reg_desc, fld_desc, fld_value) \
{\
  int res = SOC_E_NONE;\
  uint32 reg_value; \
  fld_value = 0; \
  res = soc_reg32_get(unit, reg_desc, (REG_PORT_ANY), 0, &reg_value);\
  SOCDNX_SAND_IF_ERR_EXIT(res);\
  fld_value = soc_reg_field_get(unit, reg_desc, reg_value, fld_desc); \
}
#define SOC_FIELD64_GET(reg_desc, fld_desc, fld_value) \
{\
    uint64 reg_value; \
    COMPILER_64_ZERO(fld_value); \
    SOC_REG64_GET(reg_desc, 0, reg_value); \
    fld_value = soc_reg64_field_get(unit, reg_desc, reg_value, fld_desc); \
}
#define SOC_FIELD64_SET(reg_desc, fld_desc, fld_value) \
{\
  uint64 reg_value; \
  SOC_REG64_GET(reg_desc, 0, reg_value); \
  soc_reg64_field_set(unit, reg_desc, &reg_value,     fld_desc, fld_value); \
  SOC_REG64_SET(reg_desc, 0,reg_value); \
}

#define SOC_FIELD_SET(reg_desc, fld_desc, fld_value) \
{\
  int res;\
  uint32 reg_value; \
  res = soc_reg32_get(unit, reg_desc, (REG_PORT_ANY), 0, &reg_value);\
  SOCDNX_SAND_IF_ERR_EXIT(res);\
  soc_reg_field_set(unit, reg_desc, &reg_value,     fld_desc, fld_value); \
  res = soc_reg32_set(unit, reg_desc, REG_PORT_ANY, 0, reg_value); \
  SOCDNX_SAND_IF_ERR_EXIT(res);\
}
#define SOC_FIELD32_REG64_GET(reg_desc, fld_desc, fld_value) \
{\
    uint64 reg_value; \
    fld_value=0; \
    SOC_REG64_GET(reg_desc, 0, reg_value); \
    fld_value = soc_reg64_field32_get(unit, reg_desc, reg_value, fld_desc); \
}
#define SOC_FIELD32_REG64_SET(reg_desc, fld_desc, fld32_value) \
{\
    uint64 reg64_value; \
    SOC_REG64_GET(reg_desc, 0, reg64_value); \
    soc_reg64_field32_set(unit, reg_desc, &reg64_value, fld_desc, fld32_value); \
    SOC_REG64_SET(reg_desc, 0, reg64_value); \
}




#define ARAD_OFP_RATES_EGQ_CAL_LEN_EGQ(unit, chan_arb_id) \
  (((chan_arb_id < SOC_DPP_DEFS_GET(unit, nof_big_channelized_calendars)) || \
    (chan_arb_id > (SOC_DPP_DEFS_GET(unit, nof_channelized_calendars)-1))) || \
    (chan_arb_id == SOC_DPP_DEFS_GET(unit, non_channelized_cal_id) && SOC_IS_JERICHO(unit)) ? \
   (SOC_DPP_DEFS_GET(unit, big_channelized_cal_size)) : (SOC_DPP_DEFS_GET(unit, small_channelized_cal_size)))


#define ARAD_OFP_RATES_EGQ_SCM_OFFSET_GET(unit, cal_info,cal2set) \
  ((cal_info->cal_type == ARAD_OFP_RATES_EGQ_CAL_CHAN_ARB) ? \
  (cal2set * ARAD_OFP_RATES_EGQ_CAL_LEN_EGQ(unit, cal_info->chan_arb_id) ) : \
  ((cal_info->cal_type == ARAD_OFP_RATES_EGQ_CAL_PORT_PRIORITY) ? \
  (cal2set * ARAD_OFP_RATES_CAL_LEN_EGQ_PORT_PRIO_MAX) : cal2set * ARAD_OFP_RATES_CAL_LEN_EGQ_TCG_MAX))

#define ARAD_OFP_RATES_EGQ_PMC_LEN(cal_info) \
  (cal_info->cal_type == ARAD_OFP_RATES_EGQ_CAL_CHAN_ARB ? ARAD_EGR_NOF_BASE_Q_PAIRS : \
    (cal_info->cal_type == ARAD_OFP_RATES_EGQ_CAL_PORT_PRIORITY ? ARAD_EGR_NOF_Q_PAIRS : \
    ARAD_EGR_NOF_PS*ARAD_NOF_TCGS)) 


#define ARAD_OFP_RATES_EGQ_PMC_OFFSET_GET(cal_info,egq2set,ofp_index) \
  (ARAD_OFP_RATES_EGQ_PMC_LEN(cal_info) * egq2set + ofp_index)


#define ARAD_OFP_RATES_TCG_ID_GET(ps,tcg_ndx) \
  (ps*ARAD_NOF_TCGS_IN_PS + tcg_ndx)






typedef struct
{
  
  uint32 base_q_pair;
  
  uint32 sch_rate;
  
  uint32 egq_rate;
  
  uint32 max_burst;  
  
  uint32 port_priority;
  
  ARAD_TCG_NDX tcg_ndx;
}ARAD_OFP_RATES_INTERNAL_INFO;

typedef struct
{
  
  uint32 nof_valid_entries;
  
  ARAD_OFP_RATES_INTERNAL_INFO rates[ARAD_NOF_FAP_PORTS];
}ARAD_OFP_RATES_TBL_INTERNAL_INFO;












STATIC uint32
  arad_ofp_rates_egq_shaper_config(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  ARAD_OFP_RATES_CAL_INFO       *cal_info,
    SOC_SAND_IN  uint32                         rate
  );


STATIC
  void
    arad_ARAD_OFP_RATES_CAL_INFO_clear(
      SOC_SAND_OUT ARAD_OFP_RATES_CAL_INFO *info
    );

STATIC 
  uint32
    arad_ofp_rates_retrieve_egress_shaper_setting_field(
      SOC_SAND_IN  int                       unit,
      SOC_SAND_IN  ARAD_OFP_RATES_CAL_INFO      *cal_info,
      SOC_SAND_OUT soc_field_t                  *field_name
    )
{
   SOCDNX_INIT_FUNC_DEFS

   SOCDNX_NULL_CHECK(cal_info);
   SOCDNX_NULL_CHECK(field_name);

   
  switch (cal_info->cal_type)
  {
  case ARAD_OFP_RATES_EGQ_CAL_CHAN_ARB:
    *field_name = OTM_SPR_SET_SELf;
    break;
  case ARAD_OFP_RATES_EGQ_CAL_PORT_PRIORITY:
    *field_name = QPAIR_SPR_SET_SELf;
    break;
  case ARAD_OFP_RATES_EGQ_CAL_TCG:
    *field_name = TCG_SPR_SET_SELf;
    break;
  default:
    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG_STR( "Unkonown cal info")));
    break;
  }

exit:
  SOCDNX_FUNC_RETURN
}




uint32
  arad_ofp_rates_init(
    SOC_SAND_IN  int unit
  )
{
  uint32
    fld_val,
    data,
    res = SOC_E_NONE;
  ARAD_OFP_RATES_CAL_INFO
    cal_info;
  uint8
    pp_enable,
    qpair_tcg_shapers_enable = 0x0,
    qpair_shapers_enable = 0x0,
    tcg_shapers_enable = 0x0,
    oamp_enable;
  uint32 init_max_burst = ARAD_OFP_RATES_BURST_DEAULT;
   int core=0;
   int idx, ps_id, tcg_id;
   ARAD_SW_DB_DEV_RATE            
        tcg_rate;
   ARAD_SW_DB_DEV_RATE            
        queue_rate;

  SOCDNX_INIT_FUNC_DEFS

  if (!SOC_UNIT_NUM_VALID(unit)) {
      SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG_STR( "unit not valid")));
  }
  
  SOC_CLEAR(&cal_info, ARAD_OFP_RATES_CAL_INFO, 1);

  res = READ_EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr(unit, REG_PORT_ANY, &data);
  SOCDNX_IF_ERR_EXIT(res);

  
  res = arad_fill_table_with_entry(unit, EGQ_PMCm, MEM_BLOCK_ANY, &init_max_burst);
  SOCDNX_SAND_IF_ERR_EXIT(res);
  res = arad_fill_table_with_entry(unit, EGQ_TCG_PMCm, MEM_BLOCK_ANY, &init_max_burst);
  SOCDNX_SAND_IF_ERR_EXIT(res);
  res = arad_fill_table_with_entry(unit, EGQ_QP_PMCm, MEM_BLOCK_ANY, &init_max_burst);
  SOCDNX_SAND_IF_ERR_EXIT(res);

  
  for (idx = 0; idx < ARAD_EGR_NOF_BASE_Q_PAIRS; idx++)
  {
      
      res = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.rates.egq_bursts.set(unit, core, idx, init_max_burst);
      SOCDNX_IF_ERR_EXIT(res);

      
      res = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.port_priority_cal.queue_rate.get(unit, core, idx, &queue_rate);
      SOCDNX_IF_ERR_EXIT(res);
      queue_rate.egq_bursts = init_max_burst;
      res = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.port_priority_cal.queue_rate.set(unit, core, idx, &queue_rate);
      SOCDNX_IF_ERR_EXIT(res);
  }

  for (ps_id = 0; ps_id < ARAD_EGR_NOF_PS; ps_id++)
  {
      for (tcg_id = 0; tcg_id < ARAD_NOF_TCGS; tcg_id++)
      {
          
          res = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.tcg_cal.tcg_rate.get(unit, core, ps_id, tcg_id, &tcg_rate);
          SOCDNX_IF_ERR_EXIT(res);
          tcg_rate.egq_bursts = init_max_burst;
          res = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.tcg_cal.tcg_rate.set(unit, core, ps_id, tcg_id, &tcg_rate);
          SOCDNX_IF_ERR_EXIT(res);
      }
  }
  

  
  if SOC_IS_ARAD_B0_AND_ABOVE(unit)
  {
    fld_val = soc_property_get(unit, "qpair_tcg_shapers_enable",0x1);
    qpair_tcg_shapers_enable = fld_val;
    qpair_shapers_enable = fld_val;
    tcg_shapers_enable = fld_val;
  }
  else
  {
    qpair_tcg_shapers_enable = 0x0;
    qpair_shapers_enable = 0x0;
    tcg_shapers_enable = 0x0;
  }
  
  fld_val = 0x1;
  soc_reg_field_set(unit, EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr, &data, OTM_SPR_ENAf, fld_val);

  
  fld_val = qpair_shapers_enable;
  soc_reg_field_set(unit, EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr, &data, QPAIR_SPR_ENAf, fld_val);  

  
  fld_val = tcg_shapers_enable;
  soc_reg_field_set(unit, EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr, &data, TCG_SPR_ENAf, fld_val);   
  
  if(SOC_IS_ARAD_B0_AND_ABOVE(unit)) {
      soc_reg_field_set(unit, EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr, &data, QPAIR_SPR_RESOLUTIONf, 1);  
      soc_reg_field_set(unit, EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr, &data, TCG_SPR_RESOLUTIONf, 1);  
  }

  
  soc_reg_field_set(unit, EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr, &data, IGNORE_QEMPTYf, 0);

  
  soc_reg_field_set(unit, EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr, &data, SHAPER_PACKET_RATE_CONSTf, ARAD_OFP_RATES_DEFAULT_PACKET_SIZE);

  res = WRITE_EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr(unit, REG_PORT_ANY,data);
  SOCDNX_SAND_IF_ERR_EXIT(res);

  res = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.egq_tcg_qpair_shaper_enable.set(unit,qpair_tcg_shapers_enable);
  SOCDNX_IF_ERR_EXIT(res);

  
  cal_info.cal_type = ARAD_OFP_RATES_EGQ_CAL_PORT_PRIORITY;  
  res = arad_ofp_rates_egq_shaper_config(
          unit,
          &cal_info,
          ARAD_OFP_RATES_QPAIR_INITIAL_KBPS
        );
  SOCDNX_IF_ERR_EXIT(res);

  cal_info.cal_type = ARAD_OFP_RATES_EGQ_CAL_TCG;  
  res = arad_ofp_rates_egq_shaper_config(
          unit,
          &cal_info,
          ARAD_OFP_RATES_TCG_INITIAL_KBPS
        );
  SOCDNX_IF_ERR_EXIT(res);

  
  pp_enable = SOC_DPP_PP_ENABLE(unit);
  if (pp_enable && (SOC_INFO(unit).olp_port[0]>=0))
  {
    res = arad_sch_if_shaper_rate_set(
            unit,
            core, 
            ARAD_OLP_PORT_ID,
            ARAD_OFP_RATES_OLP_GUARANTEED_KBPS
          );
    SOCDNX_IF_ERR_EXIT(res);
  }

  oamp_enable = SOC_DPP_CONFIG(unit)->pp.oamp_enable;
  if (oamp_enable)
  {
    res = arad_sch_if_shaper_rate_set(
            unit,
            core,
            ARAD_OAMP_PORT_ID,
            ARAD_OFP_RATES_OAMP_GUARANTEED_KBPS
          );
    SOCDNX_IF_ERR_EXIT(res);
  }

exit:
  SOCDNX_FUNC_RETURN
}


STATIC uint8
  arad_ofp_rates_is_cal_entry_dummy(
    SOC_SAND_IN  ARAD_OFP_EGQ_RATES_CAL_ENTRY  *cal_entry
  )
{
  ARAD_OFP_EGQ_RATES_CAL_ENTRY
    dummy_entry;

  dummy_entry.base_q_pair = ARAD_OFP_RATES_DUMMY_PORT_ID;
  dummy_entry.credit = 0;

  if (cal_entry)
  {
    return SOC_SAND_NUM2BOOL(0 == soc_sand_os_memcmp(cal_entry, &dummy_entry, sizeof(ARAD_OFP_EGQ_RATES_CAL_ENTRY)));
  }

  return FALSE;
}

STATIC uint32
  arad_ofp_rates_port2chan_arb_get_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  uint32                         tm_port,
    SOC_SAND_IN  int                            core,
    SOC_SAND_OUT ARAD_OFP_RATES_EGQ_CHAN_ARB_ID *chan_arb_id
  )
{
    uint32
      res = SOC_E_NONE;
    soc_reg_above_64_val_t
      data,
      field;
    ARAD_INTERFACE_ID
      egr_interface_id;
    uint32
      chan_arb_field_val = 0,
      chan_arb_val = 0;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(chan_arb_id);

    
    res = arad_port2egress_offset(unit, core, tm_port, &egr_interface_id);
    SOCDNX_IF_ERR_EXIT(res);

    
    res = READ_EGQ_MAPPING_INTERFACES_TO_CHAN_ARBITERr(unit,data);
    SOCDNX_SAND_IF_ERR_EXIT(res);
    soc_reg_above_64_field_get(unit,EGQ_MAPPING_INTERFACES_TO_CHAN_ARBITERr,data,MAP_IFC_TO_CHAN_ARBf,field);
    SHR_BITCOPY_RANGE(&chan_arb_field_val,0,field,ARAD_OFP_RATES_CHAN_ARB_NOF_BITS*egr_interface_id,ARAD_OFP_RATES_CHAN_ARB_NOF_BITS);
    res = arad_nif_chan_arb_field_val_to_enum(unit,chan_arb_field_val,&chan_arb_val);
    SOCDNX_SAND_IF_ERR_EXIT(res);

    switch (chan_arb_val)
    {
        case ARAD_OFP_RATES_EGQ_NOF_CHAN_ARB:    
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG_STR( "illegal chan arbiter")));
            break;
        default:
            *chan_arb_id = chan_arb_val;
            break;
    }

exit:
  SOCDNX_FUNC_RETURN;
}


STATIC uint32
  arad_ofp_rates_if_id2chan_arb_get_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  int                            core,
    SOC_SAND_IN  uint32                         tm_port,
    SOC_SAND_OUT ARAD_OFP_RATES_EGQ_CHAN_ARB_ID *chan_arb_id
  )
{
    uint32
      res = SOC_E_NONE;
    SOCDNX_INIT_FUNC_DEFS;

    res = arad_ofp_rates_port2chan_arb_get_unsafe(unit, tm_port, core, chan_arb_id);
    SOCDNX_IF_ERR_EXIT(res);

exit:
  SOCDNX_FUNC_RETURN;

}





STATIC uint32
 arad_ofp_rates_cal_improve_nof_slots(
              SOC_SAND_IN    int    unit,
              SOC_SAND_IN    uint32 *ports_rates,
              SOC_SAND_IN    uint32 nof_ports,
              SOC_SAND_IN    uint32 max_calendar_len,
              SOC_SAND_INOUT uint32 *loc_calendar_len,
              SOC_SAND_INOUT uint32 *port_nof_slots,
              SOC_SAND_OUT   uint8 *slots_added
              )
{
    SOC_SAND_U64 
        u64_1, 
        u64_2;
    uint32 
        idx,
        rem, 
        expected_credits,
        mask = ~((1 << SOC_DPP_DEFS_GET(unit, scm_qp_tcg_cr_to_add_nof_bits)) - 1);
        
    SOCDNX_INIT_FUNC_DEFS
    SOCDNX_NULL_CHECK(ports_rates);
    SOCDNX_NULL_CHECK(port_nof_slots);

    *slots_added = FALSE;
    
    for (idx = 0; idx < nof_ports; ++idx) {
        
        if (ports_rates[idx] > 0 ) {
            while(TRUE) {
                 
                 soc_sand_u64_multiply_longs(ports_rates[idx],(*loc_calendar_len) *  
                                               ARAD_EGQ_CAL_SLOT_TRAVERSE_IN_CLOCKS * ARAD_EGQ_UNITS_VAL_IN_BITS_QPAIR_TCG, &u64_1);
                 
                 rem = soc_sand_u64_devide_u64_long(&u64_1,port_nof_slots[idx] * arad_chip_kilo_ticks_per_sec_get(unit), &u64_2);
                 soc_sand_u64_to_long(&u64_2, &(expected_credits));
                 expected_credits += ((rem > 0 )? 1 : 0);
                
                 
                 if (!(expected_credits & mask)) {
                     break;
                 }

                  
                 (*loc_calendar_len)++;
                 port_nof_slots[idx]++;
                 (*slots_added) = TRUE;
                 if (max_calendar_len < *loc_calendar_len)
                 {
                     SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG_STR( "calculated cal len bigger than max")));
                 }
            }
        }
    }
exit:
  SOCDNX_FUNC_RETURN
}

STATIC uint32
  arad_ofp_rates_cal_per_len_build(
    SOC_SAND_IN   int                      unit,
    SOC_SAND_IN   uint32                    *ports_rates,
    SOC_SAND_IN   uint32                   nof_ports,
    SOC_SAND_IN   uint32                    total_credit_bandwidth,
    SOC_SAND_IN   uint32                    max_calendar_len,
    SOC_SAND_IN   uint32                    tentative_len,
    SOC_SAND_OUT  uint32                    *actual_len,
    SOC_SAND_OUT  uint32                   *port_nof_slots,
    SOC_SAND_OUT  uint32                    *deviation
  )
{
  uint32
    port,
    slots_rates[ARAD_NOF_FAP_PORTS],
    total_num_slots = 0,
    calc_deviation;
  uint32
    rem;
  uint32
    num_slots,
    temp1,
    temp2;
  SOC_SAND_U64
    u64_1,
    u64_2;

  SOCDNX_INIT_FUNC_DEFS

  SOCDNX_NULL_CHECK(actual_len);
  SOCDNX_NULL_CHECK(deviation);

  if (0 == tentative_len)
  {
    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG_STR( "illegal cal len")));
  }

  SOC_CLEAR(slots_rates, uint32, ARAD_NOF_FAP_PORTS);

  
  for (port = 0; port < nof_ports; ++port)
  {
    if (ports_rates[port] > 0)
    {
      

      soc_sand_u64_multiply_longs(ports_rates[port], tentative_len, &u64_1);
      rem = soc_sand_u64_devide_u64_long(&u64_1, total_credit_bandwidth, &u64_2);
      soc_sand_u64_to_long(&u64_2, &(num_slots));

      
      num_slots = (rem > 0 ? (num_slots + 1) : ((num_slots == 0) ? num_slots + 1 : num_slots));

      slots_rates[port] = num_slots;
      total_num_slots += num_slots;
    }
  }
  if (total_num_slots > max_calendar_len ||
      total_credit_bandwidth == 0 || total_num_slots == 0) {
      
      *actual_len = 0;
  } 
  else 
  {

      calc_deviation = 0;
      for (port = 0; port < nof_ports; ++port)
      {
        soc_sand_u64_multiply_longs(ARAD_OFP_RATES_CAL_RATE_DEVIATION, ports_rates[port], &u64_1);
        rem = soc_sand_u64_devide_u64_long(&u64_1, total_credit_bandwidth, &u64_2);
        soc_sand_u64_to_long(&u64_2, &(temp1));
        temp1 = (rem > 0 ? temp1 + 1 : temp1);

        soc_sand_u64_multiply_longs(ARAD_OFP_RATES_CAL_RATE_DEVIATION, slots_rates[port], &u64_1);
        rem = soc_sand_u64_devide_u64_long(&u64_1, total_num_slots, &u64_2);
        soc_sand_u64_to_long(&u64_2, &(temp2));
        temp2 = (rem > 0 ? temp2 + 1 : temp2);

        calc_deviation += soc_sand_abs(temp2 - temp1);
      }

      *actual_len = total_num_slots;
      SOC_COPY(port_nof_slots, slots_rates, uint32, ARAD_NOF_FAP_PORTS);
      *deviation = calc_deviation;
  }

exit:
  SOCDNX_FUNC_RETURN
}

STATIC uint32
  arad_ofp_rates_cal_len_calculate(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                    *ports_rates,
    SOC_SAND_IN  uint32                   nof_ports,
    SOC_SAND_IN  uint32                    total_credit_bandwidth,
    SOC_SAND_IN  uint32                    max_calendar_len,
    SOC_SAND_OUT uint32                    *actual_calendar_len,
    SOC_SAND_OUT uint32                   *port_nof_slots
  )
{
  uint32
    res,
    deviation,
    min_deviation,
    tentative_len,
    best_tentative_len,
    actual_len;
  uint32
    port_num_slots[ARAD_NOF_FAP_PORTS];
  uint32
    min_port_rate = 0x1;

  SOCDNX_INIT_FUNC_DEFS

  
  min_deviation = ARAD_OFP_RATES_CAL_RATE_DEVIATION;
  best_tentative_len = 0;
  for (tentative_len = 1; tentative_len <= max_calendar_len; ++tentative_len)
  {
    res = arad_ofp_rates_cal_per_len_build(
            unit,
            ports_rates,
            nof_ports,
            total_credit_bandwidth,
            max_calendar_len,
            tentative_len,
            &actual_len,
            port_num_slots,
            &deviation
          );
    SOCDNX_IF_ERR_EXIT(res);

    
    if (actual_len == 0)
    {
      continue;
    }
    if (deviation < min_deviation)
    {
      min_deviation = deviation;
      best_tentative_len = tentative_len;
      if (0 == min_deviation)
      {
        break;
      }
    }
  }
  
  if (0 == best_tentative_len)
  {
    best_tentative_len = 1;

    
    res = arad_ofp_rates_cal_per_len_build(
          unit,
          &min_port_rate,
          1,
          min_port_rate,
          max_calendar_len,
          min_port_rate,
          &actual_len,
          port_num_slots,
          &deviation
        );
    SOCDNX_IF_ERR_EXIT(res);
  }
  else
  {
    
    res = arad_ofp_rates_cal_per_len_build(
            unit,
            ports_rates,
            nof_ports,
            total_credit_bandwidth,
            max_calendar_len,
            best_tentative_len,
            &actual_len,
            port_num_slots,
            &deviation
          );
    SOCDNX_IF_ERR_EXIT(res);
  }

  *actual_calendar_len = actual_len;
  SOC_COPY(port_nof_slots, port_num_slots, uint32, ARAD_NOF_FAP_PORTS);

exit:
  SOCDNX_FUNC_RETURN
}


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
  )
{
  uint32
    slot_idx,
    port_idx,
    nof_diff_ports,
    port_num_slots[ARAD_NOF_FAP_PORTS],
    alloc_slots,
    rem_cal_len,
    max_port_idx,
    hop_size = 0,
    hop_size_small = 0,
    nof_big_hops = 0,
    port_alloc_slots,
    free_slot_cnt,
    leftovers[ARAD_NOF_FAP_PORTS];

  SOCDNX_INIT_FUNC_DEFS

  
  alloc_slots = 0;
  for (port_idx = 0; port_idx < nof_ports; ++port_idx)
  {
    alloc_slots += port_nof_slots[port_idx];
  }
  if (alloc_slots != calendar_len)
  {
    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG_STR( "Illegal calendar len")));
  }

  
  for (slot_idx = 0; slot_idx < max_calendar_len; ++slot_idx)
  {
    calendar[slot_idx] = ARAD_OFP_RATES_ILLEGAL_SCHEDULER_ID;
  }

  
  nof_diff_ports = 0;
  for (port_idx = 0; port_idx < nof_ports; ++port_idx)
  {
    if (port_nof_slots[port_idx] > 0)
    {
      ++nof_diff_ports;
    }
    leftovers[port_idx] = 0;
  }

  SOC_COPY(port_num_slots, port_nof_slots, uint32, ARAD_NOF_FAP_PORTS);

  
  alloc_slots = 0;
  rem_cal_len = calendar_len;
  max_port_idx = soc_sand_get_index_of_max_member_in_array(
                   port_num_slots,
                   nof_ports
                 );
  while (port_num_slots[max_port_idx] > 0)
  {
    
    hop_size = SOC_SAND_MIN(rem_cal_len / port_num_slots[max_port_idx], nof_diff_ports);
    if (is_alt_calc)
    {
        hop_size_small = hop_size - 1; 
        
        
        
        
        nof_big_hops = rem_cal_len - port_nof_slots[max_port_idx]*hop_size;
        if(nof_big_hops == 0)
        {
            hop_size = hop_size_small;
        }
    }
    else
    {
         
        hop_size -= is_fqp_pqp ? 1 : 0;
    }
    port_alloc_slots = 0;
    free_slot_cnt = hop_size;
    for (slot_idx = 0; slot_idx < calendar_len && port_alloc_slots < port_nof_slots[max_port_idx]; ++slot_idx)
    {
      if (calendar[slot_idx] == ARAD_OFP_RATES_ILLEGAL_SCHEDULER_ID)
      {
        if (free_slot_cnt < hop_size)
        {
          ++free_slot_cnt;
        }
        else
        {
          calendar[slot_idx] = max_port_idx;
          ++alloc_slots;
          ++port_alloc_slots;
          free_slot_cnt = 0;
          
          
          if (is_alt_calc)
          {
              if (nof_big_hops <= 0) {
                  hop_size = hop_size_small;
              } else {
                  nof_big_hops--;
              }
          }
        }
      }
    }

    port_num_slots[max_port_idx] = 0;
    rem_cal_len -= port_alloc_slots;

    
    leftovers[max_port_idx] = port_nof_slots[max_port_idx] - port_alloc_slots;

    max_port_idx = soc_sand_get_index_of_max_member_in_array(
                     port_num_slots,
                     nof_ports
                   );
  }

  
  port_idx = 0;
  free_slot_cnt = nof_diff_ports;
  for (slot_idx = 0; alloc_slots < calendar_len; slot_idx = (slot_idx + 1) % calendar_len)
  {
    if (calendar[slot_idx] == ARAD_OFP_RATES_ILLEGAL_SCHEDULER_ID)
    {
      if (free_slot_cnt == nof_diff_ports)
      {
        for ( ; leftovers[port_idx] == 0; port_idx = (port_idx + 1) % nof_ports);
        calendar[slot_idx] = port_idx;
        --leftovers[port_idx];
        port_idx = (port_idx + 1) % nof_ports;
        ++alloc_slots;
        free_slot_cnt = 0;
      }
      else
      {
        ++free_slot_cnt;
      }
    }
  }

exit:
  SOCDNX_FUNC_RETURN
}


STATIC uint32
  arad_ofp_rates_active_calendars_retrieve_sch(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint32                     sch_offset,
    SOC_SAND_IN  int                        core,
    SOC_SAND_OUT ARAD_OFP_RATES_CAL_SCH     *sch_cal,
    SOC_SAND_OUT uint32                     *sch_cal_len
  )
{
    uint32
        res,
        data,
        fld_val = 0,
        sch_len = 0;
    uint32
        sch_to_get;

    SOCDNX_INIT_FUNC_DEFS;

    *sch_cal_len = 0;

    
    res = READ_SCH_CH_NIF_CALENDAR_CONFIGURATION_CNCCm(unit, SCH_BLOCK(unit, core), sch_offset, &data);
    SOCDNX_IF_ERR_EXIT(res);

    
    fld_val = soc_SCH_CH_NIF_CALENDAR_CONFIGURATION_CNCCm_field32_get(unit, &data, DVS_CALENDAR_SEL_CH_NI_FXXf);
    sch_to_get = fld_val;
  
    
    sch_len = soc_SCH_CH_NIF_CALENDAR_CONFIGURATION_CNCCm_field32_get(unit, &data, sch_to_get == 0 ? CAL_A_LENf : CAL_B_LENf);
    
    
    *sch_cal_len = sch_len + 1;

    
    res = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_sch_cal_tbl_get, (unit, core, sch_offset, sch_to_get, *sch_cal_len, sch_cal->slots));
    SOCDNX_IF_ERR_EXIT(res);

exit:
    SOCDNX_FUNC_RETURN
}

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
  )
{
  uint32
    res = SOC_E_NONE;
  uint32
    port_nof_slots[ARAD_NOF_FAP_PORTS];

  SOCDNX_INIT_FUNC_DEFS;

  res = arad_ofp_rates_cal_len_calculate(
          unit,
          ports_rates,
          nof_ports,
          total_credit_bandwidth,
          max_calendar_len,
          calendar_len,
          port_nof_slots
        );
  SOCDNX_IF_ERR_EXIT(res);

  
  res = arad_ofp_rates_fixed_len_cal_build(
          unit,
          port_nof_slots,
          nof_ports,
          *calendar_len,
          max_calendar_len,
          0,
          is_alt_calc,
          calendar->slots
        );
  SOCDNX_IF_ERR_EXIT(res);

exit:
  SOCDNX_FUNC_RETURN;
}

STATIC uint32
  arad_ofp_rates_fill_shaper_generic_calendar_credits(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  int                       core,
    SOC_SAND_IN  ARAD_OFP_RATES_CAL_INFO  *cal_info,
    SOC_SAND_IN  uint32                     *ports_rates,
    SOC_SAND_IN  uint32                    nof_ports,
    SOC_SAND_IN  uint32                     calendar_len, 
    SOC_SAND_IN  uint8                      add_dummy_tail, 
    SOC_SAND_IN  uint32                    *slots_ports,
    SOC_SAND_OUT ARAD_OFP_RATES_CAL_EGQ       *calendar
  )
{
  uint32
    slot_idx = 0,
    port_idx = 0,
    egq_resolution,
    scm_cr_to_add_nof_bits,
    port_in_slot;
  uint32
    port_num_slots[ARAD_NOF_FAP_PORTS],
    port_credits[ARAD_NOF_FAP_PORTS];
  SOC_SAND_U64
    u64_1,
    u64_2;
  uint32
    rem;
  uint32
    temp_calendar_length,
    dummy_tail_entry,
    calcal_length,
    calcal_instances;

  SOCDNX_INIT_FUNC_DEFS

  SOCDNX_NULL_CHECK(ports_rates);
  SOCDNX_NULL_CHECK(slots_ports);
  SOCDNX_NULL_CHECK(calendar);

  SOC_CLEAR(port_num_slots, uint32, ARAD_NOF_FAP_PORTS);
  SOC_CLEAR(port_credits, uint32, ARAD_NOF_FAP_PORTS);

  dummy_tail_entry = SOC_SAND_BOOL2NUM(add_dummy_tail);
  temp_calendar_length = calendar_len - dummy_tail_entry; 
  
  
  if (cal_info->cal_type == ARAD_OFP_RATES_EGQ_CAL_CHAN_ARB) { 
      scm_cr_to_add_nof_bits = ARAD_EGQ_NIF_MAL_SCM_TBL_ENTRY_PORT_CR_TO_ADD_NOF_BITS;
  } else { 
      scm_cr_to_add_nof_bits = SOC_DPP_DEFS_GET(unit, scm_qp_tcg_cr_to_add_nof_bits);
  }

  
  for (slot_idx = 0; slot_idx < temp_calendar_length; ++slot_idx)
  {
    port_in_slot = slots_ports[slot_idx];
    calendar->slots[slot_idx].base_q_pair = port_in_slot;

    if (port_in_slot == ARAD_OFP_RATES_UNMAP_BASE_Q_PAIR)
    {
      continue;
    }

    if (port_in_slot == ARAD_OFP_RATES_ILLEGAL_SCHEDULER_ID)
    {
      SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG_STR( "Invalid port")));
    }

    ++(port_num_slots[port_in_slot]);
  }

 

  if (cal_info->cal_type == ARAD_OFP_RATES_EGQ_CAL_CHAN_ARB)
  {
    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.calcal_length.get(unit,
        core, &calcal_length));
    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.chan_arb.nof_calcal_instances.get(unit,
        core, cal_info->chan_arb_id, &calcal_instances));
  }
  else
  {
    
    calcal_length = 1;
    calcal_instances = 1;
  }
  
  for(port_idx = 0; port_idx < ARAD_NOF_FAP_PORTS; ++port_idx)
  {
    if(!ports_rates[port_idx])
    {
      continue;
    }

    if(port_num_slots[port_idx] == 0)
    {
      
      SOCDNX_EXIT_WITH_ERR_NO_MSG(SOC_E_LIMIT);
    }

    if(SOC_IS_ARAD_A0(unit) || (cal_info->cal_type==ARAD_OFP_RATES_EGQ_CAL_CHAN_ARB)) {
        
        egq_resolution = ARAD_EGQ_UNITS_VAL_IN_BITS_CHAN_ARB;
    } else {
        
        egq_resolution = ARAD_EGQ_UNITS_VAL_IN_BITS_QPAIR_TCG;
    }

    soc_sand_u64_multiply_longs(ports_rates[port_idx], calcal_length * (calendar_len) * ARAD_EGQ_CAL_SLOT_TRAVERSE_IN_CLOCKS * egq_resolution, &u64_1);

    rem = soc_sand_u64_devide_u64_long(&u64_1, calcal_instances * arad_chip_kilo_ticks_per_sec_get(unit), &u64_2);
    soc_sand_u64_to_long(&u64_2, &(port_credits[port_idx]));
    port_credits[port_idx] = (rem > 0 ? port_credits[port_idx] + 1 : port_credits[port_idx]);
  }

  for (slot_idx = 0; slot_idx < temp_calendar_length; ++slot_idx)
  {
    port_idx = slots_ports[slot_idx];
    if (port_idx == ARAD_OFP_RATES_UNMAP_BASE_Q_PAIR)
    {
      calendar->slots[slot_idx].credit = 0;
    }
    else if(port_num_slots[port_idx] == 0)
    {
      SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG_STR( "No slots port")));
    }
    else if(port_num_slots[port_idx] == 1)
    {
      calendar->slots[slot_idx].credit = port_credits[port_idx];
      --port_num_slots[port_idx];
    }
    else
    {
      calendar->slots[slot_idx].credit = port_credits[port_idx] / port_num_slots[port_idx];

      port_credits[port_idx] -= calendar->slots[slot_idx].credit;
      --port_num_slots[port_idx];
    }
    if(calendar->slots[slot_idx].credit > (1 << scm_cr_to_add_nof_bits))
    {
        
        
        SOCDNX_EXIT_WITH_ERR_NO_MSG(SOC_E_LIMIT);
    }
  }

exit:
  SOCDNX_FUNC_RETURN
}


int
  arad_ofp_rates_retrieve_egress_shaper_reg_field_names(
    SOC_SAND_IN  int                                   unit,
    SOC_SAND_IN  ARAD_OFP_RATES_CAL_INFO              *cal_info,
    SOC_SAND_IN  ARAD_OFP_RATES_CAL_SET                   cal2set,    
    SOC_SAND_IN  ARAD_OFP_RATES_EGQ_CHAN_ARB_FIELD_TYPE   field_type,
    SOC_SAND_OUT soc_reg_t                                *register_name,
    SOC_SAND_OUT soc_field_t                              *field_name
  )
{  

  SOCDNX_INIT_FUNC_DEFS

  SOCDNX_NULL_CHECK(register_name);
  SOCDNX_NULL_CHECK(field_name);
  SOCDNX_NULL_CHECK(cal_info);

  if (cal_info->cal_type == ARAD_OFP_RATES_EGQ_CAL_CHAN_ARB)
  {
    switch (cal_info->chan_arb_id)
    {
    case ARAD_OFP_RATES_EGQ_CHAN_ARB_00:
      *register_name = (cal2set == ARAD_OFP_RATES_CAL_SET_A) ? EGQ_EGRESS_SHAPER_CONFIGURATION_A_FOR_CHAN_INTERFACE_0r : 
        EGQ_EGRESS_SHAPER_CONFIGURATION_B_FOR_CHAN_INTERFACE_0r;
      switch (field_type)
      {
      case SOC_TMC_OFP_RATES_EGQ_CHAN_ARB_FIELD_RATE:
        *field_name = (cal2set == ARAD_OFP_RATES_CAL_SET_A) ? CH_0_SPR_RATE_Af:CH_0_SPR_RATE_Bf;
        break;
      case SOC_TMC_OFP_RATES_EGQ_CHAN_ARB_FIELD_MAX_BURST:
        *field_name = (cal2set == ARAD_OFP_RATES_CAL_SET_A) ? CH_0_SPR_MAX_BURST_Af:CH_0_SPR_MAX_BURST_Bf;      
        break;
      case SOC_TMC_OFP_RATES_EGQ_CHAN_ARB_FIELD_CAL_LEN:
        *field_name = (cal2set == ARAD_OFP_RATES_CAL_SET_A) ? CH_0_SPR_CAL_LEN_Af:CH_0_SPR_CAL_LEN_Bf;      
        break;
      default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG_STR( "Illegal enum")));
      }
      break;
    case ARAD_OFP_RATES_EGQ_CHAN_ARB_01:
      *register_name = (cal2set == ARAD_OFP_RATES_CAL_SET_A) ? EGQ_EGRESS_SHAPER_CONFIGURATION_A_FOR_CHAN_INTERFACE_1r : 
        EGQ_EGRESS_SHAPER_CONFIGURATION_B_FOR_CHAN_INTERFACE_1r;
      switch (field_type)
      {
      case SOC_TMC_OFP_RATES_EGQ_CHAN_ARB_FIELD_RATE:
        *field_name = (cal2set == ARAD_OFP_RATES_CAL_SET_A) ? CH_1_SPR_RATE_Af:CH_1_SPR_RATE_Bf;
        break;
      case SOC_TMC_OFP_RATES_EGQ_CHAN_ARB_FIELD_MAX_BURST:
        *field_name = (cal2set == ARAD_OFP_RATES_CAL_SET_A) ? CH_1_SPR_MAX_BURST_Af:CH_1_SPR_MAX_BURST_Bf;      
        break;
      case SOC_TMC_OFP_RATES_EGQ_CHAN_ARB_FIELD_CAL_LEN:
        *field_name = (cal2set == ARAD_OFP_RATES_CAL_SET_A) ? CH_1_SPR_CAL_LEN_Af:CH_1_SPR_CAL_LEN_Bf;      
        break;
      default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG_STR( "Illegal enum")));
      }
      break;
    case ARAD_OFP_RATES_EGQ_CHAN_ARB_02:
      *register_name = (cal2set == ARAD_OFP_RATES_CAL_SET_A) ? EGQ_EGRESS_SHAPER_CONFIGURATION_A_FOR_CHAN_INTERFACE_2r : 
        EGQ_EGRESS_SHAPER_CONFIGURATION_B_FOR_CHAN_INTERFACE_2r;
      switch (field_type)
      {
      case SOC_TMC_OFP_RATES_EGQ_CHAN_ARB_FIELD_RATE:
        *field_name = (cal2set == ARAD_OFP_RATES_CAL_SET_A) ? CH_2_SPR_RATE_Af:CH_2_SPR_RATE_Bf;
        break;
      case SOC_TMC_OFP_RATES_EGQ_CHAN_ARB_FIELD_MAX_BURST:
        *field_name = (cal2set == ARAD_OFP_RATES_CAL_SET_A) ? CH_2_SPR_MAX_BURST_Af:CH_2_SPR_MAX_BURST_Bf;      
        break;
      case SOC_TMC_OFP_RATES_EGQ_CHAN_ARB_FIELD_CAL_LEN:
        *field_name = (cal2set == ARAD_OFP_RATES_CAL_SET_A) ? CH_2_SPR_CAL_LEN_Af:CH_2_SPR_CAL_LEN_Bf;      
        break;
      default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG_STR( "Illegal enum")));
      }
      break;
    case ARAD_OFP_RATES_EGQ_CHAN_ARB_03:
      *register_name = (cal2set == ARAD_OFP_RATES_CAL_SET_A) ? EGQ_EGRESS_SHAPER_CONFIGURATION_A_FOR_CHAN_INTERFACE_3r : 
        EGQ_EGRESS_SHAPER_CONFIGURATION_B_FOR_CHAN_INTERFACE_3r;
      switch (field_type)
      {
      case SOC_TMC_OFP_RATES_EGQ_CHAN_ARB_FIELD_RATE:
        *field_name = (cal2set == ARAD_OFP_RATES_CAL_SET_A) ? CH_3_SPR_RATE_Af:CH_3_SPR_RATE_Bf;
        break;
      case SOC_TMC_OFP_RATES_EGQ_CHAN_ARB_FIELD_MAX_BURST:
        *field_name = (cal2set == ARAD_OFP_RATES_CAL_SET_A) ? CH_3_SPR_MAX_BURST_Af:CH_3_SPR_MAX_BURST_Bf;      
        break;
      case SOC_TMC_OFP_RATES_EGQ_CHAN_ARB_FIELD_CAL_LEN:
        *field_name = (cal2set == ARAD_OFP_RATES_CAL_SET_A) ? CH_3_SPR_CAL_LEN_Af:CH_3_SPR_CAL_LEN_Bf;      
        break;
      default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG_STR( "Illegal enum")));
      }
      break;
    case ARAD_OFP_RATES_EGQ_CHAN_ARB_04:
      *register_name = (cal2set == ARAD_OFP_RATES_CAL_SET_A) ? EGQ_EGRESS_SHAPER_CONFIGURATION_A_FOR_CHAN_INTERFACE_4r : 
        EGQ_EGRESS_SHAPER_CONFIGURATION_B_FOR_CHAN_INTERFACE_4r;
      switch (field_type)
      {
      case SOC_TMC_OFP_RATES_EGQ_CHAN_ARB_FIELD_RATE:
        *field_name = (cal2set == ARAD_OFP_RATES_CAL_SET_A) ? CH_4_SPR_RATE_Af:CH_4_SPR_RATE_Bf;
        break;
      case SOC_TMC_OFP_RATES_EGQ_CHAN_ARB_FIELD_MAX_BURST:
        *field_name = (cal2set == ARAD_OFP_RATES_CAL_SET_A) ? CH_4_SPR_MAX_BURST_Af:CH_4_SPR_MAX_BURST_Bf;      
        break;
      case SOC_TMC_OFP_RATES_EGQ_CHAN_ARB_FIELD_CAL_LEN:
        *field_name = (cal2set == ARAD_OFP_RATES_CAL_SET_A) ? CH_4_SPR_CAL_LEN_Af:CH_4_SPR_CAL_LEN_Bf;      
        break;
      default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG_STR( "Illegal enum")));
      }
      break;
    case ARAD_OFP_RATES_EGQ_CHAN_ARB_05:
      *register_name = (cal2set == ARAD_OFP_RATES_CAL_SET_A) ? EGQ_EGRESS_SHAPER_CONFIGURATION_A_FOR_CHAN_INTERFACE_5r : 
        EGQ_EGRESS_SHAPER_CONFIGURATION_B_FOR_CHAN_INTERFACE_5r;
      switch (field_type)
      {
      case SOC_TMC_OFP_RATES_EGQ_CHAN_ARB_FIELD_RATE:
        *field_name = (cal2set == ARAD_OFP_RATES_CAL_SET_A) ? CH_5_SPR_RATE_Af:CH_5_SPR_RATE_Bf;
        break;
      case SOC_TMC_OFP_RATES_EGQ_CHAN_ARB_FIELD_MAX_BURST:
        *field_name = (cal2set == ARAD_OFP_RATES_CAL_SET_A) ? CH_5_SPR_MAX_BURST_Af:CH_5_SPR_MAX_BURST_Bf;      
        break;
      case SOC_TMC_OFP_RATES_EGQ_CHAN_ARB_FIELD_CAL_LEN:
        *field_name = (cal2set == ARAD_OFP_RATES_CAL_SET_A) ? CH_5_SPR_CAL_LEN_Af:CH_5_SPR_CAL_LEN_Bf;      
        break;
      default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG_STR( "Illegal enum")));
      }
      break;
    case ARAD_OFP_RATES_EGQ_CHAN_ARB_06:
      *register_name = (cal2set == ARAD_OFP_RATES_CAL_SET_A) ? EGQ_EGRESS_SHAPER_CONFIGURATION_A_FOR_CHAN_INTERFACE_6r : 
        EGQ_EGRESS_SHAPER_CONFIGURATION_B_FOR_CHAN_INTERFACE_6r;
      switch (field_type)
      {
      case SOC_TMC_OFP_RATES_EGQ_CHAN_ARB_FIELD_RATE:
        *field_name = (cal2set == ARAD_OFP_RATES_CAL_SET_A) ? CH_6_SPR_RATE_Af:CH_6_SPR_RATE_Bf;
        break;
      case SOC_TMC_OFP_RATES_EGQ_CHAN_ARB_FIELD_MAX_BURST:
        *field_name = (cal2set == ARAD_OFP_RATES_CAL_SET_A) ? CH_6_SPR_MAX_BURST_Af:CH_6_SPR_MAX_BURST_Bf;      
        break;
      case SOC_TMC_OFP_RATES_EGQ_CHAN_ARB_FIELD_CAL_LEN:
        *field_name = (cal2set == ARAD_OFP_RATES_CAL_SET_A) ? CH_6_SPR_CAL_LEN_Af:CH_6_SPR_CAL_LEN_Bf;      
        break;
      default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG_STR( "Illegal enum")));
      }
      break;
    case ARAD_OFP_RATES_EGQ_CHAN_ARB_07:
      *register_name = (cal2set == ARAD_OFP_RATES_CAL_SET_A) ? EGQ_EGRESS_SHAPER_CONFIGURATION_A_FOR_CHAN_INTERFACE_7r : 
        EGQ_EGRESS_SHAPER_CONFIGURATION_B_FOR_CHAN_INTERFACE_7r;
      switch (field_type)
      {
      case SOC_TMC_OFP_RATES_EGQ_CHAN_ARB_FIELD_RATE:
        *field_name = (cal2set == ARAD_OFP_RATES_CAL_SET_A) ? CH_7_SPR_RATE_Af:CH_7_SPR_RATE_Bf;
        break;
      case SOC_TMC_OFP_RATES_EGQ_CHAN_ARB_FIELD_MAX_BURST:
        *field_name = (cal2set == ARAD_OFP_RATES_CAL_SET_A) ? CH_7_SPR_MAX_BURST_Af:CH_7_SPR_MAX_BURST_Bf;      
        break;
      case SOC_TMC_OFP_RATES_EGQ_CHAN_ARB_FIELD_CAL_LEN:
        *field_name = (cal2set == ARAD_OFP_RATES_CAL_SET_A) ? CH_7_SPR_CAL_LEN_Af:CH_7_SPR_CAL_LEN_Bf;      
        break;
      default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG_STR( "Illegal enum")));
      }
      break;
    case ARAD_OFP_RATES_EGQ_CHAN_ARB_CPU:
      *register_name = (cal2set == ARAD_OFP_RATES_CAL_SET_A) ? EGQ_EGRESS_SHAPER_CONFIGURATION_A_FOR_CHAN_INTERFACE_8r : 
        EGQ_EGRESS_SHAPER_CONFIGURATION_B_FOR_CHAN_INTERFACE_8r;
      switch (field_type)
      {
      case SOC_TMC_OFP_RATES_EGQ_CHAN_ARB_FIELD_RATE:
        *field_name = (cal2set == ARAD_OFP_RATES_CAL_SET_A) ? CH_8_SPR_RATE_Af:CH_8_SPR_RATE_Bf;
        break;
      case SOC_TMC_OFP_RATES_EGQ_CHAN_ARB_FIELD_MAX_BURST:
        *field_name = (cal2set == ARAD_OFP_RATES_CAL_SET_A) ? CH_8_SPR_MAX_BURST_Af:CH_8_SPR_MAX_BURST_Bf;      
        break;
      case SOC_TMC_OFP_RATES_EGQ_CHAN_ARB_FIELD_CAL_LEN:
        *field_name = (cal2set == ARAD_OFP_RATES_CAL_SET_A) ? CH_8_SPR_CAL_LEN_Af:CH_8_SPR_CAL_LEN_Bf;      
        break;
      default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG_STR( "Illegal enum")));
      }
      break;
    case ARAD_OFP_RATES_EGQ_CHAN_ARB_RCY:
      *register_name = (cal2set == ARAD_OFP_RATES_CAL_SET_A) ? EGQ_EGRESS_SHAPER_CONFIGURATION_A_FOR_CHAN_INTERFACE_9r : 
        EGQ_EGRESS_SHAPER_CONFIGURATION_B_FOR_CHAN_INTERFACE_9r;
      switch (field_type)
      {
      case SOC_TMC_OFP_RATES_EGQ_CHAN_ARB_FIELD_RATE:
        *field_name = (cal2set == ARAD_OFP_RATES_CAL_SET_A) ? CH_9_SPR_RATE_Af:CH_9_SPR_RATE_Bf;
        break;
      case SOC_TMC_OFP_RATES_EGQ_CHAN_ARB_FIELD_MAX_BURST:
        *field_name = (cal2set == ARAD_OFP_RATES_CAL_SET_A) ? CH_9_SPR_MAX_BURST_Af:CH_9_SPR_MAX_BURST_Bf;      
        break;
      case SOC_TMC_OFP_RATES_EGQ_CHAN_ARB_FIELD_CAL_LEN:
        *field_name = (cal2set == ARAD_OFP_RATES_CAL_SET_A) ? CH_9_SPR_CAL_LEN_Af:CH_9_SPR_CAL_LEN_Bf;      
        break;
      default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG_STR( "Illegal enum")));
      }
      break;
    case ARAD_OFP_RATES_EGQ_CHAN_ARB_NON_CHAN:
      *register_name = (cal2set == ARAD_OFP_RATES_CAL_SET_A) ? EGQ_EGRESS_SHAPER_CONFIGURATION_A_FOR_NON_CHAN_INTERFACESr : 
        EGQ_EGRESS_SHAPER_CONFIGURATION_B_FOR_NON_CHAN_INTERFACESr;
      switch (field_type)
      {
      case SOC_TMC_OFP_RATES_EGQ_CHAN_ARB_FIELD_RATE:
        *field_name = (cal2set == ARAD_OFP_RATES_CAL_SET_A) ? NONCH_SPR_RATE_Af:NONCH_SPR_RATE_Bf;
        break;
      case SOC_TMC_OFP_RATES_EGQ_CHAN_ARB_FIELD_MAX_BURST:
        *field_name = (cal2set == ARAD_OFP_RATES_CAL_SET_A) ? NONCH_SPR_MAX_BURST_Af:NONCH_SPR_MAX_BURST_Bf;      
        break;
      case SOC_TMC_OFP_RATES_EGQ_CHAN_ARB_FIELD_CAL_LEN:
        *field_name = (cal2set == ARAD_OFP_RATES_CAL_SET_A) ? NONCH_SPR_CAL_LEN_Af:NONCH_SPR_CAL_LEN_Bf;      
        break;
      default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG_STR( "Illegal enum")));
      }
      break;
    default:
      SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG_STR( "Illegal enum")));
    }
  } 
  else if (cal_info->cal_type == ARAD_OFP_RATES_EGQ_CAL_PORT_PRIORITY)
  {
    *register_name = (cal2set == ARAD_OFP_RATES_CAL_SET_A) ? EGQ_EGRESS_SHAPER_CONFIGURATION_A_FOR_QPAIRr:
      EGQ_EGRESS_SHAPER_CONFIGURATION_B_FOR_QPAIRr;
    switch (field_type)
    {
      case SOC_TMC_OFP_RATES_EGQ_CHAN_ARB_FIELD_RATE:
        *field_name = (cal2set == ARAD_OFP_RATES_CAL_SET_A) ? QPAIR_SPR_RATE_Af:QPAIR_SPR_RATE_Bf;
        break;
      case SOC_TMC_OFP_RATES_EGQ_CHAN_ARB_FIELD_MAX_BURST:
        *field_name = (cal2set == ARAD_OFP_RATES_CAL_SET_A) ? QPAIR_SPR_MAX_BURST_Af:QPAIR_SPR_MAX_BURST_Bf;      
        break;
      case SOC_TMC_OFP_RATES_EGQ_CHAN_ARB_FIELD_CAL_LEN:
        *field_name = (cal2set == ARAD_OFP_RATES_CAL_SET_A) ? QPAIR_SPR_CAL_LEN_Af:QPAIR_SPR_CAL_LEN_Bf;      
        break;
      default:
       SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG_STR( "Illegal enum")));
    }   
  } 
  else if (cal_info->cal_type == ARAD_OFP_RATES_EGQ_CAL_TCG)
  {
    *register_name = (cal2set == ARAD_OFP_RATES_CAL_SET_A) ? EGQ_EGRESS_SHAPER_CONFIGURATION_A_FOR_TCGr:
      EGQ_EGRESS_SHAPER_CONFIGURATION_B_FOR_TCGr;
    switch (field_type)
    {
      case SOC_TMC_OFP_RATES_EGQ_CHAN_ARB_FIELD_RATE:
        *field_name = (cal2set == ARAD_OFP_RATES_CAL_SET_A) ? TCG_SPR_RATE_Af:TCG_SPR_RATE_Bf;
        break;
      case SOC_TMC_OFP_RATES_EGQ_CHAN_ARB_FIELD_MAX_BURST:
        *field_name = (cal2set == ARAD_OFP_RATES_CAL_SET_A) ? TCG_SPR_MAX_BURST_Af:TCG_SPR_MAX_BURST_Bf;      
        break;
      case SOC_TMC_OFP_RATES_EGQ_CHAN_ARB_FIELD_CAL_LEN:
        *field_name = (cal2set == ARAD_OFP_RATES_CAL_SET_A) ? TCG_SPR_CAL_LEN_Af:TCG_SPR_CAL_LEN_Bf;      
        break;
      default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG_STR( "Illegal enum")));
    }
  } 
  else 
  {
    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG_STR( "Illegal enum")));
  }

exit:
  SOCDNX_FUNC_RETURN
}
  

STATIC uint32
  arad_ofp_rates_active_generic_calendars_retrieve_egq(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  int                       core,
    SOC_SAND_IN  ARAD_OFP_RATES_CAL_INFO      *cal_info,
    SOC_SAND_OUT ARAD_OFP_RATES_CAL_EGQ       *egq_cal,
    SOC_SAND_OUT uint32                     *egq_cal_len,
    SOC_SAND_IN  uint8                    remove_dummies
  )
{
  uint32
    res,
    fld_val = 0,
    temp_egq_cal_len,
    reg_val,
    egq_len = 0;
  uint32
    offset = 0,
    slot = 0;
  ARAD_EGQ_SCM_TBL_DATA
    egq_data;
  uint32
    cal2get;
  ARAD_OFP_EGQ_RATES_CAL_ENTRY
    *cal_slot = NULL;
  soc_field_t
    field_cal_set;

  SOCDNX_INIT_FUNC_DEFS

  SOCDNX_NULL_CHECK(cal_info);
  SOCDNX_NULL_CHECK(egq_cal);
  SOCDNX_NULL_CHECK(egq_cal_len);

  *egq_cal_len = 0;

  res = arad_ofp_rates_retrieve_egress_shaper_setting_field(
          unit,
          cal_info,
          &field_cal_set
        );
  SOCDNX_IF_ERR_EXIT(res);

  SOCDNX_SAND_IF_ERR_EXIT(READ_EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr(unit, core, &reg_val));
  fld_val = soc_reg_field_get(unit, EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr, reg_val, field_cal_set);
  cal2get = fld_val;

    

  
  res = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_ofp_rates_egress_shaper_cal_read, (unit, core, cal_info, cal2get, 
                                                                                SOC_TMC_OFP_RATES_EGQ_CHAN_ARB_FIELD_CAL_LEN, &egq_len));
  SOCDNX_IF_ERR_EXIT(res);

  
  
  offset = ARAD_OFP_RATES_EGQ_SCM_OFFSET_GET(unit, cal_info,cal2get);
  for (slot = 0; slot < egq_len + 1; ++slot)
  {
    cal_slot = egq_cal->slots + slot;

    res = arad_egq_scm_tbl_get_unsafe(
            unit,
            core,
            cal_info,
            offset + slot,
            &egq_data
          );
    SOCDNX_SAND_IF_ERR_EXIT(res);
    
    cal_slot->credit = egq_data.port_cr_to_add;
    cal_slot->base_q_pair = egq_data.ofp_index;
  }

  
  temp_egq_cal_len = egq_len + 1;

  
  if (remove_dummies)
  {
    for (slot = 0; slot < temp_egq_cal_len; ++slot)
    {
      cal_slot = egq_cal->slots + slot;
      if (arad_ofp_rates_is_cal_entry_dummy(cal_slot))
      {
        SOC_COPY(egq_cal->slots + slot, egq_cal->slots + slot + 1, ARAD_OFP_EGQ_RATES_CAL_ENTRY, (temp_egq_cal_len - slot - 1));
        temp_egq_cal_len -= 1;
      }
    }
  }

  *egq_cal_len = temp_egq_cal_len;

exit:
  SOCDNX_FUNC_RETURN
}

int 
arad_ofp_rates_egress_shaper_reg_field_read (
    SOC_SAND_IN  int                                      unit,
    SOC_SAND_IN  int                                      core,
    SOC_SAND_IN  ARAD_OFP_RATES_CAL_INFO                  *cal_info,   
    SOC_SAND_IN  soc_reg_t                                register_name,
    SOC_SAND_IN  soc_field_t                              field_name,
    SOC_SAND_OUT uint32                                   *data
    )
{
    uint64 field64;

    SOCDNX_INIT_FUNC_DEFS

    SOC_FIELD64_GET(register_name, field_name, field64);
    *data = COMPILER_64_LO(field64);

exit:
    SOCDNX_FUNC_RETURN
}

int 
arad_ofp_rates_egress_shaper_reg_field_write (
    SOC_SAND_IN  int                                      unit,
    SOC_SAND_IN  int                                      core,
    SOC_SAND_IN  ARAD_OFP_RATES_CAL_INFO                  *cal_info,   
    SOC_SAND_IN  soc_reg_t                                register_name,
    SOC_SAND_IN  soc_field_t                              field_name,
    SOC_SAND_OUT uint32                                   data
    )
{
    uint64 field64;

    SOCDNX_INIT_FUNC_DEFS

    COMPILER_64_SET(field64,0,data);
    SOC_FIELD64_SET(register_name, field_name, field64);

exit:
    SOCDNX_FUNC_RETURN
}

STATIC
  uint32
    arad_ofp_rates_from_egq_ports_rates_to_generic_calendar_chn_arb_aux(
      SOC_SAND_IN  int                       unit,   
      SOC_SAND_IN  uint32                     *ports_rates,
      SOC_SAND_IN  uint32                    nof_ports,
      SOC_SAND_IN  uint32                     total_shaper_bandwidth,
      SOC_SAND_IN  uint32                     max_calendar_len,
      SOC_SAND_OUT uint32                   *loc_calendar_len,
      SOC_SAND_OUT uint32                   *port_nof_slots,
      SOC_SAND_OUT uint32                   *slots_ports,
      SOC_SAND_IN  uint32                    is_alt_calc
    )
{
    uint32 res = SOC_E_NONE,
           idx,
           add_dummy_tail = 0,
           nof_dup_entries,
           multiplier = 1,
           last_port_entry = ARAD_OFP_RATES_ILLEGAL_SCHEDULER_ID,
           act_nof_ports = 0;

    SOCDNX_INIT_FUNC_DEFS
    
    if (SOC_IS_ARADPLUS_AND_BELOW(unit))
    {
        add_dummy_tail = 1;
    }

    
    for (idx = 0; idx < nof_ports; ++idx)
    {
        act_nof_ports += (ports_rates[idx] > 0 ? 1 : 0);
    }

    do
    {
        nof_dup_entries = 0;

        
        if (max_calendar_len / multiplier < act_nof_ports)
        {
          SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG_STR( "NOF ports greater than max calendar len")));
        }

        
        res = arad_ofp_rates_cal_len_calculate(
                unit,
                ports_rates,
                nof_ports,
                total_shaper_bandwidth,
                max_calendar_len / multiplier - add_dummy_tail,
                loc_calendar_len,
                port_nof_slots
              );
        SOCDNX_IF_ERR_EXIT(res);

        
        res = arad_ofp_rates_fixed_len_cal_build(
                unit,
                port_nof_slots,
                nof_ports,
                *loc_calendar_len,
                max_calendar_len / multiplier - add_dummy_tail,
                0,
                is_alt_calc,
                slots_ports
              );
        SOCDNX_IF_ERR_EXIT(res);

        
        if (SOC_IS_ARADPLUS_AND_BELOW(unit))
        {
            
            for (idx = 0; idx < *loc_calendar_len; ++idx)
            {
              nof_dup_entries += (last_port_entry == slots_ports[idx] ? 1 : 0);
              last_port_entry = slots_ports[idx];
            }
        }

        if ((multiplier *= 2) > ARAD_OFP_RATES_NOF_ITERATIONS)
        {
          SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG_STR( "Failed to calculate calendar")));
        }

    } while(*loc_calendar_len + nof_dup_entries + add_dummy_tail > max_calendar_len);

exit:
  SOCDNX_FUNC_RETURN
}


STATIC
  uint32
    arad_ofp_rates_from_egq_ports_rates_to_generic_calendar_ptc_tcg_aux(
      SOC_SAND_IN  int                       unit,   
      SOC_SAND_IN  uint32                     *ports_rates,
      SOC_SAND_IN  uint32                    nof_ports,
      SOC_SAND_IN  uint32                     max_calendar_len,
      SOC_SAND_OUT uint32                   *loc_calendar_len,
      SOC_SAND_OUT uint32                   *port_nof_slots,
      SOC_SAND_OUT uint32                   *slots_ports
    )
{
    uint32 res = SOC_E_NONE,
           idx,
           nof_dup_entries = 0,
           last_port_entry = ARAD_OFP_RATES_ILLEGAL_SCHEDULER_ID,
           act_nof_ports = 0;
    uint8  slots_added;

    SOCDNX_INIT_FUNC_DEFS
      
      
    for (idx = 0; idx < nof_ports; ++idx)
    {
        if (ports_rates[idx] > 0)
        {
          act_nof_ports++;
          port_nof_slots[idx] = 1;
        }        
    }

    *loc_calendar_len = act_nof_ports;

    if (max_calendar_len < act_nof_ports)
    {
      SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG_STR( "NOF ports greater than max_calendar_len")));
    }
    do {
      
      slots_added = FALSE;
      res = arad_ofp_rates_cal_improve_nof_slots(
            unit,
            ports_rates,
            nof_ports,
            max_calendar_len,
            loc_calendar_len,
            port_nof_slots,
            &slots_added
          );      
      SOCDNX_IF_ERR_EXIT(res);
    } while (slots_added);

    if (act_nof_ports == 0)
    {
        port_nof_slots[0] = 1;
        act_nof_ports = 1;
        *loc_calendar_len = 1;
    }

    
    res = arad_ofp_rates_fixed_len_cal_build(
            unit,
            port_nof_slots,
            nof_ports,
            *loc_calendar_len,
            max_calendar_len, 
            0,
            0,
            slots_ports
          );
    SOCDNX_IF_ERR_EXIT(res);

    for (idx = 0; idx < *loc_calendar_len; ++idx)
    {
        nof_dup_entries += (last_port_entry == slots_ports[idx] ? 1 : 0);
        last_port_entry = slots_ports[idx];
    }
    
    if (*loc_calendar_len + nof_dup_entries > max_calendar_len)
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG_STR( "Failed to calculate calendar")));
    }

exit:
  SOCDNX_FUNC_RETURN
}

STATIC
  uint32
    arad_ofp_rates_from_egq_ports_rates_to_generic_calendar(
      SOC_SAND_IN  int                       unit,
      SOC_SAND_IN  int                       core,
      SOC_SAND_IN  ARAD_OFP_RATES_CAL_INFO  *cal_info,   
      SOC_SAND_IN  uint32                     *ports_rates,
      SOC_SAND_IN  uint32                    nof_ports,
      SOC_SAND_IN  uint32                     total_shaper_bandwidth,
      SOC_SAND_IN  uint32                     max_calendar_len,
      SOC_SAND_IN  uint32                     recalc,
      SOC_SAND_IN  uint8                    add_dummy_tail,
      SOC_SAND_OUT ARAD_OFP_RATES_CAL_EGQ       *calendar,
      SOC_SAND_OUT uint32                     *calendar_len,
      SOC_SAND_IN  uint32                    is_alt_calc
    )
{
  uint32
    res = SOC_E_NONE;
  uint32
    idx = 0;
  uint32
    loc_calendar_len;
  uint32
    *port_nof_slots = NULL,
    *temp_buff = NULL,
    *slots_ports = NULL;
  uint32
    last_port_entry;

  SOCDNX_INIT_FUNC_DEFS;

  SOCDNX_ALLOC(port_nof_slots, uint32, ARAD_NOF_FAP_PORTS, "port_nof_slots");
  SOCDNX_ALLOC(slots_ports, uint32, ARAD_OFP_RATES_CAL_LEN_EGQ_MAX, "slots_ports");
  SOCDNX_ALLOC(temp_buff, uint32, ARAD_OFP_RATES_CAL_LEN_EGQ_MAX, "temp_buff");
  
  if (recalc) {
      
        if (cal_info->cal_type == ARAD_OFP_RATES_EGQ_CAL_CHAN_ARB) {
            res = arad_ofp_rates_from_egq_ports_rates_to_generic_calendar_chn_arb_aux(unit,
                                                                                     ports_rates,
                                                                                     nof_ports,
                                                                                     total_shaper_bandwidth,
                                                                                     max_calendar_len,
                                                                                     &loc_calendar_len,
                                                                                     port_nof_slots,
                                                                                     slots_ports,
                                                                                     is_alt_calc);
            SOCDNX_IF_ERR_EXIT(res);
        }
        else { 
            res = arad_ofp_rates_from_egq_ports_rates_to_generic_calendar_ptc_tcg_aux(unit,
                                                                                      ports_rates,
                                                                                      nof_ports,
                                                                                      max_calendar_len,
                                                                                      &loc_calendar_len,
                                                                                       port_nof_slots,
                                                                                      slots_ports);
            SOCDNX_IF_ERR_EXIT(res);
        }
  
        last_port_entry = ARAD_OFP_RATES_ILLEGAL_SCHEDULER_ID;

        
        if (SOC_IS_ARADPLUS_AND_BELOW(unit))
        {
            for (idx = 0; idx < loc_calendar_len; ++idx)
            {
                if (last_port_entry == slots_ports[idx])
                {
                    if (loc_calendar_len > ARAD_OFP_RATES_CAL_LEN_EGQ_MAX) {
                        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG_STR( "loc_calendar_len is above max")));
                    }
    
                    
                    SOC_COPY(temp_buff, slots_ports, uint32, ARAD_OFP_RATES_CAL_LEN_EGQ_MAX);
                    SOC_COPY(temp_buff + idx + 1, slots_ports + idx, uint32, loc_calendar_len - idx);
                    SOC_COPY(slots_ports, temp_buff, uint32, ARAD_OFP_RATES_CAL_LEN_EGQ_MAX);
                    slots_ports[idx] = ARAD_OFP_RATES_DUMMY_PORT_ID;
    
                    
                    loc_calendar_len += 1;
                }
                last_port_entry = slots_ports[idx];
            }
        }
        
        if (add_dummy_tail)
        {
            loc_calendar_len += 1;
        }

        if (loc_calendar_len > ARAD_OFP_RATES_CAL_LEN_EGQ_MAX) {
              SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG_STR( "loc_calendar_len is above max")));
        }
  } else {
      res = arad_ofp_rates_active_generic_calendars_retrieve_egq(
            unit,
            core,
            cal_info,
            calendar,
            &loc_calendar_len,
            FALSE
          );
      SOCDNX_IF_ERR_EXIT(res);

      for (idx = 0; idx < loc_calendar_len; ++idx)
      {
          slots_ports[idx] = calendar->slots[idx].base_q_pair;
      }
  }

  
  res = arad_ofp_rates_fill_shaper_generic_calendar_credits(
          unit,
          core,
          cal_info,
          ports_rates,
          nof_ports,
          loc_calendar_len,
          add_dummy_tail,
          slots_ports,
          calendar
        );
  
  if(res == SOC_E_LIMIT)
  {
    SOCDNX_EXIT_WITH_ERR_NO_MSG(res);
  }
  SOCDNX_IF_ERR_EXIT(res);

  if (add_dummy_tail)
  {
      
      calendar->slots[loc_calendar_len - 1].base_q_pair = ARAD_OFP_RATES_DUMMY_PORT_ID;
      calendar->slots[loc_calendar_len - 1].credit = 0;    
  }
    *calendar_len = loc_calendar_len;

exit:
  SOCDNX_FREE(port_nof_slots);
  SOCDNX_FREE(slots_ports);
  SOCDNX_FREE(temp_buff);

  SOCDNX_FUNC_RETURN
}


STATIC uint32
  arad_ofp_rates_from_calendar_to_ports_sch_rate(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  ARAD_OFP_RATES_CAL_SCH    *calendar,
    SOC_SAND_IN  uint32                    calendar_len,
    SOC_SAND_IN  uint32                    total_sch_rate,
    SOC_SAND_IN  uint32                    nof_ports,
    SOC_SAND_IN  int                       requested_base_q_pair, 
    SOC_SAND_OUT uint32                    *ports_rates
  )
{
    uint32
        port,
        sched,
        calc,
        slot_id;
    SOC_SAND_U64
        u64_1,
        u64_2;
    uint32
        rem;

    SOCDNX_INIT_FUNC_DEFS

    if (0 == calendar_len)
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG_STR( "Failed to calculate calendar")));
    }

    for (port = 0; port < nof_ports; ++port)
    {
        ports_rates[port] = 0;
    }

    if ((calendar_len == 1) && (calendar->slots[0] == ARAD_FAP_PORT_ID_INVALID))
    {
        ARAD_DO_NOTHING_AND_EXIT;
    }

    for (slot_id = 0; slot_id < calendar_len; ++slot_id)
    {
        sched = calendar->slots[slot_id];
        if (sched >= nof_ports)
        {
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG_STR( "Invalid port")));
        }

        
        ++ports_rates[sched];
    }

    
    if(requested_base_q_pair != ARAD_EGR_INVALID_BASE_Q_PAIR) {
        soc_sand_u64_multiply_longs(ports_rates[requested_base_q_pair], total_sch_rate, &u64_1);
        rem = soc_sand_u64_devide_u64_long(&u64_1, calendar_len, &u64_2);
        soc_sand_u64_to_long(&u64_2, &(calc));
        calc = (rem > 0 ? calc + 1 : calc);

        ports_rates[requested_base_q_pair] = calc;
    } else {
        for (port = 0; port < nof_ports; ++port)
        {
            soc_sand_u64_multiply_longs(ports_rates[port], total_sch_rate, &u64_1);
            rem = soc_sand_u64_devide_u64_long(&u64_1, calendar_len, &u64_2);
            soc_sand_u64_to_long(&u64_2, &(calc));
            calc = (rem > 0 ? calc + 1 : calc);

            ports_rates[port] = calc;
        }
    }

exit:
    SOCDNX_FUNC_RETURN
}

STATIC uint32
  arad_ofp_rates_from_generic_calendar_to_ports_egq_rate(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  int                       core,
    SOC_SAND_IN  ARAD_OFP_RATES_CAL_INFO     *cal_info,
    SOC_SAND_IN  ARAD_OFP_RATES_CAL_EGQ      *calendar,
    SOC_SAND_IN  uint32                     calendar_len,
    SOC_SAND_IN  uint8                    dummy_entry_supported,
    SOC_SAND_IN  uint32                    nof_ports,
    SOC_SAND_OUT uint32                     *ports_rates
  )
{
  uint32
    port_idx,
    calc,
    slot_id,
    egq_resolution;
  const ARAD_OFP_EGQ_RATES_CAL_ENTRY
    *slot;
  SOC_SAND_U64
    u64_1,
    u64_2;
  uint32
    rem;
  uint32
    tmp_calendar_len,
    calcal_length,
    calcal_instances;

  SOCDNX_INIT_FUNC_DEFS

  SOCDNX_NULL_CHECK(cal_info);
  SOCDNX_NULL_CHECK(calendar);

  if (0 == calendar_len)
  {
    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG_STR( "Illegal calendar len")));
  }

  for (port_idx = 0; port_idx < nof_ports; ++port_idx)
  {
    ports_rates[port_idx] = 0;
  }

  
  tmp_calendar_len = (dummy_entry_supported) ? calendar_len-1:calendar_len;

  for (slot_id = 0; slot_id < tmp_calendar_len; ++slot_id)
  {
    slot = &calendar->slots[slot_id];

    if (dummy_entry_supported && slot->base_q_pair == ARAD_OFP_RATES_DUMMY_PORT_ID)
    {
      continue;
    }
    if (slot->base_q_pair >= nof_ports)
    {
      SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG_STR( "Illegal base_q_pair")));
    }
    
    ports_rates[slot->base_q_pair] += slot->credit;
  }

    
  if (cal_info->cal_type == ARAD_OFP_RATES_EGQ_CAL_CHAN_ARB)
  {
    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.calcal_length.get(unit,
        core, &calcal_length));
    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.chan_arb.nof_calcal_instances.get(unit,
        core, cal_info->chan_arb_id, &calcal_instances));
  }
  else
  {
    
    calcal_length = 1;
    calcal_instances = 1;
  }

  if(SOC_IS_ARAD_A0(unit) || (cal_info->cal_type==ARAD_OFP_RATES_EGQ_CAL_CHAN_ARB)) {
    
    egq_resolution = ARAD_EGQ_UNITS_VAL_IN_BITS_CHAN_ARB;
  } else {
    
    egq_resolution = ARAD_EGQ_UNITS_VAL_IN_BITS_QPAIR_TCG;
  }

  for (port_idx = 0; port_idx < nof_ports; ++port_idx)
  {
    soc_sand_u64_multiply_longs(calcal_instances * ports_rates[port_idx], arad_chip_kilo_ticks_per_sec_get(unit), &u64_1);
    rem = soc_sand_u64_devide_u64_long(&u64_1, calcal_length * calendar_len * egq_resolution * ARAD_EGQ_CAL_SLOT_TRAVERSE_IN_CLOCKS, &u64_2);
    soc_sand_u64_to_long(&u64_2, &(calc));
    calc = (rem > 0 ? calc + 1 : calc);

    ports_rates[port_idx] = calc;
  }

exit:
  SOCDNX_FUNC_RETURN
}




uint32
  arad_ofp_rates_egq_shaper_rate_to_internal(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  ARAD_OFP_RATES_EGQ_CAL_TYPE cal_type,
    SOC_SAND_IN  uint32                      rate_kbps,
    SOC_SAND_IN  int                         round_up_flag,
    SOC_SAND_OUT uint32                     *rate_internal
  )
{
  uint32
    device_ticks_per_sec,
    rate_int,
    reminder,
    egq_resolution;
  SOC_SAND_U64
    calc,
    calc2;

  SOCDNX_INIT_FUNC_DEFS

  device_ticks_per_sec = arad_chip_ticks_per_sec_get(unit);

  if (device_ticks_per_sec == 0)
  {
    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG_STR( "NOF ticks equals 0")));
  }

  if (rate_kbps == 0)
  {
    rate_int = 0;
  }
  else
  {
    if(SOC_IS_ARAD_A0(unit) || (cal_type==ARAD_OFP_RATES_EGQ_CAL_CHAN_ARB)) {
        
        egq_resolution = ARAD_EGQ_UNITS_VAL_IN_BITS_CHAN_ARB;
    } else {
        
        egq_resolution = ARAD_EGQ_UNITS_VAL_IN_BITS_QPAIR_TCG;
    }

    soc_sand_u64_multiply_longs(rate_kbps, (1000 * egq_resolution), &calc);

    reminder = soc_sand_u64_devide_u64_long(&calc, device_ticks_per_sec, &calc2);
    if ((reminder != 0) && (round_up_flag == TRUE))
    {
        
        if(soc_sand_u64_add_long(&calc2, 1))
        {
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG_STR( "reminder!=0 and Overflow")));
        }
    }   
    if (soc_sand_u64_to_long(&calc2, &rate_int))
    {
      SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG_STR( "Overflow")));
    }
  }

  *rate_internal =  rate_int;

exit:
  SOCDNX_FUNC_RETURN
}


uint32
  arad_ofp_rates_egq_shaper_rate_from_internal(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  ARAD_OFP_RATES_EGQ_CAL_TYPE cal_type,
    SOC_SAND_IN  uint32                      rate_internal,
    SOC_SAND_OUT uint32                     *rate_kbps
  )
{
  uint32
    device_ticks_per_sec,
    rate_kb,
    egq_resolution;
  SOC_SAND_U64
    calc,
    calc2;

  SOCDNX_INIT_FUNC_DEFS

  device_ticks_per_sec = arad_chip_ticks_per_sec_get(unit);
  if (rate_internal == 0)
  {
    rate_kb = 0;
  }
  else
  {
      soc_sand_u64_multiply_longs(rate_internal, device_ticks_per_sec, &calc);

      if(SOC_IS_ARAD_A0(unit) || (cal_type==ARAD_OFP_RATES_EGQ_CAL_CHAN_ARB)) {
        
        egq_resolution = ARAD_EGQ_UNITS_VAL_IN_BITS_CHAN_ARB;
      } else {
        
        egq_resolution = ARAD_EGQ_UNITS_VAL_IN_BITS_QPAIR_TCG;
      }

      soc_sand_u64_devide_u64_long(&calc, (1000 * egq_resolution), &calc2);

      if (soc_sand_u64_to_long(&calc2, &rate_kb))
      {
          SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG_STR( "Overflow")));
      }
  }

  *rate_kbps =  rate_kb;

exit:
  SOCDNX_FUNC_RETURN
}




STATIC uint32
  arad_ofp_rates_egq_shaper_config(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  ARAD_OFP_RATES_CAL_INFO       *cal_info,
    SOC_SAND_IN  uint32                         rate
  )
{
  uint32
    res = SOC_E_NONE;
  uint32
    fld_val = 0;
  uint64
    field64;
  soc_reg_t
    reg_shpr_name[ARAD_OFP_NOF_RATES_CAL_SETS];
  soc_field_t
    field_shpr_rate_name[ARAD_OFP_NOF_RATES_CAL_SETS],
    field_shpr_burst_name[ARAD_OFP_NOF_RATES_CAL_SETS];
  ARAD_OFP_RATES_CAL_SET
    cal2set;

  SOCDNX_INIT_FUNC_DEFS  

  COMPILER_64_ZERO(field64);
  
    for (cal2set = ARAD_OFP_RATES_CAL_SET_A; cal2set < ARAD_OFP_NOF_RATES_CAL_SETS; ++cal2set) {

        res = arad_ofp_rates_retrieve_egress_shaper_reg_field_names(
                unit,
                cal_info,
                cal2set,
                SOC_TMC_OFP_RATES_EGQ_CHAN_ARB_FIELD_RATE,
                &(reg_shpr_name[cal2set]),
                &(field_shpr_rate_name[cal2set])
              );
        SOCDNX_IF_ERR_EXIT(res);

        res = arad_ofp_rates_retrieve_egress_shaper_reg_field_names(
                unit,
                cal_info,
                cal2set,
                SOC_TMC_OFP_RATES_EGQ_CHAN_ARB_FIELD_MAX_BURST,
                &(reg_shpr_name[cal2set]),
                &(field_shpr_burst_name[cal2set])
              );
        SOCDNX_IF_ERR_EXIT(res);
    }

    res = arad_ofp_rates_egq_shaper_rate_to_internal(
            unit,
            cal_info->cal_type,
            rate,
            FALSE,
            &fld_val
          );
    SOCDNX_IF_ERR_EXIT(res);

    

    for (cal2set = ARAD_OFP_RATES_CAL_SET_A; cal2set < ARAD_OFP_NOF_RATES_CAL_SETS; ++cal2set) {
      COMPILER_64_ZERO(field64);
      COMPILER_64_ADD_32(field64,fld_val);
      SOC_FIELD64_SET(reg_shpr_name[cal2set], field_shpr_rate_name[cal2set], field64);
      COMPILER_64_SET(field64, 0, 0x1fff);
      SOC_FIELD64_SET(reg_shpr_name[cal2set], field_shpr_burst_name[cal2set], field64);
    }

exit:
  SOCDNX_FUNC_RETURN
}


STATIC uint32
  arad_ofp_rates_egq_shaper_retrieve(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  ARAD_OFP_RATES_EGQ_CHAN_ARB_ID   chan_arb_id,
    SOC_SAND_IN  uint8                    rate_enable,
    SOC_SAND_OUT  uint32                    *rate
  )
{
  uint32
    res = SOC_E_NONE;
  uint32
    fld_val;  
  soc_reg_t
    reg_egress_shpr;
  soc_field_t
    field_egress_shpr_rate;
  ARAD_OFP_RATES_CAL_INFO
    cal_info;
  uint64
    field64;

  SOCDNX_INIT_FUNC_DEFS

  arad_ARAD_OFP_RATES_CAL_INFO_clear(&cal_info);

  cal_info.cal_type = ARAD_OFP_RATES_EGQ_CAL_CHAN_ARB;
  cal_info.chan_arb_id = chan_arb_id;

  res = arad_ofp_rates_retrieve_egress_shaper_reg_field_names(
          unit,
          &cal_info,
          ARAD_OFP_RATES_CAL_SET_A,
          SOC_TMC_OFP_RATES_EGQ_CHAN_ARB_FIELD_RATE,
          &reg_egress_shpr,
          &field_egress_shpr_rate
        );
  SOCDNX_IF_ERR_EXIT(res);

  if (rate_enable)
  {
    SOC_FIELD64_GET(reg_egress_shpr, field_egress_shpr_rate, field64);
    fld_val = COMPILER_64_LO(field64);

    res = arad_ofp_rates_egq_shaper_rate_from_internal(
            unit,
            cal_info.cal_type,
            fld_val,
            rate
          );
    SOCDNX_IF_ERR_EXIT(res);
  }

exit:
  SOCDNX_FUNC_RETURN
}

uint32
  arad_ofp_rates_max_credit_empty_port_set(
     SOC_SAND_IN int    unit,
     SOC_SAND_IN int arg
     )
{
    int    core;
    uint32 reg_val;

    SOCDNX_INIT_FUNC_DEFS

    if (SOC_IS_ARADPLUS(unit)) {
        SOC_DPP_CORES_ITER(SOC_CORE_ALL, core) {
            
            SOCDNX_IF_ERR_EXIT(READ_EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr(unit, core, &reg_val));
            soc_reg_field_set(unit, EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr, &reg_val, EMPTY_PORT_STOP_COLLECTING_ENf, arg ? 1 : 0);
            SOCDNX_IF_ERR_EXIT(WRITE_EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr(unit, core, reg_val));

            
            SOCDNX_IF_ERR_EXIT(READ_EGQ_EMPTY_PORT_MAX_CREDIT_VALUEr(unit, core, &reg_val));
            soc_reg_field_set(unit, EGQ_EMPTY_PORT_MAX_CREDIT_VALUEr, &reg_val, EMPTY_PORT_MAX_CREDITf, arg);
            SOCDNX_IF_ERR_EXIT(WRITE_EGQ_EMPTY_PORT_MAX_CREDIT_VALUEr(unit, core, reg_val));
        }
    }

exit:
  SOCDNX_FUNC_RETURN
}

uint32
  arad_ofp_rates_max_credit_empty_port_get(
     SOC_SAND_IN int    unit,
     SOC_SAND_OUT int* arg
     )
{
    uint32 reg_val;
    SOCDNX_INIT_FUNC_DEFS

    if (SOC_IS_ARADPLUS(unit)) {
        
        SOCDNX_IF_ERR_EXIT(READ_EGQ_EMPTY_PORT_MAX_CREDIT_VALUEr(unit, 0, &reg_val));
        *arg = soc_reg_field_get(unit, EGQ_EMPTY_PORT_MAX_CREDIT_VALUEr, reg_val, EMPTY_PORT_MAX_CREDITf);
    }

exit:
  SOCDNX_FUNC_RETURN
}


STATIC uint32
  arad_ofp_rates_tcg_id_egq_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  int                       core,
    SOC_SAND_IN  uint32                    tm_port,
    SOC_SAND_IN  ARAD_TCG_NDX              tcg_ndx,
    SOC_SAND_OUT uint8                     *is_egq_valid
  )
{
  uint32
    res;
  uint32
    nof_priorities,
    priority_i;
  ARAD_EGR_QUEUING_TCG_INFO
    tcg_port_info;

  SOCDNX_INIT_FUNC_DEFS

  SOCDNX_NULL_CHECK(is_egq_valid);

  *is_egq_valid = FALSE;
    
  res = soc_port_sw_db_tm_port_to_out_port_priority_get(unit, core, tm_port, &nof_priorities);
  SOCDNX_SAND_IF_ERR_EXIT(res);

  ARAD_EGR_QUEUING_TCG_INFO_clear(&tcg_port_info);
  res = arad_egr_queuing_ofp_tcg_get_unsafe(
          unit,
          core,
          tm_port,            
          &tcg_port_info
        );
  SOCDNX_SAND_IF_ERR_EXIT(res);


  for (priority_i = 0; priority_i < nof_priorities; ++priority_i)
  {
    if (tcg_port_info.tcg_ndx[priority_i] == tcg_ndx)
    {
      *is_egq_valid = TRUE;
      break;
    }
  }

exit:
  SOCDNX_FUNC_RETURN
}

STATIC uint32
  arad_ofp_rates_tcg_id_sch_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  int                       core, 
    SOC_SAND_IN  uint32                    tm_port,
    SOC_SAND_IN  ARAD_TCG_NDX              tcg_ndx,
    SOC_SAND_OUT uint8                     *is_sch_valid
  )
{
  uint32 
    res;
  uint32
    priority_i,
    nof_priorities;
  ARAD_SCH_PORT_INFO
    sch_port_info;

  SOCDNX_INIT_FUNC_DEFS

  SOCDNX_NULL_CHECK(is_sch_valid);

  arad_ARAD_SCH_PORT_INFO_clear(&sch_port_info);  

  res = soc_port_sw_db_tm_port_to_out_port_priority_get(unit, core, tm_port, &nof_priorities);
  SOCDNX_SAND_IF_ERR_EXIT(res);

  *is_sch_valid = FALSE;

  res = arad_sch_port_sched_get_unsafe(
            unit,
            core,
            tm_port,
            &sch_port_info
          );
  SOCDNX_SAND_IF_ERR_EXIT(res);

  for (priority_i = 0; priority_i < nof_priorities; ++priority_i)
  {
    if (sch_port_info.tcg_ndx[priority_i] == tcg_ndx)
    {
      *is_sch_valid = TRUE;
      break;
    }
  }

exit:
  SOCDNX_FUNC_RETURN
}


uint32
  arad_ofp_rates_egq_chnif_shaper_set_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                    core,
    SOC_SAND_IN  uint32                 tm_port,
    SOC_SAND_IN  uint32                  rate_kbps,
    SOC_SAND_IN  uint32                  max_burst
  )
{
  uint32
    res,
    rate_internal,
    rate_nof_bits,
    burst_nof_bits;
  ARAD_OFP_RATES_EGQ_CHAN_ARB_ID
    chan_arb_id;
  soc_reg_above_64_val_t
    data_above_64;
  soc_field_t
    field_rate_name,
    field_burst_name;
  
  SOCDNX_INIT_FUNC_DEFS

  SOC_REG_ABOVE_64_CLEAR(data_above_64);
  
  res = arad_ofp_rates_if_id2chan_arb_get_unsafe(
          unit,
          core,
          tm_port,
          &chan_arb_id
        );
  SOCDNX_IF_ERR_EXIT(res);

  if (chan_arb_id == ARAD_OFP_RATES_EGQ_CHAN_ARB_NON_CHAN)
  {
    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG_STR( "Interface is not channelized")));
  }

  if (max_burst > ARAD_OFP_RATES_CHNIF_BURST_LIMIT_MAX) {
          SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG_STR( "max burst is above max")));
  }

  switch(chan_arb_id)
  {
  case ARAD_OFP_RATES_EGQ_CHAN_ARB_00:
    field_rate_name = CH_0_RATEf;
    field_burst_name = CH_0_MAX_BURSTf;
    break;
  case ARAD_OFP_RATES_EGQ_CHAN_ARB_01:
    field_rate_name = CH_1_RATEf;
    field_burst_name = CH_1_MAX_BURSTf;
    break;
  case ARAD_OFP_RATES_EGQ_CHAN_ARB_02:
    field_rate_name = CH_2_RATEf;
    field_burst_name = CH_2_MAX_BURSTf;
    break;
  case ARAD_OFP_RATES_EGQ_CHAN_ARB_03:
    field_rate_name = CH_3_RATEf;
    field_burst_name = CH_3_MAX_BURSTf;
    break;
  case ARAD_OFP_RATES_EGQ_CHAN_ARB_04:
    field_rate_name = CH_4_RATEf;
    field_burst_name = CH_4_MAX_BURSTf;
    break;
  case ARAD_OFP_RATES_EGQ_CHAN_ARB_05:
    field_rate_name = CH_5_RATEf;
    field_burst_name = CH_5_MAX_BURSTf;
    break;
  case ARAD_OFP_RATES_EGQ_CHAN_ARB_06:
    field_rate_name = CH_6_RATEf;
    field_burst_name = CH_6_MAX_BURSTf;
    break;
  case ARAD_OFP_RATES_EGQ_CHAN_ARB_07:
    field_rate_name = CH_7_RATEf;
    field_burst_name = CH_7_MAX_BURSTf;
    break;
  case ARAD_OFP_RATES_EGQ_CHAN_ARB_08:
    field_rate_name = CH_8_RATEf;
    field_burst_name = CH_8_MAX_BURSTf;
    break;
  case ARAD_OFP_RATES_EGQ_CHAN_ARB_09:
    field_rate_name = CH_9_RATEf;
    field_burst_name = CH_9_MAX_BURSTf;
    break;
  default:
    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG_STR( "Illegal chan_arb")));
  }

  
  res = READ_EGQ_CHANNELIZED_INTERFACE_RATEr(unit, data_above_64);
  SOCDNX_SAND_IF_ERR_EXIT(res);

  res = arad_ofp_rates_egq_shaper_rate_to_internal(
          unit,
          ARAD_OFP_RATES_EGQ_CAL_CHAN_ARB,
          rate_kbps,
          FALSE,
          &rate_internal
        );
  SOCDNX_IF_ERR_EXIT(res);

  rate_nof_bits = soc_reg_field_length(unit, EGQ_CHANNELIZED_INTERFACE_RATEr, field_rate_name);

  res = soc_sand_bitstream_set_any_field(&rate_internal,rate_nof_bits*chan_arb_id,rate_nof_bits,data_above_64);
  SOCDNX_SAND_IF_ERR_EXIT(res);

  res = WRITE_EGQ_CHANNELIZED_INTERFACE_RATEr(unit, data_above_64);
  SOCDNX_SAND_IF_ERR_EXIT(res);

  
  SOC_REG_ABOVE_64_CLEAR(data_above_64);
  res = READ_EGQ_CHANNELIZED_INTERFACE_MAX_BURSTr(unit, data_above_64);
  SOCDNX_SAND_IF_ERR_EXIT(res);

  burst_nof_bits = soc_reg_field_length(unit, EGQ_CHANNELIZED_INTERFACE_MAX_BURSTr, field_burst_name);

  res = soc_sand_bitstream_set_any_field(&max_burst,burst_nof_bits*chan_arb_id,burst_nof_bits,data_above_64);
  SOCDNX_SAND_IF_ERR_EXIT(res);

  res = WRITE_EGQ_CHANNELIZED_INTERFACE_MAX_BURSTr(unit, data_above_64);
  SOCDNX_SAND_IF_ERR_EXIT(res);

exit:
  SOCDNX_FUNC_RETURN
}



STATIC
  void
    arad_ARAD_OFP_RATES_CAL_INFO_clear(
      SOC_SAND_OUT ARAD_OFP_RATES_CAL_INFO *info
    )
{
  if (info) {
      sal_memset(info, 0x0, sizeof(ARAD_OFP_RATES_CAL_INFO)); 
  }
}






STATIC uint32
  arad_ofp_shapers_generic_enable(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  soc_port_t               port,
    SOC_SAND_IN  soc_reg_t                reg_type,
    SOC_SAND_IN  uint32                   enable
  )
{
    uint32
        result;
    uint32
        base_q_pair,
        nof_pairs,
        tm_port;
    soc_reg_above_64_val_t
        reg_256_val;
    int core;

    SOCDNX_INIT_FUNC_DEFS

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port, &tm_port, &core)); 

    switch (reg_type) {
        case EGQ_TCG_SPR_DISr:
              
               result = READ_EGQ_TCG_SPR_DISr(unit, core, reg_256_val);
               SOCDNX_IF_ERR_EXIT(result);

               SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_port_to_out_port_priority_get(unit, core, tm_port, &nof_pairs));
               SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, tm_port, &base_q_pair));

               if (nof_pairs != ARAD_OFP_RATES_NOF_PRIORITIES_TCG_SUPPORT) {
                   goto exit;
               }
               
               if(enable) {
                   SHR_BITCLR_RANGE(reg_256_val,base_q_pair,nof_pairs);
               } else {
                   SHR_BITSET_RANGE(reg_256_val,base_q_pair,nof_pairs);
               }

               
               result=WRITE_EGQ_TCG_SPR_DISr(unit, core, reg_256_val);
               SOCDNX_IF_ERR_EXIT(result);
               break;
               
       case EGQ_OTM_PORT_SPR_DISr:
               
               result = READ_EGQ_OTM_PORT_SPR_DISr(unit, core, reg_256_val);
               SOCDNX_IF_ERR_EXIT(result);

               SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_port_to_out_port_priority_get(unit, core, tm_port,&nof_pairs));
               SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, tm_port, &base_q_pair));

               
               if(enable) {
                   SHR_BITCLR(reg_256_val,base_q_pair);
               } else {
                   SHR_BITSET(reg_256_val,base_q_pair);
               }
               
               result=WRITE_EGQ_OTM_PORT_SPR_DISr(unit, core, reg_256_val);
               SOCDNX_IF_ERR_EXIT(result);
               break;

    case EGQ_QPAIR_SPR_DISr:
    default:
            
            result = READ_EGQ_QPAIR_SPR_DISr(unit, core, reg_256_val);
            SOCDNX_IF_ERR_EXIT(result);

            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_port_to_out_port_priority_get(unit, core, tm_port,&nof_pairs));
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, tm_port, &base_q_pair));

            
            if(enable) {
                SHR_BITCLR_RANGE(reg_256_val,base_q_pair,nof_pairs);
            } else {
                SHR_BITSET_RANGE(reg_256_val,base_q_pair,nof_pairs);
            }
            
            result=WRITE_EGQ_QPAIR_SPR_DISr(unit, core, reg_256_val);
            SOCDNX_IF_ERR_EXIT(result);
            break;
    }
exit:
    SOCDNX_FUNC_RETURN
}

uint32
  arad_ofp_q_pair_shapers_enable(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  soc_port_t               port,
    SOC_SAND_IN  uint32                   enable
  )
{
   
    int res, core;
    uint32 q_pair_shp_en_f,reg_val, tm_port;

    SOCDNX_INIT_FUNC_DEFS;

    if (SOC_IS_ARAD_B0_AND_ABOVE(unit)){

        res =  soc_port_sw_db_local_to_tm_port_get(unit, port, &tm_port, &core); 
        SOCDNX_SAND_IF_ERR_EXIT(res);

        res=READ_EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr(unit, core, &reg_val);
        SOCDNX_SAND_IF_ERR_EXIT(res);

        q_pair_shp_en_f = soc_reg_field_get( unit, EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr, reg_val, QPAIR_SPR_ENAf);
        if (q_pair_shp_en_f) {
            res= arad_ofp_shapers_generic_enable( unit,port,EGQ_QPAIR_SPR_DISr,enable);
            SOCDNX_SAND_IF_ERR_EXIT(res);
        }

    }
exit:
    SOCDNX_FUNC_RETURN
}

uint32
  arad_ofp_tcg_shapers_enable(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  soc_port_t               port,
    SOC_SAND_IN  uint32                   enable
  )
{
    int res, core;
    uint32 tcg_shp_en_f,reg_val, tm_port;

    SOCDNX_INIT_FUNC_DEFS
    if (SOC_IS_ARAD_B0_AND_ABOVE(unit)){

        res =  soc_port_sw_db_local_to_tm_port_get(unit, port, &tm_port, &core); 
        SOCDNX_SAND_IF_ERR_EXIT(res);

        res=READ_EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr( unit, core,&reg_val);
        SOCDNX_SAND_IF_ERR_EXIT(res);

        tcg_shp_en_f = soc_reg_field_get( unit, EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr, reg_val, TCG_SPR_ENAf);
        if (tcg_shp_en_f) {
            res= arad_ofp_shapers_generic_enable( unit,port,EGQ_TCG_SPR_DISr,enable);
            SOCDNX_IF_ERR_EXIT(res);
        }
    }
exit:
    SOCDNX_FUNC_RETURN
}


uint32
  jericho_ofp_otm_shapers_enable(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  soc_port_t               port,
    SOC_SAND_IN  uint32                   enable
  )
{
    int res, core;
    uint32 otm_shp_en_f,reg_val, tm_port;

    SOCDNX_INIT_FUNC_DEFS

    res =  soc_port_sw_db_local_to_tm_port_get(unit, port, &tm_port, &core);
    SOCDNX_SAND_IF_ERR_EXIT(res);

    res=READ_EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr( unit, core,&reg_val);
    SOCDNX_SAND_IF_ERR_EXIT(res);

    otm_shp_en_f = soc_reg_field_get( unit, EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr, reg_val, OTM_SPR_ENAf);
    if (otm_shp_en_f) {
        res= arad_ofp_shapers_generic_enable( unit,port,EGQ_OTM_PORT_SPR_DISr,enable);
        SOCDNX_IF_ERR_EXIT(res);
    }

exit:
    SOCDNX_FUNC_RETURN
}

uint32
  arad_ofp_otm_shapers_disable(
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  soc_port_t port,
    SOC_SAND_IN  uint32     queue_rates_size,
    SOC_SAND_OUT uint32*    queue_rates

  )
{
    uint32
        res,
        otm_shp_en_f,
        reg_val,
        rate_mbps,
        egq_rate,
        channels,
        base_q_pair,
        tm_port;
    int
        core;

    SOCDNX_INIT_FUNC_DEFS;

    if(queue_rates_size < ARAD_EGR_NOF_Q_PAIRS) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG_STR( "queue_rates is too small")));
    }


    res=soc_port_sw_db_local_to_tm_port_get(unit, port, &tm_port, &core);
    SOCDNX_IF_ERR_EXIT(res);

    
    res=READ_EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr(unit, core,&reg_val);
    SOCDNX_SAND_IF_ERR_EXIT(res);
    otm_shp_en_f = soc_reg_field_get( unit, EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr, reg_val, OTM_SPR_ENAf);

    if (otm_shp_en_f) {
        
        res = soc_port_sw_db_interface_rate_get(unit, port, &rate_mbps);
        SOCDNX_SAND_IF_ERR_EXIT(res);

        res = soc_port_sw_db_num_of_channels_get(unit, port, &channels);
        SOCDNX_SAND_IF_ERR_EXIT(res);


        
        egq_rate = (rate_mbps * ARAD_RATE_1K) / channels;

        
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, tm_port, &base_q_pair));
        SOCDNX_SAND_IF_ERR_EXIT(arad_sw_db_egq_port_rate_get(unit, core, base_q_pair, &queue_rates[base_q_pair]));

        
        res = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_ofp_rates_egq_single_port_rate_sw_set,(unit, core, tm_port, egq_rate));
        SOCDNX_IF_ERR_EXIT(res);

        
        res = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_ofp_rates_egq_single_port_rate_hw_set,(unit, core, tm_port, 0));
    }
    
exit:
    SOCDNX_FUNC_RETURN
}

uint32
  arad_ofp_otm_shapers_set(
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  soc_port_t port,
    SOC_SAND_IN  uint32     queue_rates_size,
    SOC_SAND_IN  uint32*    queue_rates
  )
{
    uint32
        res,
        base_q_pair,
        tm_port;
    int
        core;

    SOCDNX_INIT_FUNC_DEFS;
    
    if(queue_rates_size < ARAD_EGR_NOF_Q_PAIRS) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG_STR( "queue_rates is too small")));
    }    

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port, &tm_port, &core));

    
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, tm_port, &base_q_pair));
   
    
    res = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_ofp_rates_egq_single_port_rate_sw_set,
         (unit,
         core,
         tm_port,
         queue_rates[base_q_pair]));
    SOCDNX_IF_ERR_EXIT(res);


    
    res = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_ofp_rates_egq_single_port_rate_hw_set,
            (unit,
            core,
            tm_port,
            0));
    SOCDNX_IF_ERR_EXIT(res);

exit:
    SOCDNX_FUNC_RETURN
}


uint32
  arad_ofp_if_shaper_disable(
    SOC_SAND_IN  int                                         unit,
    SOC_SAND_IN  soc_port_t                                  port,
    SOC_SAND_OUT uint32*                                     shpr_rate_reg_val
  )
{
    uint32
        res,
        max_rate,
        tm_port;
    int core;

    SOCDNX_INIT_FUNC_DEFS 
        
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port, &tm_port, &core));            

    
    res = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_ofp_rates_egq_interface_shaper_get,(unit, core, tm_port, shpr_rate_reg_val));
    SOCDNX_IF_ERR_EXIT(res);

    
    res=soc_port_sw_db_interface_rate_get(unit, port, &max_rate);
    SOCDNX_SAND_IF_ERR_EXIT(res);

    
    max_rate = max_rate*ARAD_RATE_1K;

        
    res = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_ofp_rates_egq_interface_shaper_set,(unit, core, tm_port, SOC_TMC_OFP_SHPR_UPDATE_MODE_OVERRIDE,max_rate));
    SOCDNX_IF_ERR_EXIT(res);

exit:
    SOCDNX_FUNC_RETURN
}


uint32
  arad_ofp_if_spr_rate_by_reg_val_set(
     SOC_SAND_IN int                   unit,
     SOC_SAND_IN uint32                port,                            
     SOC_SAND_IN uint32                reg_val
     )
{
    uint32
        res,
        tm_port;
    soc_reg_t
        reg_shpr_name[ARAD_OFP_NOF_RATES_CAL_SETS];
    soc_field_t
        field_shpr_rate_name[ARAD_OFP_NOF_RATES_CAL_SETS];
    ARAD_OFP_RATES_CAL_INFO 
        cal_info;
    ARAD_OFP_RATES_CAL_SET
        cal2set;
    int
        core;
    SOCDNX_INIT_FUNC_DEFS;

    
    SOC_CLEAR(&cal_info, ARAD_OFP_RATES_CAL_INFO, 1);

    res = soc_port_sw_db_local_to_tm_port_get(unit, port, &tm_port, &core);
    SOCDNX_IF_ERR_EXIT(res);

    res = arad_ofp_rates_if_id2chan_arb_get_unsafe(unit, core, tm_port, &(cal_info.chan_arb_id));
    SOCDNX_IF_ERR_EXIT(res);

    cal_info.cal_type = ARAD_OFP_RATES_EGQ_CAL_CHAN_ARB;

    

    for (cal2set = ARAD_OFP_RATES_CAL_SET_A; cal2set < ARAD_OFP_NOF_RATES_CAL_SETS; ++cal2set) {

      res = arad_ofp_rates_retrieve_egress_shaper_reg_field_names(
              unit,
              &cal_info,
              cal2set,
              SOC_TMC_OFP_RATES_EGQ_CHAN_ARB_FIELD_RATE,
              &(reg_shpr_name[cal2set]),
              &(field_shpr_rate_name[cal2set])
            );
      SOCDNX_IF_ERR_EXIT(res);
    }
     

    
    for (cal2set = ARAD_OFP_RATES_CAL_SET_A; cal2set < ARAD_OFP_NOF_RATES_CAL_SETS; ++cal2set) {
        SOC_FIELD32_REG64_SET(reg_shpr_name[cal2set], field_shpr_rate_name[cal2set], reg_val);
    }

exit:
    SOCDNX_FUNC_RETURN
}

STATIC uint32
  arad_ofp_rates_active_egq_generic_calendars_config(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  int                       core,
    SOC_SAND_IN  ARAD_OFP_RATES_CAL_INFO   *cal_info,
    SOC_SAND_IN  ARAD_OFP_RATES_CAL_EGQ    *egq_cal,
    SOC_SAND_IN  uint32                     egq_cal_len
  )
{
    uint32  res,
            fld_val = 0,
            egq_len = 0,
            offset = 0,
            slot = 0,
            egq_to_set,
            reg_val,
            scm_cr_to_add_nof_bits;
    ARAD_EGQ_SCM_TBL_DATA
            egq_data;
    soc_field_t
            field_cal_set_name;

  SOCDNX_INIT_FUNC_DEFS

  SOCDNX_NULL_CHECK(egq_cal);
  SOCDNX_NULL_CHECK(cal_info);

  
  if (cal_info->cal_type == ARAD_OFP_RATES_EGQ_CAL_CHAN_ARB) { 
      scm_cr_to_add_nof_bits = ARAD_EGQ_NIF_MAL_SCM_TBL_ENTRY_PORT_CR_TO_ADD_NOF_BITS;
  } else { 
      scm_cr_to_add_nof_bits = SOC_DPP_DEFS_GET(unit, scm_qp_tcg_cr_to_add_nof_bits);
  }

  
  if ((egq_cal_len > ARAD_OFP_RATES_EGQ_CAL_LEN_EGQ(unit, cal_info->chan_arb_id)) ||  (egq_cal_len < 1)){
  SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG_STR( "egq_cal_len is out of range")));
  }


  

  res = arad_ofp_rates_retrieve_egress_shaper_setting_field(
          unit,
          cal_info,
          &field_cal_set_name
        );
  SOCDNX_IF_ERR_EXIT(res);

  SOCDNX_SAND_IF_ERR_EXIT(READ_EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr(unit, core, &reg_val));
  fld_val = soc_reg_field_get(unit, EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr, reg_val, field_cal_set_name);
  egq_to_set = (fld_val == ARAD_OFP_RATES_CAL_SET_A)?ARAD_OFP_RATES_CAL_SET_B:ARAD_OFP_RATES_CAL_SET_A;

  
  egq_len = egq_cal_len - 1;

  
  res = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_ofp_rates_egress_shaper_cal_write, (unit, core, cal_info, egq_to_set, 
                                                                                SOC_TMC_OFP_RATES_EGQ_CHAN_ARB_FIELD_CAL_LEN, egq_len));
  SOCDNX_SAND_IF_ERR_EXIT(res);

  if(unit >= SOC_MAX_NUM_DEVICES) {
    SOCDNX_EXIT_WITH_ERR(SOC_E_UNIT, (_BSL_SOCDNX_MSG_STR( "ERROR: invalid unit")));
  }
  

  
  for (slot = 0; slot < egq_cal_len; ++slot)
  {
    offset = ARAD_OFP_RATES_EGQ_SCM_OFFSET_GET(unit, cal_info,egq_to_set);
    egq_data.port_cr_to_add = egq_cal->slots[slot].credit;
    egq_data.ofp_index = egq_cal->slots[slot].base_q_pair;
    
    if (egq_data.port_cr_to_add > (1 << scm_cr_to_add_nof_bits)) {
          SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG_STR( "loc_calendar_len is above max")));
    }

    res = arad_egq_scm_tbl_set_unsafe(
            unit,
            core,
            cal_info,
            offset + slot,
            &egq_data
          );
    SOCDNX_SAND_IF_ERR_EXIT(res);
  }

exit:
  SOCDNX_FUNC_RETURN
}

STATIC uint32
  arad_ofp_rates_active_sch_calendars_config(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint32                     sch_offset,
    SOC_SAND_IN  int                        core,
                 ARAD_OFP_RATES_CAL_SCH     *sch_cal,
    SOC_SAND_IN  uint32                     sch_cal_len
  )
{
    uint32  res,
            data,
            fld_val = 0,
            sch_len = 0,
            sch_to_set;

    SOCDNX_INIT_FUNC_DEFS;
  
    
    if ((sch_cal_len > ARAD_OFP_RATES_CAL_LEN_SCH_MAX) ||  (sch_cal_len < 1)){
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG_STR( "sch_cal_len is out of range")));
    }
    
    
    res = READ_SCH_CH_NIF_CALENDAR_CONFIGURATION_CNCCm(unit, SCH_BLOCK(unit, core), sch_offset, &data);
    SOCDNX_IF_ERR_EXIT(res);

    fld_val = soc_SCH_CH_NIF_CALENDAR_CONFIGURATION_CNCCm_field32_get(unit, &data, DVS_CALENDAR_SEL_CH_NI_FXXf);
    sch_to_set = (fld_val == ARAD_OFP_RATES_CAL_SET_A) ? ARAD_OFP_RATES_CAL_SET_B : ARAD_OFP_RATES_CAL_SET_A;
    
    
    sch_len = sch_cal_len - 1;

    
    res = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_sch_cal_tbl_set, (unit, core, sch_offset, sch_to_set, sch_cal_len, sch_cal->slots));
    SOCDNX_IF_ERR_EXIT(res);

     
    soc_SCH_CH_NIF_CALENDAR_CONFIGURATION_CNCCm_field32_set(unit, &data, (sch_to_set == 0 ? CAL_A_LENf : CAL_B_LENf), sch_len);
    soc_SCH_CH_NIF_CALENDAR_CONFIGURATION_CNCCm_field32_set(unit, &data, DVS_CALENDAR_SEL_CH_NI_FXXf, sch_to_set);

    res = WRITE_SCH_CH_NIF_CALENDAR_CONFIGURATION_CNCCm(unit, SCH_BLOCK(unit, core), sch_offset, &data);    
    SOCDNX_IF_ERR_EXIT(res);

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC uint32
 arad_ofp_rates_sw_db_port_priority_nof_valid_queues_get(
          SOC_SAND_IN  int    unit,
          SOC_SAND_IN  int    core,
          SOC_SAND_IN  uint32 tm_port,   
          SOC_SAND_IN  uint32 priority_ndx, 
          SOC_SAND_OUT uint32 *nof_valid_queues
        )
{
    uint32  
        res = SOC_SAND_NO_ERR,
        fap_port_ndx,
        base_q_pair,
        port_base_q_pair,
        ptc_ndx,
        is_ptc_found = FALSE,
        nof_priorities,
        flags;
    ARAD_SW_DB_DEV_EGR_PORT_PRIORITY
        rates;
    soc_pbmp_t
        pbmp;
    soc_port_t
        port_i;
    int core_i;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(nof_valid_queues);

    *nof_valid_queues = 0;
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, tm_port, &port_base_q_pair));

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_valid_ports_core_get(unit, core, 0, &pbmp));

    SOC_PBMP_ITER(pbmp, port_i) {

      SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port_i, &flags)); 
      if (SOC_PORT_IS_NOT_VALID_FOR_EGRESS_TM(flags))
      {
          continue;
      }

      SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port_i, &fap_port_ndx, &core_i));

      SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_port_to_base_q_pair_get(unit,core_i,fap_port_ndx, &base_q_pair));
      
      if (base_q_pair == ARAD_EGR_INVALID_BASE_Q_PAIR)
      {
        continue;
      }
      res = soc_port_sw_db_tm_port_to_out_port_priority_get(unit, core_i, fap_port_ndx, &nof_priorities);
      SOCDNX_IF_ERR_EXIT(res);

      res = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.port_priority_cal.get(unit,core,&rates);
      SOCDNX_IF_ERR_EXIT(res);

      for (ptc_ndx = 0; ptc_ndx < nof_priorities; ++ptc_ndx)
      {
        if((port_base_q_pair == base_q_pair) && (ptc_ndx == priority_ndx)){
            is_ptc_found = TRUE;
        }
        if (rates.queue_rate[base_q_pair + ptc_ndx].valid)
        {
            *nof_valid_queues += 1;
        }
      }
    }
    if(!is_ptc_found){
        *nof_valid_queues += 1;
    }
exit:   
    SOCDNX_FUNC_RETURN;
}


STATIC uint32
 arad_ofp_rates_sw_db_tcg_nof_valid_entries_get(
          SOC_SAND_IN  int    unit,
          SOC_SAND_IN  int    core,
          SOC_SAND_IN  uint32 tm_port,   
          SOC_SAND_IN  uint32 tcg, 
          SOC_SAND_OUT uint32 *nof_valid_entries
        )
{
    uint32  
        res = SOC_SAND_NO_ERR,
        fap_port_ndx,
        base_q_pair,
        ps,
        tcg_ps,
        is_tcg_found = FALSE,
        tcg_ndx,
        nof_priorities,
        port_i;
    ARAD_SW_DB_DEV_EGR_TCG 
        rates;
    soc_pbmp_t
        pbmp;
    int
      core_i;
    uint32
        flags;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(nof_valid_entries);
    *nof_valid_entries = 0;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, tm_port, &base_q_pair));

    tcg_ps = ARAD_BASE_PORT_TC2PS(base_q_pair);

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_valid_ports_core_get(unit, core, 0, &pbmp));
    SOC_PBMP_ITER(pbmp, port_i) 
    {
      SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port_i, &flags));
      if (SOC_PORT_IS_NOT_VALID_FOR_EGRESS_TM(flags)) {
        continue;
      }

      SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port_i, &fap_port_ndx, &core_i));

      SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, fap_port_ndx,  &base_q_pair));
      
      if (base_q_pair == ARAD_EGR_INVALID_BASE_Q_PAIR)
      {
        continue;
      }

      res = soc_port_sw_db_tm_port_to_out_port_priority_get(unit, core_i, fap_port_ndx, &nof_priorities);
      SOCDNX_IF_ERR_EXIT(res);

      ps = ARAD_BASE_PORT_TC2PS(base_q_pair);

      res = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.tcg_cal.get(unit,core,&rates);
      SOCDNX_IF_ERR_EXIT(res);

      for (tcg_ndx = 0; tcg_ndx < nof_priorities; ++tcg_ndx)
      {
        if((ps == tcg_ps) && (tcg_ndx == tcg)){
            is_tcg_found = TRUE;
        }
        if (rates.tcg_rate[ps][tcg_ndx].valid)
        {
            *nof_valid_entries += 1;
        }
      }
    }

    if(!is_tcg_found){
        *nof_valid_entries += 1;
    }
exit:   
    SOCDNX_FUNC_RETURN
}


uint32
  arad_ofp_rates_port_priority_max_burst_for_fc_queues_verify(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                 rate
  )
{

    SOCDNX_INIT_FUNC_DEFS

    if (rate > ARAD_OFP_RATES_FC_Q_BURST_LIMIT_MAX) {
      SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG_STR( "rate is above max")));
    }

exit:   
    SOCDNX_FUNC_RETURN
}

uint32
  arad_ofp_rates_port_priority_max_burst_for_empty_queues_verify(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                 rate
  )
{

    SOCDNX_INIT_FUNC_DEFS

    if (rate > ARAD_OFP_RATES_EMPTY_Q_BURST_LIMIT_MAX) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG_STR( "rate is above max")));
    }

exit:   
    SOCDNX_FUNC_RETURN
}



uint32
  arad_ofp_rates_tcg_shaper_verify(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                    core,
    SOC_SAND_IN  uint32                 tm_port,
    SOC_SAND_IN  ARAD_TCG_NDX           tcg_ndx,
    SOC_SAND_IN  uint32                 rate,
    SOC_SAND_IN  ARAD_OFP_RATES_GENERIC_FUNC_STATE set_state
  )
{
  uint32
    res;
  uint32
    base_q_pair,
    nof_priorities,
    nof_valid_entries = 0,
    max_kbps;
  uint8
    is_sch_valid=0,
    is_egq_valid;

  SOCDNX_INIT_FUNC_DEFS;

  res = arad_fap_port_id_verify(unit, tm_port);
  SOCDNX_SAND_IF_ERR_EXIT(res);

  SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, tm_port, &base_q_pair));
  if (base_q_pair == ARAD_EGR_INVALID_BASE_Q_PAIR)
  {
    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG_STR( "Illegal port")));
  }

  res = soc_port_sw_db_tm_port_to_out_port_priority_get(unit, core, tm_port, &nof_priorities);
  SOCDNX_IF_ERR_EXIT(res);

  if (nof_priorities != ARAD_OFP_RATES_NOF_PRIORITIES_TCG_SUPPORT) {
   SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG_STR( "nof prioritiees doesn't support tcg")));
  }

  
  
  if ((tcg_ndx > ARAD_TCG_MAX) ||  (tcg_ndx < ARAD_TCG_MIN)){
      SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG_STR( "tcg ndx is out of range")));
    }

  
  switch (set_state) {
  case ARAD_OFP_RATES_GENERIC_FUNC_STATE_TCG_SCH_RATE:

      res = arad_ofp_rates_tcg_id_sch_verify(unit, core, tm_port, tcg_ndx, &is_sch_valid); 
      SOCDNX_IF_ERR_EXIT(res);

      max_kbps = (is_sch_valid) ? ARAD_IF_MAX_RATE_KBPS(unit):0;
      if (rate > max_kbps) {
          SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG_STR( "rate is above max")));
      }
      break;

  case ARAD_OFP_RATES_GENERIC_FUNC_STATE_TCG_SCH_BURST:
  case ARAD_OFP_RATES_GENERIC_FUNC_STATE_TCG_EGQ_BURST:

      if (rate > ARAD_OFP_RATES_BURST_LIMIT_MAX) {
          SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG_STR( "burst is above max")));
      }
      break;

  case ARAD_OFP_RATES_GENERIC_FUNC_STATE_TCG_EGQ_RATE:

      res = arad_ofp_rates_tcg_id_egq_verify(unit,core,tm_port,tcg_ndx,&is_egq_valid);
      SOCDNX_IF_ERR_EXIT(res);

      max_kbps = (is_egq_valid) ? ARAD_IF_MAX_RATE_KBPS(unit):0;
      if (rate > max_kbps) {
          SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG_STR( "rate is above max")));
      }
      break;
  default:
      SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG_STR( "Illegal enum")));
  }
  

  res = arad_ofp_rates_sw_db_tcg_nof_valid_entries_get(unit,core,tm_port,tcg_ndx,&nof_valid_entries);
  SOCDNX_IF_ERR_EXIT(res);

  if (nof_valid_entries > ARAD_EGR_NOF_TCG_IDS){
      SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG_STR( "egq_cal_len is out of range")));
    }

exit:
  SOCDNX_FUNC_RETURN
}


uint32
  arad_ofp_rates_port_priority_shaper_verify(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                    core,
    SOC_SAND_IN  uint32                 tm_port,
    SOC_SAND_IN  uint32                 priority_ndx,
    SOC_SAND_IN  uint32                 rate,
    SOC_SAND_IN  ARAD_OFP_RATES_GENERIC_FUNC_STATE set_state
  )
{
  uint32
    res;
  uint32
    base_q_pair,
    nof_valid_queues,
    nof_priorities;

  SOCDNX_INIT_FUNC_DEFS


  res = arad_fap_port_id_verify(unit, tm_port);
  SOCDNX_SAND_IF_ERR_EXIT(res);

  SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, tm_port, &base_q_pair));

  if (base_q_pair == ARAD_EGR_INVALID_BASE_Q_PAIR)
  {
    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG_STR( "illegal port")));
  }

  SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_port_to_out_port_priority_get(unit, core, tm_port, &nof_priorities));

  if (priority_ndx > nof_priorities) {
      SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG_STR( "Priority index out of range")));
  }

  switch (set_state) {
  case ARAD_OFP_RATES_GENERIC_FUNC_STATE_PTC_EGQ_RATE:
  case ARAD_OFP_RATES_GENERIC_FUNC_STATE_PTC_SCH_RATE:
       if (rate > ARAD_IF_MAX_RATE_KBPS(unit)) {
         SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG_STR( "rate above max")));
       }
       break;
  case ARAD_OFP_RATES_GENERIC_FUNC_STATE_PTC_SCH_BURST:
       if (rate > ARAD_OFP_RATES_SCH_BURST_LIMIT_MAX) {
          SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG_STR( "burst above max")));
       }
       break;
  case ARAD_OFP_RATES_GENERIC_FUNC_STATE_PTC_EGQ_BURST:
      if (rate > ARAD_OFP_RATES_BURST_LIMIT_MAX) {
          SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG_STR( "burst above max")));
       }
      break;
 
  default:
      SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG_STR( "Illegal enum")));
  }
  
  res = arad_ofp_rates_sw_db_port_priority_nof_valid_queues_get(
          unit,
          core,
          tm_port,   
          priority_ndx, 
          &nof_valid_queues
        );
  SOCDNX_IF_ERR_EXIT(res);

  
  if (nof_valid_queues > ARAD_EGR_NOF_Q_PAIRS){
      SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG_STR( "nof_valid_queues is out of range")));
    }

exit:
  SOCDNX_FUNC_RETURN
}

int
  arad_ofp_rates_single_port_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                    *rate
  )
{
  SOCDNX_INIT_FUNC_DEFS;

  

  if (rate) {
    if (*rate > ARAD_IF_MAX_RATE_KBPS(unit)) {
      SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG_STR( "rate is above max")));
    }
  }
  
exit:
  SOCDNX_FUNC_RETURN
}


int
    arad_ofp_rates_port2chan_arb_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                fap_port,
    SOC_SAND_OUT  ARAD_OFP_RATES_EGQ_CHAN_ARB_ID *chan_arb_id
    )
{
    uint32 res = SOC_E_NONE;
    soc_reg_above_64_val_t data,
                           field;
    ARAD_INTERFACE_ID egr_interface_id;
    uint32  chan_arb_field_val = 0,
            chan_arb_val = 0;
    int core=0;
    SOCDNX_INIT_FUNC_DEFS

    SOCDNX_NULL_CHECK(chan_arb_id);

    res = arad_port2egress_offset(unit, core, fap_port, &egr_interface_id);
    SOCDNX_IF_ERR_EXIT(res);

    
    res = READ_EGQ_MAPPING_INTERFACES_TO_CHAN_ARBITERr(unit,data);
    SOCDNX_SAND_IF_ERR_EXIT(res);
    soc_reg_above_64_field_get(unit,EGQ_MAPPING_INTERFACES_TO_CHAN_ARBITERr,data,MAP_IFC_TO_CHAN_ARBf,field);
    SHR_BITCOPY_RANGE(&chan_arb_field_val,0,field,ARAD_OFP_RATES_CHAN_ARB_NOF_BITS*egr_interface_id,ARAD_OFP_RATES_CHAN_ARB_NOF_BITS);
    res = arad_nif_chan_arb_field_val_to_enum(unit,chan_arb_field_val,&chan_arb_val);
    SOCDNX_SAND_IF_ERR_EXIT(res);

    switch (chan_arb_val)
    {
    case ARAD_OFP_RATES_EGQ_NOF_CHAN_ARB:    
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG_STR( "Chan arbiter not allocated for port = %d"), fap_port));
        break;
    default:
        *chan_arb_id = chan_arb_val;
        break;
    }
    
    
exit:
    SOCDNX_FUNC_RETURN
     
}
                        
int
    arad_ofp_rates_port2chan_cal_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core,
    SOC_SAND_IN  uint32                tm_port,
    SOC_SAND_OUT uint32                *calendar
    )
{
    ARAD_OFP_RATES_EGQ_CHAN_ARB_ID chan_arb_id;

    SOCDNX_INIT_FUNC_DEFS

    SOCDNX_IF_ERR_EXIT(arad_ofp_rates_port2chan_arb_unsafe(unit, tm_port, &chan_arb_id));
    *calendar = (uint32)(chan_arb_id);

exit:
    SOCDNX_FUNC_RETURN
}
                        
int
    arad_ofp_rates_sch_single_port_rate_hw_get(
        SOC_SAND_IN  int                    unit,
        SOC_SAND_IN  int                    core,
        SOC_SAND_IN  uint32                 tm_port,
        SOC_SAND_OUT uint32                 *rate
    )
{
    uint32 
        res = SOC_E_NONE,
        sch_cal_len,
        *sch_rates = NULL,
        is_channelized_id,
        sch_chan_arb_rate,
        sch_offset,
        egress_offset;
    soc_reg_above_64_val_t
        data;
    ARAD_OFP_RATES_CAL_SCH      
        *sch_cal = NULL;
    uint32
        base_q_pair;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_ALLOC(sch_rates, uint32, ARAD_NOF_FAP_PORTS,"sch_rates");
    SOCDNX_ALLOC(sch_cal, ARAD_OFP_RATES_CAL_SCH, 1,"sch_cal");

    
    res = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port2egress_offset, (unit, core, tm_port, &egress_offset));
    SOCDNX_IF_ERR_EXIT(res);

    
    SOCDNX_IF_ERR_EXIT(READ_SCH_FC_MAP_FCMm(unit, SCH_BLOCK(unit, core), egress_offset, &data));
    sch_offset = soc_SCH_FC_MAP_FCMm_field32_get(unit,&data,FC_MAP_FCMf);

    is_channelized_id = ARAD_SCH_IS_CHNIF_ID(unit, sch_offset);

    if (is_channelized_id)
    {
        sch_cal_len = 0;

        
        res = arad_ofp_rates_active_calendars_retrieve_sch(
                unit,
                sch_offset,
                core,
                sch_cal,
                &sch_cal_len
            );
        SOCDNX_IF_ERR_EXIT(res);

        
        res = arad_sch_ch_if_rate_get_unsafe(
            unit,
            core,
            sch_offset, 
            &sch_chan_arb_rate);
        SOCDNX_IF_ERR_EXIT(res);

        
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, tm_port, &base_q_pair));
        if (base_q_pair == ARAD_EGR_INVALID_BASE_Q_PAIR)
        {
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG_STR( "Illegal port")));
        }

        res = arad_ofp_rates_from_calendar_to_ports_sch_rate(
              unit,
              sch_cal,
              sch_cal_len,
              sch_chan_arb_rate,
              ARAD_EGR_NOF_BASE_Q_PAIRS,
              base_q_pair,
              sch_rates
            );
        SOCDNX_IF_ERR_EXIT(res);
        
        *rate = sch_rates[base_q_pair];
    }
    else {
        res = arad_sch_if_shaper_rate_get(unit, core, tm_port, rate);
        SOCDNX_IF_ERR_EXIT(res);
    }
                            
exit:
    SOCDNX_FREE(sch_rates);
    SOCDNX_FREE(sch_cal);
    SOCDNX_FUNC_RETURN;
     
}
                        
int
    arad_ofp_rates_sch_single_port_rate_sw_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core,
    SOC_SAND_IN  uint32                tm_port,
    SOC_SAND_IN  uint32                rate
    )
{
    uint32 res = SOC_E_NONE,
           base_q_pair,
           sch_offset,
           egress_offset;
    soc_reg_above_64_val_t
        data;

    SOC_SAND_OCC_BM_PTR modified_e2e_interfaces_occ;
    
    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, tm_port, &base_q_pair));

    
    res = arad_sw_db_sch_port_rate_set(unit, core, base_q_pair, rate);
    SOCDNX_SAND_IF_ERR_EXIT(res);
    res = arad_sw_db_is_port_valid_set(unit, core, base_q_pair, TRUE);
    SOCDNX_SAND_IF_ERR_EXIT(res);


    res = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.modified_e2e_interfaces_occ.get(unit,core, &modified_e2e_interfaces_occ);
    SOCDNX_IF_ERR_EXIT(res);

    
    SOCDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port2egress_offset, (unit, core, tm_port, &egress_offset)));

    
    SOCDNX_IF_ERR_EXIT(READ_SCH_FC_MAP_FCMm(unit, SCH_BLOCK(unit, core), egress_offset, &data));
    sch_offset = soc_SCH_FC_MAP_FCMm_field32_get(unit,&data,FC_MAP_FCMf);

    
    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_occ_bm_occup_status_set(unit, modified_e2e_interfaces_occ, sch_offset, TRUE));

exit:
    SOCDNX_FUNC_RETURN;
}

int
    arad_ofp_rates_is_channalized(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core, 
    SOC_SAND_IN  uint32                tm_port, 
    SOC_SAND_OUT uint32                *is_channalzied)
{
    uint32 res;
    ARAD_OFP_RATES_EGQ_CHAN_ARB_ID chan_arb_id;
    SOCDNX_INIT_FUNC_DEFS;

    
    res = arad_ofp_rates_port2chan_arb_unsafe(unit, tm_port, &chan_arb_id);
    SOCDNX_IF_ERR_EXIT(res);

    
    *is_channalzied = SOC_SAND_NUM2BOOL_INVERSE(chan_arb_id == ARAD_OFP_RATES_EGQ_CHAN_ARB_NON_CHAN);

exit:
    SOCDNX_FUNC_RETURN;
}

int
    arad_ofp_rates_sch_single_port_rate_hw_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                is_alt_calc
    )
{
    uint32 res = SOC_E_NONE,
        is_channelized_id,
        base_q_pair,
        sch_calendar_len,
        total_if_rate = 0,
        current_if_rate = 0,
        nof_valid_ports = 0,
        max_cal_size,
        sch_offset,
        sch_offset_i,
        tm_port_i = 0,
        tm_port = 0,
        egress_offset,
        egress_offset_i;
    uint32
        *sch_rates = NULL,
        flags;
    uint8 is_valid = FALSE, is_modified = FALSE;
    int core;
    ARAD_OFP_RATES_CAL_SCH
        *sch_calendar = NULL;
    soc_reg_above_64_val_t
        data;
    uint32
        is_rate_decreased =  FALSE;
    soc_pbmp_t ifs_bmp, pors_bmp;
    soc_port_t port, port_i;
    soc_port_if_t interface;
    SOC_SAND_OCC_BM_PTR modified_e2e_interfaces_occ;
    SOC_TMC_PORT_HEADER_TYPE header_type_out;
#ifdef BCM_LB_SUPPORT
    soc_pbmp_t lb_ports;
    uint32     lb_port_num = 0;
#endif

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_ALLOC(sch_rates, uint32, ARAD_NOF_FAP_PORTS, "sch_rates");
    SOCDNX_ALLOC(sch_calendar, ARAD_OFP_RATES_CAL_SCH, 1,"sch_calendar");

    SOC_DPP_CORES_ITER(SOC_CORE_ALL, core) {

        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_valid_ports_core_get(unit, core, 0, &pors_bmp));
        res = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.modified_e2e_interfaces_occ.get(unit, core, &modified_e2e_interfaces_occ);
        SOCDNX_IF_ERR_EXIT(res);

        for (sch_offset = 0; sch_offset < SOC_DPP_IMP_DEFS_GET(unit, nof_core_interfaces); sch_offset++) {        

            sal_memset(sch_rates,0,sizeof(uint32)*ARAD_NOF_FAP_PORTS);
            sal_memset(sch_calendar,0,sizeof(ARAD_OFP_RATES_CAL_SCH));

            SOCDNX_SAND_IF_ERR_EXIT(soc_sand_occ_bm_is_occupied(unit, modified_e2e_interfaces_occ, sch_offset, &is_modified));
            if (!is_modified) {
                continue;
            }

            
            SOCDNX_SAND_IF_ERR_EXIT(soc_sand_occ_bm_occup_status_set(unit, modified_e2e_interfaces_occ, sch_offset, FALSE));

            sch_offset_i = INVALID_CALENDAR;
            
            SOC_PBMP_ITER(pors_bmp, port) {
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port, &flags)); 
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface));
                if (SOC_PORT_IS_NOT_VALID_FOR_EGRESS_TM(flags) || SOC_PORT_IS_VIRTUAL_RCY_INTERFACE(flags) || interface == SOC_PORT_IF_NOCXN)
                {
                    continue;
                }
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port, &tm_port, &core));
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, tm_port, &base_q_pair));
                if (base_q_pair == ARAD_EGR_INVALID_BASE_Q_PAIR)
                {
                    continue;
                }

                
                res = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port2egress_offset, (unit, core, tm_port, &egress_offset));
                SOCDNX_IF_ERR_EXIT(res);

                
                SOCDNX_IF_ERR_EXIT(READ_SCH_FC_MAP_FCMm(unit, SCH_BLOCK(unit, core), egress_offset, &data));
                sch_offset_i = soc_SCH_FC_MAP_FCMm_field32_get(unit,&data,FC_MAP_FCMf);
                if (sch_offset == sch_offset_i) {
                    break;
                }
            }

            if (sch_offset != sch_offset_i) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG_STR("No port found with the modified E2E interface")));
            }

            is_channelized_id = ARAD_SCH_IS_CHNIF_ID(unit, sch_offset);
            if (is_channelized_id)
            {
                total_if_rate = 0;

                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_to_local_port_get(unit, core, tm_port, &port));
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_ports_to_same_interface_get(unit, port, &ifs_bmp));
                SOCDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port2egress_offset, (unit, core, tm_port, &egress_offset)));

#ifdef BCM_LB_SUPPORT
                if (SOC_IS_QAX(unit) && SOC_DPP_CONFIG(unit)->qax->lbi_en) {
                    if (egress_offset == SOC_QAX_EGR_IF_LBG_RESERVE) {
                        SOCDNX_IF_ERR_EXIT(qax_lb_ports_on_reserve_intf_get(unit, &lb_ports, &lb_port_num));
                        SOC_PBMP_ASSIGN(ifs_bmp, lb_ports);
                    }
                }
#endif

                
                SOC_PBMP_ITER(ifs_bmp, port_i){
                    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port_i, &tm_port_i, &core));
                    SOCDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port2egress_offset, (unit, core, tm_port_i, &egress_offset_i)));
                    if (egress_offset == egress_offset_i) {
                        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, tm_port_i, &base_q_pair));
                        SOCDNX_IF_ERR_EXIT(arad_sw_db_is_port_valid_get(unit, core, base_q_pair, &is_valid));
                        SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.header_type_out.get(unit, port_i, &header_type_out));
                        if(is_valid && (header_type_out != SOC_TMC_PORT_HEADER_TYPE_TDM)) {
                            SOCDNX_SAND_IF_ERR_EXIT(arad_sw_db_sch_port_rate_get(unit, core, base_q_pair, &sch_rates[base_q_pair]));
                            total_if_rate += sch_rates[base_q_pair];
                            nof_valid_ports += 1;
                        }
                    }
                }

                
                res = arad_sch_ch_if_rate_get_unsafe(unit, core, sch_offset, &current_if_rate);
                SOCDNX_IF_ERR_EXIT(res);

                
                if (current_if_rate > total_if_rate) {
                    is_rate_decreased = TRUE;
                }

                if (!is_rate_decreased) {
                    
                    res = arad_sch_ch_if_rate_set_unsafe(
                      unit,
                      core,
                      sch_offset,
                      total_if_rate
                    );
                    SOCDNX_IF_ERR_EXIT(res);
                }
                                                 
                res = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_sch_cal_max_size_get, (unit, sch_offset, &max_cal_size));
                SOCDNX_IF_ERR_EXIT(res);

                
                res = arad_ofp_rates_from_rates_to_calendar(unit,
                                                            sch_rates,
                                                            ARAD_EGR_NOF_BASE_Q_PAIRS,
                                                            total_if_rate,
                                                            max_cal_size,
                                                            sch_calendar,
                                                            &sch_calendar_len,
                                                            is_alt_calc);
                SOCDNX_IF_ERR_EXIT(res);

                res = arad_ofp_rates_active_sch_calendars_config(unit,
                                                         sch_offset,
                                                         core,
                                                         sch_calendar,
                                                         sch_calendar_len);
                SOCDNX_IF_ERR_EXIT(res);

                if (is_rate_decreased) {
                    
                    res = arad_sch_ch_if_rate_set_unsafe(
                      unit,
                      core,
                      sch_offset,
                      total_if_rate
                    );
                    SOCDNX_IF_ERR_EXIT(res);
                }
                
            }

            else {
                res = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, tm_port, &base_q_pair);
                SOCDNX_IF_ERR_EXIT(res);
                res = arad_sw_db_is_port_valid_get(unit, core, base_q_pair, &is_valid);
                SOCDNX_IF_ERR_EXIT(res);
                if (is_valid)
                {
                    nof_valid_ports += 1;
                    
                    res = arad_sw_db_sch_port_rate_get(unit, core, base_q_pair, &sch_rates[0]);
                    SOCDNX_SAND_IF_ERR_EXIT(res);
                    res = arad_sch_if_shaper_rate_set(unit, core, tm_port, sch_rates[0]);
                    SOCDNX_IF_ERR_EXIT(res);
                }
                else
                {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG_STR( "Invalid port")));
                }
            }
        }
    }
exit:
    SOCDNX_FREE(sch_rates);
    SOCDNX_FREE(sch_calendar);
    SOCDNX_FUNC_RETURN;
}

uint32
    arad_ofp_rates_egq_single_port_rate_hw_get_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                    core,
    SOC_SAND_IN  uint32                 tm_port,
    SOC_SAND_OUT uint32                 *rate
    )
{
    uint32 res = SOC_E_NONE,
           egq_cal_len,
           *egq_rates = NULL,
           base_q_pair;
    uint8 add_dummy_tail = FALSE;
    ARAD_OFP_RATES_CAL_EGQ      
           *egq_cal = NULL;
    ARAD_OFP_RATES_CAL_INFO
           cal_info;

    SOCDNX_INIT_FUNC_DEFS

    SOCDNX_NULL_CHECK(rate);

    SOCDNX_ALLOC(egq_rates, uint32, ARAD_NOF_FAP_PORTS,"egq_rates");
    SOCDNX_ALLOC(egq_cal, ARAD_OFP_RATES_CAL_EGQ, 1,"egq_cal");
    arad_ARAD_OFP_RATES_CAL_INFO_clear(&cal_info);
      

    res = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_ofp_rates_port2chan_cal_get, (unit, core, tm_port, &cal_info.chan_arb_id));
    SOCDNX_IF_ERR_EXIT(res);

    
    if (cal_info.chan_arb_id == INVALID_CALENDAR) {
        *rate = 0;
    } else {
        cal_info.cal_type = ARAD_OFP_RATES_EGQ_CAL_CHAN_ARB;

        egq_cal_len = 0;
        
        res = arad_ofp_rates_active_generic_calendars_retrieve_egq(
                unit,
                core,
                &cal_info,
                egq_cal,
                &egq_cal_len,
                FALSE
            );
        SOCDNX_IF_ERR_EXIT(res);
        
        
        if (SOC_IS_ARADPLUS_AND_BELOW(unit))
        {
            add_dummy_tail = TRUE;
        }
        
        res = arad_ofp_rates_from_generic_calendar_to_ports_egq_rate(
              unit,
              core,
              &cal_info,
              egq_cal,
              egq_cal_len,
              add_dummy_tail,
              ARAD_EGR_NOF_BASE_Q_PAIRS,
              egq_rates
            );
        SOCDNX_IF_ERR_EXIT(res);
        
        res = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, tm_port, &base_q_pair);
        SOCDNX_IF_ERR_EXIT(res);
        if (base_q_pair == ARAD_EGR_INVALID_BASE_Q_PAIR)
        {
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG_STR( "Illegal port")));
        }
        *rate = egq_rates[base_q_pair];
    }
                            
exit:
    SOCDNX_FREE(egq_rates);
    SOCDNX_FREE(egq_cal);
    SOCDNX_FUNC_RETURN;
     
}

int
    arad_ofp_rates_egq_single_port_rate_sw_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core,
    SOC_SAND_IN  uint32                tm_port,
    SOC_SAND_IN  uint32                rate
    )
{
    uint32 res = SOC_E_NONE,
           base_q_pair,
           chan_arb;
    SOC_SAND_OCC_BM_PTR 
           modified_cals_occ;
    soc_port_if_t   
           interface_type;
    soc_port_t      
           port;

    SOCDNX_INIT_FUNC_DEFS

    
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_to_local_port_get(unit, core, tm_port, &port));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface_type));
    if (interface_type == SOC_PORT_IF_ERP) { 
        SOC_EXIT;
    }

    
    res = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, tm_port, &base_q_pair);
    SOCDNX_IF_ERR_EXIT(res);

    
    res = arad_sw_db_egq_port_rate_set(unit, core, base_q_pair, rate);
    SOCDNX_SAND_IF_ERR_EXIT(res);
    res = arad_sw_db_is_port_valid_set(unit, core, base_q_pair, TRUE);
    SOCDNX_SAND_IF_ERR_EXIT(res);

    res = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.modified_channelized_cals_occ.get(unit, core, &modified_cals_occ);
    SOCDNX_IF_ERR_EXIT(res);

    res = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_ofp_rates_port2chan_cal_get, (unit, core, tm_port, &chan_arb));
    SOCDNX_IF_ERR_EXIT(res);

    
    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_occ_bm_occup_status_set(unit, modified_cals_occ, chan_arb, TRUE));

exit:
    SOCDNX_FUNC_RETURN
     
}

int
    arad_ofp_rates_egq_single_port_rate_sw_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core,
    SOC_SAND_IN  uint32                tm_port,
    SOC_SAND_OUT uint32                *rate
    )
{
    uint32 res = SOC_E_NONE,
           base_q_pair;
    soc_port_if_t   
           interface_type;
    soc_port_t      
           port;

    SOCDNX_INIT_FUNC_DEFS

    
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_to_local_port_get(unit, core, tm_port, &port));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface_type));
    if (interface_type == SOC_PORT_IF_ERP) { 
        *rate = 0;
        SOC_EXIT;
    }

    
    res = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, tm_port, &base_q_pair);
    SOCDNX_IF_ERR_EXIT(res);

    
    res = arad_sw_db_egq_port_rate_get(unit, core, base_q_pair, rate);
    SOCDNX_SAND_IF_ERR_EXIT(res);

exit:
    SOCDNX_FUNC_RETURN
     
}

STATIC uint32
    arad_ofp_rates_calcal_config_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core
    )
{
    uint32 
        res = SOC_E_NONE,
        tm_port,
        idx,
        act_cal,
        chan_arb_i,
        egq_calcal_chan_arb_rate_requested = 0,
        egq_calcal_calendar_len = 0,
        egq_calcal_rates[SOC_DPP_DEFS_MAX(NOF_CHANNELIZED_CALENDARS)],
        egq_calcal_instances[SOC_DPP_DEFS_MAX(NOF_CHANNELIZED_CALENDARS)],
        base_q_pair,
        egq_rate,
        port_i,
        reg_val,
        flags;
    uint8  
        is_valid;
    ARAD_OFP_RATES_CAL_SCH
        *egq_calcal_calendar = NULL;
    ARAD_OFP_RATES_CAL_SET
        cal2set;
    ARAD_EGQ_CCM_TBL_DATA
        ccm_tbl_data;
    soc_pbmp_t
        pbmp;
    int
        core_i;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_ALLOC(egq_calcal_calendar, ARAD_OFP_RATES_CAL_SCH, 1, "egq_calcal_calendar");

    
    egq_calcal_chan_arb_rate_requested = 0;
    sal_memset(egq_calcal_rates,0,sizeof(uint32)*SOC_DPP_DEFS_MAX(NOF_CHANNELIZED_CALENDARS));
    sal_memset(egq_calcal_instances,0,sizeof(uint32)*SOC_DPP_DEFS_MAX(NOF_CHANNELIZED_CALENDARS));

    
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_valid_ports_core_get(unit, core, 0, &pbmp));
    SOC_PBMP_ITER(pbmp, port_i)
    {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port_i, &flags)); 
        if (SOC_PORT_IS_NOT_VALID_FOR_EGRESS_TM(flags))
        {
            continue;
        }
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port_i, &tm_port, &core_i));

        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_port_to_base_q_pair_get(unit,core_i, tm_port,  &base_q_pair));
        
        if (base_q_pair == ARAD_EGR_INVALID_BASE_Q_PAIR) {
            continue;
        }

        res = arad_sw_db_is_port_valid_get(unit, core_i, base_q_pair, &is_valid);
        SOCDNX_SAND_IF_ERR_EXIT(res);

        if(!is_valid) {
            continue;
        }

        res = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_ofp_rates_port2chan_cal_get, (unit, core_i, tm_port, &chan_arb_i));
        SOCDNX_IF_ERR_EXIT(res);

        if(chan_arb_i == ARAD_OFP_RATES_EGQ_CHAN_ARB_INVALID) {
            continue;
        }

        res = arad_sw_db_egq_port_rate_get(unit, core_i, base_q_pair, &egq_rate); 
        SOCDNX_SAND_IF_ERR_EXIT(res);

        egq_calcal_chan_arb_rate_requested += egq_rate;
        egq_calcal_rates[chan_arb_i] += egq_rate;

    }

    
    res = arad_ofp_rates_from_rates_to_calendar(
            unit,
            egq_calcal_rates,
            SOC_DPP_DEFS_GET(unit, nof_channelized_calendars),
            egq_calcal_chan_arb_rate_requested,
            ARAD_OFP_RATES_CALCAL_LEN_EGQ_MAX,
            egq_calcal_calendar,
            &egq_calcal_calendar_len,
            0
          );
    SOCDNX_IF_ERR_EXIT(res);

    
    SOCDNX_SAND_IF_ERR_EXIT(READ_EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr(unit, core, &reg_val));
    act_cal = soc_reg_field_get(unit, EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr, reg_val, OTM_SPR_SET_SELf);
    cal2set = (act_cal == ARAD_OFP_RATES_CAL_SET_A)?ARAD_OFP_RATES_CAL_SET_B:ARAD_OFP_RATES_CAL_SET_A;

    
    
    SOCDNX_SAND_IF_ERR_EXIT(READ_EGQ_EGRESS_SHAPER_CALENDARS_ARBITRATION_CYCLE_LENGTHr(unit, core, &reg_val));
    
    if (cal2set == ARAD_OFP_RATES_CAL_SET_A)
    {
      soc_reg_field_set(unit, EGQ_EGRESS_SHAPER_CALENDARS_ARBITRATION_CYCLE_LENGTHr, &reg_val, CAL_CAL_LEN_Af, egq_calcal_calendar_len - 1);
    }
    else
    {
      soc_reg_field_set(unit, EGQ_EGRESS_SHAPER_CALENDARS_ARBITRATION_CYCLE_LENGTHr, &reg_val, CAL_CAL_LEN_Bf, egq_calcal_calendar_len - 1);
    }
    
    SOCDNX_SAND_IF_ERR_EXIT(WRITE_EGQ_EGRESS_SHAPER_CALENDARS_ARBITRATION_CYCLE_LENGTHr(unit, core, reg_val));
    
    res = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.calcal_length.set(unit, core, egq_calcal_calendar_len);
    SOCDNX_IF_ERR_EXIT(res);

    
    for (idx = 0; idx < egq_calcal_calendar_len; ++idx)
    {
      egq_calcal_instances[egq_calcal_calendar->slots[idx]] += 1;
      ccm_tbl_data.interface_select = egq_calcal_calendar->slots[idx];
      res = arad_egq_ccm_tbl_set_unsafe(
              unit,
              core,
              idx + (cal2set) * ARAD_OFP_RATES_CALCAL_LEN_EGQ_MAX,
              &ccm_tbl_data
            );
      SOCDNX_SAND_IF_ERR_EXIT(res);
    }   
    
    
    for (chan_arb_i = 0; chan_arb_i < SOC_DPP_DEFS_GET(unit, nof_channelized_calendars); ++chan_arb_i)
    {
        res = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.chan_arb.nof_calcal_instances.set(unit, core, chan_arb_i, egq_calcal_instances[chan_arb_i]);
        SOCDNX_IF_ERR_EXIT(res);
    }

exit:
    SOCDNX_FREE(egq_calcal_calendar);
    SOCDNX_FUNC_RETURN
     
}

int
    arad_ofp_rates_egq_single_port_rate_hw_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                is_alt_calc
    )
{
    uint32 
        res = SOC_E_NONE,
        tm_port_i,
        calendar_tm_port = 0,
        base_q_pair,
        egq_calendar_len,
        cur_chan_arb_i,
        egq_if_rate_requested = 0,
        recalc,
        act_cal,
        reg_val,
        if_internal_rate,
        if_id,
        nof_instances,
        *egq_rates = NULL,
        new_cal = INVALID_CALENDAR,
        flags;
    uint8 
        is_valid,
        is_cal_modified = FALSE,
        add_dummy_tail = FALSE;
    ARAD_OFP_RATES_CAL_EGQ
        *egq_calendar = NULL;
    ARAD_OFP_RATES_CAL_INFO
        cal_info ;
    ARAD_OFP_RATES_CAL_SET
        cal2set;
    soc_pbmp_t 
        pbmp;
    int 
        core_i,
        chan_arb_i,
        is_single_cal_mode,
        update_required,
        is_high_priority_port = 0,
        is_channelized;
    soc_port_t 
        port_i,
        calendar_port;
    SOC_SAND_OCC_BM_PTR 
        modified_cals_occ;
    SOCDNX_INIT_FUNC_DEFS

    SOCDNX_ALLOC(egq_rates, uint32, ARAD_NOF_FAP_PORTS, "egq_rates");
    SOCDNX_ALLOC(egq_calendar, ARAD_OFP_RATES_CAL_EGQ, 1,"egq_calendar");

    
    SOC_DPP_CORES_ITER(SOC_CORE_ALL, core_i) {

        update_required = 1;

        res = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.modified_channelized_cals_occ.get(unit, core_i, &modified_cals_occ);
        SOCDNX_IF_ERR_EXIT(res);

        for (chan_arb_i = 0 ; chan_arb_i < SOC_DPP_DEFS_GET(unit, nof_channelized_calendars) ; ++chan_arb_i) {
            SOCDNX_SAND_IF_ERR_EXIT(soc_sand_occ_bm_is_occupied(unit, modified_cals_occ, chan_arb_i, &is_cal_modified));
            if (is_cal_modified) {
                break;
            }
        }

        
        if (!is_cal_modified) {
            continue;
        }

        sal_memset(egq_rates,0,ARAD_NOF_FAP_PORTS*sizeof(uint32));
        sal_memset(egq_calendar,0,sizeof(ARAD_OFP_RATES_CAL_EGQ));

        
        SOCDNX_SAND_IF_ERR_EXIT(READ_EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr(unit, core_i, &reg_val));
        act_cal = soc_reg_field_get(unit, EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr, reg_val, OTM_SPR_SET_SELf);
        cal2set = (act_cal == ARAD_OFP_RATES_CAL_SET_A)?ARAD_OFP_RATES_CAL_SET_B:ARAD_OFP_RATES_CAL_SET_A;
        
        while(update_required)
        {
        update_required = 0;
        
        res = arad_ofp_rates_calcal_config_unsafe(unit, core_i);
        SOCDNX_IF_ERR_EXIT(res);
        
        for (chan_arb_i = 0 ; chan_arb_i < SOC_DPP_DEFS_GET(unit, nof_channelized_calendars) ; ++chan_arb_i) {

            SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.chan_arb.nof_calcal_instances.get(unit,
                core_i, chan_arb_i, &nof_instances));
            if (nof_instances > 0) {
                
                egq_if_rate_requested = 0;
                sal_memset(egq_rates,0,ARAD_NOF_FAP_PORTS*sizeof(uint32));
            
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_valid_ports_core_get(unit, core_i, 0, &pbmp));

                calendar_port = -1;
                SOC_PBMP_ITER(pbmp, port_i)
                {
                    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port_i, &flags)); 
                    if (SOC_PORT_IS_NOT_VALID_FOR_EGRESS_TM(flags))
                    {
                        continue;
                    }
                    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port_i, &tm_port_i, &core_i));

                    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core_i, tm_port_i,  &base_q_pair));
                    
                    if (base_q_pair == ARAD_EGR_INVALID_BASE_Q_PAIR)
                    {
                        continue;
                    }

                    res = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_ofp_rates_port2chan_cal_get, (unit, core_i, tm_port_i, &cur_chan_arb_i));
                    SOCDNX_IF_ERR_EXIT(res);

                    if(cur_chan_arb_i != chan_arb_i) { 
                        continue; 
                    } else {    
                        if (calendar_port == -1) {
                            calendar_port = port_i;
                            calendar_tm_port = tm_port_i;
                        }
                    }

                    res = arad_sw_db_is_port_valid_get(unit, core_i, base_q_pair, &is_valid);
                    SOCDNX_SAND_IF_ERR_EXIT(res);
                    if (is_valid)
                    {
                        
                        res = arad_sw_db_egq_port_rate_get(unit, core_i, base_q_pair, &egq_rates[base_q_pair]);
                        SOCDNX_SAND_IF_ERR_EXIT(res);
                        egq_if_rate_requested += egq_rates[base_q_pair];
                    }
                }

                
                cal_info.chan_arb_id = chan_arb_i; 
                cal_info.cal_type = ARAD_OFP_RATES_EGQ_CAL_CHAN_ARB;

                
                recalc = FALSE;
                SOCDNX_SAND_IF_ERR_EXIT(soc_sand_occ_bm_is_occupied(unit, modified_cals_occ, chan_arb_i, &is_cal_modified));
                if (is_cal_modified == TRUE) {
                    recalc = TRUE;
                    
                    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_occ_bm_occup_status_set(unit, modified_cals_occ, chan_arb_i, FALSE));
                }
                
                if (SOC_IS_ARADPLUS_AND_BELOW(unit))
                {
                    add_dummy_tail = TRUE;
                }
                res = arad_ofp_rates_from_egq_ports_rates_to_generic_calendar(
                          unit,
                          core_i,
                          &cal_info,
                          egq_rates,
                          ARAD_EGR_NOF_BASE_Q_PAIRS,
                          egq_if_rate_requested,
                          ARAD_OFP_RATES_EGQ_CAL_LEN_EGQ(unit, cal_info.chan_arb_id),
                          recalc,
                          add_dummy_tail,
                          egq_calendar,
                          &egq_calendar_len,
                          is_alt_calc
                        );
                
                if (SOC_IS_JERICHO(unit) && calendar_port != -1)
                {
                    
                    if(res == SOC_E_LIMIT)
                    {
                        res = arad_ofp_rates_from_egq_ports_rates_to_generic_calendar(
                          unit,
                          core_i,
                          &cal_info,
                          egq_rates,
                          ARAD_EGR_NOF_BASE_Q_PAIRS,
                          egq_if_rate_requested,
                          SOC_DPP_DEFS_GET(unit, big_channelized_cal_size),
                          recalc,
                          add_dummy_tail,
                          egq_calendar,
                          &egq_calendar_len,
                          is_alt_calc
                        );
                        SOCDNX_IF_ERR_EXIT(res);
                        SOCDNX_IF_ERR_EXIT(soc_jer_ofp_rates_calendar_allocate(unit, core_i, calendar_tm_port, egq_calendar_len, cal_info.chan_arb_id, &new_cal));
                        
                        if (new_cal != INVALID_CALENDAR)
                        {
                            SOCDNX_SAND_IF_ERR_EXIT(soc_sand_occ_bm_occup_status_set(unit, modified_cals_occ, new_cal, TRUE));
                            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_single_cal_mode_get(unit, calendar_port, &is_single_cal_mode));
                            if(is_single_cal_mode)
                            {
                                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_high_priority_cal_set(unit, calendar_port, new_cal));
                                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_low_priority_cal_set(unit, calendar_port, new_cal));
                            } else {
                                SOCDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_egr_queuing_is_high_priority_port_get,(unit, \
                                                                                    core_i, calendar_tm_port, &is_high_priority_port)));
                                if(is_high_priority_port)
                                {
                                    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_high_priority_cal_set(unit, calendar_port, new_cal));
                                } else {
                                    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_low_priority_cal_set(unit, calendar_port, new_cal));
                                }
                            }
                            
                            update_required = 1;
                            continue;
                        } else {
                            SOCDNX_EXIT_WITH_ERR(SOC_E_RESOURCE, (_BSL_SOCDNX_MSG("No calendars are left for port %d \n"), calendar_port));
                        }
                    }
                }
                SOCDNX_IF_ERR_EXIT(res);
                
                res = arad_ofp_rates_active_egq_generic_calendars_config(
                                                                 unit,
                                                                 core_i,
                                                                 &cal_info,
                                                                 egq_calendar,
                                                                 egq_calendar_len);
                SOCDNX_IF_ERR_EXIT(res);

                
                if (SOC_IS_JERICHO(unit) && calendar_port != -1) {

                    
                    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_channelized_port_get(unit, calendar_port, &is_channelized));
                    if (is_channelized) {
                        res = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port2egress_offset, (unit, core_i, calendar_tm_port, &if_id));
                        SOCDNX_IF_ERR_EXIT(res);
                    } else {
                        if_id = SOC_DPP_DEFS_GET(unit, non_channelized_cal_id);
                    }

                    res = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_ofp_rates_interface_internal_rate_get, (unit, core_i, if_id, &if_internal_rate));
                    SOCDNX_IF_ERR_EXIT(res);                        

                    
                    res = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_ofp_rates_egress_shaper_cal_write, (unit, core_i, &cal_info, cal2set, 
                                                                                                  SOC_TMC_OFP_RATES_EGQ_CHAN_ARB_FIELD_RATE, if_internal_rate));
                    SOCDNX_IF_ERR_EXIT(res);
                }
            }
        }

        }
        
        SOCDNX_SAND_IF_ERR_EXIT(READ_EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr(unit, core_i, &reg_val));
        soc_reg_field_set(unit, EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr, &reg_val, OTM_SPR_SET_SELf, cal2set);
        SOCDNX_SAND_IF_ERR_EXIT(WRITE_EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr(unit, core_i, reg_val));
    }

exit:
    SOCDNX_FREE(egq_rates);
    SOCDNX_FREE(egq_calendar);
    SOCDNX_FUNC_RETURN
}

STATIC uint32
    arad_ofp_rates_max_burst_generic_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  int                      core,
    SOC_SAND_IN  uint32                   base_q_pair,
    SOC_SAND_IN  ARAD_OFP_RATES_CAL_INFO *cal_info,
    SOC_SAND_IN  uint32                   max_burst
    )
{
    uint32  res = SOC_E_NONE,
            fld_val,
            reg_val,
            egq_to_set,
            offset;
    soc_field_t
            field_cal_set_name;
    ARAD_EGQ_PMC_TBL_DATA
            pmc_tbl_data;

    SOCDNX_INIT_FUNC_DEFS

    SOCDNX_NULL_CHECK(cal_info);

    res = arad_ofp_rates_retrieve_egress_shaper_setting_field(
      unit,
      cal_info,
      &field_cal_set_name
    );

    SOCDNX_IF_ERR_EXIT(res);
    
    SOCDNX_SAND_IF_ERR_EXIT(READ_EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr(unit, core, &reg_val));
    fld_val = soc_reg_field_get(unit, EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr, reg_val, OTM_SPR_SET_SELf);
    egq_to_set = (fld_val == ARAD_OFP_RATES_CAL_SET_A) ? ARAD_OFP_RATES_CAL_SET_A : ARAD_OFP_RATES_CAL_SET_B;

    
    offset = ARAD_OFP_RATES_EGQ_PMC_OFFSET_GET(cal_info,egq_to_set,base_q_pair);
    pmc_tbl_data.port_max_credit = max_burst;

    res = arad_egq_pmc_tbl_set_unsafe(
              unit,
              core,
              cal_info,
              offset,
              &pmc_tbl_data
            );
    SOCDNX_SAND_IF_ERR_EXIT(res);

    egq_to_set = !egq_to_set;
    offset = ARAD_OFP_RATES_EGQ_PMC_OFFSET_GET(cal_info,egq_to_set,base_q_pair);
    res = arad_egq_pmc_tbl_set_unsafe(
              unit,
              core,
              cal_info,
              offset,
              &pmc_tbl_data
            );
    SOCDNX_SAND_IF_ERR_EXIT(res);

exit:
    SOCDNX_FUNC_RETURN
     
}

uint32
    arad_ofp_rates_egq_port_priority_max_burst_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core,
    SOC_SAND_IN  uint32                tm_port,   
    SOC_SAND_IN  uint32                priority_ndx, 
    SOC_SAND_IN  uint32                max_burst
    )
{
    uint32  res = SOC_E_NONE,
            base_q_pair;
    ARAD_OFP_RATES_CAL_INFO
            cal_info;
    uint8
        egq_tcg_qpair_shaper_enable;

    SOCDNX_INIT_FUNC_DEFS;

    res = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.egq_tcg_qpair_shaper_enable.get(unit, &egq_tcg_qpair_shaper_enable);
    SOCDNX_IF_ERR_EXIT(res);
    if (egq_tcg_qpair_shaper_enable)
    {
        
        res = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, tm_port, &base_q_pair);
        SOCDNX_IF_ERR_EXIT(res);

        
        cal_info.cal_type = ARAD_OFP_RATES_EGQ_CAL_PORT_PRIORITY;

        res = arad_ofp_rates_max_burst_generic_set_unsafe(unit,
                                                          core,
                                                          (base_q_pair + priority_ndx),
                                                          &cal_info,
                                                          max_burst);
        SOCDNX_IF_ERR_EXIT(res);
    }
    
exit:
    SOCDNX_FUNC_RETURN
     
}

uint32
    arad_ofp_rates_egq_tcg_max_burst_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core,
    SOC_SAND_IN  uint32                tm_port,   
    SOC_SAND_IN  uint32                tcg_ndx, 
    SOC_SAND_IN  uint32                max_burst
    )
{
    uint32  res = SOC_E_NONE,
            tcg_id,
            ps,
            base_q_pair;
    ARAD_OFP_RATES_CAL_INFO
            cal_info;
    uint8
        egq_tcg_qpair_shaper_enable;

    SOCDNX_INIT_FUNC_DEFS;

    res = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.egq_tcg_qpair_shaper_enable.get(unit, &egq_tcg_qpair_shaper_enable);
    SOCDNX_IF_ERR_EXIT(res);
  if (egq_tcg_qpair_shaper_enable)
  {
        
        res = soc_port_sw_db_tm_port_to_base_q_pair_get(unit,core,tm_port,&base_q_pair);
        SOCDNX_IF_ERR_EXIT(res);

        
        ps = ARAD_BASE_PORT_TC2PS(base_q_pair); 
        tcg_id = ARAD_OFP_RATES_TCG_ID_GET(ps,tcg_ndx);

        
        cal_info.cal_type = ARAD_OFP_RATES_EGQ_CAL_TCG;

        res = arad_ofp_rates_max_burst_generic_set_unsafe(unit,
                                                          core,
                                                          tcg_id,
                                                          &cal_info,
                                                          max_burst);
        SOCDNX_IF_ERR_EXIT(res);
  }
    
exit:
    SOCDNX_FUNC_RETURN
     
}

int
    arad_ofp_rates_single_port_max_burst_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core,
    SOC_SAND_IN  uint32                tm_port,
    SOC_SAND_IN  uint32                max_burst
    )
{
    uint32  res = SOC_E_NONE,
            base_q_pair;
    ARAD_OFP_RATES_CAL_INFO
            cal_info;
    SOCDNX_INIT_FUNC_DEFS

    
    res = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, tm_port, &base_q_pair);
    SOCDNX_IF_ERR_EXIT(res);

    
    cal_info.cal_type = ARAD_OFP_RATES_EGQ_CAL_CHAN_ARB;

    res = arad_ofp_rates_max_burst_generic_set_unsafe(unit,
                                                      core,
                                                      base_q_pair,
                                                      &cal_info,
                                                      max_burst);
    SOCDNX_IF_ERR_EXIT(res);
    
exit:
    SOCDNX_FUNC_RETURN
     
}

STATIC uint32
    arad_ofp_rates_max_burst_generic_get_unsafe(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  int                     core,
    SOC_SAND_IN  uint32                  tm_port,
    SOC_SAND_IN  ARAD_OFP_RATES_CAL_INFO *cal_info,
    SOC_SAND_OUT  uint32                 *max_burst
    )
{
    uint32  res = SOC_E_NONE,
            egq_to_get,
            fld_val,
            reg_val,
            offset;
    soc_field_t
            field_cal_set_name;
    ARAD_EGQ_PMC_TBL_DATA
            pmc_tbl_data;

    SOCDNX_INIT_FUNC_DEFS

    SOCDNX_NULL_CHECK(max_burst);
    SOCDNX_NULL_CHECK(cal_info);

    res = arad_ofp_rates_retrieve_egress_shaper_setting_field(
      unit,
      cal_info,
      &field_cal_set_name
    );
    SOCDNX_IF_ERR_EXIT(res);
    
    SOCDNX_SAND_IF_ERR_EXIT(READ_EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr(unit, core, &reg_val));
    fld_val = soc_reg_field_get(unit, EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr, reg_val, OTM_SPR_SET_SELf);
    egq_to_get = (fld_val == ARAD_OFP_RATES_CAL_SET_A) ? ARAD_OFP_RATES_CAL_SET_A : ARAD_OFP_RATES_CAL_SET_B;

    
    offset = ARAD_OFP_RATES_EGQ_PMC_OFFSET_GET(cal_info,egq_to_get,tm_port);

    res = arad_egq_pmc_tbl_get_unsafe(
              unit,
              core,
              cal_info,
              offset,
              &pmc_tbl_data
            );
    SOCDNX_SAND_IF_ERR_EXIT(res);

    *max_burst = pmc_tbl_data.port_max_credit;

exit:
    SOCDNX_FUNC_RETURN
     
}

int
    arad_ofp_rates_single_port_max_burst_get_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                    core,
    SOC_SAND_IN  uint32                 tm_port,
    SOC_SAND_OUT  uint32                *max_burst
    )
{
    uint32  res = SOC_E_NONE,
            base_q_pair;
    ARAD_OFP_RATES_CAL_INFO
            cal_info;
    SOCDNX_INIT_FUNC_DEFS

    SOCDNX_NULL_CHECK(max_burst);

    
    res = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, tm_port, &base_q_pair);
    SOCDNX_IF_ERR_EXIT(res);

    
    cal_info.cal_type = ARAD_OFP_RATES_EGQ_CAL_CHAN_ARB;

    res = arad_ofp_rates_max_burst_generic_get_unsafe(unit,
                                                      core,
                                                      base_q_pair,
                                                      &cal_info,
                                                      max_burst);
    SOCDNX_IF_ERR_EXIT(res);

exit:
    SOCDNX_FUNC_RETURN
     
}

uint32
    arad_ofp_rates_egq_tcg_max_burst_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core,
    SOC_SAND_IN  uint32                tm_port,   
    SOC_SAND_IN  uint32                tcg_ndx, 
    SOC_SAND_OUT  uint32               *max_burst
    )
{
    uint32  res = SOC_E_NONE,
            tcg_id,
            ps,
            base_q_pair;
    ARAD_OFP_RATES_CAL_INFO
            cal_info;
    uint8
        egq_tcg_qpair_shaper_enable;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(max_burst);

    res = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.egq_tcg_qpair_shaper_enable.get(unit, &egq_tcg_qpair_shaper_enable);
    SOCDNX_IF_ERR_EXIT(res);
  if (egq_tcg_qpair_shaper_enable)
  {
        
        res = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, tm_port, &base_q_pair);
        SOCDNX_IF_ERR_EXIT(res);

        
        ps = ARAD_BASE_PORT_TC2PS(base_q_pair); 
        tcg_id = ARAD_OFP_RATES_TCG_ID_GET(ps,tcg_ndx);

        
        cal_info.cal_type = ARAD_OFP_RATES_EGQ_CAL_TCG;

        res = arad_ofp_rates_max_burst_generic_get_unsafe(unit,
                                                          core,
                                                          tcg_id,
                                                          &cal_info,
                                                          max_burst);
        SOCDNX_IF_ERR_EXIT(res);
  }
    
exit:
    SOCDNX_FUNC_RETURN
     
}

uint32
    arad_ofp_rates_egq_port_priority_max_burst_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core,
    SOC_SAND_IN  uint32                tm_port,     
    SOC_SAND_IN  uint32                priority_ndx, 
    SOC_SAND_OUT  uint32               *max_burst
    )
{
    uint32  res = SOC_E_NONE,
            base_q_pair;
    ARAD_OFP_RATES_CAL_INFO
            cal_info;
    uint8
        egq_tcg_qpair_shaper_enable;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(max_burst);

    res = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.egq_tcg_qpair_shaper_enable.get(unit, &egq_tcg_qpair_shaper_enable);
    SOCDNX_IF_ERR_EXIT(res);
    if (egq_tcg_qpair_shaper_enable)
    {
        
        res = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, tm_port, &base_q_pair);
        SOCDNX_IF_ERR_EXIT(res);

        
        cal_info.cal_type = ARAD_OFP_RATES_EGQ_CAL_PORT_PRIORITY;

        res = arad_ofp_rates_max_burst_generic_get_unsafe(unit,
                                                          core,
                                                          (base_q_pair + priority_ndx),
                                                          &cal_info,
                                                          max_burst);
        SOCDNX_IF_ERR_EXIT(res);
    }
exit:
    SOCDNX_FUNC_RETURN
     
}

uint32
  arad_ofp_rates_egq_interface_shaper_set_unsafe(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  int                         core,
    SOC_SAND_IN  uint32                      tm_port,
    SOC_SAND_IN SOC_TMC_OFP_SHPR_UPDATE_MODE rate_update_mode,
    SOC_SAND_IN  uint32                      if_shaper_rate
  )
{
  uint32
    res = SOC_E_NONE,
    if_rate = if_shaper_rate;
  ARAD_OFP_RATES_CAL_INFO
    cal_info;
  ARAD_INTERFACE_ID         
      if_id;

  SOCDNX_INIT_FUNC_DEFS;

  
  res = arad_port_to_interface_map_get(
          unit,
          core,
          tm_port,
          &if_id,
          NULL
        );
 SOCDNX_IF_ERR_EXIT(res);

  if (if_id == ARAD_NIF_ID_NONE)
  {
    
    ARAD_DO_NOTHING_AND_EXIT;
  }

  cal_info.cal_type = ARAD_OFP_RATES_EGQ_CAL_CHAN_ARB;
  res = arad_ofp_rates_port2chan_arb_unsafe(unit,tm_port,&cal_info.chan_arb_id);
  SOCDNX_IF_ERR_EXIT(res);
   
  res = arad_ofp_rates_egq_shaper_config(
          unit,
          &cal_info,
          if_rate
        );
  SOCDNX_IF_ERR_EXIT(res);

exit:
  SOCDNX_FUNC_RETURN
}


uint32
  arad_ofp_rates_interface_shaper_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  int                       core,
    SOC_SAND_IN  uint32                    tm_port

  )
{
    uint32
        res = SOC_E_NONE;
  
    ARAD_INTERFACE_ID
        if_id;

    SOCDNX_INIT_FUNC_DEFS;

    
    res = arad_port_to_interface_map_get(
          unit,
          core,
          tm_port,
          &if_id,
          NULL
        );
    SOCDNX_IF_ERR_EXIT(res);

    res = arad_interface_id_verify(
          unit,
          if_id
        );
    SOCDNX_SAND_IF_ERR_EXIT(res);

exit:
    SOCDNX_FUNC_RETURN
}

uint32
  arad_ofp_rates_egq_interface_shaper_get_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  int                       core,
    SOC_SAND_IN  uint32                    tm_port,
    SOC_SAND_OUT uint32                    *if_shaper_rate
  )
{
  uint32
    res = SOC_E_NONE;
  uint32
    chan_arb_id;  
  ARAD_INTERFACE_ID         
      if_id;

  SOCDNX_INIT_FUNC_DEFS;
  
  SOCDNX_NULL_CHECK(if_shaper_rate);

  
  res = arad_port_to_interface_map_get(
          unit,
          core,
          tm_port,
          &if_id,
          NULL
        );
  SOCDNX_IF_ERR_EXIT(res);

  SOCDNX_NULL_CHECK(if_shaper_rate);

  res = arad_ofp_rates_if_id2chan_arb_get_unsafe(unit, core, tm_port, &chan_arb_id);
  SOCDNX_IF_ERR_EXIT(res);

  res = arad_ofp_rates_egq_shaper_retrieve(
          unit,
          chan_arb_id,
          TRUE,
          if_shaper_rate
        );
  SOCDNX_IF_ERR_EXIT(res);

exit:
  SOCDNX_FUNC_RETURN
}

uint32
    arad_ofp_rates_port_priority_max_burst_for_empty_queues_set_unsafe(
       SOC_SAND_IN  int    unit,
       SOC_SAND_IN  uint32 max_burst_empty_queues
       )
{
    uint32 res = SOC_SAND_NO_ERR,
           reg_val;
    int    core;

    SOCDNX_INIT_FUNC_DEFS

    
    if (SOC_IS_ARADPLUS(unit)) {
        SOC_DPP_CORES_ITER(SOC_CORE_ALL, core) {
            
            res=READ_EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr(unit, core, &reg_val);
            SOCDNX_SAND_IF_ERR_EXIT(res);
            soc_reg_field_set(unit, EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr, &reg_val, EMPTY_QP_STOP_COLLECTING_ENf, max_burst_empty_queues ? 1 : 0);
            res=WRITE_EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr(unit, core, reg_val);
            SOCDNX_SAND_IF_ERR_EXIT(res);
            
            res=READ_EGQ_QP_MAX_CREDIT_VALUEr(unit, core,&reg_val);
            SOCDNX_SAND_IF_ERR_EXIT(res);
            soc_reg_field_set(unit, EGQ_QP_MAX_CREDIT_VALUEr, &reg_val, EMPTY_QP_MAX_CREDITf, max_burst_empty_queues);
            res=WRITE_EGQ_QP_MAX_CREDIT_VALUEr(unit, core, reg_val);
            SOCDNX_SAND_IF_ERR_EXIT(res);
        }
    }
    

exit:
  SOCDNX_FUNC_RETURN
}


uint32
    arad_ofp_rates_port_priority_max_burst_for_fc_queues_set_unsafe(
       SOC_SAND_IN  int    unit,
       SOC_SAND_IN  uint32 max_burst_fc_queues
       )
{
    uint32 res = SOC_SAND_NO_ERR,
           reg_val;
    int    core;

    SOCDNX_INIT_FUNC_DEFS

    
    if (SOC_IS_ARADPLUS(unit)) {
        SOC_DPP_CORES_ITER(SOC_CORE_ALL, core) {
            
            res=READ_EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr(unit, core, &reg_val);
            SOCDNX_SAND_IF_ERR_EXIT(res);
            soc_reg_field_set(unit, EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr, &reg_val, FC_QP_STOP_COLLECTING_ENf, max_burst_fc_queues ? 1 : 0);
            res=WRITE_EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr(unit, core, reg_val);
            SOCDNX_SAND_IF_ERR_EXIT(res);
             
            res=READ_EGQ_QP_MAX_CREDIT_VALUEr(unit, core,&reg_val);
            SOCDNX_SAND_IF_ERR_EXIT(res);
            soc_reg_field_set(unit, EGQ_QP_MAX_CREDIT_VALUEr, &reg_val, FC_QP_MAX_CREDITf, max_burst_fc_queues);
            res=WRITE_EGQ_QP_MAX_CREDIT_VALUEr(unit, core, reg_val);
            SOCDNX_SAND_IF_ERR_EXIT(res);
        }
    }
    


exit:
  SOCDNX_FUNC_RETURN
}

uint32
    arad_ofp_rates_port_priority_max_burst_for_fc_queues_get_unsafe(
       SOC_SAND_IN  int    unit,
       SOC_SAND_OUT  uint32 *max_burst_fc_queues
       )
{
    uint32 res = SOC_SAND_NO_ERR,
           reg_val;

    SOCDNX_INIT_FUNC_DEFS

    SOCDNX_NULL_CHECK(max_burst_fc_queues);

    
    if (SOC_IS_ARADPLUS(unit)) {
        res=READ_EGQ_QP_MAX_CREDIT_VALUEr(unit, 0,&reg_val);
        SOCDNX_SAND_IF_ERR_EXIT(res);
        *max_burst_fc_queues = soc_reg_field_get(unit, EGQ_QP_MAX_CREDIT_VALUEr, reg_val, FC_QP_MAX_CREDITf);
    }
    


exit:
  SOCDNX_FUNC_RETURN
}

uint32
    arad_ofp_rates_port_priority_max_burst_for_empty_queues_get_unsafe(
       SOC_SAND_IN  int    unit,
       SOC_SAND_OUT  uint32 *max_burst_empty_queues
       )
{
    uint32 res = SOC_SAND_NO_ERR,
           reg_val;

    SOCDNX_INIT_FUNC_DEFS

    SOCDNX_NULL_CHECK(max_burst_empty_queues);

    
    if (SOC_IS_ARADPLUS(unit)) {
        res=READ_EGQ_QP_MAX_CREDIT_VALUEr(unit, 0,&reg_val);
        SOCDNX_SAND_IF_ERR_EXIT(res);
        *max_burst_empty_queues = soc_reg_field_get(unit, EGQ_QP_MAX_CREDIT_VALUEr, reg_val, EMPTY_QP_MAX_CREDITf);
    }
    

exit:
  SOCDNX_FUNC_RETURN
}

int
    arad_ofp_rates_egq_tcg_rate_sw_set_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                    core, 
    SOC_SAND_IN  uint32                 tm_port, 
    SOC_SAND_IN  ARAD_TCG_NDX           tcg_ndx,   
    SOC_SAND_IN  uint32                 tcg_rate
    )
{
    uint32 res = SOC_E_NONE,
           base_q_pair,
           ps;
    ARAD_SW_DB_DEV_RATE
           rate;
    uint8
        egq_tcg_qpair_shaper_enable;

    SOCDNX_INIT_FUNC_DEFS;

    res = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.egq_tcg_qpair_shaper_enable.get(unit, &egq_tcg_qpair_shaper_enable);
    SOCDNX_IF_ERR_EXIT(res);
  if (egq_tcg_qpair_shaper_enable)
  {

        
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, tm_port, &base_q_pair));

        
        ps = ARAD_BASE_PORT_TC2PS(base_q_pair);
        
        sal_memset(&rate,0,sizeof(rate));
        rate.valid = TRUE;
        rate.egq_rates  = tcg_rate;
        
        res = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.tcg_cal.tcg_rate.set(
                unit,
                core,
                ps,
                tcg_ndx,
                &rate
              );
        SOCDNX_IF_ERR_EXIT(res);

  }
exit:
    SOCDNX_FUNC_RETURN
}


uint32
    arad_ofp_rates_egq_port_priority_rate_sw_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core,  
    SOC_SAND_IN  uint32                tm_port, 
    SOC_SAND_IN  uint32                prio_ndx,   
    SOC_SAND_IN  uint32                ptc_rate
    )
{
    uint32 res = SOC_E_NONE,
           base_q_pair;
    ARAD_SW_DB_DEV_RATE
           rate;
    uint8
        egq_tcg_qpair_shaper_enable;
 
    SOCDNX_INIT_FUNC_DEFS;

    res = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.egq_tcg_qpair_shaper_enable.get(unit, &egq_tcg_qpair_shaper_enable);
    SOCDNX_IF_ERR_EXIT(res);
    if (egq_tcg_qpair_shaper_enable)
    {
        
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, tm_port, &base_q_pair));

        res = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.port_priority_cal.queue_rate.get(
                unit,
                core,
                (base_q_pair + prio_ndx),
                &rate
              );
        SOCDNX_IF_ERR_EXIT(res);

        
        rate.valid = TRUE;
        rate.egq_rates  = ptc_rate;
        
        res = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.port_priority_cal.queue_rate.set(
                unit,
                core,
                (base_q_pair + prio_ndx),
                &rate
              );
        SOCDNX_IF_ERR_EXIT(res);
    }
exit:
    SOCDNX_FUNC_RETURN
}


int
    arad_ofp_rates_egq_tcg_rate_hw_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core,
    SOC_SAND_IN  ARAD_FAP_PORT_ID      tm_port, 
    SOC_SAND_IN  ARAD_TCG_NDX          tcg_ndx,   
    SOC_SAND_OUT  uint32               *tcg_rate
    )
{
    uint32 res = SOC_E_NONE,
           egq_cal_len,
           *egq_rates = NULL,
           base_q_pair,
           ps,
           tcg_id;

    ARAD_OFP_RATES_CAL_INFO 
           cal_info;

    ARAD_OFP_RATES_CAL_EGQ      
           *egq_cal = NULL;
    uint8
        egq_tcg_qpair_shaper_enable;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(tcg_rate);

    SOCDNX_ALLOC(egq_rates, uint32, ARAD_NOF_FAP_PORTS,"egq_rates");
    SOCDNX_ALLOC(egq_cal, ARAD_OFP_RATES_CAL_EGQ, 1,"egq_cal");

    arad_ARAD_OFP_RATES_CAL_INFO_clear(&cal_info);

    res = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.egq_tcg_qpair_shaper_enable.get(unit, &egq_tcg_qpair_shaper_enable);
    SOCDNX_IF_ERR_EXIT(res);
  if (egq_tcg_qpair_shaper_enable)
  {

    cal_info.cal_type = ARAD_OFP_RATES_EGQ_CAL_TCG;

    egq_cal_len = 0;
    
    res = arad_ofp_rates_active_generic_calendars_retrieve_egq(
            unit,
            core,
            &cal_info,
            egq_cal,
            &egq_cal_len,
            FALSE
        );
    SOCDNX_IF_ERR_EXIT(res);

    
    res = arad_ofp_rates_from_generic_calendar_to_ports_egq_rate(
          unit,
          core,
          &cal_info,
          egq_cal,
          egq_cal_len,
          FALSE,
          ARAD_EGR_NOF_Q_PAIRS,
          egq_rates
        );
    SOCDNX_IF_ERR_EXIT(res);
    
    res = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, tm_port, &base_q_pair);
    SOCDNX_IF_ERR_EXIT(res);
    if (base_q_pair == ARAD_EGR_INVALID_BASE_Q_PAIR)
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG_STR( "Illegal port")));
    }
    ps = ARAD_BASE_PORT_TC2PS(base_q_pair);
    tcg_id = ARAD_OFP_RATES_TCG_ID_GET(ps,tcg_ndx);
    *tcg_rate = egq_rates[tcg_id];
  }
exit:
    SOCDNX_FREE(egq_rates);
    SOCDNX_FREE(egq_cal);
    SOCDNX_FUNC_RETURN
}

uint32
    arad_ofp_rates_egq_port_priority_rate_hw_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core, 
    SOC_SAND_IN  uint32                tm_port, 
    SOC_SAND_IN  ARAD_TCG_NDX          ptc_ndx,   
    SOC_SAND_OUT uint32                *ptc_rate
    )
{
   uint32 res = SOC_E_NONE,
           egq_cal_len,
           *egq_rates = NULL,
           base_q_pair;

    ARAD_OFP_RATES_CAL_EGQ      
           *egq_cal = NULL;
    ARAD_OFP_RATES_CAL_INFO
           cal_info;
    uint8
        egq_tcg_qpair_shaper_enable;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(ptc_rate);

    SOCDNX_ALLOC(egq_cal, ARAD_OFP_RATES_CAL_EGQ, 1,"egq_cal");

    res = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.egq_tcg_qpair_shaper_enable.get(unit, &egq_tcg_qpair_shaper_enable);
    SOCDNX_IF_ERR_EXIT(res);

    if (egq_tcg_qpair_shaper_enable)
    {
        SOCDNX_ALLOC(egq_rates, uint32, ARAD_NOF_FAP_PORTS,"egq_rates");
        arad_ARAD_OFP_RATES_CAL_INFO_clear(&cal_info);
          
        cal_info.cal_type = ARAD_OFP_RATES_EGQ_CAL_PORT_PRIORITY;

        egq_cal_len = 0;
        
        res = arad_ofp_rates_active_generic_calendars_retrieve_egq(
                unit,
                core,
                &cal_info,
                egq_cal,
                &egq_cal_len,
                FALSE
            );
        SOCDNX_IF_ERR_EXIT(res);
        
        
        res = arad_ofp_rates_from_generic_calendar_to_ports_egq_rate(
              unit,
              core,
              &cal_info,
              egq_cal,
              egq_cal_len,
              FALSE,
              ARAD_EGR_NOF_Q_PAIRS,
              egq_rates
            );
        SOCDNX_IF_ERR_EXIT(res);
        
        res = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, tm_port, &base_q_pair);
        SOCDNX_IF_ERR_EXIT(res);
        if (base_q_pair == ARAD_EGR_INVALID_BASE_Q_PAIR)
        {
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG_STR( "Illegal port")));
        }
        *ptc_rate = egq_rates[base_q_pair + ptc_ndx];
    }

exit:
    SOCDNX_FREE(egq_rates);
    SOCDNX_FREE(egq_cal);
    SOCDNX_FUNC_RETURN
}


int
    arad_ofp_rates_egq_tcg_rate_hw_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core
    )
{
    uint32 
        res = SOC_E_NONE,
        *egq_rates = NULL,
        egq_calendar_len = 0,
        fap_port_ndx,
        base_q_pair,
        nof_priorities,
        ps,
        recalc = TRUE,
        tcg_ndx,
        tcg_id,
        act_cal,
        sum_of_tcg_requested = 0,
        reg_val,
        port_i,
        flags;
    ARAD_OFP_RATES_CAL_EGQ
        *egq_calendar = NULL;
    ARAD_OFP_RATES_CAL_INFO
        cal_info; 
    ARAD_SW_DB_DEV_EGR_TCG
        rates;
    ARAD_OFP_RATES_CAL_SET
        cal2set;
    soc_pbmp_t
        pbmp;
    int
      core_i;
    uint8
        egq_tcg_qpair_shaper_enable;

    SOCDNX_INIT_FUNC_DEFS;
  
    SOC_CLEAR(&cal_info, ARAD_OFP_RATES_CAL_INFO, 1);
    SOCDNX_ALLOC(egq_rates, uint32, ARAD_NOF_FAP_PORTS,"egq_rates");
    SOCDNX_ALLOC(egq_calendar, ARAD_OFP_RATES_CAL_EGQ, 1,"egq_calendar");

    res = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.egq_tcg_qpair_shaper_enable.get(unit, &egq_tcg_qpair_shaper_enable);
    SOCDNX_IF_ERR_EXIT(res);
  if (egq_tcg_qpair_shaper_enable)
  {
        
        SOCDNX_SAND_IF_ERR_EXIT(READ_EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr(unit, core, &reg_val));
        act_cal = soc_reg_field_get(unit, EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr, reg_val, TCG_SPR_SET_SELf);
        cal2set = (act_cal == ARAD_OFP_RATES_CAL_SET_A)?ARAD_OFP_RATES_CAL_SET_B:ARAD_OFP_RATES_CAL_SET_A;

        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_valid_ports_core_get(unit, core, 0, &pbmp));

        SOC_PBMP_ITER(pbmp, port_i)
        {
          SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port_i, &flags)); 
          if (SOC_PORT_IS_NOT_VALID_FOR_EGRESS_TM(flags))
          {
              continue;
          }
          SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port_i, &fap_port_ndx, &core_i));
          SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core_i, fap_port_ndx, &base_q_pair));
          if (base_q_pair == ARAD_EGR_INVALID_BASE_Q_PAIR)
          {
            continue;
          }

          SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_port_to_out_port_priority_get(unit, core_i, fap_port_ndx, &nof_priorities));

          ps = ARAD_BASE_PORT_TC2PS(base_q_pair);

          res = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.tcg_cal.get(unit,core_i,&rates);
          SOCDNX_IF_ERR_EXIT(res);

          for (tcg_ndx = 0; tcg_ndx < nof_priorities; ++tcg_ndx)
          {
            if (rates.tcg_rate[ps][tcg_ndx].valid)
            {
                tcg_id = ARAD_OFP_RATES_TCG_ID_GET(ps,tcg_ndx);
                egq_rates[tcg_id] = rates.tcg_rate[ps][tcg_ndx].egq_rates;
                sum_of_tcg_requested += rates.tcg_rate[ps][tcg_ndx].egq_rates;
            }
          }
        }

        cal_info.cal_type = ARAD_OFP_RATES_EGQ_CAL_TCG;
        res = arad_ofp_rates_from_egq_ports_rates_to_generic_calendar(unit,
                                                                      core,
                                                                      &cal_info,
                                                                      egq_rates,
                                                                      ARAD_EGR_NOF_TCG_IDS,
                                                                      sum_of_tcg_requested,
                                                                      ARAD_OFP_RATES_CAL_LEN_EGQ_TCG_MAX,
                                                                      recalc,
                                                                      FALSE,
                                                                      egq_calendar,
                                                                      &egq_calendar_len,
                                                                      0);
        SOCDNX_IF_ERR_EXIT(res);

        
        res = arad_ofp_rates_active_egq_generic_calendars_config(unit,
                                                           core,
                                                           &cal_info,
                                                           egq_calendar,
                                                           egq_calendar_len);
        SOCDNX_IF_ERR_EXIT(res);

        
        SOCDNX_SAND_IF_ERR_EXIT(READ_EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr(unit, core, &reg_val));
        soc_reg_field_set(unit, EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr, &reg_val, TCG_SPR_SET_SELf, cal2set);
        SOCDNX_SAND_IF_ERR_EXIT(WRITE_EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr(unit, core, reg_val));
  }

exit:
    SOCDNX_FREE(egq_rates);
    SOCDNX_FREE(egq_calendar);
    SOCDNX_FUNC_RETURN;
}



uint32
    arad_ofp_rates_egq_port_priority_rate_hw_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core
    )
{
    uint32 
        res = SOC_E_NONE,
        *egq_rates = NULL,
        egq_calendar_len = 0,
        fap_port_ndx,
        base_q_pair,
        nof_priorities,
        ptc_ndx,
        recalc = TRUE,
        act_cal,
        sum_of_ptc_requested = 0,
        port_i,
        reg_val,
        flags;
    ARAD_OFP_RATES_CAL_EGQ
        *egq_calendar = NULL;
    ARAD_OFP_RATES_CAL_INFO
        cal_info; 
    ARAD_SW_DB_DEV_EGR_PORT_PRIORITY
        rates;
    ARAD_OFP_RATES_CAL_SET
        cal2set;
    soc_pbmp_t
        pbmp;
    int
        core_i;
    uint8
        egq_tcg_qpair_shaper_enable;

    SOCDNX_INIT_FUNC_DEFS
  
    SOC_CLEAR(&cal_info, ARAD_OFP_RATES_CAL_INFO, 1);
    SOCDNX_ALLOC(egq_rates, uint32, ARAD_NOF_FAP_PORTS,"egq_rates");
    SOCDNX_ALLOC(egq_calendar, ARAD_OFP_RATES_CAL_EGQ, 1,"egq_calendar");

    
    SOCDNX_SAND_IF_ERR_EXIT(READ_EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr(unit, core, &reg_val));
    act_cal = soc_reg_field_get(unit, EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr, reg_val, QPAIR_SPR_SET_SELf);
    cal2set = (act_cal == ARAD_OFP_RATES_CAL_SET_A)?ARAD_OFP_RATES_CAL_SET_B:ARAD_OFP_RATES_CAL_SET_A;

    res = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.egq_tcg_qpair_shaper_enable.get(unit, &egq_tcg_qpair_shaper_enable);
    SOCDNX_IF_ERR_EXIT(res);
    if (egq_tcg_qpair_shaper_enable)
    {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_valid_ports_core_get(unit, core, 0, &pbmp));

        SOC_PBMP_ITER(pbmp, port_i)
        {
          SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port_i, &flags)); 
          if (SOC_PORT_IS_NOT_VALID_FOR_EGRESS_TM(flags))
          {
              continue;
          }
          SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port_i, &fap_port_ndx, &core_i));
          SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core_i, fap_port_ndx, &base_q_pair));
          
          if (base_q_pair == ARAD_EGR_INVALID_BASE_Q_PAIR)
          {
            continue;
          }

          SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_port_to_out_port_priority_get(unit, core_i, fap_port_ndx,  &nof_priorities));

          res = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.port_priority_cal.get(unit,core_i,&rates);

          SOCDNX_IF_ERR_EXIT(res);

          for (ptc_ndx = 0; ptc_ndx < nof_priorities; ++ptc_ndx)
          {
            if (rates.queue_rate[base_q_pair + ptc_ndx].valid)
            {
                egq_rates[base_q_pair + ptc_ndx] = rates.queue_rate[base_q_pair + ptc_ndx].egq_rates;
                sum_of_ptc_requested += rates.queue_rate[base_q_pair + ptc_ndx].egq_rates;
            }
          }
        }

        cal_info.cal_type = ARAD_OFP_RATES_EGQ_CAL_PORT_PRIORITY;
        res = arad_ofp_rates_from_egq_ports_rates_to_generic_calendar(
              unit,
              core,
              &cal_info,
              egq_rates,
              ARAD_EGR_NOF_TCG_IDS,
              sum_of_ptc_requested,
              ARAD_OFP_RATES_CAL_LEN_EGQ_TCG_MAX,
              recalc,
              FALSE,
              egq_calendar,
              &egq_calendar_len,
              0
            );
        SOCDNX_IF_ERR_EXIT(res);

        
        res = arad_ofp_rates_active_egq_generic_calendars_config(unit,
                                                           core,
                                                           &cal_info,
                                                           egq_calendar,
                                                           egq_calendar_len);
        SOCDNX_IF_ERR_EXIT(res);

        
        SOCDNX_SAND_IF_ERR_EXIT(READ_EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr(unit, core, &reg_val));
        soc_reg_field_set(unit, EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr, &reg_val, QPAIR_SPR_SET_SELf, cal2set);
        SOCDNX_SAND_IF_ERR_EXIT(WRITE_EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr(unit, core, reg_val));
    }

exit:
    SOCDNX_FREE(egq_rates);
    SOCDNX_FREE(egq_calendar);
    SOCDNX_FUNC_RETURN
}



STATIC uint32
    arad_ofp_rates_tcg_ptc_sch_generic_set_unsafe(
    SOC_SAND_IN  int                   unit,  
    SOC_SAND_IN  int                   core,  
    SOC_SAND_IN  uint32                tm_port, 
    SOC_SAND_IN  uint32                octet_ndx,   
    SOC_SAND_IN  uint32                rate,
    SOC_SAND_IN  ARAD_OFP_RATES_GENERIC_FUNC_STATE func_state
    )
{
    uint32 res = SOC_E_NONE;

    SOCDNX_INIT_FUNC_DEFS

    switch (func_state) {
    case ARAD_OFP_RATES_GENERIC_FUNC_STATE_PTC_SCH_BURST:
        res = arad_sch_port_priority_shaper_max_burst_set_unsafe(unit, core, tm_port, octet_ndx, rate);
        SOCDNX_IF_ERR_EXIT(res);
        break;
    case ARAD_OFP_RATES_GENERIC_FUNC_STATE_PTC_SCH_RATE:
        res = arad_sch_port_priority_shaper_rate_set_unsafe(unit, core, tm_port, octet_ndx, rate);
        SOCDNX_SAND_IF_ERR_EXIT(res);
        break;
    case ARAD_OFP_RATES_GENERIC_FUNC_STATE_TCG_SCH_RATE:
        res = arad_sch_tcg_shaper_rate_set_unsafe(unit, core, tm_port, octet_ndx,rate);
        SOCDNX_SAND_IF_ERR_EXIT(res);
        break;
    case ARAD_OFP_RATES_GENERIC_FUNC_STATE_TCG_SCH_BURST:
        res = arad_sch_tcg_shaper_max_burst_set_unsafe(unit, core, tm_port, octet_ndx, rate);
        SOCDNX_IF_ERR_EXIT(res);
        break;
         
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG_STR( "Illegal enum")));
    }

exit:
    SOCDNX_FUNC_RETURN
}

STATIC uint32
    arad_ofp_rates_tcg_ptc_sch_generic_get_unsafe(
    SOC_SAND_IN  int                   unit,  
    SOC_SAND_IN  int                   core,
    SOC_SAND_IN  uint32                tm_port,
    SOC_SAND_IN  uint32                octet_ndx,   
    SOC_SAND_OUT uint32                *rate,
    SOC_SAND_IN  ARAD_OFP_RATES_GENERIC_FUNC_STATE func_state
    )
{
    uint32 res = SOC_E_NONE;
    ARAD_SCH_TCG_RATE_INFO
        tcg_sch_rate_info;
    ARAD_SCH_PORT_PRIORITY_RATE_INFO
        ptc_sch_rate_info;


    SOCDNX_INIT_FUNC_DEFS

    SOCDNX_NULL_CHECK(rate);

    switch (func_state) {
    case ARAD_OFP_RATES_GENERIC_FUNC_STATE_PTC_SCH_BURST:
    case ARAD_OFP_RATES_GENERIC_FUNC_STATE_PTC_SCH_RATE:
        res = arad_sch_port_priority_shaper_rate_get_unsafe(
              unit, core,
              tm_port,
              octet_ndx,
              &ptc_sch_rate_info
            );
        SOCDNX_SAND_IF_ERR_EXIT(res);

        switch (func_state) {
        case ARAD_OFP_RATES_GENERIC_FUNC_STATE_PTC_SCH_RATE:
             *rate = ptc_sch_rate_info.rate;
             break;
        case ARAD_OFP_RATES_GENERIC_FUNC_STATE_PTC_SCH_BURST:
             *rate =  ptc_sch_rate_info.max_burst;
             break;
        default:
           SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG_STR( "Illegal enum")));
        }
        break;
    break;

    case ARAD_OFP_RATES_GENERIC_FUNC_STATE_TCG_SCH_RATE:
    case ARAD_OFP_RATES_GENERIC_FUNC_STATE_TCG_SCH_BURST:
    
        res = arad_sch_tcg_shaper_rate_get_unsafe(
              unit, core,
              tm_port,
              octet_ndx,
              &tcg_sch_rate_info
            );
        SOCDNX_SAND_IF_ERR_EXIT(res);

        switch (func_state) {
        case ARAD_OFP_RATES_GENERIC_FUNC_STATE_TCG_SCH_RATE:
             *rate = tcg_sch_rate_info.rate;
             break;
        case ARAD_OFP_RATES_GENERIC_FUNC_STATE_TCG_SCH_BURST:
             *rate = tcg_sch_rate_info.max_burst;
             break;
        
        
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG_STR( "Illegal enum")));
        }
        break;
    
    
    default:
       SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG_STR( "Illegal enum")));
    }

exit:
    SOCDNX_FUNC_RETURN
}


uint32
    arad_ofp_rates_sch_tcg_rate_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core,
    SOC_SAND_IN  uint32                tm_port, 
    SOC_SAND_IN  ARAD_TCG_NDX          tcg_ndx,   
    SOC_SAND_IN  uint32                tcg_rate
    )
{
    uint32 res = SOC_E_NONE;

    SOCDNX_INIT_FUNC_DEFS

    res = arad_ofp_rates_tcg_ptc_sch_generic_set_unsafe(unit, core,
                                                      tm_port, 
                                                      tcg_ndx,   
                                                      tcg_rate,
                                                      ARAD_OFP_RATES_GENERIC_FUNC_STATE_TCG_SCH_RATE);
    SOCDNX_IF_ERR_EXIT(res);

exit:
    SOCDNX_FUNC_RETURN
}


int
arad_ofp_rates_sch_tcg_shaper_disable(
    SOC_SAND_IN     int               unit,
    SOC_SAND_IN     int               core,
    SOC_SAND_IN     uint32            base_qpair,
    SOC_SAND_IN     uint32            nof_q_pairs)
{
    uint32  data = 0x7FFFFFF;
    uint32 q_pair;

    SOCDNX_INIT_FUNC_DEFS;


    
    for (q_pair = base_qpair; q_pair < base_qpair + nof_q_pairs; q_pair++)
    {
        SOCDNX_IF_ERR_EXIT(WRITE_SCH_CIR_SHAPERS_STATIC_TABEL_CSSTm(unit, SCH_BLOCK(unit, core), q_pair, &data)); 
    } 
 
exit:
    SOCDNX_FUNC_RETURN;
}

uint32
    arad_ofp_rates_sch_tcg_max_burst_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core, 
    SOC_SAND_IN  uint32                tm_port, 
    SOC_SAND_IN  ARAD_TCG_NDX          tcg_ndx,   
    SOC_SAND_IN  uint32                max_burst
    )
{
    uint32 res = SOC_E_NONE;

    SOCDNX_INIT_FUNC_DEFS

    res = arad_ofp_rates_tcg_ptc_sch_generic_set_unsafe(unit, core,
                                                              tm_port, 
                                                              tcg_ndx,   
                                                              max_burst,
                                                              ARAD_OFP_RATES_GENERIC_FUNC_STATE_TCG_SCH_BURST);
    SOCDNX_IF_ERR_EXIT(res);

exit:
    SOCDNX_FUNC_RETURN
}

uint32
    arad_ofp_rates_sch_port_priority_rate_set_unsafe(
    SOC_SAND_IN  int                   unit,  
    SOC_SAND_IN  int                   core,  
    SOC_SAND_IN  uint32                tm_port, 
    SOC_SAND_IN  uint32                prio_ndx,   
    SOC_SAND_IN  uint32                tcg_rate
    )
{
    uint32 res = SOC_E_NONE;

    SOCDNX_INIT_FUNC_DEFS


    res = arad_ofp_rates_tcg_ptc_sch_generic_set_unsafe(unit, core,
                                                              tm_port, 
                                                              prio_ndx,   
                                                              tcg_rate,
                                                              ARAD_OFP_RATES_GENERIC_FUNC_STATE_PTC_SCH_RATE);
    SOCDNX_IF_ERR_EXIT(res);

exit:
    SOCDNX_FUNC_RETURN
}

uint32
    arad_ofp_rates_sch_port_priority_rate_sw_set_unsafe(
    SOC_SAND_IN  int                   unit,  
    SOC_SAND_IN  int                   core,  
    SOC_SAND_IN  uint32                tm_port, 
    SOC_SAND_IN  uint32                prio_ndx,   
    SOC_SAND_IN  uint32                rate
    )
{
    uint32 base_port_tc, offset;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, tm_port, &base_port_tc));
    offset = base_port_tc + prio_ndx;
    SOCDNX_IF_ERR_EXIT(arad_sw_db_sch_priority_port_rate_set(unit, core, offset, rate, 1));
    
exit:
    SOCDNX_FUNC_RETURN
}

uint32
    arad_ofp_rates_sch_port_priority_max_burst_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core, 
    SOC_SAND_IN  uint32                tm_port, 
    SOC_SAND_IN  uint32                prio_ndx,   
    SOC_SAND_IN  uint32                max_burst
    )
{
    uint32 res = SOC_E_NONE;

    SOCDNX_INIT_FUNC_DEFS

    res = arad_ofp_rates_tcg_ptc_sch_generic_set_unsafe(unit, core,
                                                              tm_port, 
                                                              prio_ndx,   
                                                              max_burst,
                                                              ARAD_OFP_RATES_GENERIC_FUNC_STATE_PTC_SCH_BURST);
    SOCDNX_IF_ERR_EXIT(res);

exit:
    SOCDNX_FUNC_RETURN
}


uint32
    arad_ofp_rates_sch_tcg_rate_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core,     
    SOC_SAND_IN  uint32                tm_port, 
    SOC_SAND_IN  ARAD_TCG_NDX          tcg_ndx,   
    SOC_SAND_OUT uint32                *tcg_rate
    )
{
    uint32 res = SOC_E_NONE;

    SOCDNX_INIT_FUNC_DEFS

    SOCDNX_NULL_CHECK(tcg_rate);


    res = arad_ofp_rates_tcg_ptc_sch_generic_get_unsafe(unit,
                                                        core,
                                                        tm_port, 
                                                        tcg_ndx,   
                                                        tcg_rate,
                                                        ARAD_OFP_RATES_GENERIC_FUNC_STATE_TCG_SCH_RATE);
    SOCDNX_IF_ERR_EXIT(res);

exit:
    SOCDNX_FUNC_RETURN
}

uint32
    arad_ofp_rates_sch_tcg_max_burst_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core, 
    SOC_SAND_IN  uint32                tm_port, 
    SOC_SAND_IN  ARAD_TCG_NDX          tcg_ndx,   
    SOC_SAND_OUT uint32                *max_burst
    )
{
    uint32 res = SOC_E_NONE;

    SOCDNX_INIT_FUNC_DEFS

    SOCDNX_NULL_CHECK(max_burst);


    res = arad_ofp_rates_tcg_ptc_sch_generic_get_unsafe(unit,
                                                        core,
                                                        tm_port, 
                                                        tcg_ndx,   
                                                        max_burst,
                                                        ARAD_OFP_RATES_GENERIC_FUNC_STATE_TCG_SCH_BURST);
    SOCDNX_IF_ERR_EXIT(res);

exit:
    SOCDNX_FUNC_RETURN
}

uint32
    arad_ofp_rates_sch_port_priority_rate_get_unsafe(
    SOC_SAND_IN  int                   unit,  
    SOC_SAND_IN  int                   core,  
    SOC_SAND_IN  uint32                tm_port, 
    SOC_SAND_IN  uint32                prio_ndx,   
    SOC_SAND_OUT  uint32               *tcg_rate
    )
{
    uint32 res = SOC_E_NONE;

    SOCDNX_INIT_FUNC_DEFS

    SOCDNX_NULL_CHECK(tcg_rate);

    res = arad_ofp_rates_tcg_ptc_sch_generic_get_unsafe(unit,
                                                        core,
                                                        tm_port, 
                                                        prio_ndx,   
                                                        tcg_rate,
                                                        ARAD_OFP_RATES_GENERIC_FUNC_STATE_PTC_SCH_RATE);
    SOCDNX_IF_ERR_EXIT(res);

exit:
    SOCDNX_FUNC_RETURN
}

uint32
    arad_ofp_rates_sch_port_priority_max_burst_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core, 
    SOC_SAND_IN  ARAD_FAP_PORT_ID      tm_port, 
    SOC_SAND_IN  uint32                prio_ndx,   
    SOC_SAND_OUT  uint32               *max_burst
    )
{
    uint32 res = SOC_E_NONE;

    SOCDNX_INIT_FUNC_DEFS

    SOCDNX_NULL_CHECK(max_burst);

    res = arad_ofp_rates_tcg_ptc_sch_generic_get_unsafe(unit,
                                                        core,
                                                        tm_port, 
                                                        prio_ndx,   
                                                        max_burst,
                                                        ARAD_OFP_RATES_GENERIC_FUNC_STATE_PTC_SCH_BURST);
    SOCDNX_IF_ERR_EXIT(res);

exit:
    SOCDNX_FUNC_RETURN
}

int
    arad_ofp_rates_packet_mode_packet_size_get (
        SOC_SAND_IN   int                   unit, 
        SOC_SAND_IN   int                   core,  
        SOC_SAND_OUT  uint32                *num_of_bytes
        )
{
    uint32 data;

    SOCDNX_INIT_FUNC_DEFS

    SOCDNX_SAND_IF_ERR_EXIT(READ_EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr(unit, core, &data));

    
    *num_of_bytes = soc_reg_field_get(unit, EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr, data, SHAPER_PACKET_RATE_CONSTf);

exit:
    SOCDNX_FUNC_RETURN
}

int
arad_ofp_rates_egress_shaper_cal_write (
    SOC_SAND_IN  int                                      unit,
    SOC_SAND_IN  int                                      core,
    SOC_SAND_IN  SOC_TMC_OFP_RATES_CAL_INFO              *cal_info,
    SOC_SAND_IN  SOC_TMC_OFP_RATES_CAL_SET                cal2set,    
    SOC_SAND_IN  SOC_TMC_OFP_RATES_EGQ_CHAN_ARB_FIELD_TYPE field_type,
    SOC_SAND_IN  uint32                                   data
    )
{
    soc_field_t field_name;
    soc_reg_t register_name;

    SOCDNX_INIT_FUNC_DEFS

    
    SOCDNX_IF_ERR_EXIT(arad_ofp_rates_retrieve_egress_shaper_reg_field_names(
                                                                unit,
                                                                cal_info,
                                                                cal2set,    
                                                                field_type,
                                                                &register_name,
                                                                &field_name));

    
    SOCDNX_IF_ERR_EXIT(arad_ofp_rates_egress_shaper_reg_field_write (
                                                              unit,
                                                              core,
                                                              cal_info,   
                                                              register_name,
                                                              field_name,
                                                              data));
exit:
    SOCDNX_FUNC_RETURN
}

int
arad_ofp_rates_egress_shaper_cal_read (
    SOC_SAND_IN  int                                      unit,
    SOC_SAND_IN  int                                      core,
    SOC_SAND_IN  SOC_TMC_OFP_RATES_CAL_INFO              *cal_info,
    SOC_SAND_IN  SOC_TMC_OFP_RATES_CAL_SET                cal2set,    
    SOC_SAND_IN  SOC_TMC_OFP_RATES_EGQ_CHAN_ARB_FIELD_TYPE field_type,
    SOC_SAND_OUT uint32                                   *data
    )
{
    soc_field_t field_name;
    soc_reg_t register_name;

    SOCDNX_INIT_FUNC_DEFS

    
    SOCDNX_IF_ERR_EXIT(arad_ofp_rates_retrieve_egress_shaper_reg_field_names(
                                                                unit,
                                                                cal_info,
                                                                cal2set,    
                                                                field_type,
                                                                &register_name,
                                                                &field_name));

    
    SOCDNX_IF_ERR_EXIT(arad_ofp_rates_egress_shaper_reg_field_read (
                                                              unit,
                                                              core,
                                                              cal_info,   
                                                              register_name,
                                                              field_name,
                                                              data));
exit:
    SOCDNX_FUNC_RETURN
}



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif 

