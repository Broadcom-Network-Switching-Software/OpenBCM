/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: jer_egr_queuing.c
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_COSQ

#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/port_sw_db.h>
#include <soc/dpp/mbcm.h>
#include <soc/dpp/SAND/Utils/sand_occupation_bitmap.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>

#include <soc/dpp/ARAD/arad_sw_db.h>
#include <soc/dpp/ARAD/arad_scheduler_device.h>
#include <soc/dpp/ARAD/arad_scheduler_end2end.h>
#include <soc/dpp/ARAD/arad_scheduler_flow_converts.h>
#include <soc/dpp/ARAD/arad_scheduler_ports.h>
#include <soc/dpp/ARAD/arad_egr_queuing.h>

#include <soc/dpp/JER/jer_sch.h>
#include <soc/dpp/JER/jer_tbls.h>

#include <soc/dpp/QAX/qax_link_bonding.h>

#define CAL_0_1_SIZE  1024
#define CAL_2_15_SIZE 256
#define JER_SCH_HR_PRIORITY_LOW_VAL  (0)
#define JER_SCH_HR_PRIORITY_HIGH_VAL (15)


#define JER_SCH_CREDIT_WORTH_DIV_MIN (1)  
#define JER_SCH_CREDIT_WORTH_DIV_MAX (0x1fff) 

#define JER_SCH_SHAPERS_QUANTA_TO_ADD_MAX (0x7ff)

#define JER_SCH_MAX_BUCKET_TABLE_NOF_INDEXES (7)

typedef struct slow_rate_max_bucket_s {
    int    rate;
    uint32 max_bucket;
} slow_rate_max_bucket_t;

STATIC slow_rate_max_bucket_t slow_rate_max_bucket[JER_SCH_MAX_BUCKET_TABLE_NOF_INDEXES] = {
    
        {0,         1},
        {9781,      2},
        {18386,     3},
        {33271,     4},
        {57806,     5},
        {95507,     6},
        {146926,    7}
};


uint32
_jer_sch_rates_switch_calendars(int unit, int core, int new_calendar_table_num, int new_divider);
uint32
_jer_sch_rates_calculate_new_calendars(int unit, int core, int max_rate_increased, int *new_rate_configured);
uint32
_jer_sch_rates_calculate_new_divider(int unit, int core, int hr_calendar_table_num, int max_rate_increased, int old_divider ,uint32 *new_divider);
STATIC uint32
_jer_sch_slow_rate_max_bucket_get(int unit, int slow_rate, uint32 *max_bucket);




int
  soc_jer_sch_init(
    SOC_SAND_IN   int                    unit
    )
{
    soc_pbmp_t pbmp;
    soc_port_t port_i;
    uint32 reg_val, flags, rate_kbps, slow_level, slow_type, mem_val32;
    int core;
    uint32 slow_rate_max_level;
    char* slow_rate_max_level_str;
    int default_slow_rates[8] = {3, 6, 12, 24, 36, 48, 64, 96};
    uint32 is_skip_for_lb = 0;
#ifdef BCM_LB_SUPPORT
    uint32 egress_offset = 0;
    uint32 tm_port = 0;
    uint32 is_alloced_e2e_if_for_lb = 0;
#endif

    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_valid_ports_get(unit, 0, &pbmp));
    SOC_PBMP_ITER(pbmp, port_i) {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port_i, &flags));
#ifdef BCM_LB_SUPPORT
         
        if (SOC_IS_QAX(unit) && SOC_DPP_CONFIG(unit)->qax->lbi_en) {
            is_skip_for_lb = 0;
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port_i, &tm_port, &core));
            SOCDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port2egress_offset, (unit, core, tm_port, &egress_offset)));

            if (egress_offset == SOC_QAX_EGR_IF_LBG_RESERVE) {
                if (is_alloced_e2e_if_for_lb == 0) {
                    is_alloced_e2e_if_for_lb = 1;
                }
                else {
                    is_skip_for_lb = 1;
                }
            }
        }
#endif
        if (!(SOC_PORT_IS_NOT_VALID_FOR_EGRESS_TM(flags) || is_skip_for_lb)) {
            SOCDNX_IF_ERR_EXIT(soc_jer_sch_e2e_interface_allocate(unit, port_i));
        }
    }

    
    SOCDNX_SAND_IF_ERR_EXIT(arad_scheduler_end2end_regs_init(unit));


    
    SOC_DPP_CORES_ITER(SOC_CORE_ALL, core) 
    {
        
        SOCDNX_IF_ERR_EXIT(READ_SCH_HR_PRIORITY_MASKr(unit, core, 0, &reg_val));
        soc_reg_field_set(unit, SCH_HR_PRIORITY_MASKr, &reg_val, HR_PRIORITY_MASK_Nf, JER_SCH_HR_PRIORITY_LOW_VAL);
        SOCDNX_IF_ERR_EXIT(WRITE_SCH_HR_PRIORITY_MASKr(unit, core, 0, reg_val));
        
        SOCDNX_IF_ERR_EXIT(READ_SCH_HR_PRIORITY_MASKr(unit, core, 1, &reg_val));
        soc_reg_field_set(unit, SCH_HR_PRIORITY_MASKr, &reg_val, HR_PRIORITY_MASK_Nf, JER_SCH_HR_PRIORITY_HIGH_VAL);
        SOCDNX_IF_ERR_EXIT(WRITE_SCH_HR_PRIORITY_MASKr(unit, core, 1, reg_val)); 

    }

    
    SOC_DPP_CORES_ITER(SOC_CORE_ALL, core)
    {
        SOCDNX_IF_ERR_EXIT(READ_SCH_SCHEDULER_CONFIGURATION_REGISTERr(unit, core, &reg_val));
        soc_reg_field_set(unit, SCH_SCHEDULER_CONFIGURATION_REGISTERr, &reg_val, SLOW_FACTOR_ENABLEf, 0x1);
        
        if (SOC_IS_QAX(unit)) {
            soc_reg_field_set(unit, SCH_SCHEDULER_CONFIGURATION_REGISTERr, &reg_val, ADD_OFFSET_TO_FLOWf, 0x1);
        }
        
        if (SOC_IS_QUX(unit)) {
            soc_reg_field_set(unit, SCH_SCHEDULER_CONFIGURATION_REGISTERr, &reg_val, REMOVE_OFFSET_FROM_FLOWf, 0x1);
        }
        SOCDNX_IF_ERR_EXIT(WRITE_SCH_SCHEDULER_CONFIGURATION_REGISTERr(unit, core, reg_val));
        if (SOC_IS_QAX(unit) && !SOC_IS_QUX(unit))
        {
            
            SOCDNX_IF_ERR_EXIT(READ_IPS_CR_FLW_ID_VERIFY_1r(unit, core, &reg_val));
            soc_reg_field_set(unit, IPS_CR_FLW_ID_VERIFY_1r, &reg_val,CR_FLW_ID_CHECK_MASKf, 0x10000); 
            SOCDNX_IF_ERR_EXIT(WRITE_IPS_CR_FLW_ID_VERIFY_1r(unit, core, reg_val));
        }
    }

    slow_rate_max_level_str = soc_property_get_str(unit, spn_SLOW_MAX_RATE_LEVEL);

    if ( (slow_rate_max_level_str == NULL) || (sal_strcmp(slow_rate_max_level_str, "HIGH")==0) )
    {
        slow_rate_max_level = 3; 
        SOC_DPP_CONFIG(unit)->arad->init.max_burst_default_value_bucket_width = (0x7f)*256;
    } 
    else if (sal_strcmp(slow_rate_max_level_str, "NORMAL") == 0)
    {
        slow_rate_max_level = 2; 
        SOC_DPP_CONFIG(unit)->arad->init.max_burst_default_value_bucket_width = (0xff)*256;
    } 
    else if (sal_strcmp(slow_rate_max_level_str, "LOW") == 0)
    {
        slow_rate_max_level = 1; 
        SOC_DPP_CONFIG(unit)->arad->init.max_burst_default_value_bucket_width = (0x1ff)*256;
    }
    else
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("Unsupported properties: slow_rate_max_level should be LOW/NORMAL/HIGH")));
    }

    
    SOCDNX_IF_ERR_EXIT(READ_SCH_SHAPER_CONFIGURATION_REGISTER_1r(unit, SOC_CORE_ALL, &reg_val));
    soc_reg_field_set(unit, SCH_SHAPER_CONFIGURATION_REGISTER_1r, &reg_val, SLOW_MAX_BUCKET_WIDTHf, slow_rate_max_level);
    soc_reg_field_set(unit, SCH_SHAPER_CONFIGURATION_REGISTER_1r, &reg_val, OVERRIDE_SLOW_RATEf, 1);
    SOCDNX_IF_ERR_EXIT(WRITE_SCH_SHAPER_CONFIGURATION_REGISTER_1r(unit, SOC_CORE_ALL, reg_val));
    
    for(slow_level=0 ; slow_level<8 ; slow_level++) {
        SOCDNX_IF_ERR_EXIT(READ_SCH_SLOW_SCALE_A_SSAm(unit, SCH_BLOCK(unit, SOC_CORE_ALL), slow_level, &mem_val32));
        soc_mem_field32_set(unit, SCH_SLOW_SCALE_A_SSAm, &mem_val32, MAX_BUCKETf, (1 << slow_rate_max_level) - 1 );
        SOCDNX_IF_ERR_EXIT(WRITE_SCH_SLOW_SCALE_A_SSAm(unit, SCH_BLOCK(unit, SOC_CORE_ALL), slow_level, &mem_val32));

        SOCDNX_IF_ERR_EXIT(READ_SCH_SLOW_SCALE_B_SSBm(unit, SCH_BLOCK(unit, SOC_CORE_ALL), slow_level, &mem_val32));
        soc_mem_field32_set(unit, SCH_SLOW_SCALE_B_SSBm, &mem_val32, MAX_BUCKETf, (1 << slow_rate_max_level) - 1 );
        SOCDNX_IF_ERR_EXIT(WRITE_SCH_SLOW_SCALE_B_SSBm(unit, SCH_BLOCK(unit, SOC_CORE_ALL), slow_level, &mem_val32));
    }
 
    for(slow_level=0 ; slow_level<8 ; slow_level++) {

        rate_kbps = default_slow_rates[slow_level]*1000000;

        for(slow_type=1 ; slow_type<=2 ; slow_type++) {
            SOCDNX_IF_ERR_EXIT(jer_sch_slow_max_rates_per_level_set(unit, SOC_CORE_ALL, slow_level, slow_type, rate_kbps));
        }
    }

    SOCDNX_IF_ERR_EXIT(READ_SCH_MEM_01F00000m(unit, SCH_BLOCK(unit, SOC_CORE_ALL), 0, &mem_val32));
    soc_mem_field32_set(unit, SCH_MEM_01F00000m, &mem_val32, ITEM_0_0f, 0x1);
    SOCDNX_IF_ERR_EXIT(WRITE_SCH_MEM_01F00000m(unit, SCH_BLOCK(unit, SOC_CORE_ALL), 0, &mem_val32));

    
    SOC_DPP_CORES_ITER(SOC_CORE_ALL, core){
        SOCDNX_IF_ERR_EXIT(READ_SCH_DELETE_MECHANISM_CONFIGURATION_REGISTERr(unit, core, &reg_val));
        soc_reg_field_set(unit, SCH_DELETE_MECHANISM_CONFIGURATION_REGISTERr, &reg_val, DLM_ENABLEf, 0x7);
        SOCDNX_IF_ERR_EXIT(WRITE_SCH_DELETE_MECHANISM_CONFIGURATION_REGISTERr(unit, core, reg_val));
    }

    if (SOC_IS_JERICHO_PLUS_ONLY(unit)) {
        SOCDNX_IF_ERR_EXIT(READ_CFC_RESERVED_SPARE_0r(unit, &reg_val));
        soc_reg_field_set(unit, CFC_RESERVED_SPARE_0r, &reg_val, SCH_OOB_ERR_VALID_CFGf, 0x1);
        SOCDNX_IF_ERR_EXIT(WRITE_CFC_RESERVED_SPARE_0r(unit, reg_val));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
  jer_sch_device_rate_entry_core_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32              rci_level_ndx,
    SOC_SAND_IN  uint32              nof_active_links_ndx,
    SOC_SAND_OUT uint32              *rate
  )
{
  uint32
    interval_in_clock_128_th,
    calc,
    offset,
  credit_worth,
    res;
  ARAD_SCH_DRM_TBL_DATA
    drm_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_DEVICE_RATE_ENTRY_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(rate);

  
  offset = (nof_active_links_ndx * SOC_DPP_DEFS_GET(unit, nof_rci_levels)) + rci_level_ndx;
  res = jer_sch_drm_tbl_get_unsafe(
          unit, core,
          offset,
          &drm_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 4, exit);

  interval_in_clock_128_th = drm_tbl_data.device_rate;
  
  if (0 == interval_in_clock_128_th)
  {
    *rate = 0;
  }
  else
  {
    
    res = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mgmt_credit_worth_get,(unit, &credit_worth))) ;
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);
    calc = (credit_worth * SOC_SAND_NOF_BITS_IN_CHAR) *
           (arad_chip_mega_ticks_per_sec_get(unit) * ARAD_SCH_DEVICE_RATE_INTERVAL_RESOLUTION);
    calc = SOC_SAND_DIV_ROUND(calc, interval_in_clock_128_th);
    *rate = calc;
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in jer_sch_device_rate_entry_core_get_unsafe()",0,0);
}

