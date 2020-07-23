/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: qax_stat_if.c
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_COSQ

#include <shared/bsl.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/port_sw_db.h>
#include <soc/dcmn/dcmn_mem.h>
#include <soc/dpp/ARAD/arad_stat_if.h>
#include <soc/dpp/QAX/qax_stat_if.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>

uint32 soc_qax_stat_if_init_set(int unit, ARAD_INIT_STAT_IF *stat_if)
{
    uint32 reg_val;
    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_IF_ERR_EXIT(READ_CGM_INITr(unit, SOC_CORE_ALL, &reg_val));
    soc_reg_field_set(unit,CGM_INITr, &reg_val, STAT_IF_INITf, 0);
    SOCDNX_IF_ERR_EXIT(WRITE_CGM_INITr(unit, SOC_CORE_ALL, reg_val));

    SOCDNX_IF_ERR_EXIT(soc_qax_stat_if_report_info_set(unit, 1, stat_if));
    
exit:
    SOCDNX_FUNC_RETURN;
}

STATIC
uint32 _soc_qax_stat_if_report_info_verify(int unit, ARAD_STAT_IF_REPORT_INFO *info)
{
    int threshold_index;
    uint32 val;
    int max_queue_size = SOC_IS_QUX(unit) ?
                         QUX_REPORT_QUEUE_SIZE_MAX : QAX_REPORT_QUEUE_SIZE_MAX;
    SOCDNX_INIT_FUNC_DEFS;
    
    if (info->mode == SOC_TMC_STAT_IF_REPORT_QSIZE)
    {
        
        if (info->if_scrubber_queue_max > max_queue_size ||
            info->if_scrubber_queue_min > info->if_scrubber_queue_max)
        {
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("Invalid scrubber queue range\n")));
        }
        
        if (info->if_qsize_queue_max > max_queue_size ||
            info->if_qsize_queue_min > info->if_qsize_queue_max)
        {
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("Invalid queue range\n")));
        }
        
        for(threshold_index = 0; threshold_index < SOC_TMC_STAT_NOF_THRESHOLD_RANGES; threshold_index++) {
            val = info->if_scrubber_buffer_descr_th[threshold_index];
            if (val != ARAD_STAT_IF_REPORT_THRESHOLD_IGNORED && val > QAX_STAT_IF_REPORT_DESC_THRESHOLD_MAX) 
            {
                SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("Invalid thresholds value %d \n"), val));                
            }
            val = info->if_scrubber_bdb_th[threshold_index];
            if (val != ARAD_STAT_IF_REPORT_THRESHOLD_IGNORED && val > QAX_STAT_IF_REPORT_DESC_THRESHOLD_MAX) 
            {
                SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("Invalid thresholds value %d \n"), val));              
            }
            val = info->if_scrubber_buffer_descr_th[threshold_index];
            if (val != ARAD_STAT_IF_REPORT_THRESHOLD_IGNORED && val > QAX_STAT_IF_REPORT_DESC_THRESHOLD_MAX) 
            {
                SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("Invalid thresholds value %d \n"), val));                  
            }
        }
    }
    
exit:
    SOCDNX_FUNC_RETURN;
}

STATIC uint32 
_qax_stat_if_set_queue_range(int unit, int sif_port,int is_scrubber,uint32 min_queue,uint32 max_queue)
{
    uint64 reg_val64;
    SOCDNX_INIT_FUNC_DEFS;

    if (is_scrubber)
    {
        SOCDNX_IF_ERR_EXIT(READ_CGM_ST_QSIZE_SCRB_SETTINGSr(unit, &reg_val64));
        soc_reg64_field32_set(unit, CGM_ST_QSIZE_SCRB_SETTINGSr, &reg_val64, ST_QSIZE_SCRB_QNUM_LOWf, min_queue);
        soc_reg64_field32_set(unit, CGM_ST_QSIZE_SCRB_SETTINGSr, &reg_val64, ST_QSIZE_SCRB_QNUM_HIGHf, max_queue);
        SOCDNX_IF_ERR_EXIT(WRITE_CGM_ST_QSIZE_SCRB_SETTINGSr(unit, reg_val64));
    }
    else
    {

        SOCDNX_IF_ERR_EXIT(READ_CGM_ST_QNUM_RANGE_FILTERr(unit, &reg_val64));
        soc_reg64_field32_set(unit, CGM_ST_QNUM_RANGE_FILTERr, &reg_val64, ST_QNUM_FILTER_LOW_TH_0f, min_queue);
        soc_reg64_field32_set(unit, CGM_ST_QNUM_RANGE_FILTERr, &reg_val64, ST_QNUM_FILTER_HIGH_TH_0f, max_queue);
        soc_reg64_field32_set(unit, CGM_ST_QNUM_RANGE_FILTERr, &reg_val64, ST_QNUM_FILTER_LOW_TH_1f, min_queue);
        soc_reg64_field32_set(unit, CGM_ST_QNUM_RANGE_FILTERr, &reg_val64, ST_QNUM_FILTER_HIGH_TH_1f, max_queue);
        SOCDNX_IF_ERR_EXIT(WRITE_CGM_ST_QNUM_RANGE_FILTERr(unit, reg_val64));       
    }
exit:
    SOCDNX_FUNC_RETURN;
}

