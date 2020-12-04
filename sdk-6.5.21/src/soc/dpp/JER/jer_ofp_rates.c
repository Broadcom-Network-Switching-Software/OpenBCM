/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: jer_ofp_rates.c
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif



#define _ERR_MSG_MODULE_NAME BSL_SOC_EGRESS

#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>
#include <sal/compiler.h>

#include <soc/mcm/memregs.h>

#include <soc/dpp/drv.h>
#include <soc/dpp/port_sw_db.h>
#include <soc/dpp/mbcm.h>

#include <soc/dpp/JER/jer_ports.h>
#include <soc/dpp/JER/jer_ofp_rates.h>
#include <soc/dpp/JER/jer_egr_queuing.h>
#include <soc/dpp/QAX/qax_link_bonding.h>

#include <soc/dpp/ARAD/arad_sw_db.h>
#include <soc/dpp/ARAD/arad_ofp_rates.h>
#include <soc/dpp/ARAD/arad_api_mgmt.h>
#include <soc/dpp/SAND/Utils/sand_occupation_bitmap.h>

#define SOC_JER_OFP_RATES_NOF_CALS_IN_DUAL_MODE 2


STATIC int
soc_jer_ofp_rates_interface_internal_rate_set(int unit, int core, uint32 egr_if_id , uint32 internal_rate, int is_cal_shaper)
{
    soc_reg_above_64_val_t data;

    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_IF_ERR_EXIT(READ_EGQ_EGRESS_SHAPER_CONFIGURATIONm(unit, EGQ_BLOCK(unit, core), egr_if_id, &data));

    if (is_cal_shaper) { 
        
        if (internal_rate > SOC_DPP_DEFS_GET(unit, cal_internal_rate_max)) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("calendar rate for interface is too big \n")));
        }
        soc_EGQ_EGRESS_SHAPER_CONFIGURATIONm_field32_set(unit, &data, CH_SPR_RATE_Af, internal_rate);
        soc_EGQ_EGRESS_SHAPER_CONFIGURATIONm_field32_set(unit, &data, CH_SPR_RATE_Bf, internal_rate);
    } else { 
        
        if (internal_rate > ARAD_OFP_RATES_EGRESS_SHAPER_MAX_INTERNAL_IF_RATE) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("interface rate for interface is too big \n")));
        }           
        soc_EGQ_EGRESS_SHAPER_CONFIGURATIONm_field32_set(unit, &data, CH_IF_SHAPER_RATEf, internal_rate);
    }

    
    SOCDNX_IF_ERR_EXIT(WRITE_EGQ_EGRESS_SHAPER_CONFIGURATIONm(unit, EGQ_BLOCK(unit, core), egr_if_id, &data));

exit:
    SOCDNX_FUNC_RETURN
}

int
soc_jer_ofp_rates_interface_internal_rate_get(
    SOC_SAND_IN   int                   unit, 
    SOC_SAND_IN   int                   core, 
    SOC_SAND_IN   uint32                egr_if_id, 
    SOC_SAND_OUT  uint32                *internal_rate)
{
    soc_reg_above_64_val_t data;

    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_IF_ERR_EXIT(READ_EGQ_EGRESS_SHAPER_CONFIGURATIONm(unit, EGQ_BLOCK(unit, core), egr_if_id, &data));    
    *internal_rate = soc_EGQ_EGRESS_SHAPER_CONFIGURATIONm_field32_get(unit, &data, CH_IF_SHAPER_RATEf);

exit:
    SOCDNX_FUNC_RETURN
}