uint32
  jer_sch_device_rate_entry_core_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32              rci_level_ndx,
    SOC_SAND_IN  uint32              nof_active_links_ndx,
    SOC_SAND_IN  uint32              rate
  )
{
  uint32
    interval_in_clock_128_th,
    calc,
    offset,
    credit_worth,
    res;
  ARAD_SCH_DRM_TBL_DATA
    drm_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_DEVICE_RATE_ENTRY_SET_UNSAFE);
  
  res = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mgmt_credit_worth_get,(unit, &credit_worth))) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 4, exit);
  
  if (0 == rate)
  {
    interval_in_clock_128_th = 0;
  }
  else
  {
    calc = (credit_worth * SOC_SAND_NOF_BITS_IN_CHAR) *
           (arad_chip_mega_ticks_per_sec_get(unit) * ARAD_SCH_DEVICE_RATE_INTERVAL_RESOLUTION);
    calc = SOC_SAND_DIV_ROUND(calc, rate);
    interval_in_clock_128_th = calc;
  }
  SOC_SAND_LIMIT_FROM_ABOVE(interval_in_clock_128_th, ARAD_SCH_DEVICE_RATE_INTERVAL_MAX);
  if (interval_in_clock_128_th != 0)
  {
    SOC_SAND_LIMIT_FROM_BELOW(interval_in_clock_128_th, ARAD_SCH_DEVICE_RATE_INTERVAL_MIN);
  }
  offset = (nof_active_links_ndx * SOC_DPP_DEFS_GET(unit, nof_rci_levels)) + rci_level_ndx;
  drm_tbl_data.device_rate = interval_in_clock_128_th;
  
  res = 0 ;
  res = jer_sch_drm_tbl_set_unsafe(
          unit, core,
          offset, &drm_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in jer_sch_device_rate_entry_core_set_unsafe()",0,0);
}


int 
soc_jer_sch_e2e_interface_allocate(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  soc_port_t      port
    )
{
    uint8           found = FALSE;
    uint32          num_of_big_calendars, num_of_channelized_interfaces, num_of_interfaces, e2e_if, tm_port, data_32, egress_offset, base_q_pair,
                    non_channelized_port_offset, is_master;
    int             is_channelized, core, start, end;
    soc_port_if_t   interface_type;
    soc_reg_above_64_val_t tbl_data;
    SOC_SAND_OCC_BM_PTR ifs_occ;
    ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE ilkn_tdm_dedicated_queuing;
    soc_error_t rv;

    SOCDNX_INIT_FUNC_DEFS

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_master_get(unit, port, &is_master));

    if (is_master) {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port, &tm_port, &core));
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_channelized_port_get(unit, port, &is_channelized)); 
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface_type));
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, tm_port, &base_q_pair));
        SOCDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port2egress_offset, (unit, core, tm_port, &egress_offset)));

        num_of_channelized_interfaces = SOC_DPP_IMP_DEFS_GET(unit, nof_channelized_interfaces);
        num_of_big_calendars = SOC_DPP_DEFS_GET(unit, nof_big_channelized_calendars);
        num_of_interfaces = SOC_DPP_IMP_DEFS_GET(unit, nof_core_interfaces);

        ilkn_tdm_dedicated_queuing = SOC_DPP_CONFIG(unit)->arad->init.ilkn_tdm_dedicated_queuing;
        end = 1;
        
        if (ilkn_tdm_dedicated_queuing == ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE_ON && interface_type == SOC_PORT_IF_ILKN ) 
        {
            SOC_SAND_SET_BIT(egress_offset, 0, 0); 
            end++;
	    }

        for (start = 0; start < end; start++) {
            egress_offset+= start; 
            
            
            rv = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.e2e_interfaces_occ.get(unit, core, &ifs_occ);
            SOCDNX_IF_ERR_EXIT(rv);
            if (is_channelized) {
                
                if (interface_type == SOC_PORT_IF_ILKN) {
                    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_occ_bm_get_next_in_range(unit, ifs_occ, 0, (num_of_big_calendars-1), TRUE, &e2e_if, &found));
                }

                if (!found) {
                    
                    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_occ_bm_get_next_in_range(unit, ifs_occ, num_of_big_calendars, (num_of_channelized_interfaces-1), TRUE, &e2e_if, &found));
                }

                if (!found && interface_type != SOC_PORT_IF_ILKN) {
                    
                    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_occ_bm_get_next_in_range(unit, ifs_occ, 0, (num_of_channelized_interfaces-1), TRUE, &e2e_if, &found));
                }
            } else { 

                
                SOCDNX_SAND_IF_ERR_EXIT(soc_sand_occ_bm_get_next_in_range(unit, ifs_occ, num_of_channelized_interfaces, (num_of_interfaces-1), TRUE, &e2e_if, &found));

                if (!found) {
                    
                    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_occ_bm_get_next_in_range(unit, ifs_occ, num_of_big_calendars, (num_of_interfaces-1), TRUE, &e2e_if, &found));
                }

                if (!found) {
                    
                    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_occ_bm_get_next_in_range(unit, ifs_occ, 0, (num_of_interfaces-1), TRUE, &e2e_if, &found));
                }

            }

            if (!found) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_RESOURCE, (_BSL_SOCDNX_MSG("No e2e interfaces are left for port %d \n"), port));
            } else {
                
                SOCDNX_SAND_IF_ERR_EXIT(soc_sand_occ_bm_occup_status_set(unit, ifs_occ, e2e_if, TRUE));

                
                data_32 = 0;
                SOC_REG_ABOVE_64_CLEAR(tbl_data);

                SOCDNX_IF_ERR_EXIT(READ_SCH_FC_MAP_FCMm(unit, SCH_BLOCK(unit, core), egress_offset, &data_32));
                soc_SCH_FC_MAP_FCMm_field32_set(unit, &data_32 ,FC_MAP_FCMf, e2e_if);
                SOCDNX_IF_ERR_EXIT(WRITE_SCH_FC_MAP_FCMm(unit,SCH_BLOCK(unit, core), egress_offset, &data_32));

                if (!ARAD_SCH_IS_CHNIF_ID(unit, e2e_if)) {
                    
                    non_channelized_port_offset = ARAD_SCH_OFFSET_TO_NON_CHANNELIZED_OFFSET(unit, e2e_if);
                    SOCDNX_IF_ERR_EXIT(READ_SCH_ONE_PORT_NIF_CONFIGURATION_OPNCm(unit,SCH_BLOCK(unit, core),non_channelized_port_offset,tbl_data));
                    soc_SCH_ONE_PORT_NIF_CONFIGURATION_OPNCm_field32_set(unit,tbl_data,PORT_IDf,base_q_pair);
                    SOCDNX_IF_ERR_EXIT(WRITE_SCH_ONE_PORT_NIF_CONFIGURATION_OPNCm(unit,SCH_BLOCK(unit, core),non_channelized_port_offset,tbl_data));    
                }
            }

            {
                uint32 array_index, bit_index, sizeof_element ;

                
                sizeof_element = 32 ;
                array_index = e2e_if / sizeof_element ;
                bit_index = e2e_if % sizeof_element ;
                SOCDNX_IF_ERR_EXIT(READ_SCH_DVS_NIF_CONFIGr(unit, core, array_index, &data_32)) ;
                data_32 &= SOC_SAND_RBIT(bit_index) ;
                WRITE_SCH_DVS_NIF_CONFIGr(unit, core, array_index, data_32) ;
            }
	    }
    }