uint32 soc_qax_stat_if_report_info_set(int unit, int nof_sif_ports, ARAD_INIT_STAT_IF *stat_if)
{
    uint32 reg_val_cgm_stat_port_setting, reg_val_cgm_ing_stat_setting, reg_val, reg_val2 ,field_val=0, min_machine_clock_value, max_machine_clock_value, limit_prd, mem_val;
    uint64 mem_val64;
    soc_reg_above_64_val_t data_th;
    int enable_per_port = 0, threshold_index, index;
    int is_billing_sif_0 = 0;
    uint32 mnt, expo;
    uint64 reg_val64;

    SOCDNX_INIT_FUNC_DEFS;
    
    
    SOCDNX_IF_ERR_EXIT(_soc_qax_stat_if_report_info_verify(unit, &(stat_if->stat_if_info[0])));
    SOCDNX_IF_ERR_EXIT(READ_CGM_ST_PORT_SETTINGSr(unit, &reg_val_cgm_stat_port_setting));
    SOCDNX_IF_ERR_EXIT(READ_CGM_ST_ING_BILL_SETTINGSr(unit, &reg_val_cgm_ing_stat_setting));    
    switch (stat_if->stat_if_info[0].mode)
    {
        case SOC_TMC_STAT_IF_REPORT_MODE_BILLING:
        case SOC_TMC_STAT_IF_REPORT_MODE_BILLING_INGRESS:
            enable_per_port = 1;
            is_billing_sif_0 = 1;
            
            if(stat_if->stat_if_info[0].mode == SOC_TMC_STAT_IF_REPORT_MODE_BILLING)
            {
                soc_reg_field_set(unit, CGM_ST_PORT_SETTINGSr, &reg_val_cgm_stat_port_setting, ST_PORT_RPRT_MODEf, 0);            
            }
            else
            {
                soc_reg_field_set(unit, CGM_ST_PORT_SETTINGSr, &reg_val_cgm_stat_port_setting, ST_PORT_RPRT_MODEf, 2);            
            }

            
            field_val = (FALSE == (stat_if->stat_if_info[0].is_idle_reports_present)) ? ARAD_STAT_IF_REPORT_BILLING_NO_IDLE_PERIOD : ARAD_STAT_IF_REPORT_BILLING_MAX_IDLE_PERIOD;
            soc_reg_field_set(unit, CGM_ST_PORT_SETTINGSr, &reg_val_cgm_stat_port_setting, ST_PORT_EMPTY_RPRT_PRDf, field_val);
              
            if (stat_if->stat_if_info[0].if_billing_filter_reports_egress)
            {
                SOCDNX_IF_ERR_EXIT(READ_EPNI_STATISTICS_REPORTING_CONFIGURATIONr(unit, 0, &reg_val2));
                soc_reg_field_set(unit, EPNI_STATISTICS_REPORTING_CONFIGURATIONr, &reg_val2, STATISTICS_CONDITIONAL_REPORTING_CONFIGURATIONf, 0);
                SOCDNX_IF_ERR_EXIT(WRITE_EPNI_STATISTICS_REPORTING_CONFIGURATIONr(unit, 0, reg_val2));
            }

            
            soc_reg_field_set(unit, CGM_ST_ING_BILL_SETTINGSr, &reg_val_cgm_ing_stat_setting, ST_ING_BILL_MC_ONCEf, stat_if->stat_if_info[0].if_report_mc_once);
            
            soc_reg_field_set(unit, CGM_ST_ING_BILL_SETTINGSr, &reg_val_cgm_ing_stat_setting, ST_ING_BILL_STAMP_QNUMf, stat_if->stat_if_info[0].if_billing_ingress_queue_stamp);
            
            soc_reg_field_set(unit, CGM_ST_ING_BILL_SETTINGSr, &reg_val_cgm_ing_stat_setting, ST_ING_BILL_STAMP_RJCT_REASONf, stat_if->stat_if_info[0].if_billing_ingress_drop_reason);
            
            if (stat_if->stat_if_info[0].if_billing_filter_reports_ingress)
            {
                soc_reg_field_set(unit, CGM_ST_ING_BILL_SETTINGSr, &reg_val_cgm_ing_stat_setting, ST_ING_BILL_PP_FILTER_ENf, 1);
            }
            soc_reg_field_set(unit, CGM_ST_PORT_SETTINGSr, &reg_val_cgm_stat_port_setting, ST_PORT_RPRT_SIZEf, 0);                      

            
            for (index = 0 ; index < 256 ;index++)
            {
                SOCDNX_IF_ERR_EXIT(READ_EPNI_PP_COUNTER_TABLEm(unit, SOC_CORE_ALL, index, &mem_val64));
                soc_mem_field32_set(unit, EPNI_PP_COUNTER_TABLEm, &mem_val64, COUNTER_COMPENSATIONf, 0x2);
                soc_mem_field32_set(unit, EPNI_PP_COUNTER_TABLEm, &mem_val64, COUNTER_MODE_2f, stat_if->stat_if_info[0].if_etpp_counter_mode[0]);
                soc_mem_field32_set(unit, EPNI_PP_COUNTER_TABLEm, &mem_val64, COUNTER_MODE_3f, stat_if->stat_if_info[0].if_etpp_counter_mode[1]);        
                SOCDNX_IF_ERR_EXIT(WRITE_EPNI_PP_COUNTER_TABLEm(unit, SOC_CORE_ALL, index, &mem_val64));
            }                                  
            break;

        case SOC_TMC_STAT_IF_REPORT_QSIZE:
            
            soc_reg_field_set(unit, CGM_ST_PORT_SETTINGSr, &reg_val_cgm_stat_port_setting, ST_PORT_RPRT_MODEf, 1);
            
            field_val = (FALSE == (stat_if->stat_if_info[0].is_idle_reports_present)) ? ARAD_STAT_IF_REPORT_QSIZE_NO_IDLE_PERIOD : ARAD_STAT_IF_REPORT_QSIZE_MAX_IDLE_PERIOD;
            SOCDNX_IF_ERR_EXIT(READ_CGM_ST_QSIZE_RPRT_PRDr(unit, &reg_val));
            soc_reg_field_set(unit, CGM_ST_QSIZE_RPRT_PRDr, &reg_val, ST_QSIZE_RPRT_PRDf, field_val);
            SOCDNX_IF_ERR_EXIT(WRITE_CGM_ST_QSIZE_RPRT_PRDr(unit, reg_val));

            
            soc_reg_field_set(unit, CGM_ST_PORT_SETTINGSr, &reg_val_cgm_stat_port_setting, ST_PORT_RPRT_SIZEf, stat_if->stat_if_info[0].report_size);

            
            SOCDNX_IF_ERR_EXIT(_qax_stat_if_set_queue_range(unit, 0, 1, stat_if->stat_if_info[0].if_scrubber_queue_min, stat_if->stat_if_info[0].if_scrubber_queue_max));
            
            SOCDNX_SAND_IF_ERR_EXIT(arad_chip_time_to_ticks(unit, stat_if->stat_if_info[0].if_scrubber_rate_min, TRUE, 1, TRUE, &min_machine_clock_value));
            SOCDNX_SAND_IF_ERR_EXIT(arad_chip_time_to_ticks(unit, stat_if->stat_if_info[0].if_scrubber_rate_max, TRUE, 1, TRUE, &max_machine_clock_value));
            
            SOCDNX_IF_ERR_EXIT(READ_CGM_ST_QSIZE_SCRB_SETTINGSr(unit, &reg_val64));
            soc_reg64_field32_set(unit, CGM_ST_QSIZE_SCRB_SETTINGSr, &reg_val64, ST_QSIZE_SCRB_MIN_PRDf, min_machine_clock_value/2);
            soc_reg64_field32_set(unit, CGM_ST_QSIZE_SCRB_SETTINGSr, &reg_val64, ST_QSIZE_SCRB_MAX_PRDf, max_machine_clock_value/2);
            
            soc_reg64_field32_set(unit, CGM_ST_QSIZE_SCRB_SETTINGSr, &reg_val64, ST_QSIZE_SCRB_ENf, ((stat_if->stat_if_info[0].if_scrubber_rate_max == 0) ? FALSE : TRUE));
            SOCDNX_IF_ERR_EXIT(WRITE_CGM_ST_QSIZE_SCRB_SETTINGSr(unit, reg_val64));

            
            
            for(threshold_index = 0; threshold_index < ARAD_STAT_IF_REPORT_THRESHOLD_LINES; threshold_index++) 
            {
                SOCDNX_IF_ERR_EXIT(READ_CGM_STAT_SCRB_THm(unit, SOC_CORE_ALL, threshold_index, data_th));
                
                SOCDNX_SAND_IF_ERR_EXIT(soc_sand_break_to_mnt_exp_round_up(stat_if->stat_if_info[0].if_scrubber_sram_pdbs_th[threshold_index], QAX_SCRUBBER_TRESHOLDS_MANTISSA_NOF_BITS, QAX_SCRUBBER_TRESHOLDS_EXP_NOF_BITS, 0, &mnt, &expo));
                arad_iqm_mantissa_exponent_set(unit, mnt, expo, QAX_SCRUBBER_TRESHOLDS_MANTISSA_NOF_BITS, &field_val);                
                field_val = (ARAD_STAT_IF_REPORT_THRESHOLD_IGNORED == (stat_if->stat_if_info[0].if_scrubber_sram_pdbs_th[threshold_index])) ? QAX_STAT_IF_REPORT_THRESHOLD_IGNORED_VALUE : field_val;
                soc_mem_field32_set(unit, CGM_STAT_SCRB_THm, data_th, SRAM_PDBSf, field_val);

                
                SOCDNX_SAND_IF_ERR_EXIT(soc_sand_break_to_mnt_exp_round_up(stat_if->stat_if_info[0].if_scrubber_bdb_th[threshold_index], QAX_SCRUBBER_TRESHOLDS_MANTISSA_NOF_BITS, QAX_SCRUBBER_TRESHOLDS_EXP_NOF_BITS, 0, &mnt, &expo));
                arad_iqm_mantissa_exponent_set(unit, mnt, expo, QAX_SCRUBBER_TRESHOLDS_MANTISSA_NOF_BITS, &field_val);                               
                field_val = (ARAD_STAT_IF_REPORT_THRESHOLD_IGNORED == (stat_if->stat_if_info[0].if_scrubber_bdb_th[threshold_index])) ? QAX_STAT_IF_REPORT_THRESHOLD_IGNORED_VALUE : field_val;
                soc_mem_field32_set(unit, CGM_STAT_SCRB_THm, data_th, DRAM_BDBSf, field_val);

                
                SOCDNX_SAND_IF_ERR_EXIT(soc_sand_break_to_mnt_exp_round_up(stat_if->stat_if_info[0].if_scrubber_sram_buffers_th[threshold_index], QAX_SCRUBBER_TRESHOLDS_MANTISSA_NOF_BITS, QAX_SCRUBBER_TRESHOLDS_EXP_NOF_BITS, 0, &mnt, &expo));
                arad_iqm_mantissa_exponent_set(unit, mnt, expo, QAX_SCRUBBER_TRESHOLDS_MANTISSA_NOF_BITS, &field_val);                                             
                field_val = (ARAD_STAT_IF_REPORT_THRESHOLD_IGNORED == (stat_if->stat_if_info[0].if_scrubber_sram_buffers_th[threshold_index])) ? QAX_STAT_IF_REPORT_THRESHOLD_IGNORED_VALUE : field_val;
                soc_mem_field32_set(unit, CGM_STAT_SCRB_THm, data_th, SRAM_BUFFERSf, field_val);

                SOCDNX_IF_ERR_EXIT(WRITE_CGM_STAT_SCRB_THm(unit, SOC_CORE_ALL, threshold_index, data_th));   
            }

            
            SOCDNX_IF_ERR_EXIT(_qax_stat_if_set_queue_range(unit, 0, 0, stat_if->stat_if_info[0].if_qsize_queue_min, stat_if->stat_if_info[0].if_qsize_queue_max));
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("Invalid statistics interface mode %d "), stat_if->stat_if_info[0].mode));
            break;
            
    }
    
    switch (stat_if->stat_if_info[0].if_nof_reports_per_packet)
    {
        case SOC_TMC_STAT_IF_NOF_REPORTS_16:
            field_val = 1;
            break;
        case SOC_TMC_STAT_IF_NOF_REPORTS_32:
            field_val = 2;
            break;
        case SOC_TMC_STAT_IF_NOF_REPORTS_64:
            field_val = 3;
            break;
        case SOC_TMC_STAT_IF_NOF_REPORTS_128:
            field_val = 4;
            break;
        default:
            
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("Invalid statistics number of reports mode %d "), stat_if->stat_if_info[0].if_nof_reports_per_packet));
            break;
    }
    soc_reg_field_set(unit, CGM_ST_PORT_SETTINGSr, &reg_val_cgm_stat_port_setting, ST_PORT_PKT_SIZEf, field_val);
    SOCDNX_IF_ERR_EXIT(WRITE_CGM_ST_PORT_SETTINGSr(unit, reg_val_cgm_stat_port_setting)); 
    SOCDNX_IF_ERR_EXIT(WRITE_CGM_ST_ING_BILL_SETTINGSr(unit, reg_val_cgm_ing_stat_setting));  

    
    SOCDNX_SAND_IF_ERR_EXIT(arad_stat_if_rate_limit_prd_get(unit, &(stat_if->stat_if_info[0]), &limit_prd));
    SOCDNX_IF_ERR_EXIT(READ_CGM_ST_PORT_FC_SETTINGSr(unit, &reg_val));
    soc_reg_field_set(unit, CGM_ST_PORT_FC_SETTINGSr, &reg_val, ST_PORT_FC_PRDf, limit_prd);
    SOCDNX_IF_ERR_EXIT(WRITE_CGM_ST_PORT_FC_SETTINGSr(unit, reg_val));

    
    if (enable_per_port)
    {
        soc_pbmp_t stat_port_bm;
        int i, core;
        uint32 flags, base_q_pair, tm_port, nof_pairs, curr_q_pair;
        soc_reg_above_64_val_t data;
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_valid_ports_get(unit, SOC_PORT_FLAGS_NETWORK_INTERFACE, &stat_port_bm));
        SOC_PBMP_OR(stat_port_bm, PBMP_RCY_ALL(unit));
        PBMP_ITER(stat_port_bm, i)
        { 
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, i, &flags));
            if (!(SOC_PORT_IS_ELK_INTERFACE(flags) || SOC_PORT_IS_STAT_INTERFACE(flags)))
            {
                if (stat_if->stat_if_info[0].stat_if_port_enable[i])
                {
                    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, i, &tm_port, &core));
                    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, tm_port, &base_q_pair));
                    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_port_to_out_port_priority_get(unit, core, tm_port, &nof_pairs));
                    for (curr_q_pair = base_q_pair; curr_q_pair - base_q_pair < nof_pairs; curr_q_pair++)
                    {
                        SOCDNX_IF_ERR_EXIT(READ_EGQ_PCTm(unit, SOC_CORE_ALL, curr_q_pair, data));
                        soc_mem_field32_set(unit, EGQ_PCTm, data, STATISTICS_ENf, 1);
                        SOCDNX_IF_ERR_EXIT(WRITE_EGQ_PCTm(unit, SOC_CORE_ALL, curr_q_pair, data));
                    }
                }
            }
        }
    }

    

    if (is_billing_sif_0)
    {    
        SOCDNX_IF_ERR_EXIT(READ_CGM_ENABLE_DYNAMIC_MEMORY_ACCESSr_REG32(unit, SOC_CORE_ALL, &reg_val));
        soc_reg_field_set(unit, CGM_ENABLE_DYNAMIC_MEMORY_ACCESSr, &reg_val, ENABLE_DYNAMIC_MEMORY_ACCESSf, 1);
        SOCDNX_IF_ERR_EXIT(WRITE_CGM_ENABLE_DYNAMIC_MEMORY_ACCESSr_REG32(unit, SOC_CORE_ALL, reg_val));
        
        
        for (index = 0; index < 256; index++)
        {
            SOCDNX_IF_ERR_EXIT(READ_CGM_IPP_HCPm(unit,SOC_CORE_ALL,index,&mem_val));
            soc_mem_field32_set(unit, CGM_IPP_HCPm, &mem_val, STAT_PROFILEf, 0);
            SOCDNX_IF_ERR_EXIT(WRITE_CGM_IPP_HCPm(unit, SOC_CORE_ALL, index, &mem_val));
        }
        for (index = 0; index < 64; index++)
        {
            SOCDNX_IF_ERR_EXIT(READ_CGM_VOQ_HCPm(unit,SOC_CORE_ALL,index,&mem_val));
            soc_mem_field32_set(unit, CGM_VOQ_HCPm, &mem_val, STAT_PROFILEf, 0);
            SOCDNX_IF_ERR_EXIT(WRITE_CGM_VOQ_HCPm(unit, SOC_CORE_ALL, index, &mem_val));
        }        
        
        
        
                
        SOCDNX_IF_ERR_EXIT(READ_CGM_STAT_CPMm(unit, SOC_CORE_ALL, 0, &mem_val));
        soc_mem_field32_set(unit, CGM_STAT_CPMm, &mem_val, HDR_DELTAf, 0x4);
        SOCDNX_IF_ERR_EXIT(WRITE_CGM_STAT_CPMm(unit, SOC_CORE_ALL, 0, &mem_val));

        for(index = 0; index <32; index++)
        {
            SOCDNX_IF_ERR_EXIT(READ_CGM_STAT_HCMm(unit, SOC_CORE_ALL, index, &mem_val));
            soc_mem_field32_set(unit, CGM_STAT_HCMm, &mem_val, MASKf, 0x6);
            SOCDNX_IF_ERR_EXIT(WRITE_CGM_STAT_HCMm(unit, SOC_CORE_ALL, index, &mem_val));        
        }
        
        for (index = 0 ; index < 256 ;index++)
        {
            SOCDNX_IF_ERR_EXIT(READ_EPNI_PP_COUNTER_TABLEm(unit, SOC_CORE_ALL, index, &mem_val64));
            soc_mem_field32_set(unit, EPNI_PP_COUNTER_TABLEm, &mem_val64, COUNTER_COMPENSATIONf, 0x2);         
            SOCDNX_IF_ERR_EXIT(WRITE_EPNI_PP_COUNTER_TABLEm(unit, SOC_CORE_ALL, index, &mem_val64));
        }
    }

    
    SOCDNX_IF_ERR_EXIT(READ_CGM_ST_QSIZE_RPRT_QSIZE_SELr(unit, &reg_val));
    soc_reg_field_set(unit, CGM_ST_QSIZE_RPRT_QSIZE_SELr, &reg_val, ST_QSIZE_RPRT_QSIZE_SELf, 0x1);
    SOCDNX_IF_ERR_EXIT(WRITE_CGM_ST_QSIZE_RPRT_QSIZE_SELr(unit, reg_val));    
    
exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t soc_qax_stat_if_queue_range_set(int unit, int core, int is_scrubber, int min_queue, int max_queue)
{
    int max_queue_size = SOC_IS_QUX(unit) ?
                         QUX_REPORT_QUEUE_SIZE_MAX : QAX_REPORT_QUEUE_SIZE_MAX;
    SOCDNX_INIT_FUNC_DEFS;
    
    if ((min_queue < 0) || (max_queue < min_queue) || (max_queue > max_queue_size))        
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid queue range\n")));
    }

    SOCDNX_IF_ERR_EXIT(_qax_stat_if_set_queue_range(unit, core, is_scrubber, min_queue, max_queue));

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t soc_qax_stat_if_queue_range_get(int unit, int sif_port, int is_scrubber, int* min_queue, int* max_queue)
{
    uint64 reg_val64;
    SOCDNX_INIT_FUNC_DEFS;
    if (is_scrubber)
    {
        SOCDNX_IF_ERR_EXIT(READ_CGM_ST_QSIZE_SCRB_SETTINGSr(unit, &reg_val64));
        *min_queue = soc_reg64_field32_get(unit, CGM_ST_QSIZE_SCRB_SETTINGSr, reg_val64, ST_QSIZE_SCRB_QNUM_LOWf);
        *max_queue = soc_reg64_field32_get(unit, CGM_ST_QSIZE_SCRB_SETTINGSr, reg_val64, ST_QSIZE_SCRB_QNUM_HIGHf);
    }
    else
    {
        SOCDNX_IF_ERR_EXIT(READ_CGM_ST_QNUM_RANGE_FILTERr(unit, &reg_val64));
        *min_queue = soc_reg64_field32_get(unit, CGM_ST_QNUM_RANGE_FILTERr, reg_val64, ST_QNUM_FILTER_LOW_TH_0f);
        *max_queue = soc_reg64_field32_get(unit, CGM_ST_QNUM_RANGE_FILTERr, reg_val64, ST_QNUM_FILTER_HIGH_TH_0f);   
    }
exit:
    SOCDNX_FUNC_RETURN;
}


#undef _ERR_MSG_MODULE_NAME