int
soc_jer_ofp_rates_init(SOC_SAND_IN int unit)
{
    int                    core, cal_id, idx, ps_id, tcg_id, res;
    soc_reg_above_64_val_t data64;
    uint32                 fld_val, data;
    uint32 init_max_burst = ARAD_OFP_RATES_BURST_DEAULT;
    ARAD_SW_DB_DEV_RATE tcg_rate;
    ARAD_SW_DB_DEV_RATE queue_rate;

    SOCDNX_INIT_FUNC_DEFS;

    SOC_DPP_CORES_ITER(SOC_CORE_ALL, core) {

        SOCDNX_SAND_IF_ERR_EXIT(READ_EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr(unit, core, &data));

        
        SOCDNX_SAND_IF_ERR_EXIT(arad_fill_table_with_entry(unit, EGQ_PMCm, EGQ_BLOCK(unit, core), &init_max_burst));

        SOCDNX_SAND_IF_ERR_EXIT(arad_fill_table_with_entry(unit, EGQ_TCG_PMCm, EGQ_BLOCK(unit, core), &init_max_burst));

        SOCDNX_SAND_IF_ERR_EXIT(arad_fill_table_with_entry(unit, EGQ_QP_PMCm, EGQ_BLOCK(unit, core), &init_max_burst));

        
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

        
        fld_val = 0x1;
        soc_reg_field_set(unit, EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr, &data, INTERFACE_SPR_ENAf, fld_val);
        soc_reg_field_set(unit, EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr, &data, OTM_SPR_ENAf, fld_val);
        soc_reg_field_set(unit, EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr, &data, QPAIR_SPR_ENAf, fld_val);
        soc_reg_field_set(unit, EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr, &data, TCG_SPR_ENAf, fld_val);

        
        soc_reg_field_set(unit, EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr, &data, QPAIR_SPR_RESOLUTIONf, 1);
        soc_reg_field_set(unit, EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr, &data, TCG_SPR_RESOLUTIONf, 1);

        
        soc_reg_field_set(unit, EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr, &data, IGNORE_QEMPTYf, 0);

        
        soc_reg_field_set(unit, EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr, &data, SHAPER_PACKET_RATE_CONSTf, ARAD_OFP_RATES_DEFAULT_PACKET_SIZE);

        SOCDNX_SAND_IF_ERR_EXIT(WRITE_EGQ_EGRESS_SHAPER_ENABLE_SETTINGSr(unit, core, data));

        SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.egq_tcg_qpair_shaper_enable.set(unit,TRUE));

        
        for (cal_id = 0; cal_id < SOC_DPP_DEFS_GET(unit, nof_channelized_calendars); cal_id++) {
            
            SOCDNX_IF_ERR_EXIT(READ_EGQ_EGRESS_SHAPER_CONFIGURATIONm(unit, EGQ_BLOCK(unit, core), cal_id, &data64));

                
            soc_EGQ_EGRESS_SHAPER_CONFIGURATIONm_field32_set(unit, &data64, CH_IF_SHAPER_RATEf, ARAD_OFP_RATES_EGRESS_SHAPER_MAX_INTERNAL_IF_RATE);
            
            soc_EGQ_EGRESS_SHAPER_CONFIGURATIONm_field32_set(unit, &data64, CH_IF_SHAPER_MAX_BURSTf, ARAD_OFP_RATES_EGRESS_SHAPER_MAX_INTERNAL_IF_BURST);
                
            soc_EGQ_EGRESS_SHAPER_CONFIGURATIONm_field32_set(unit, &data64, CH_SPR_RATE_Af, SOC_DPP_DEFS_GET(unit, cal_internal_rate_max));
            soc_EGQ_EGRESS_SHAPER_CONFIGURATIONm_field32_set(unit, &data64, CH_SPR_RATE_Bf, SOC_DPP_DEFS_GET(unit, cal_internal_rate_max));
            
            soc_EGQ_EGRESS_SHAPER_CONFIGURATIONm_field32_set(unit, &data64, CH_SPR_MAX_BURST_Af, ARAD_OFP_RATES_EGRESS_SHAPER_MAX_INTERNAL_CAL_BURST);
            soc_EGQ_EGRESS_SHAPER_CONFIGURATIONm_field32_set(unit, &data64, CH_SPR_MAX_BURST_Bf, ARAD_OFP_RATES_EGRESS_SHAPER_MAX_INTERNAL_CAL_BURST);
            
            soc_EGQ_EGRESS_SHAPER_CONFIGURATIONm_field32_set(unit, &data64, CH_SPR_CAL_LEN_Af, 0x0);
            soc_EGQ_EGRESS_SHAPER_CONFIGURATIONm_field32_set(unit, &data64, CH_SPR_CAL_LEN_Bf, 0x0);

            
            SOCDNX_IF_ERR_EXIT(WRITE_EGQ_EGRESS_SHAPER_CONFIGURATIONm(unit, EGQ_BLOCK(unit, core), cal_id, &data64));
        }
    }

exit:
    SOCDNX_FUNC_RETURN
}

int
soc_jer_ofp_rates_egq_interface_shaper_set(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  int                          core,
    SOC_SAND_IN  uint32                       tm_port,
    SOC_SAND_IN  SOC_TMC_OFP_SHPR_UPDATE_MODE rate_update_mode,
    SOC_SAND_IN  uint32                       if_shaper_rate
  )
{
    uint32       egr_if_id, internal_rate, low_priority_cal, high_priority_cal, cal_id;
    int          is_single_cal_mode = 0;
    int          is_channelized;
    soc_port_t   port;   
    soc_port_if_t interface_type;
    ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE ilkn_tdm_dedicated_queuing;
#ifdef BCM_LB_SUPPORT
    soc_port_t port_i;
    soc_pbmp_t lb_ports;
    uint32     lb_port_num = 0;
    uint32     egress_offset = 0;
    uint32     high_priority_cal_i = INVALID_CALENDAR, low_priority_cal_i = INVALID_CALENDAR;
#endif

    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_to_local_port_get(unit, core, tm_port, &port));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_channelized_port_get(unit,port, &is_channelized));
    if (is_channelized) {
        SOCDNX_IF_ERR_EXIT(soc_jer_egr_port2egress_offset(unit, core, tm_port, &egr_if_id));
    } else {
        egr_if_id = SOC_DPP_DEFS_GET(unit, non_channelized_cal_id);
    }

    
    
    
    
    
    SOCDNX_IF_ERR_EXIT(arad_ofp_rates_egq_shaper_rate_to_internal
        (unit, ARAD_OFP_RATES_EGQ_CAL_CHAN_ARB, if_shaper_rate, (is_channelized == FALSE), &internal_rate));

    
     
    
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface_type));
    ilkn_tdm_dedicated_queuing = SOC_DPP_CONFIG(unit)->arad->init.ilkn_tdm_dedicated_queuing;

    if (ilkn_tdm_dedicated_queuing == ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE_ON && interface_type == SOC_PORT_IF_ILKN) {
        
        SOC_SAND_SET_BIT(egr_if_id, 0x0, 0); 
        SOCDNX_IF_ERR_EXIT(soc_jer_ofp_rates_interface_internal_rate_set(unit, core, egr_if_id, internal_rate, FALSE ));

        
        egr_if_id +=1; 
    }
    SOCDNX_IF_ERR_EXIT(soc_jer_ofp_rates_interface_internal_rate_set(unit, core, egr_if_id, internal_rate, FALSE ));




    
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_single_cal_mode_get(unit, port, &is_single_cal_mode)); 
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_low_priority_cal_get(unit, port, &low_priority_cal));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_high_priority_cal_get(unit, port, &high_priority_cal));