exit:
    SOCDNX_FUNC_RETURN
}


int 
soc_jer_sch_e2e_interface_deallocate(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  soc_port_t      port
    )
{
    uint32          e2e_if, tm_port, data_32, egress_offset, non_channelized_port_offset, is_master;
    int             is_channelized, core, start, end;
    soc_port_if_t   interface_type;
    soc_reg_above_64_val_t tbl_data;
    SOC_SAND_OCC_BM_PTR ifs_occ;
    ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE ilkn_tdm_dedicated_queuing;
    soc_error_t rv;

    SOCDNX_INIT_FUNC_DEFS

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_master_get(unit, port, &is_master));

    if (is_master) {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port, &tm_port, &core));
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_channelized_port_get(unit, port, &is_channelized)); 
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface_type));
        SOCDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port2egress_offset, (unit, core, tm_port, &egress_offset)));

        ilkn_tdm_dedicated_queuing = SOC_DPP_CONFIG(unit)->arad->init.ilkn_tdm_dedicated_queuing;
        end = 1;
        
        if (ilkn_tdm_dedicated_queuing == ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE_ON && interface_type == SOC_PORT_IF_ILKN ) 
        {
            SOC_SAND_SET_BIT(egress_offset, 0, 0); 
            end++;
	    }

        for (start = 0; start < end; start++) {
            egress_offset+=start; 
            
            
            rv = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.e2e_interfaces_occ.get(unit, core, &ifs_occ);
            SOCDNX_IF_ERR_EXIT(rv);

            data_32 = 0;
            SOC_REG_ABOVE_64_CLEAR(tbl_data);
            SOCDNX_IF_ERR_EXIT(READ_SCH_FC_MAP_FCMm(unit, SCH_BLOCK(unit, core), egress_offset, &data_32));
            
            e2e_if = soc_SCH_FC_MAP_FCMm_field32_get(unit, &data_32, FC_MAP_FCMf);

            
            soc_SCH_FC_MAP_FCMm_field32_set(unit, &data_32 ,FC_MAP_FCMf, 0);
            SOCDNX_IF_ERR_EXIT(WRITE_SCH_FC_MAP_FCMm(unit,SCH_BLOCK(unit, core), egress_offset, &data_32));

            
            SOCDNX_SAND_IF_ERR_EXIT(soc_sand_occ_bm_occup_status_set(unit, ifs_occ, e2e_if, FALSE));

            if (!ARAD_SCH_IS_CHNIF_ID(unit, e2e_if)) {
                
                non_channelized_port_offset = ARAD_SCH_OFFSET_TO_NON_CHANNELIZED_OFFSET(unit, e2e_if);
                SOCDNX_IF_ERR_EXIT(READ_SCH_ONE_PORT_NIF_CONFIGURATION_OPNCm(unit,SCH_BLOCK(unit, core),non_channelized_port_offset,tbl_data));
                soc_SCH_ONE_PORT_NIF_CONFIGURATION_OPNCm_field32_set(unit,tbl_data,PORT_IDf,0);
                soc_SCH_ONE_PORT_NIF_CONFIGURATION_OPNCm_field32_set(unit,tbl_data,PORT_NIF_MAX_CR_RATEf,0);
                SOCDNX_IF_ERR_EXIT(WRITE_SCH_ONE_PORT_NIF_CONFIGURATION_OPNCm(unit,SCH_BLOCK(unit, core),non_channelized_port_offset,tbl_data));    
            }

            {
                uint32 array_index, bit_index, sizeof_element ;

                
                sizeof_element = 32 ;
                array_index = e2e_if / sizeof_element ;
                bit_index = e2e_if % sizeof_element ;
                SOCDNX_IF_ERR_EXIT(READ_SCH_DVS_NIF_CONFIGr(unit, core, array_index, &data_32)) ;
                data_32 |= SOC_SAND_BIT(bit_index) ;
                WRITE_SCH_DVS_NIF_CONFIGr(unit, core, array_index, data_32) ;
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN
}

int
  soc_jer_sch_cal_max_size_get(
    SOC_SAND_IN   int                    unit,
    SOC_SAND_IN   uint32                 sch_offset,
    SOC_SAND_OUT  uint32*                max_cal_size
   )
{
    uint32 cal_select;
    SOCDNX_INIT_FUNC_DEFS;

    cal_select = sch_offset / 2;

    if(cal_select == 0 || cal_select == 1) {
        *max_cal_size = CAL_0_1_SIZE;
    } else {
        *max_cal_size = CAL_2_15_SIZE;
    }

    SOCDNX_FUNC_RETURN;
}

static soc_mem_t cal_memories[] = {
    SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_0m,
    SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_1m,
    SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_2m,
    SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_3m,
    SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_4m,
    SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_5m,
    SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_6m,
    SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_7m,
    SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_8m,
    SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_9m,
    SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_10m,
    SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_11m,
    SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_12m,
    SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_13m,
    SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_14m,
    SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_15m
};

int
  soc_jer_sch_cal_tbl_set(
    SOC_SAND_IN   int                    unit,
    SOC_SAND_IN   int                    core_id,
    SOC_SAND_IN   uint32                 sch_offset,
    SOC_SAND_IN   uint32                 sch_to_set ,
    SOC_SAND_IN   uint32                 slots_count,
                  uint32*                slots
  )
{
    uint32
        cal_offset,
        cal_select,
        cal_internal_select,
        cal_size,
        slot,
        entry;
    soc_mem_t
        memory;

    SOCDNX_INIT_FUNC_DEFS;

    cal_select = sch_offset / 2; 
    cal_internal_select = sch_offset % 2; 

    if(cal_select == 0 || cal_select == 1) {
        cal_size = CAL_0_1_SIZE;
    } else {
        cal_size = CAL_2_15_SIZE;
    }

    if(slots_count > cal_size) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL,(_BSL_SOCDNX_MSG("Slots count is out of range")));
    }

    
    cal_offset = cal_internal_select * (2 * cal_size) + sch_to_set * cal_size;

    
    memory = cal_memories[cal_select];
    for (slot=0; slot < slots_count; slot++) {
        entry = 0;
        soc_mem_field_set(unit, memory, &entry, PORT_SELf, &(slots[slot]));
        SOCDNX_IF_ERR_EXIT(soc_mem_write(unit, memory, SCH_BLOCK(unit, core_id), cal_offset+slot, &entry));
    }

exit:
    SOCDNX_FUNC_RETURN;
}


int
  soc_jer_sch_cal_tbl_get(
    SOC_SAND_IN   int                    unit,
    SOC_SAND_IN   int                    core_id,
    SOC_SAND_IN   uint32                 sch_offset,
    SOC_SAND_IN   uint32                 sch_to_set ,
    SOC_SAND_IN   uint32                 slots_count,
    SOC_SAND_OUT  uint32*                slots
  )
{
    uint32
        cal_offset,
        cal_select,
        cal_internal_select,
        cal_size,
        slot,
        entry;
    soc_mem_t
        memory;

    SOCDNX_INIT_FUNC_DEFS;

    cal_select = sch_offset / 2; 
    cal_internal_select = sch_offset % 2; 

    if(cal_select == 0 || cal_select == 1) {
        cal_size = CAL_0_1_SIZE;
    } else {
        cal_size = CAL_2_15_SIZE;
    }

    if(slots_count > cal_size) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL,(_BSL_SOCDNX_MSG("Slots count is out of range")));
    }

    
    cal_offset = cal_internal_select * (2 * cal_size) + sch_to_set * cal_size;

    
    memory = cal_memories[cal_select];
    for (slot=0; slot < slots_count; slot++) {
        entry = 0;
        SOCDNX_IF_ERR_EXIT(soc_mem_read(unit, memory, SCH_BLOCK(unit, core_id), cal_offset+slot, &entry));
        soc_mem_field_get(unit, memory, &entry, PORT_SELf, &(slots[slot]));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_jer_sch_prio_propagation_enable_set(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  int enable
    )
{
    uint32 reg_val;
    int core;
    SOCDNX_INIT_FUNC_DEFS;

    
    SOC_DPP_CORES_ITER(SOC_CORE_ALL, core) 
    {
        SOCDNX_IF_ERR_EXIT(READ_SCH_SCHEDULER_CONFIGURATION_REGISTERr(unit, core, &reg_val));
        soc_reg_field_set(unit, SCH_SCHEDULER_CONFIGURATION_REGISTERr, &reg_val, INTERFACE_PRIORITY_PROPAGATION_ENABLEf, enable? 1:0);
        SOCDNX_IF_ERR_EXIT(WRITE_SCH_SCHEDULER_CONFIGURATION_REGISTERr(unit, core, reg_val));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_jer_sch_prio_propagation_enable_get(
    SOC_SAND_IN  int unit,
    SOC_SAND_OUT  int* enable
    )
{
    uint32 reg_val;
    SOCDNX_INIT_FUNC_DEFS;

    

    SOCDNX_IF_ERR_EXIT(READ_SCH_SCHEDULER_CONFIGURATION_REGISTERr(unit, 0, &reg_val));
    *enable = soc_reg_field_get(unit, SCH_SCHEDULER_CONFIGURATION_REGISTERr, reg_val, INTERFACE_PRIORITY_PROPAGATION_ENABLEf);
    
exit:
    SOCDNX_FUNC_RETURN;
}

int soc_jer_sch_prio_propagation_port_set(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  soc_port_t port,
    SOC_SAND_IN  int cosq,
    SOC_SAND_IN  int is_high_prio
   )
{
    uint32 mem_val, base_q_pair, tm_port;
    int core, offset;
    soc_error_t rv;
    SOCDNX_INIT_FUNC_DEFS;

    rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.base_q_pair.get(unit, port, &base_q_pair);
    SOCDNX_IF_ERR_EXIT(rv);
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port, &tm_port, &core));

    offset = base_q_pair + cosq;
    if (offset < 0 || offset > 255)
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid offset for port hr %d \n"), offset));
    }

    SOCDNX_IF_ERR_EXIT(READ_SCH_HR_SCHEDULER_CONFIGURATION_SHCm(unit, SCH_BLOCK(unit, core), offset, &mem_val));
    soc_mem_field32_set(unit, SCH_HR_SCHEDULER_CONFIGURATION_SHCm, &mem_val,HR_PRIORITY_MASK_SELECTf, is_high_prio);
    SOCDNX_IF_ERR_EXIT(WRITE_SCH_HR_SCHEDULER_CONFIGURATION_SHCm(unit, SCH_BLOCK(unit, core), offset, &mem_val)); 


exit:
    SOCDNX_FUNC_RETURN;
}