#ifdef BCM_LB_SUPPORT
    
    if (SOC_IS_QAX(unit) && SOC_DPP_CONFIG(unit)->qax->lbi_en) {
        SOCDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port2egress_offset, (unit, core, tm_port, &egress_offset)));
        if (egress_offset == SOC_QAX_EGR_IF_LBG_RESERVE) {
            SOCDNX_IF_ERR_EXIT(qax_lb_ports_on_reserve_intf_get(unit, &lb_ports, &lb_port_num));
            SOC_PBMP_ITER(lb_ports, port_i) {
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_high_priority_cal_get(unit, port_i, &high_priority_cal_i));
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_low_priority_cal_get(unit, port_i, &low_priority_cal_i));
                if (high_priority_cal == INVALID_CALENDAR)    high_priority_cal = high_priority_cal_i;
                if (low_priority_cal == INVALID_CALENDAR)     low_priority_cal = low_priority_cal_i;
            }
        }
    }
#endif

    if (is_single_cal_mode) { 
        
        cal_id = INVALID_CALENDAR;
        if (high_priority_cal != INVALID_CALENDAR) {
            cal_id = high_priority_cal;
        } else {
            if (low_priority_cal != INVALID_CALENDAR) {
                cal_id = low_priority_cal;
            }
        }

        if (cal_id != INVALID_CALENDAR) {
            SOCDNX_IF_ERR_EXIT(soc_jer_ofp_rates_interface_internal_rate_set(unit, core, cal_id, 
                                                                             SOC_DPP_DEFS_GET(unit, cal_internal_rate_max), TRUE ));
        }
    } else { 
        
        if (low_priority_cal != INVALID_CALENDAR) {
            SOCDNX_IF_ERR_EXIT(soc_jer_ofp_rates_interface_internal_rate_set(unit, core, low_priority_cal, internal_rate, TRUE ));
        }

        if (high_priority_cal != INVALID_CALENDAR) {
            SOCDNX_IF_ERR_EXIT(soc_jer_ofp_rates_interface_internal_rate_set(unit, core, high_priority_cal, internal_rate, TRUE ));
        }
    }

exit:
    SOCDNX_FUNC_RETURN
}


int
soc_jer_ofp_rates_egq_interface_shaper_get(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  int                       core,
    SOC_SAND_IN  uint32                    tm_port,
    SOC_SAND_OUT uint32                    *if_shaper_rate
  )
{
    uint32 egr_if_id, internal_rate;
    soc_port_t port;
    int is_channelized;

    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_to_local_port_get(unit, core, tm_port, &port));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_channelized_port_get(unit,port, &is_channelized));
    if (is_channelized) {
        SOCDNX_IF_ERR_EXIT(soc_jer_egr_port2egress_offset(unit, core, tm_port, &egr_if_id));
    } else {
        egr_if_id = SOC_DPP_DEFS_GET(unit, non_channelized_cal_id);
    }

    
    SOCDNX_IF_ERR_EXIT(soc_jer_ofp_rates_interface_internal_rate_get(unit, core, egr_if_id, &internal_rate));

    
    SOCDNX_IF_ERR_EXIT(arad_ofp_rates_egq_shaper_rate_from_internal(unit, ARAD_OFP_RATES_EGQ_CAL_CHAN_ARB, internal_rate, if_shaper_rate));

exit:
    SOCDNX_FUNC_RETURN
}


int
soc_jer_ofp_rates_egq_single_port_rate_sw_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core,
    SOC_SAND_IN  uint32                tm_port,
    SOC_SAND_IN  uint32                rate
    )
{
    uint32     base_q_pair, base_q_pair_i, low_priority_cal, high_priority_cal, old_cal, new_cal, tm_port_i, num_required_slots,
               num_required_slots_h, num_required_slots_l, rate_i;
    int        is_channelized, is_single_cal_mode, is_high_priority_port, is_high_priority_port_i, core_i, i;
    soc_port_t port, port_i;
    soc_pbmp_t ports_bm;
    SOC_SAND_OCC_BM_PTR modified_cals_occ;
    soc_port_if_t   interface_type;
    soc_error_t rv;
#ifdef BCM_LB_SUPPORT
    soc_pbmp_t lb_ports;
    uint32     lb_port_num = 0;
    uint32     egress_offset = 0;
    uint32     high_priority_cal_i = INVALID_CALENDAR, low_priority_cal_i = INVALID_CALENDAR;
#endif

    SOCDNX_INIT_FUNC_DEFS

    
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_to_local_port_get(unit, core, tm_port, &port));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_channelized_port_get(unit, port, &is_channelized));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface_type));

    
    if (interface_type == SOC_PORT_IF_ERP) { 
        SOC_EXIT;
    }

    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.modified_channelized_cals_occ.get(unit, core, &modified_cals_occ);
    SOCDNX_IF_ERR_EXIT(rv);


    if (is_channelized) {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_high_priority_cal_get(unit, port, &high_priority_cal));
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_low_priority_cal_get(unit, port, &low_priority_cal));
        SOCDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_egr_queuing_is_high_priority_port_get,(unit, core, tm_port, &is_high_priority_port)));
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_single_cal_mode_get(unit, port, &is_single_cal_mode));
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_ports_to_same_interface_get(unit, port, &ports_bm));        

#ifdef BCM_LB_SUPPORT
        if (SOC_IS_QAX(unit) && SOC_DPP_CONFIG(unit)->qax->lbi_en) {
            SOCDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port2egress_offset, (unit, core, tm_port, &egress_offset)));
            if (egress_offset == SOC_QAX_EGR_IF_LBG_RESERVE) {
                SOCDNX_IF_ERR_EXIT(qax_lb_ports_on_reserve_intf_get(unit, &lb_ports, &lb_port_num));
                SOC_PBMP_ASSIGN(ports_bm, lb_ports);
                SOC_PBMP_ITER(ports_bm, port_i) {
                    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_high_priority_cal_get(unit, port_i, &high_priority_cal_i));
                    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_low_priority_cal_get(unit, port_i, &low_priority_cal_i));
                    if (high_priority_cal == INVALID_CALENDAR)    high_priority_cal = high_priority_cal_i;
                    if (low_priority_cal == INVALID_CALENDAR)     low_priority_cal = low_priority_cal_i;
                }
            }
        }
#endif

        if (is_single_cal_mode) {
            
            if (high_priority_cal == INVALID_CALENDAR && low_priority_cal == INVALID_CALENDAR) {
                
                old_cal = INVALID_CALENDAR;                           
            } else {
                if (high_priority_cal != INVALID_CALENDAR && low_priority_cal != INVALID_CALENDAR && high_priority_cal != low_priority_cal) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("port %d is in single cal mode but has 2 calendars\n"), port));
                }
                
                old_cal = (high_priority_cal != INVALID_CALENDAR) ? high_priority_cal : low_priority_cal;
            }

            
            num_required_slots = 0;
            SOC_PBMP_ITER(ports_bm, port_i) {
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port_i, &tm_port_i, &core_i));
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core_i, tm_port_i,  &base_q_pair_i));
                SOCDNX_SAND_IF_ERR_EXIT(arad_sw_db_egq_port_rate_get(unit, core_i, base_q_pair_i, &rate_i));
                
                if ( ((tm_port_i != tm_port) && (rate_i != 0)) || ((tm_port_i == tm_port) && (rate != 0)) ) {
                    num_required_slots++;
                }
            }

            SOCDNX_IF_ERR_EXIT(soc_jer_ofp_rates_calendar_allocate(unit, core, tm_port, num_required_slots, old_cal, &new_cal));

            
            if (new_cal != INVALID_CALENDAR) {
                SOCDNX_SAND_IF_ERR_EXIT(soc_sand_occ_bm_occup_status_set(unit, modified_cals_occ, new_cal, TRUE));
            }

            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_high_priority_cal_set(unit, port, new_cal));
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_low_priority_cal_set(unit, port, new_cal));

        } else { 

            
            num_required_slots = num_required_slots_h = num_required_slots_l = 0;
            SOC_PBMP_ITER(ports_bm, port_i) {
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port_i, &tm_port_i, &core_i));
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core_i, tm_port_i,  &base_q_pair_i));
                SOCDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_egr_queuing_is_high_priority_port_get,(unit, core_i, tm_port_i, &is_high_priority_port_i)));
                SOCDNX_SAND_IF_ERR_EXIT(arad_sw_db_egq_port_rate_get(unit, core_i, base_q_pair_i, &rate_i));
                
                if ( ((tm_port_i != tm_port) && (rate_i != 0)) || ((tm_port_i == tm_port) && (rate != 0)) ) {
                    if (is_high_priority_port_i) {
                        num_required_slots_h++;
                    } else {
                        num_required_slots_l++;
                    }
                }
            }

            
            for (i = 0; i < SOC_JER_OFP_RATES_NOF_CALS_IN_DUAL_MODE; i++) {

                
                is_high_priority_port = !is_high_priority_port; 
                old_cal = is_high_priority_port ? high_priority_cal : low_priority_cal;
                num_required_slots = is_high_priority_port ? num_required_slots_h : num_required_slots_l;

                SOCDNX_IF_ERR_EXIT(soc_jer_ofp_rates_calendar_allocate(unit, core, tm_port, num_required_slots, old_cal, &new_cal));

                if (is_high_priority_port) {
                    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_high_priority_cal_set(unit, port, new_cal));
                } else {
                    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_low_priority_cal_set(unit, port, new_cal));
                }
                
                if (new_cal != INVALID_CALENDAR) {
                    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_occ_bm_occup_status_set(unit, modified_cals_occ, new_cal, TRUE));
                }
            }
        }
    } else { 
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_high_priority_cal_set(unit, port, SOC_DPP_DEFS_GET(unit, non_channelized_cal_id)));
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_low_priority_cal_set(unit, port, SOC_DPP_DEFS_GET(unit, non_channelized_cal_id)));
        
        SOCDNX_SAND_IF_ERR_EXIT(soc_sand_occ_bm_occup_status_set(unit, modified_cals_occ, SOC_DPP_DEFS_GET(unit, non_channelized_cal_id), TRUE));
    }

    
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, tm_port, &base_q_pair));

    
    SOCDNX_SAND_IF_ERR_EXIT(arad_sw_db_egq_port_rate_set(unit, core, base_q_pair, rate));

    
    SOCDNX_SAND_IF_ERR_EXIT(arad_sw_db_is_port_valid_set(unit, core, base_q_pair, TRUE));