int soc_jer_sch_prio_propagation_port_get(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  soc_port_t port,
    SOC_SAND_IN  int cosq,
    SOC_SAND_OUT  int *is_high_prio
   )
{
    uint32 mem_val, base_q_pair, field_val, tm_port;
    int offset, core;
    soc_error_t rv;
    SOCDNX_INIT_FUNC_DEFS;

    rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.base_q_pair.get(unit, port, &base_q_pair);
    SOCDNX_IF_ERR_EXIT(rv);
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port, &tm_port, &core));

    offset = base_q_pair + cosq;
    if (offset < 0 || offset > 255)
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid offset for port hr %d \n"), offset));
    }
       
    SOCDNX_IF_ERR_EXIT(READ_SCH_HR_SCHEDULER_CONFIGURATION_SHCm(unit, SCH_BLOCK(unit, core), offset, &mem_val));
    soc_mem_field_get(unit, SCH_HR_SCHEDULER_CONFIGURATION_SHCm, &mem_val, HR_PRIORITY_MASK_SELECTf, &field_val);

    *is_high_prio = field_val;
    
exit:
    SOCDNX_FUNC_RETURN;
}

uint32
  jer_sch_slow_max_rates_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 slow_rate_type,
    SOC_SAND_IN  int                 slow_rate_val
  )
{
    int core;
    
    SOCDNX_INIT_FUNC_DEFS;

    
    
    SOC_DPP_CORES_ITER(SOC_CORE_ALL, core)
    {
        SOCDNX_IF_ERR_EXIT(jer_sch_slow_max_rates_per_level_set(unit, core, 0, slow_rate_type, slow_rate_val));
    }

exit:
    SOCDNX_FUNC_RETURN; 
   
}

uint32
jer_sch_slow_max_rates_per_level_set(
   SOC_SAND_IN int unit, 
   SOC_SAND_IN int core, 
   SOC_SAND_IN int level , 
   SOC_SAND_IN int slow_rate_type, 
   SOC_SAND_IN int slow_rate_val)
{
    soc_field_info_t peak_rate_man_fld, peak_rate_exp_fld;
    ARAD_SCH_SUBFLOW sub_flow;
    ARAD_SCH_INTERNAL_SUB_FLOW_DESC internal_sub_flow;
    uint32 mem_val32, slow_fld_val, max_bucket;

    SOCDNX_INIT_FUNC_DEFS;

    arad_ARAD_SCH_SUBFLOW_clear(unit, &sub_flow);

    JER_TBL_SOC_REF(unit, SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDSm, PEAK_RATE_MAN_EVENf, &peak_rate_man_fld);
    JER_TBL_SOC_REF(unit, SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDSm, PEAK_RATE_EXP_EVENf, &peak_rate_exp_fld);

    sub_flow.shaper.max_rate = slow_rate_val;

    SOCDNX_SAND_IF_ERR_EXIT(arad_sch_to_internal_subflow_shaper_convert(unit, &sub_flow, &internal_sub_flow, TRUE));

    slow_fld_val = 0;
    slow_fld_val |= ARAD_FLD_IN_PLACE(internal_sub_flow.peak_rate_exp, peak_rate_exp_fld);
    slow_fld_val |= ARAD_FLD_IN_PLACE(internal_sub_flow.peak_rate_man, peak_rate_man_fld);

    SOCDNX_SAND_IF_ERR_EXIT(_jer_sch_slow_rate_max_bucket_get(unit, slow_rate_val, &max_bucket));

    if (slow_rate_type == 1)
    {
        SOCDNX_IF_ERR_EXIT(READ_SCH_SLOW_SCALE_A_SSAm(unit, SCH_BLOCK(unit, core), level, &mem_val32));
        soc_mem_field_set(unit, SCH_SLOW_SCALE_A_SSAm, &mem_val32, SLOW_RATEf,  &slow_fld_val);
        soc_mem_field_set(unit, SCH_SLOW_SCALE_A_SSAm, &mem_val32, MAX_BUCKETf, &max_bucket);
        SOCDNX_IF_ERR_EXIT(WRITE_SCH_SLOW_SCALE_A_SSAm(unit, SCH_BLOCK(unit, core), level, &mem_val32));
    }
    else
    {
        SOCDNX_IF_ERR_EXIT(READ_SCH_SLOW_SCALE_B_SSBm(unit, SCH_BLOCK(unit, core), level, &mem_val32));
        soc_mem_field_set(unit, SCH_SLOW_SCALE_B_SSBm, &mem_val32, SLOW_RATEf,  &slow_fld_val);
        soc_mem_field_set(unit, SCH_SLOW_SCALE_B_SSBm, &mem_val32, MAX_BUCKETf, &max_bucket);
        SOCDNX_IF_ERR_EXIT(WRITE_SCH_SLOW_SCALE_B_SSBm(unit, SCH_BLOCK(unit, core), level, &mem_val32));
    }
    
exit:
    SOCDNX_FUNC_RETURN;

}

STATIC uint32
_jer_sch_slow_rate_max_bucket_get(
        int unit,
        int slow_rate,
        uint32 *max_bucket)
{
    int index, bucket_width;
    uint32 reg_val;
    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_IF_ERR_EXIT(READ_SCH_SHAPER_CONFIGURATION_REGISTER_1r(unit, SOC_CORE_ALL, &reg_val));
    bucket_width = soc_reg_field_get(unit, SCH_SHAPER_CONFIGURATION_REGISTER_1r, reg_val, SLOW_MAX_BUCKET_WIDTHf);

    
    for (index = 0; index < JER_SCH_MAX_BUCKET_TABLE_NOF_INDEXES; index++)
    {
        if (index == (JER_SCH_MAX_BUCKET_TABLE_NOF_INDEXES - 1))
        {
            
            break;
        }
        else if (slow_rate < (slow_rate_max_bucket[index+1].rate * 1000  ))
        {
            
            break;
        }
    }

    
    if (index == JER_SCH_MAX_BUCKET_TABLE_NOF_INDEXES)
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Max bucket value was not found for slow rate %d \n"), slow_rate));
    }

    *max_bucket = slow_rate_max_bucket[index].max_bucket;

    *max_bucket = SOC_SAND_MIN(*max_bucket, (1 << bucket_width) - 1);

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
  jer_sch_slow_max_rates_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 slow_rate_type,
    SOC_SAND_OUT int      *slow_rate_val
  )
{
   
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(jer_sch_slow_max_rates_per_level_get(unit, 0, 0, slow_rate_type, slow_rate_val));

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
jer_sch_slow_max_rates_per_level_get(
   SOC_SAND_IN  int   unit,
   SOC_SAND_IN  int   core,
   SOC_SAND_IN  int   level,
   SOC_SAND_IN  int   slow_rate_type,
   SOC_SAND_OUT int*  slow_rate_val)
{
    uint32 slow_fld_val, mem_val32;
    ARAD_SCH_SUBFLOW sub_flow;
    ARAD_SCH_INTERNAL_SUB_FLOW_DESC internal_sub_flow;
    soc_field_info_t peak_rate_man_fld, peak_rate_exp_fld;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(slow_rate_val);

    
    internal_sub_flow.max_burst = 0;
    internal_sub_flow.slow_rate_index = 0;

    
    JER_TBL_SOC_REF(unit, SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDSm, PEAK_RATE_MAN_EVENf, &peak_rate_man_fld);
    JER_TBL_SOC_REF(unit, SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDSm, PEAK_RATE_EXP_EVENf, &peak_rate_exp_fld);

    if (slow_rate_type == 1)
    {
        SOCDNX_IF_ERR_EXIT(READ_SCH_SLOW_SCALE_A_SSAm(unit, SCH_BLOCK(unit, core), level, &mem_val32));
        soc_mem_field_get(unit, SCH_SLOW_SCALE_A_SSAm, &mem_val32, SLOW_RATEf, &slow_fld_val);
    }
    else
    {
        SOCDNX_IF_ERR_EXIT(READ_SCH_SLOW_SCALE_B_SSBm(unit, SCH_BLOCK(unit, core), level, &mem_val32));
        soc_mem_field_get(unit, SCH_SLOW_SCALE_B_SSBm, &mem_val32, SLOW_RATEf, &slow_fld_val);
    }

    internal_sub_flow.peak_rate_exp = ARAD_FLD_FROM_PLACE(slow_fld_val, peak_rate_exp_fld);
    internal_sub_flow.peak_rate_man = ARAD_FLD_FROM_PLACE(slow_fld_val, peak_rate_man_fld);

    
    SOCDNX_SAND_IF_ERR_EXIT(arad_sch_from_internal_subflow_shaper_convert(unit, &internal_sub_flow, &sub_flow));

    *slow_rate_val = sub_flow.shaper.max_rate;

exit:
    SOCDNX_FUNC_RETURN;

}


int
_jer_sch_rates_calendar_index_get(int unit, int core, int offset, int is_hr_calendar ,uint32 *index)
{
    uint32 field_val ,mem_val, i;
    SOCDNX_INIT_FUNC_DEFS;

    
    if (SOC_DPP_CONFIG(unit)->arad->init.dynamic_port_enable)
    {
        *index = offset;
    }
    else 
    {
        int found = 0;
        
        for (i = 0 ; i < ARAD_EGR_NOF_Q_PAIRS ; i++)
        {
            if (is_hr_calendar)
            {
                SOCDNX_IF_ERR_EXIT(READ_SCH_PIR_SHAPER_CALENDAR_PSCm(unit, SCH_BLOCK(unit, core), i, &mem_val));
                soc_mem_field_get(unit, SCH_PIR_SHAPER_CALENDAR_PSCm, &mem_val, HR_NUMf, &field_val);
            }
            else
            {
                SOCDNX_IF_ERR_EXIT(READ_SCH_CIR_SHAPER_CALENDAR_CSCm(unit, SCH_BLOCK(unit, core), i, &mem_val));
                soc_mem_field_get(unit, SCH_CIR_SHAPER_CALENDAR_CSCm, &mem_val, PG_NUMf, &field_val);
            }
            if (field_val == offset)
            {
                found = 1;
                *index = i;
                break;
            }
        }
        if (!found)
        {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Unsupported hr\tcg number : %d"), offset));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC
uint32
_jer_sch_rates_port_get_max_rate(int unit, int core, int is_port_priority, uint32* maximum_rate, int* maximum_multiplicity)
{
    int maximum = 0, multiplicity = 0, i;
    int rate;
    uint8 valid;
    SOCDNX_INIT_FUNC_DEFS;

    for (i = 0; i < ARAD_EGR_NOF_Q_PAIRS ; i++)
    {
        if (is_port_priority)
        {
            SOCDNX_IF_ERR_EXIT(arad_sw_db_sch_priority_port_rate_get(unit, core, i, &rate, &valid));
        }
        else
        {
            SOCDNX_IF_ERR_EXIT(arad_sw_db_sch_port_tcg_rate_get(unit, core, i, &rate, &valid));
        }
        if (valid && (rate >= maximum))
        {
            if (rate > maximum)
            {
                multiplicity = 1;
                maximum = rate;
            } 
            else
            {
                multiplicity++;
            }
        }
    }
    *maximum_rate = maximum;
    *maximum_multiplicity = multiplicity;

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
jer_ofp_rates_sch_port_priority_hw_set(
   SOC_SAND_IN   int    unit,
   SOC_SAND_IN   int    core)
{

    uint32 mem_val, reg_val, reg_val2,
           access_period, cal_length;
    int    psst_table_location, is_new_rate_configured = 0;



    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(READ_SCH_PS_CALENDAR_SELECTr(unit, core, &reg_val));
    soc_reg_field_set(unit, SCH_PS_CALENDAR_SELECTr, &reg_val, PS_CALENDAR_SELECTf, 0); 
    SOCDNX_IF_ERR_EXIT(WRITE_SCH_PS_CALENDAR_SELECTr(unit, core, reg_val));
    
    SOCDNX_IF_ERR_EXIT(READ_SCH_PIR_SHAPERS_CONFIGURATIONr(unit, core, &reg_val));
    access_period = soc_reg_field_get(unit, SCH_PIR_SHAPERS_CONFIGURATIONr, reg_val, PIR_SHAPERS_CAL_ACCESS_PERIODf);
    cal_length = soc_reg_field_get(unit, SCH_PIR_SHAPERS_CONFIGURATIONr, reg_val, PIR_SHAPERS_CAL_LENGTHf);

    SOCDNX_IF_ERR_EXIT(READ_SCH_PIR_SHAPERS_CONFIGURATION_1r(unit, core, &reg_val2));
    soc_reg_field_set(unit, SCH_PIR_SHAPERS_CONFIGURATION_1r, &reg_val2, PIR_SHAPERS_CAL_ACCESS_PERIOD_1f, access_period);
    soc_reg_field_set(unit, SCH_PIR_SHAPERS_CONFIGURATION_1r, &reg_val2, PIR_SHAPERS_CAL_LENGTH_1f, cal_length);
    SOCDNX_IF_ERR_EXIT(WRITE_SCH_PIR_SHAPERS_CONFIGURATION_1r(unit, core, reg_val2));

    
    SOCDNX_IF_ERR_EXIT(READ_SCH_CIR_SHAPERS_CONFIGURATIONr(unit, core, &reg_val));
    access_period = soc_reg_field_get(unit, SCH_CIR_SHAPERS_CONFIGURATIONr, reg_val, CIR_SHAPERS_CAL_ACCESS_PERIODf);
    cal_length = soc_reg_field_get(unit, SCH_CIR_SHAPERS_CONFIGURATIONr, reg_val, CIR_SHAPERS_CAL_LENGTHf);

    SOCDNX_IF_ERR_EXIT(READ_SCH_CIR_SHAPERS_CONFIGURATION_1r(unit, core, &reg_val2));
    soc_reg_field_set(unit, SCH_CIR_SHAPERS_CONFIGURATION_1r, &reg_val2, CIR_SHAPERS_CAL_ACCESS_PERIOD_1f, access_period);
    soc_reg_field_set(unit, SCH_CIR_SHAPERS_CONFIGURATION_1r, &reg_val2, CIR_SHAPERS_CAL_LENGTH_1f, cal_length);
    SOCDNX_IF_ERR_EXIT(WRITE_SCH_CIR_SHAPERS_CONFIGURATION_1r(unit, core, reg_val2));

    SOCDNX_IF_ERR_EXIT(READ_SCH_REBOUNDED_CREDIT_CONFIGURATIONr(unit, core, &reg_val));
    soc_reg_field_set(unit, SCH_REBOUNDED_CREDIT_CONFIGURATIONr, &reg_val, REBOUNDED_CREDIT_WORTHf, JER_SCH_CREDIT_WORTH_DIV_MIN);
    SOCDNX_IF_ERR_EXIT(WRITE_SCH_REBOUNDED_CREDIT_CONFIGURATIONr(unit, core, reg_val));
    SOCDNX_IF_ERR_EXIT(READ_SCH_ASSIGNED_CREDIT_CONFIGURATIONr(unit, core, &reg_val));
    soc_reg_field_set(unit, SCH_ASSIGNED_CREDIT_CONFIGURATIONr, &reg_val, ASSIGNED_CREDIT_WORTHf, JER_SCH_CREDIT_WORTH_DIV_MIN);
    SOCDNX_IF_ERR_EXIT(WRITE_SCH_ASSIGNED_CREDIT_CONFIGURATIONr(unit, core, reg_val));
    SOCDNX_IF_ERR_EXIT(READ_SCH_REBOUNDED_CREDIT_CONFIGURATION_1r(unit, core, &reg_val));
    soc_reg_field_set(unit, SCH_REBOUNDED_CREDIT_CONFIGURATION_1r, &reg_val, REBOUNDED_CREDIT_WORTH_1f, JER_SCH_CREDIT_WORTH_DIV_MIN);
    SOCDNX_IF_ERR_EXIT(WRITE_SCH_REBOUNDED_CREDIT_CONFIGURATION_1r(unit, core, reg_val)); 
    SOCDNX_IF_ERR_EXIT(READ_SCH_ASSIGNED_CREDIT_CONFIGURATION_1r(unit, core, &reg_val));
    soc_reg_field_set(unit, SCH_ASSIGNED_CREDIT_CONFIGURATION_1r, &reg_val, ASSIGNED_CREDIT_WORTH_1f, JER_SCH_CREDIT_WORTH_DIV_MIN);
    SOCDNX_IF_ERR_EXIT(WRITE_SCH_ASSIGNED_CREDIT_CONFIGURATION_1r(unit, core, reg_val));

    
    for (psst_table_location = 0 ; psst_table_location < 512; psst_table_location++)
    {
        
        SOCDNX_IF_ERR_EXIT(READ_SCH_PIR_SHAPERS_STATIC_TABEL_PSSTm(unit,SCH_BLOCK(unit,core),psst_table_location,&mem_val));
        soc_SCH_PIR_SHAPERS_STATIC_TABEL_PSSTm_field32_set(unit,&mem_val,QUANTA_TO_ADDf,0);
        SOCDNX_IF_ERR_EXIT(WRITE_SCH_PIR_SHAPERS_STATIC_TABEL_PSSTm(unit,SCH_BLOCK(unit,core),psst_table_location,&mem_val));
    }


    
     SOCDNX_IF_ERR_EXIT(_jer_sch_rates_calculate_new_calendars(unit, core, 0, &is_new_rate_configured));
     if (is_new_rate_configured != 1) {
         SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL,(_BSL_SOCDNX_MSG("Could not find valid REBOUNDED_CREDIT_CONFIGURATION")));
     }



exit:
    SOCDNX_FUNC_RETURN;
}


uint32
jer_ofp_rates_sch_port_priority_rate_set(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  int                core,
    SOC_SAND_IN  uint32             tm_port,   
    SOC_SAND_IN  uint32             port_priority_ndx, 
    SOC_SAND_IN  uint32             rate
    )
{

    uint32 credit_div, reg_val, nof_ticks, quanta_nof_bits, quanta, offset,
           base_port_tc, hr_calendar_table_num, index, mem_val, max_rate_before_change, max_rate_after_change = 0;
    int old_rate, increasing_maximum_rate = 0 , decreasing_unique_maximum_rate = 0 , is_new_rate_configured = 0, maximum_multiplicity;
    uint8 valid;
    
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, tm_port, &base_port_tc));

    SOCDNX_IF_ERR_EXIT(_jer_sch_rates_port_get_max_rate(unit, core, 1, &max_rate_before_change, &maximum_multiplicity)); 

    offset = base_port_tc + port_priority_ndx;

    SOCDNX_IF_ERR_EXIT(arad_sw_db_sch_priority_port_rate_get(unit, core, offset, &old_rate, &valid));

    
    
    if ((old_rate == max_rate_before_change) && (rate < old_rate) && (maximum_multiplicity == 1) )
    {
        decreasing_unique_maximum_rate = 1;
    }
    
    else if (rate > max_rate_before_change)
    {
        increasing_maximum_rate = 1;
    }

    
    SOCDNX_IF_ERR_EXIT(arad_sw_db_sch_priority_port_rate_set(unit, core, offset, rate, 1));

    
    if (!increasing_maximum_rate)
    {
        SOCDNX_IF_ERR_EXIT(_jer_sch_rates_port_get_max_rate(unit, core, 1, &max_rate_after_change, &maximum_multiplicity));
    }

    SOCDNX_IF_ERR_EXIT(READ_SCH_PS_CALENDAR_SELECTr(unit, core, &reg_val));
    hr_calendar_table_num = soc_reg_field_get(unit, SCH_PS_CALENDAR_SELECTr, reg_val, PS_CALENDAR_SELECTf);

    
    SOCDNX_IF_ERR_EXIT(arad_sch_calendar_info_get(unit, core, hr_calendar_table_num, 1, &credit_div, &nof_ticks, &quanta_nof_bits));

    if (rate == 0)
    {
        quanta = 0;
    }
    else
    {    
      
        SOCDNX_IF_ERR_EXIT(arad_sch_port_rate_kbits_per_sec_to_qaunta(unit, rate, credit_div, nof_ticks, &quanta)); 
        
    }
    if (((quanta > 2047) && (increasing_maximum_rate)) || decreasing_unique_maximum_rate)
    {
        SOCDNX_IF_ERR_EXIT(_jer_sch_rates_calculate_new_calendars(unit, core, increasing_maximum_rate, &is_new_rate_configured));
    }

    if (!is_new_rate_configured)
    {
        SOCDNX_IF_ERR_EXIT(_jer_sch_rates_calendar_index_get(unit, core, offset, 1, &index));

        
        SOCDNX_IF_ERR_EXIT(READ_SCH_PIR_SHAPERS_STATIC_TABEL_PSSTm(unit, SCH_BLOCK(unit,core ), index + hr_calendar_table_num * 256, &mem_val));
        soc_mem_field32_set(unit, SCH_PIR_SHAPERS_STATIC_TABEL_PSSTm, &mem_val, QUANTA_TO_ADDf, quanta);
        SOCDNX_IF_ERR_EXIT(WRITE_SCH_PIR_SHAPERS_STATIC_TABEL_PSSTm(unit, SCH_BLOCK(unit, core), index + hr_calendar_table_num * 256, &mem_val));
    }
    

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
  jer_ofp_rates_sch_port_priority_rate_get(
    SOC_SAND_IN     int               unit,
    SOC_SAND_IN     int               core,
    SOC_SAND_IN     uint32            tm_port,
    SOC_SAND_IN     uint32            priority_ndx,    
    SOC_SAND_OUT    uint32            *rate
  )
{
    uint32 reg_val, hr_calendar_table_num, credit_div, offset, index, mem_val, 
           nof_ticks, base_port_tc, rate_internal, quanta , quanta_nof_bits;

    SOCDNX_INIT_FUNC_DEFS;
    
    
    SOCDNX_IF_ERR_EXIT(READ_SCH_PS_CALENDAR_SELECTr(unit, core, &reg_val));
    hr_calendar_table_num = soc_reg_field_get(unit, SCH_PS_CALENDAR_SELECTr, reg_val, PS_CALENDAR_SELECTf);
   
    
    SOCDNX_IF_ERR_EXIT(arad_sch_calendar_info_get(unit, core, hr_calendar_table_num, 1, &credit_div, &nof_ticks, &quanta_nof_bits));
  
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, tm_port, &base_port_tc));
    
    offset = base_port_tc + priority_ndx;
    
    SOCDNX_IF_ERR_EXIT(_jer_sch_rates_calendar_index_get(unit, core, offset, 1, &index));

    
    SOCDNX_IF_ERR_EXIT(READ_SCH_PIR_SHAPERS_STATIC_TABEL_PSSTm(unit, SCH_BLOCK(unit,core), index + 256*hr_calendar_table_num, &mem_val));
    quanta = soc_mem_field32_get(unit, SCH_PIR_SHAPERS_STATIC_TABEL_PSSTm, &mem_val, QUANTA_TO_ADDf);

    SOCDNX_IF_ERR_EXIT(arad_sch_port_qunta_to_rate_kbits_per_sec(
        unit,
        quanta,
        credit_div,
        nof_ticks,
        &rate_internal
      ));
        
    *rate = rate_internal; 

exit:
  SOCDNX_FUNC_RETURN;
}

uint32
jer_ofp_rates_sch_port_priority_max_burst_set(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  int                core,
    SOC_SAND_IN  uint32             tm_port,   
    SOC_SAND_IN  uint32             port_priority, 
    SOC_SAND_IN  uint32             max_burst 
   )
{
    uint32 base_port_tc, offset, index, mem_val, reg_val, hr_calendar_table_num;

    SOCDNX_INIT_FUNC_DEFS;
    
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, tm_port, &base_port_tc));

    SOCDNX_IF_ERR_EXIT(READ_SCH_PS_CALENDAR_SELECTr(unit, core, &reg_val));
    hr_calendar_table_num = soc_reg_field_get(unit, SCH_PS_CALENDAR_SELECTr, reg_val, PS_CALENDAR_SELECTf);

    offset = base_port_tc + port_priority;
    
    SOCDNX_IF_ERR_EXIT(_jer_sch_rates_calendar_index_get(unit, core, offset, 1, &index));

    

    SOCDNX_IF_ERR_EXIT(READ_SCH_PIR_SHAPERS_STATIC_TABEL_PSSTm(unit, SCH_BLOCK(unit,core ), index + hr_calendar_table_num*256, &mem_val));
    soc_mem_field32_set(unit, SCH_PIR_SHAPERS_STATIC_TABEL_PSSTm, &mem_val, MAX_BURSTf, max_burst);
    SOCDNX_IF_ERR_EXIT(WRITE_SCH_PIR_SHAPERS_STATIC_TABEL_PSSTm(unit, SCH_BLOCK(unit, core), index + hr_calendar_table_num*256, &mem_val));

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
jer_ofp_rates_sch_port_priority_max_burst_get(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  int                core,
    SOC_SAND_IN  uint32             tm_port,   
    SOC_SAND_IN  uint32             port_priority, 
    SOC_SAND_OUT uint32             *max_burst
   )
{
    uint32 base_port_tc, reg_val, offset, index, mem_val, hr_calendar_table_num;

    SOCDNX_INIT_FUNC_DEFS;
    
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, tm_port, &base_port_tc));

    
    SOCDNX_IF_ERR_EXIT(READ_SCH_PS_CALENDAR_SELECTr(unit, core, &reg_val));
    hr_calendar_table_num = soc_reg_field_get(unit, SCH_PS_CALENDAR_SELECTr, reg_val, PS_CALENDAR_SELECTf);

    offset = base_port_tc + port_priority;
    
    SOCDNX_IF_ERR_EXIT(_jer_sch_rates_calendar_index_get(unit, core, offset, 1, &index));

    

    SOCDNX_IF_ERR_EXIT(READ_SCH_PIR_SHAPERS_STATIC_TABEL_PSSTm(unit, SCH_BLOCK(unit,core ), index + hr_calendar_table_num*256, &mem_val));
    *max_burst = soc_mem_field32_get(unit, SCH_PIR_SHAPERS_STATIC_TABEL_PSSTm, &mem_val, MAX_BURSTf);