exit:
    SOCDNX_FUNC_RETURN
}

int 
soc_jer_ofp_rates_calendar_allocate(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  int             core,
    SOC_SAND_IN  uint32          tm_port,
    SOC_SAND_IN  uint32          num_required_slots,
    SOC_SAND_IN  uint32          old_cal,
    SOC_SAND_OUT uint32          *new_cal)
{
    SOC_SAND_OCC_BM_PTR cals_occ;
    uint32              num_of_big_calendars, num_of_calendars, small_cal_size;
    uint8               found;
    soc_port_t          port;
    soc_error_t         rv;
    uint32 num_required_slots_final; 
    soc_port_if_t       interface;
    SOCDNX_INIT_FUNC_DEFS

    *new_cal = INVALID_CALENDAR;

    num_of_calendars = SOC_DPP_DEFS_GET(unit, nof_channelized_calendars);
    num_of_big_calendars = SOC_DPP_DEFS_GET(unit, nof_big_channelized_calendars);
    small_cal_size = SOC_DPP_DEFS_GET(unit, small_channelized_cal_size);

    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.channelized_cals_occ.get(unit, core, &cals_occ);
    SOCDNX_IF_ERR_EXIT(rv);
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_to_local_port_get(unit, core, tm_port, &port));

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit,port,&interface));

    
    if((interface == _SHR_PORT_IF_ILKN) && SOC_DPP_CONFIG(unit)->jer->tm.is_ilkn_big_cal && (num_required_slots>0))
    {
        num_required_slots_final = SOC_DPP_DEFS_GET(unit, big_channelized_cal_size);
    } 
    else
    {
        num_required_slots_final = num_required_slots;
    }

    if (old_cal == INVALID_CALENDAR) { 
        if (num_required_slots_final != 0) {
            found = FALSE;
            if (num_required_slots_final <= small_cal_size) { 
                SOCDNX_SAND_IF_ERR_EXIT(soc_sand_occ_bm_get_next_in_range(unit, cals_occ, num_of_big_calendars, (num_of_calendars-2), TRUE, new_cal, &found));  
            } 

            if (!found) { 
                SOCDNX_SAND_IF_ERR_EXIT(soc_sand_occ_bm_get_next_in_range(unit, cals_occ, 0, (num_of_big_calendars -1), TRUE, new_cal, &found));
                if (!found) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_RESOURCE, (_BSL_SOCDNX_MSG("No calendars are left for port %d \n"), port));
                }
            }

            
            SOCDNX_SAND_IF_ERR_EXIT(soc_sand_occ_bm_occup_status_set(unit, cals_occ, *new_cal, TRUE));
        }

    } else { 

        if (num_required_slots_final == 0) {
                SOCDNX_IF_ERR_EXIT(soc_jer_ofp_rates_calendar_deallocate(unit, core, old_cal));
        } else { 
            found = FALSE;
            if (num_required_slots_final <= small_cal_size) { 
                if (old_cal < num_of_big_calendars) { 
                    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_occ_bm_get_next_in_range(unit, cals_occ, num_of_big_calendars, (num_of_calendars-2), TRUE, new_cal, &found));
                    if (!found) {
                        
                        *new_cal = old_cal;
                    }
                } else {
                     *new_cal = old_cal;
                }
            } else { 
                if (old_cal >=  num_of_big_calendars) { 
                    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_occ_bm_get_next_in_range(unit, cals_occ, 0, (num_of_big_calendars -1), TRUE, new_cal, &found));
                    if (!found) {
                        SOCDNX_EXIT_WITH_ERR(SOC_E_RESOURCE, (_BSL_SOCDNX_MSG("No calendars are left for port %d \n"), port));
                    }
                } else {
                    
                    *new_cal = old_cal;
                }
            }

            if (found) { 
                
                SOCDNX_IF_ERR_EXIT(soc_jer_ofp_rates_calendar_deallocate(unit, core, old_cal));
                
                SOCDNX_SAND_IF_ERR_EXIT(soc_sand_occ_bm_occup_status_set(unit, cals_occ, *new_cal, TRUE));
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN
}