exit:
    SOCDNX_FUNC_RETURN;
}


uint32
jer_ofp_rates_sch_tcg_shaper_rate_set(
    SOC_SAND_IN     int               unit,
    SOC_SAND_IN     int               core,
    SOC_SAND_IN     uint32            tm_port,
    SOC_SAND_IN     ARAD_TCG_NDX      tcg_ndx,
    SOC_SAND_IN     uint32               rate)
{
    uint32  credit_div, reg_val ,nof_ticks, quanta_nof_bits, quanta, base_port_tc, hr_calendar_table_num,
            offset, index, mem_val , decreasing_unique_maximum_rate = 0, increasing_maximum_rate = 0, max_rate_before_change;
    int old_rate, maximum_multiplicity, is_new_rate_configured = 0;
    uint8 valid;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, tm_port, &base_port_tc));

    SOCDNX_IF_ERR_EXIT(_jer_sch_rates_port_get_max_rate(unit, core, 0, &max_rate_before_change, &maximum_multiplicity)); 

    offset = ARAD_SCH_PORT_TCG_ID_GET(base_port_tc,tcg_ndx);

    SOCDNX_IF_ERR_EXIT(arad_sw_db_sch_port_tcg_rate_get(unit, core, offset, &old_rate, &valid));

    
    
    if ((old_rate == max_rate_before_change) && (rate < old_rate) && (maximum_multiplicity == 1) )
    {
        decreasing_unique_maximum_rate = 1;
    }
    
    else if (rate > max_rate_before_change)
    {
        increasing_maximum_rate = 1;
    }

    SOCDNX_IF_ERR_EXIT(arad_sw_db_sch_port_tcg_rate_set(unit, core, offset, rate, 1));

    SOCDNX_IF_ERR_EXIT(READ_SCH_PS_CALENDAR_SELECTr(unit, core, &reg_val));
    hr_calendar_table_num = soc_reg_field_get(unit, SCH_PS_CALENDAR_SELECTr, reg_val, PS_CALENDAR_SELECTf);

    
    SOCDNX_IF_ERR_EXIT(arad_sch_calendar_info_get(unit, core, hr_calendar_table_num, 0, &credit_div, &nof_ticks, &quanta_nof_bits));

    if (rate == 0)
    {
        quanta = 0;
    }
    else
    {    
      
        SOCDNX_IF_ERR_EXIT(arad_sch_port_rate_kbits_per_sec_to_qaunta(unit, rate, credit_div, nof_ticks, &quanta));  
        
    }

    if (((quanta > 2047) && (increasing_maximum_rate)) || decreasing_unique_maximum_rate)
    {
        SOCDNX_IF_ERR_EXIT(_jer_sch_rates_calculate_new_calendars(unit, core, increasing_maximum_rate, &is_new_rate_configured));
    }

    if (!is_new_rate_configured)
    {
        SOCDNX_IF_ERR_EXIT(_jer_sch_rates_calendar_index_get(unit, core, offset, 0, &index));

        
        SOCDNX_IF_ERR_EXIT(READ_SCH_CIR_SHAPERS_STATIC_TABEL_CSSTm(unit, SCH_BLOCK(unit,core ), index + 256*hr_calendar_table_num, &mem_val));
        soc_mem_field32_set(unit, SCH_CIR_SHAPERS_STATIC_TABEL_CSSTm, &mem_val, QUANTA_TO_ADDf, quanta);
        SOCDNX_IF_ERR_EXIT(WRITE_SCH_CIR_SHAPERS_STATIC_TABEL_CSSTm(unit, SCH_BLOCK(unit, core), index + 256*hr_calendar_table_num, &mem_val));
    }


exit:
    SOCDNX_FUNC_RETURN;
}

uint32
jer_ofp_rates_sch_tcg_shaper_rate_get(
    SOC_SAND_IN     int               unit,
    SOC_SAND_IN     int               core,
    SOC_SAND_IN     uint32            tm_port,
    SOC_SAND_IN     ARAD_TCG_NDX      tcg_ndx,
    SOC_SAND_OUT    uint32*               rate)
{
    uint32  reg_val, hr_calendar_table_num, quanta_nof_bits, credit_div, index, 
            nof_ticks, base_port_tc, rate_internal, quanta = 0, offset, mem_val;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(READ_SCH_PS_CALENDAR_SELECTr(unit, core, &reg_val));
    hr_calendar_table_num = soc_reg_field_get(unit, SCH_PS_CALENDAR_SELECTr, reg_val, PS_CALENDAR_SELECTf);
   
    SOCDNX_IF_ERR_EXIT(arad_sch_calendar_info_get(unit, core, hr_calendar_table_num, 0, &credit_div, &nof_ticks, &quanta_nof_bits));
  
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, tm_port, &base_port_tc));
    
    offset = ARAD_SCH_PORT_TCG_ID_GET(base_port_tc,tcg_ndx);

    SOCDNX_IF_ERR_EXIT(_jer_sch_rates_calendar_index_get(unit, core, offset, 0, &index));

    
    SOCDNX_IF_ERR_EXIT(READ_SCH_CIR_SHAPERS_STATIC_TABEL_CSSTm(unit, SCH_BLOCK(unit,core), index + 256*hr_calendar_table_num, &mem_val));
    quanta = soc_mem_field32_get(unit, SCH_CIR_SHAPERS_STATIC_TABEL_CSSTm, &mem_val, QUANTA_TO_ADDf);

    if (quanta == 0) 
    {
        rate_internal = 0;
    } 
    else 
    {
        SOCDNX_SAND_IF_ERR_EXIT(arad_sch_port_qunta_to_rate_kbits_per_sec(
            unit,
            quanta,
            credit_div,
            nof_ticks,
            &rate_internal
        )); 
    }
    *rate = rate_internal; 

exit:
    SOCDNX_FUNC_RETURN;
}


int
jer_ofp_rates_sch_tcg_shaper_disable(
    SOC_SAND_IN     int               unit,
    SOC_SAND_IN     int               core,
    SOC_SAND_IN     uint32            base_q_pair,
    SOC_SAND_IN     uint32            nof_q_pairs)

{
    uint32  data = 0x7FFFFFF;
    int cal_index;
    uint32 tcg_ndx, offset, index;
    int rate;
    uint8 valid;

    SOCDNX_INIT_FUNC_DEFS;

    for (tcg_ndx = 0; tcg_ndx < nof_q_pairs; tcg_ndx++)
    {
        
        offset = base_q_pair + tcg_ndx;
        SOCDNX_IF_ERR_EXIT(arad_sw_db_sch_port_tcg_rate_get(unit, core, offset, &rate, &valid));

        if (valid)
        {
            SOCDNX_IF_ERR_EXIT(_jer_sch_rates_calendar_index_get(unit, core, offset, 0, &index));
            
            for (cal_index = 0; cal_index < 2; cal_index++)
            {
                SOCDNX_IF_ERR_EXIT(WRITE_SCH_CIR_SHAPERS_STATIC_TABEL_CSSTm(unit, SCH_BLOCK(unit, core), index + 256*cal_index, &data));
            }
            
            
            SOCDNX_IF_ERR_EXIT(arad_sw_db_sch_port_tcg_rate_set(unit, core, offset, 0, 0));
        }
    }
exit:
    SOCDNX_FUNC_RETURN;
}