int
soc_jer_ofp_rates_calendar_deallocate(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  int             core,
    SOC_SAND_IN  uint32          cal_id)
{
    SOC_SAND_OCC_BM_PTR cals_occ;
    soc_error_t         rv;

    SOCDNX_INIT_FUNC_DEFS

    rv = sw_state_access[unit].dpp.soc.arad.tm.arad_egr_ports.channelized_cals_occ.get(unit, core, &cals_occ);
    SOCDNX_IF_ERR_EXIT(rv);

    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_occ_bm_occup_status_set(unit, cals_occ, cal_id, FALSE));

exit:
    SOCDNX_FUNC_RETURN
}


int
soc_jer_ofp_rates_port2chan_cal_get(
    SOC_SAND_IN  int        unit, 
    SOC_SAND_IN  int        core, 
    SOC_SAND_IN  uint32     tm_port, 
    SOC_SAND_OUT uint32     *calendar)
{
    soc_port_t port;
    int        is_high_priority_port;
#ifdef BCM_LB_SUPPORT
    soc_port_t port_i;
    soc_pbmp_t lb_ports;
    uint32     lb_port_num = 0;
    uint32     egress_offset = 0;
    uint32     high_priority_cal = INVALID_CALENDAR, high_priority_cal_i = INVALID_CALENDAR;
    uint32     low_priority_cal = INVALID_CALENDAR, low_priority_cal_i = INVALID_CALENDAR;
#endif

    SOCDNX_INIT_FUNC_DEFS

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_to_local_port_get(unit, core, tm_port, &port)); 
    SOCDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_egr_queuing_is_high_priority_port_get,(unit, core, tm_port, &is_high_priority_port)));

    if (is_high_priority_port) {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_high_priority_cal_get(unit, port, calendar));
    } else {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_low_priority_cal_get(unit, port, calendar));
    }

#ifdef BCM_LB_SUPPORT
    
    if (SOC_IS_QAX(unit) && SOC_DPP_CONFIG(unit)->qax->lbi_en) {
        SOCDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port2egress_offset, (unit, core, tm_port, &egress_offset)));
        if (egress_offset == SOC_QAX_EGR_IF_LBG_RESERVE) {
            SOCDNX_IF_ERR_EXIT(qax_lb_ports_on_reserve_intf_get(unit, &lb_ports, &lb_port_num));
            SOC_PBMP_ITER(lb_ports, port_i) {
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_high_priority_cal_get(unit, port_i, &high_priority_cal_i));
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_low_priority_cal_get(unit, port_i, &low_priority_cal_i));
                if (high_priority_cal == INVALID_CALENDAR)    high_priority_cal = high_priority_cal_i;
                if (low_priority_cal == INVALID_CALENDAR)     low_priority_cal = low_priority_cal_i;
            }
            *calendar = is_high_priority_port ? high_priority_cal : low_priority_cal;
        }
    }
#endif


exit:
    SOCDNX_FUNC_RETURN
}


int
soc_jer_ofp_rates_retrieve_egress_shaper_reg_field_names(
    SOC_SAND_IN  int                                      unit,
    SOC_SAND_IN  ARAD_OFP_RATES_CAL_INFO                  *cal_info,
    SOC_SAND_IN  ARAD_OFP_RATES_CAL_SET                   cal2set,    
    SOC_SAND_IN  ARAD_OFP_RATES_EGQ_CHAN_ARB_FIELD_TYPE   field_type,
    SOC_SAND_OUT soc_mem_t                                *memory_name,
    SOC_SAND_OUT soc_field_t                              *field_name
  )
{
    SOCDNX_INIT_FUNC_DEFS

    SOCDNX_NULL_CHECK(memory_name);
    SOCDNX_NULL_CHECK(field_name);
    SOCDNX_NULL_CHECK(cal_info);

    if (cal_info->cal_type == ARAD_OFP_RATES_EGQ_CAL_CHAN_ARB || 
        cal_info->cal_type == ARAD_OFP_RATES_EGQ_CAL_PORT_PRIORITY ||
        cal_info->cal_type == ARAD_OFP_RATES_EGQ_CAL_TCG)
    {
        *memory_name = EGQ_EGRESS_SHAPER_CONFIGURATIONm;
        switch (field_type)
        {
        case SOC_TMC_OFP_RATES_EGQ_CHAN_ARB_FIELD_RATE:
            *field_name = (cal2set == ARAD_OFP_RATES_CAL_SET_A) ? CH_SPR_RATE_Af:CH_SPR_RATE_Bf;
            break;
        case SOC_TMC_OFP_RATES_EGQ_CHAN_ARB_FIELD_MAX_BURST:
            *field_name = (cal2set == ARAD_OFP_RATES_CAL_SET_A) ? CH_SPR_MAX_BURST_Af:CH_SPR_MAX_BURST_Bf;      
            break;
        case SOC_TMC_OFP_RATES_EGQ_CHAN_ARB_FIELD_CAL_LEN:
            *field_name = (cal2set == ARAD_OFP_RATES_CAL_SET_A) ? CH_SPR_CAL_LEN_Af:CH_SPR_CAL_LEN_Bf;      
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG_STR( "Illegal enum")));
        }
    } else {
      SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG_STR( "Illegal enum")));
    }