uint32
jer_ofp_rates_sch_tcg_shaper_max_burst_set(
    SOC_SAND_IN     int               unit,
    SOC_SAND_IN     int               core,
    SOC_SAND_IN     uint32            tm_port,
    SOC_SAND_IN     ARAD_TCG_NDX      tcg_ndx,
    SOC_SAND_IN     uint32               max_burst)
{
    uint32 base_port_tc, offset, tbl_data, index, mem_val, hr_calendar_table_num, reg_val;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, tm_port, &base_port_tc));

    SOCDNX_IF_ERR_EXIT(READ_SCH_PS_CALENDAR_SELECTr(unit, core, &reg_val));
    hr_calendar_table_num = soc_reg_field_get(unit, SCH_PS_CALENDAR_SELECTr, reg_val, PS_CALENDAR_SELECTf);

    offset = ARAD_SCH_PORT_TCG_ID_GET(base_port_tc,tcg_ndx);

    SOCDNX_IF_ERR_EXIT(_jer_sch_rates_calendar_index_get(unit, core, offset, 0, &index));

    SOCDNX_IF_ERR_EXIT(READ_SCH_CIR_SHAPERS_STATIC_TABEL_CSSTm(unit,SCH_BLOCK(unit,core),index + 256*hr_calendar_table_num ,&tbl_data));
    soc_mem_field32_set(unit, SCH_CIR_SHAPERS_STATIC_TABEL_CSSTm, &mem_val, MAX_BURSTf, max_burst);
    SOCDNX_IF_ERR_EXIT(WRITE_SCH_CIR_SHAPERS_STATIC_TABEL_CSSTm(unit,SCH_BLOCK(unit,core),index + 256*hr_calendar_table_num,&tbl_data));

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
jer_ofp_rates_sch_tcg_shaper_max_burst_get(
    SOC_SAND_IN     int               unit,
    SOC_SAND_IN     int               core,
    SOC_SAND_IN     uint32            tm_port,
    SOC_SAND_IN     ARAD_TCG_NDX      tcg_ndx,
    SOC_SAND_OUT    uint32*               max_burst)
{
    uint32 base_port_tc, offset, index, mem_val, reg_val, hr_calendar_table_num;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, tm_port, &base_port_tc));

    
    SOCDNX_IF_ERR_EXIT(READ_SCH_PS_CALENDAR_SELECTr(unit, core, &reg_val));
    hr_calendar_table_num = soc_reg_field_get(unit, SCH_PS_CALENDAR_SELECTr, reg_val, PS_CALENDAR_SELECTf);

    offset = ARAD_SCH_PORT_TCG_ID_GET(base_port_tc,tcg_ndx);

    SOCDNX_IF_ERR_EXIT(_jer_sch_rates_calendar_index_get(unit, core, offset, 0, &index));

    SOCDNX_IF_ERR_EXIT(READ_SCH_CIR_SHAPERS_STATIC_TABEL_CSSTm(unit,SCH_BLOCK(unit,core),index +hr_calendar_table_num*256 ,&mem_val));
    *max_burst = soc_mem_field32_get(unit, SCH_CIR_SHAPERS_STATIC_TABEL_CSSTm, &mem_val, MAX_BURSTf);

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
_jer_sch_rates_switch_calendars(int unit, int core, int new_calendar_table_num, int new_divider)
{
    uint32 mem_val, credit_div, nof_ticks, quanta_nof_bits, reg_val, cal_length_psst, cal_length_csst, hr_num, tcg_index,
           quanta, old_index, new_index, max_burst;
    uint8 valid;
    int rate, i;
    SOCDNX_INIT_FUNC_DEFS;

    
    if (new_calendar_table_num == 0)
    {
        SOCDNX_IF_ERR_EXIT(READ_SCH_REBOUNDED_CREDIT_CONFIGURATIONr(unit, core, &reg_val));
        soc_reg_field_set(unit, SCH_REBOUNDED_CREDIT_CONFIGURATIONr, &reg_val, REBOUNDED_CREDIT_WORTHf, new_divider);
        SOCDNX_IF_ERR_EXIT(WRITE_SCH_REBOUNDED_CREDIT_CONFIGURATIONr(unit, core, reg_val));

        SOCDNX_IF_ERR_EXIT(READ_SCH_ASSIGNED_CREDIT_CONFIGURATIONr(unit, core, &reg_val));
        soc_reg_field_set(unit, SCH_ASSIGNED_CREDIT_CONFIGURATIONr, &reg_val, ASSIGNED_CREDIT_WORTHf, new_divider);
        SOCDNX_IF_ERR_EXIT(WRITE_SCH_ASSIGNED_CREDIT_CONFIGURATIONr(unit, core, reg_val));
    }
    else
    {
        SOCDNX_IF_ERR_EXIT(READ_SCH_REBOUNDED_CREDIT_CONFIGURATION_1r(unit, core, &reg_val));
        soc_reg_field_set(unit, SCH_REBOUNDED_CREDIT_CONFIGURATION_1r, &reg_val, REBOUNDED_CREDIT_WORTH_1f, new_divider);
        SOCDNX_IF_ERR_EXIT(WRITE_SCH_REBOUNDED_CREDIT_CONFIGURATION_1r(unit, core, reg_val));

        SOCDNX_IF_ERR_EXIT(READ_SCH_ASSIGNED_CREDIT_CONFIGURATION_1r(unit, core, &reg_val));
        soc_reg_field_set(unit, SCH_ASSIGNED_CREDIT_CONFIGURATION_1r, &reg_val, ASSIGNED_CREDIT_WORTH_1f, new_divider);
        SOCDNX_IF_ERR_EXIT(WRITE_SCH_ASSIGNED_CREDIT_CONFIGURATION_1r(unit, core, reg_val));
    }

    
    SOCDNX_IF_ERR_EXIT(READ_SCH_PIR_SHAPERS_CONFIGURATIONr(unit, core, &reg_val));
    cal_length_psst = soc_reg_field_get(unit, SCH_PIR_SHAPERS_CONFIGURATIONr, reg_val, PIR_SHAPERS_CAL_LENGTHf) + 1;
    SOCDNX_IF_ERR_EXIT(READ_SCH_CIR_SHAPERS_CONFIGURATIONr(unit, core, &reg_val));
    cal_length_csst = soc_reg_field_get(unit, SCH_CIR_SHAPERS_CONFIGURATIONr, reg_val, CIR_SHAPERS_CAL_LENGTHf) + 1;
   
    
    SOCDNX_IF_ERR_EXIT(arad_sch_calendar_info_get(unit, core, new_calendar_table_num, 1, &credit_div, &nof_ticks, &quanta_nof_bits));    
    for (i = 0 ; i < cal_length_psst; i++)
    {
        SOCDNX_IF_ERR_EXIT(READ_SCH_PIR_SHAPER_CALENDAR_PSCm(unit, SCH_BLOCK(unit, core), i, &mem_val));
        hr_num = soc_mem_field32_get(unit, SCH_PIR_SHAPER_CALENDAR_PSCm, &mem_val, HR_NUMf);

        SOCDNX_IF_ERR_EXIT(arad_sw_db_sch_priority_port_rate_get(unit, core, hr_num, &rate, &valid));
        if (valid)
        {
            
            if (rate == 0)
            {
                quanta = 0;
            }
            else
            {                  
                SOCDNX_IF_ERR_EXIT(arad_sch_port_rate_kbits_per_sec_to_qaunta(unit, rate, credit_div, nof_ticks, &quanta));  
                SOC_SAND_LIMIT_FROM_ABOVE(quanta, SOC_SAND_BITS_MASK(quanta_nof_bits-1,0));
            }
            old_index = (new_calendar_table_num == 0) ? i+256:i ;
            new_index = (old_index + 256) % 512;

            
            SOCDNX_IF_ERR_EXIT(READ_SCH_PIR_SHAPERS_STATIC_TABEL_PSSTm(unit, SCH_BLOCK(unit, core), old_index, &mem_val));
            max_burst = soc_mem_field32_get(unit, SCH_PIR_SHAPERS_STATIC_TABEL_PSSTm, &mem_val, MAX_BURSTf);
            
            SOCDNX_IF_ERR_EXIT(READ_SCH_PIR_SHAPERS_STATIC_TABEL_PSSTm(unit, SCH_BLOCK(unit, core), new_index, &mem_val));
            soc_mem_field32_set(unit, SCH_PIR_SHAPERS_STATIC_TABEL_PSSTm, &mem_val, QUANTA_TO_ADDf, quanta);
            soc_mem_field32_set(unit, SCH_PIR_SHAPERS_STATIC_TABEL_PSSTm, &mem_val, MAX_BURSTf, max_burst);
            SOCDNX_IF_ERR_EXIT(WRITE_SCH_PIR_SHAPERS_STATIC_TABEL_PSSTm(unit, SCH_BLOCK(unit, core), new_index, &mem_val));
        }
    }
    
    SOCDNX_IF_ERR_EXIT(arad_sch_calendar_info_get(unit, core, new_calendar_table_num, 0, &credit_div, &nof_ticks, &quanta_nof_bits)); 
    for (i = 0; i < cal_length_csst; i++)
    {
        SOCDNX_IF_ERR_EXIT(READ_SCH_CIR_SHAPER_CALENDAR_CSCm(unit, SCH_BLOCK(unit, core), i, &mem_val));
        tcg_index = soc_mem_field32_get(unit, SCH_CIR_SHAPER_CALENDAR_CSCm, &mem_val, PG_NUMf);

        SOCDNX_IF_ERR_EXIT(arad_sw_db_sch_port_tcg_rate_get(unit, core, tcg_index, &rate, &valid));
        if (valid)
        {
            
            if (rate == 0)
            {
                quanta = 0;
            }
            else
            {                  
                SOCDNX_IF_ERR_EXIT(arad_sch_port_rate_kbits_per_sec_to_qaunta(unit, rate, credit_div, nof_ticks, &quanta));  
                SOC_SAND_LIMIT_FROM_ABOVE(quanta, SOC_SAND_BITS_MASK(quanta_nof_bits-1,0));
            }
            old_index = (new_calendar_table_num == 0) ? i+256:i;
            new_index = (old_index + 256) % 512;

            
            SOCDNX_IF_ERR_EXIT(READ_SCH_CIR_SHAPERS_STATIC_TABEL_CSSTm(unit, SCH_BLOCK(unit, core), old_index, &mem_val));
            max_burst = soc_mem_field32_get(unit, SCH_CIR_SHAPERS_STATIC_TABEL_CSSTm, &mem_val, MAX_BURSTf);

            SOCDNX_IF_ERR_EXIT(READ_SCH_CIR_SHAPERS_STATIC_TABEL_CSSTm(unit, SCH_BLOCK(unit, core), new_index, &mem_val));
            soc_mem_field32_set(unit, SCH_CIR_SHAPERS_STATIC_TABEL_CSSTm, &mem_val, QUANTA_TO_ADDf, quanta);
            soc_mem_field32_set(unit, SCH_CIR_SHAPERS_STATIC_TABEL_CSSTm, &mem_val, MAX_BURSTf, max_burst);
            SOCDNX_IF_ERR_EXIT(WRITE_SCH_CIR_SHAPERS_STATIC_TABEL_CSSTm(unit, SCH_BLOCK(unit, core), new_index, &mem_val));
        }
    }

    
    SOCDNX_IF_ERR_EXIT(READ_SCH_PS_CALENDAR_SELECTr(unit, core, &reg_val));
    soc_reg_field_set(unit, SCH_PS_CALENDAR_SELECTr, &reg_val, PS_CALENDAR_SELECTf, new_calendar_table_num);
    SOCDNX_IF_ERR_EXIT(WRITE_SCH_PS_CALENDAR_SELECTr(unit, core, reg_val));

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
_jer_sch_rates_calculate_new_divider(int unit, int core, int hr_calendar_table_num, int max_rate_increased, int old_divider ,uint32 *new_divider)
{
    uint32 nof_ticks, quanta_nof_bits, new_quanta,
           curr_divider, iterate_divider;
    uint32 new_max_rate, new_max_rate_psst, new_max_rate_csst;
    int max_psst_multiplicity, max_csst_multiplicity;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(_jer_sch_rates_port_get_max_rate(unit, core, 1, &new_max_rate_psst, &max_psst_multiplicity));
    SOCDNX_IF_ERR_EXIT(_jer_sch_rates_port_get_max_rate(unit, core, 0, &new_max_rate_csst, &max_csst_multiplicity));
    if (new_max_rate_psst == 0 && new_max_rate_csst == 0)
    {
        curr_divider = old_divider; 
    }
    else
    {
        if ( new_max_rate_psst > new_max_rate_csst )
        {
            new_max_rate = new_max_rate_psst;
            SOCDNX_IF_ERR_EXIT(arad_sch_calendar_info_get(unit, core, hr_calendar_table_num, 1, &curr_divider, &nof_ticks, &quanta_nof_bits));
        }
        else
        {
            new_max_rate = new_max_rate_csst;
            SOCDNX_IF_ERR_EXIT(arad_sch_calendar_info_get(unit, core, hr_calendar_table_num, 0, &curr_divider, &nof_ticks, &quanta_nof_bits));
        }
        curr_divider = old_divider;

        if (max_rate_increased) 
        {
            for (curr_divider = old_divider; curr_divider >= JER_SCH_CREDIT_WORTH_DIV_MIN ; curr_divider--)
            {
                SOCDNX_SAND_IF_ERR_EXIT(arad_sch_port_rate_kbits_per_sec_to_qaunta(unit, new_max_rate, curr_divider, nof_ticks, &new_quanta));
                if(new_quanta <= JER_SCH_SHAPERS_QUANTA_TO_ADD_MAX)
                {
                    break;
                }
            }
        }
        else 
        {
            
            for (iterate_divider = curr_divider + 1; iterate_divider < JER_SCH_CREDIT_WORTH_DIV_MAX; iterate_divider++)
            {
                SOCDNX_SAND_IF_ERR_EXIT(arad_sch_port_rate_kbits_per_sec_to_qaunta(unit, new_max_rate, iterate_divider, nof_ticks, &new_quanta));
                if(new_quanta > JER_SCH_SHAPERS_QUANTA_TO_ADD_MAX)
                {
                    break;
                }
                curr_divider = iterate_divider;
            }
        }
    }
    *new_divider = curr_divider;

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
_jer_sch_rates_calculate_new_calendars(int unit, int core, int max_rate_increased, int *new_rate_configured)
{
    int hr_calendar_table_num;
    uint32 divider, reg_val, new_divider, old_divider;

    SOCDNX_INIT_FUNC_DEFS;
    
    

    SOCDNX_IF_ERR_EXIT(READ_SCH_PS_CALENDAR_SELECTr(unit, core, &reg_val));
    hr_calendar_table_num = soc_reg_field_get(unit, SCH_PS_CALENDAR_SELECTr, reg_val, PS_CALENDAR_SELECTf);

    if (hr_calendar_table_num == 0)
    {
        SOCDNX_IF_ERR_EXIT(READ_SCH_REBOUNDED_CREDIT_CONFIGURATIONr(unit, core, &reg_val));
        divider = soc_reg_field_get(unit, SCH_REBOUNDED_CREDIT_CONFIGURATIONr, reg_val, REBOUNDED_CREDIT_WORTHf); 
    }
    else
    {
        SOCDNX_IF_ERR_EXIT(READ_SCH_REBOUNDED_CREDIT_CONFIGURATION_1r(unit, core, &reg_val));
        divider = soc_reg_field_get(unit, SCH_REBOUNDED_CREDIT_CONFIGURATION_1r, reg_val, REBOUNDED_CREDIT_WORTH_1f);
    }
    old_divider = divider;

    SOCDNX_IF_ERR_EXIT(_jer_sch_rates_calculate_new_divider(unit, core, hr_calendar_table_num, max_rate_increased, old_divider, &new_divider));
    
    if (old_divider == new_divider)  
    {
        *new_rate_configured = 0;
        SOC_EXIT;
    }

    
    
    _jer_sch_rates_switch_calendars(unit, core, (hr_calendar_table_num+1)%2, new_divider);
    *new_rate_configured = 1;

exit:
    SOCDNX_FUNC_RETURN;

}


#undef _ERR_MSG_MODULE_NAME