exit:
    SOCDNX_FUNC_RETURN
}


int 
soc_jer_ofp_rates_egress_shaper_mem_field_read (
    SOC_SAND_IN  int                                      unit,
    SOC_SAND_IN  int                                      core,
    SOC_SAND_IN  ARAD_OFP_RATES_CAL_INFO                  *cal_info,   
    SOC_SAND_IN  soc_mem_t                                mem_name,
    SOC_SAND_IN  soc_field_t                              field_name,
    SOC_SAND_OUT uint32                                   *data
    )
{
    soc_reg_above_64_val_t data_above_64;
    uint32 offset = 0;

    SOCDNX_INIT_FUNC_DEFS

    switch (cal_info->cal_type) {
        case SOC_TMC_OFP_RATES_EGQ_CAL_CHAN_ARB:
            offset = cal_info->chan_arb_id;
            break;
        case SOC_TMC_OFP_RATES_EGQ_CAL_TCG:
            offset = JERICHO_OFP_RATES_EGRESS_SHAPER_CONFIG_CAL_ID_TCG;
            break;
        case SOC_TMC_OFP_RATES_EGQ_CAL_PORT_PRIORITY:
            offset = JERICHO_OFP_RATES_EGRESS_SHAPER_CONFIG_CAL_ID_QUEUE_PAIR;
            break;
        default:
            break;
    }

    
    SOCDNX_IF_ERR_EXIT(READ_EGQ_EGRESS_SHAPER_CONFIGURATIONm(unit, EGQ_BLOCK(unit, core), offset, &data_above_64));
    *data = soc_EGQ_EGRESS_SHAPER_CONFIGURATIONm_field32_get(unit, &data_above_64, field_name);

exit:
    SOCDNX_FUNC_RETURN
}

int 
soc_jer_ofp_rates_egress_shaper_mem_field_write (
    SOC_SAND_IN  int                                      unit,
    SOC_SAND_IN  int                                      core,
    SOC_SAND_IN  ARAD_OFP_RATES_CAL_INFO                  *cal_info,   
    SOC_SAND_IN  soc_mem_t                                mem_name,
    SOC_SAND_IN  soc_field_t                              field_name,
    SOC_SAND_OUT uint32                                   data
    )
{
    soc_reg_above_64_val_t data_above_64;
    uint32 offset = 0;

    SOCDNX_INIT_FUNC_DEFS

    switch (cal_info->cal_type) {
        case SOC_TMC_OFP_RATES_EGQ_CAL_CHAN_ARB:
            offset = cal_info->chan_arb_id;
            break;
        case SOC_TMC_OFP_RATES_EGQ_CAL_TCG:
            offset = JERICHO_OFP_RATES_EGRESS_SHAPER_CONFIG_CAL_ID_TCG;
            break;
        case SOC_TMC_OFP_RATES_EGQ_CAL_PORT_PRIORITY:
            offset = JERICHO_OFP_RATES_EGRESS_SHAPER_CONFIG_CAL_ID_QUEUE_PAIR;
            break;
        default:
            break;
    }

    
    SOCDNX_IF_ERR_EXIT(READ_EGQ_EGRESS_SHAPER_CONFIGURATIONm(unit, EGQ_BLOCK(unit, core), offset, &data_above_64));
    
    soc_EGQ_EGRESS_SHAPER_CONFIGURATIONm_field32_set(unit, &data_above_64, field_name, data);
    
    SOCDNX_IF_ERR_EXIT(WRITE_EGQ_EGRESS_SHAPER_CONFIGURATIONm(unit, EGQ_BLOCK(unit, core), offset, &data_above_64));

exit:
    SOCDNX_FUNC_RETURN
}

soc_mem_t
  soc_jer_ofp_rates_egq_scm_chan_arb_id2scm_id(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint32 chan_arb_id
  )
{  
  soc_mem_t
    egq_scm_name;
  
  switch (chan_arb_id)
  {
      case 0:
        egq_scm_name = EGQ_CH_SCM_0m;
        break;
      case 1:
        egq_scm_name = EGQ_CH_SCM_1m;
        break;
      case 2:
        egq_scm_name = EGQ_CH_SCM_2m;
        break;
      case 3:
        egq_scm_name = EGQ_CH_SCM_3m;
        break;
      case 4:
        egq_scm_name = EGQ_CH_SCM_4m;
        break;
      case 5:
        egq_scm_name = EGQ_CH_SCM_5m;
        break;
      case 6:
        egq_scm_name = EGQ_CH_SCM_6m;
        break;
      case 7:
        egq_scm_name = EGQ_CH_SCM_7m;
        break;
      case 8:
        egq_scm_name = EGQ_CH_SCM_8m;
        break;
      case 9:
        egq_scm_name = EGQ_CH_SCM_9m;
        break;
      case 10:
        egq_scm_name = EGQ_CH_SCM_10m;
        break;
      case 11:
        egq_scm_name = EGQ_CH_SCM_11m;
        break;
      case 12:
        egq_scm_name = EGQ_CH_SCM_12m;
        break;
      case 13:
        egq_scm_name = EGQ_CH_SCM_13m;
        break;
      case 14:
        egq_scm_name = EGQ_CH_SCM_14m;
        break;
      case 15:
        egq_scm_name = EGQ_CH_SCM_15m;
        break;
      case 16:
        egq_scm_name = EGQ_CH_SCM_16m;
        break;
      case 17:
        egq_scm_name = EGQ_CH_SCM_17m;
        break;
      case 18:
        egq_scm_name = EGQ_CH_SCM_18m;
        break;
      case 19:
        egq_scm_name = EGQ_CH_SCM_19m;
        break;
      case 20:
        egq_scm_name = EGQ_CH_SCM_20m;
        break;
      case 21:
        egq_scm_name = EGQ_CH_SCM_21m;
        break;
      case 22:
        egq_scm_name = EGQ_CH_SCM_22m;
        break;
      case 23:
        egq_scm_name = EGQ_CH_SCM_23m;
        break;
      case 24:
        egq_scm_name = EGQ_CH_SCM_24m;
        break;
      case 25:
        egq_scm_name = EGQ_CH_SCM_25m;
        break;
      case 26:
        egq_scm_name = EGQ_CH_SCM_26m;
        break;
      case 27:
        egq_scm_name = EGQ_CH_SCM_27m;
        break;
      case 28:
        egq_scm_name = EGQ_CH_SCM_28m;
        break;
      case 29:
        egq_scm_name = EGQ_CH_SCM_29m;
        break;
      case 30:
        egq_scm_name = EGQ_CH_SCM_30m;
        break;
      case 31:
        egq_scm_name = EGQ_CH_SCM_31m;
        break;
      case JERICHO_OFP_RATES_EGRESS_SHAPER_CONFIG_CAL_ID_NON_CHN:
        egq_scm_name = EGQ_NONCH_SCMm;
        break;
      default:
        egq_scm_name = EGQ_CH_0_SCMm;
  }

  return egq_scm_name;
}

int
soc_jer_ofp_rates_egress_shaper_cal_write (
    SOC_SAND_IN  int                                      unit,
    SOC_SAND_IN  int                                      core,
    SOC_SAND_IN  SOC_TMC_OFP_RATES_CAL_INFO              *cal_info,
    SOC_SAND_IN  SOC_TMC_OFP_RATES_CAL_SET                cal2set,    
    SOC_SAND_IN  SOC_TMC_OFP_RATES_EGQ_CHAN_ARB_FIELD_TYPE field_type,
    SOC_SAND_IN  uint32                                   data
    )
{
    soc_field_t field_name;
    soc_mem_t memory_name;

    SOCDNX_INIT_FUNC_DEFS

    
    SOCDNX_IF_ERR_EXIT(soc_jer_ofp_rates_retrieve_egress_shaper_reg_field_names(
                                                                unit,
                                                                cal_info,
                                                                cal2set,    
                                                                field_type,
                                                                &memory_name,
                                                                &field_name));

    
    SOCDNX_IF_ERR_EXIT(soc_jer_ofp_rates_egress_shaper_mem_field_write (
                                                              unit,
                                                              core,
                                                              cal_info,   
                                                              memory_name,
                                                              field_name,
                                                              data));
exit:
    SOCDNX_FUNC_RETURN
}

int
soc_jer_ofp_rates_egress_shaper_cal_read (
    SOC_SAND_IN  int                                      unit,
    SOC_SAND_IN  int                                      core,
    SOC_SAND_IN  SOC_TMC_OFP_RATES_CAL_INFO              *cal_info,
    SOC_SAND_IN  SOC_TMC_OFP_RATES_CAL_SET                cal2set,    
    SOC_SAND_IN  SOC_TMC_OFP_RATES_EGQ_CHAN_ARB_FIELD_TYPE field_type,
    SOC_SAND_OUT uint32                                   *data
    )
{
    soc_field_t field_name;
    soc_mem_t memory_name;

    SOCDNX_INIT_FUNC_DEFS

    
    SOCDNX_IF_ERR_EXIT(soc_jer_ofp_rates_retrieve_egress_shaper_reg_field_names(
                                                                unit,
                                                                cal_info,
                                                                cal2set,    
                                                                field_type,
                                                                &memory_name,
                                                                &field_name));

    
    SOCDNX_IF_ERR_EXIT(soc_jer_ofp_rates_egress_shaper_mem_field_read (
                                                              unit,
                                                              core,
                                                              cal_info,   
                                                              memory_name,
                                                              field_name,
                                                              data));
exit:
    SOCDNX_FUNC_RETURN
}

#undef _ERR_MSG_MODULE_NAME

