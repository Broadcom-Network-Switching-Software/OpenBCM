/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: jer_stat_if.c
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

#include <soc/dpp/JER/jer_stat_if.h>

uint32 soc_jer_stat_if_init_set(int unit, ARAD_INIT_STAT_IF *stat_if)
{
    uint32 reg_val, field_val;    
    int mode;
    uint32 current;

    SOCDNX_INIT_FUNC_DEFS;

    if (stat_if->core_mode == SOC_TMC_SIF_CORE_MODE_DEDICATED)
    {
        field_val = 0;
    }
    else if (stat_if->core_mode == SOC_TMC_SIF_CORE_MODE_COMBINED)
    {
        field_val = 1;
    }
    else
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("Invalid statistics interface core mode\n")));
    }

    SOCDNX_IF_ERR_EXIT(READ_ECI_GLOBAL_GENERAL_CFG_4r(unit, &reg_val));
    soc_reg_field_set(unit, ECI_GLOBAL_GENERAL_CFG_4r, &reg_val, SINGLE_STAT_PORTf, field_val);
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_GLOBAL_GENERAL_CFG_4r(unit, reg_val));

    
    SOCDNX_IF_ERR_EXIT(READ_IQMT_IQMT_INITr(unit, &reg_val));
    soc_reg_field_set(unit,IQMT_IQMT_INITr, &reg_val, SIF_INITf, 0);
    SOCDNX_IF_ERR_EXIT(WRITE_IQMT_IQMT_INITr(unit, reg_val));

    mode = stat_if->stat_if_etpp_mode;
    if (mode != -1)
    {
        if(!SOC_IS_JERICHO_PLUS(unit))
        {
            SOCDNX_IF_ERR_EXIT(READ_EPNI_CFG_PP_LIF_COUNTER_MODEr(unit, SOC_CORE_ALL, &current));
            soc_reg_field_set(unit, EPNI_CFG_PP_LIF_COUNTER_MODEr, &current, CFG_PP_LIF_COUNTER_MODEf, mode);
            SOCDNX_IF_ERR_EXIT(WRITE_EPNI_CFG_PP_LIF_COUNTER_MODEr(unit, SOC_CORE_ALL, current));        
        }
    }

    SOCDNX_IF_ERR_EXIT(soc_jer_stat_if_report_info_set(unit, stat_if));   

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC
uint32 _soc_jer_stat_if_report_info_verify(int unit, ARAD_STAT_IF_REPORT_INFO *info)
{
    int threshold_index;
    uint32 val;
    SOCDNX_INIT_FUNC_DEFS;
    
    if (info->mode == SOC_TMC_STAT_IF_REPORT_QSIZE)
    {
        
        if (info->if_scrubber_queue_max > ARAD_STAT_IF_REPORT_SCRUBBER_QUEUE_MAX ||
            info->if_scrubber_queue_min > info->if_scrubber_queue_max)
        {
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("Invalid scrubber queue range\n")));
        }
        
        if (info->if_qsize_queue_max > JER_STAT_IF_REPORT_QSIZE_QUEUE_MAX ||
            info->if_qsize_queue_min > info->if_qsize_queue_max)
        {
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("Invalid queue range\n")));
        }
        
        for(threshold_index = 0; threshold_index < SOC_TMC_STAT_NOF_THRESHOLD_RANGES; threshold_index++) {
            val = info->if_scrubber_buffer_descr_th[threshold_index];
            if (val != ARAD_STAT_IF_REPORT_THRESHOLD_IGNORED && val > ARAD_STAT_IF_REPORT_DESC_THRESHOLD_MAX) 
            {
                SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("Invalid thresholds value %d \n"), val));                
            }
            val = info->if_scrubber_bdb_th[threshold_index];
            if (val != ARAD_STAT_IF_REPORT_THRESHOLD_IGNORED && val > ARAD_STAT_IF_REPORT_DESC_THRESHOLD_MAX) 
            {
                SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("Invalid thresholds value %d \n"), val));              
            }
            val = info->if_scrubber_buffer_descr_th[threshold_index];
            if (val != ARAD_STAT_IF_REPORT_THRESHOLD_IGNORED && val > ARAD_STAT_IF_REPORT_DESC_THRESHOLD_MAX) 
            {
                SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("Invalid thresholds value %d \n"), val));                  
            }
        }
    }
    
exit:
    SOCDNX_FUNC_RETURN;
}

STATIC uint32 
_jer_stat_if_set_queue_range(int unit, int sif_port,int is_scrubber,uint32 min_queue,uint32 max_queue)
{
    uint64 reg_val64;
    SOCDNX_INIT_FUNC_DEFS;

    if (is_scrubber)
    {
        SOCDNX_IF_ERR_EXIT(READ_IQM_STATISTICS_SCR_QUEUE_RANGEr(unit, sif_port, &reg_val64));
        soc_reg64_field32_set(unit, IQM_STATISTICS_SCR_QUEUE_RANGEr, &reg_val64, ST_SCR_LOW_QNUMf, min_queue);
        soc_reg64_field32_set(unit, IQM_STATISTICS_SCR_QUEUE_RANGEr, &reg_val64, ST_SCR_HIGH_QNUMf, max_queue);
        SOCDNX_IF_ERR_EXIT(WRITE_IQM_STATISTICS_SCR_QUEUE_RANGEr(unit, sif_port, reg_val64));
    }
    else
    {

        SOCDNX_IF_ERR_EXIT(READ_IQM_STATISTICS_REPORT_QUEUE_RANGE_0r(unit, sif_port, &reg_val64));
        soc_reg64_field32_set(unit, IQM_STATISTICS_REPORT_QUEUE_RANGE_0r, &reg_val64, ST_RPT_QUEUE_RANGE_0_LOWf, min_queue);
        soc_reg64_field32_set(unit, IQM_STATISTICS_REPORT_QUEUE_RANGE_0r, &reg_val64, ST_RPT_QUEUE_RANGE_0_HIGHf, max_queue);
        SOCDNX_IF_ERR_EXIT(WRITE_IQM_STATISTICS_REPORT_QUEUE_RANGE_0r(unit, sif_port, reg_val64));

        SOCDNX_IF_ERR_EXIT(READ_IQM_STATISTICS_REPORT_QUEUE_RANGE_1r(unit, sif_port, &reg_val64));
        soc_reg64_field32_set(unit, IQM_STATISTICS_REPORT_QUEUE_RANGE_1r, &reg_val64, ST_RPT_QUEUE_RANGE_1_LOWf, min_queue);
        soc_reg64_field32_set(unit, IQM_STATISTICS_REPORT_QUEUE_RANGE_1r, &reg_val64, ST_RPT_QUEUE_RANGE_1_HIGHf, max_queue);
        SOCDNX_IF_ERR_EXIT(WRITE_IQM_STATISTICS_REPORT_QUEUE_RANGE_1r(unit, sif_port, reg_val64));       
    }
exit:
    SOCDNX_FUNC_RETURN;
}

uint32 soc_jer_stat_if_report_info_set(int unit, ARAD_INIT_STAT_IF *stat_if)
{
    uint32 reg_val_iqmt_stat, reg_val, reg_val2 ,field_val=0, min_machine_clock_value, max_machine_clock_value, limit_prd, mem_val;
    uint64 mem_val64;
    soc_reg_above_64_val_t data_th;
    int enable_per_port = 0, threshold_index, index, i;
    int is_billing_sif_0 = 0, is_billing_sif_1 = 0;
    SOCDNX_INIT_FUNC_DEFS;


    
    
    SOCDNX_IF_ERR_EXIT(_soc_jer_stat_if_report_info_verify(unit, &(stat_if->stat_if_info[0])));
    SOCDNX_IF_ERR_EXIT(READ_IQMT_STATISTICS_REPORT_PORT_0r(unit, &reg_val_iqmt_stat));
    switch (stat_if->stat_if_info[0].mode)
    {
        case SOC_TMC_STAT_IF_REPORT_MODE_BILLING:
        case SOC_TMC_STAT_IF_REPORT_MODE_BILLING_INGRESS:
            enable_per_port = 1;
            is_billing_sif_0 = 1;
            
            soc_reg_field_set(unit, IQMT_STATISTICS_REPORT_PORT_0r, &reg_val_iqmt_stat, STP_0_ST_RPT_MODEf, 0);
            
            field_val = (FALSE == (stat_if->stat_if_info[0].is_idle_reports_present)) ? ARAD_STAT_IF_REPORT_BILLING_NO_IDLE_PERIOD:ARAD_STAT_IF_REPORT_BILLING_MAX_IDLE_PERIOD;
            soc_reg_field_set(unit, IQMT_STATISTICS_REPORT_PORT_0r, &reg_val_iqmt_stat, STP_0_ST_EMPTY_RPT_PRDf, field_val);
             
            
            field_val = (stat_if->stat_if_info[0].mode == SOC_TMC_STAT_IF_REPORT_MODE_BILLING_INGRESS)? 1:0;
            SOCDNX_IF_ERR_EXIT(READ_IQMT_STATISTICS_GENERAL_CONFIGUARTIONr(unit, &reg_val));
            soc_reg_field_set(unit, IQMT_STATISTICS_GENERAL_CONFIGUARTIONr, &reg_val, DEVICE_0_ST_BILL_EGR_SRC_SELf, field_val);

            soc_reg_field_set(unit, IQMT_STATISTICS_GENERAL_CONFIGUARTIONr, &reg_val, IQM_0_ST_BILL_ING_ENQ_ENf, 1);
          
            if (stat_if->stat_if_info[0].mode == SOC_TMC_STAT_IF_REPORT_MODE_BILLING_INGRESS)
            {
                soc_reg_field_set(unit, IQMT_STATISTICS_GENERAL_CONFIGUARTIONr, &reg_val, IQM_0_ST_BILL_ING_DEQ_ENf, 1);
            }
            else
            {
                soc_reg_field_set(unit, IQMT_STATISTICS_GENERAL_CONFIGUARTIONr, &reg_val, EPNI_0_ST_BILL_EGR_ENf, 1);               
            }
            
            if (stat_if->stat_if_info[0].if_billing_filter_reports_egress)
            {
                SOCDNX_IF_ERR_EXIT(READ_EPNI_STATISTICS_REPORTING_CONFIGURATIONr(unit, 0, &reg_val2));
                soc_reg_field_set(unit, EPNI_STATISTICS_REPORTING_CONFIGURATIONr, &reg_val2, STATISTICS_CONDITIONAL_REPORTING_CONFIGURATIONf, 0);
                SOCDNX_IF_ERR_EXIT(WRITE_EPNI_STATISTICS_REPORTING_CONFIGURATIONr(unit, 0, reg_val2));
                SOCDNX_IF_ERR_EXIT(READ_EPNI_GLOBAL_CONFIGr(unit, 0, &reg_val2));
                soc_reg_field_set(unit, EPNI_GLOBAL_CONFIGr, &reg_val2, STATISTICS_ENAf, 0xD);
                SOCDNX_IF_ERR_EXIT(WRITE_EPNI_GLOBAL_CONFIGr(unit, 0, reg_val2));
                soc_reg_field_set(unit, IQMT_STATISTICS_GENERAL_CONFIGUARTIONr, &reg_val, EPNI_0_ST_BILL_EGR_COND_RPT_ENf, 1);
            }            
  
            SOCDNX_IF_ERR_EXIT(WRITE_IQMT_STATISTICS_GENERAL_CONFIGUARTIONr(unit, reg_val));

            SOCDNX_IF_ERR_EXIT(READ_IQM_STATISTICS_BILL_REPORT_CONFIGURATIONr(unit, 0, &reg_val));
            
            soc_reg_field_set(unit, IQM_STATISTICS_BILL_REPORT_CONFIGURATIONr, &reg_val, ST_RPT_MC_ONCEf, stat_if->stat_if_info[0].if_report_mc_once);
            
            soc_reg_field_set(unit, IQM_STATISTICS_BILL_REPORT_CONFIGURATIONr, &reg_val, ST_BILL_STAMP_QNUMf, stat_if->stat_if_info[0].if_billing_ingress_queue_stamp);
            
            soc_reg_field_set(unit, IQM_STATISTICS_BILL_REPORT_CONFIGURATIONr, &reg_val, ST_BILL_STAMP_DROP_REASONf, stat_if->stat_if_info[0].if_billing_ingress_drop_reason);
            
            if (stat_if->stat_if_info[0].if_billing_filter_reports_ingress)
            {
                soc_reg_field_set(unit, IQM_STATISTICS_BILL_REPORT_CONFIGURATIONr, &reg_val, ST_BILL_INGR_FILTER_DISf, 0);
            }
            SOCDNX_IF_ERR_EXIT(WRITE_IQM_STATISTICS_BILL_REPORT_CONFIGURATIONr(unit, 0, reg_val));  
            
            SOCDNX_IF_ERR_EXIT(READ_IQMT_STATISTICS_REPORT_PORT_0r(unit, &reg_val));
            soc_reg_field_set(unit, IQMT_STATISTICS_REPORT_PORT_0r, &reg_val, STP_0_ST_RPT_SIZEf, 0);
            SOCDNX_IF_ERR_EXIT(WRITE_IQMT_STATISTICS_REPORT_PORT_0r(unit, reg_val));
                                  
            break;

        case SOC_TMC_STAT_IF_REPORT_QSIZE:
            
            soc_reg_field_set(unit, IQMT_STATISTICS_REPORT_PORT_0r, &reg_val_iqmt_stat, STP_0_ST_RPT_MODEf, 1);
            
            field_val = (FALSE == (stat_if->stat_if_info[0].is_idle_reports_present)) ? ARAD_STAT_IF_REPORT_BILLING_NO_IDLE_PERIOD:ARAD_STAT_IF_REPORT_BILLING_MAX_IDLE_PERIOD;
            soc_reg_field_set(unit, IQMT_STATISTICS_REPORT_PORT_0r, &reg_val_iqmt_stat, STP_0_ST_EMPTY_RPT_PRDf, field_val);
            
            field_val = (FALSE == (stat_if->stat_if_info[0].is_idle_reports_present)) ? ARAD_STAT_IF_REPORT_QSIZE_NO_IDLE_PERIOD:ARAD_STAT_IF_REPORT_QSIZE_MAX_IDLE_PERIOD;
            SOCDNX_IF_ERR_EXIT(READ_IQMT_STATISTICS_GENERAL_CONFIGUARTIONr(unit, &reg_val));
            soc_reg_field_set(unit, IQMT_STATISTICS_GENERAL_CONFIGUARTIONr, &reg_val, IQM_0_ST_QSZ_RPT_PRDf, field_val);
            SOCDNX_IF_ERR_EXIT(WRITE_IQMT_STATISTICS_GENERAL_CONFIGUARTIONr(unit, reg_val));

            
            if(stat_if->stat_if_info[0].report_size == SOC_TMC_STAT_IF_REPORT_SIZE_61b)
            {
                field_val = 1;
            }
            else if(stat_if->stat_if_info[0].report_size == SOC_TMC_STAT_IF_REPORT_SIZE_64b)
            {
                field_val = 0;
            }
            else
            {
                SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("Invalid report size mode %d "), stat_if->stat_if_info[0].report_size));            
            }
            soc_reg_field_set(unit, IQMT_STATISTICS_REPORT_PORT_0r, &reg_val_iqmt_stat, STP_0_ST_RPT_SIZEf, field_val);

            
            SOCDNX_IF_ERR_EXIT(_jer_stat_if_set_queue_range(unit, 0, 1, stat_if->stat_if_info[0].if_scrubber_queue_min, stat_if->stat_if_info[0].if_scrubber_queue_max));
            
            SOCDNX_SAND_IF_ERR_EXIT(arad_chip_time_to_ticks(unit, stat_if->stat_if_info[0].if_scrubber_rate_min, TRUE, 1, TRUE, &min_machine_clock_value));
            SOCDNX_SAND_IF_ERR_EXIT(arad_chip_time_to_ticks(unit, stat_if->stat_if_info[0].if_scrubber_rate_max, TRUE, 1, TRUE, &max_machine_clock_value));
            
            SOCDNX_IF_ERR_EXIT(READ_IQMT_IQM_0_SCR_PRDr(unit, &reg_val));
            soc_reg_field_set(unit, IQMT_IQM_0_SCR_PRDr, &reg_val, IQM_0_ST_SCR_MIN_PRDf, min_machine_clock_value/2);
            soc_reg_field_set(unit, IQMT_IQM_0_SCR_PRDr, &reg_val, IQM_0_ST_SCR_MAX_PRDf, max_machine_clock_value/2);
            SOCDNX_IF_ERR_EXIT(WRITE_IQMT_IQM_0_SCR_PRDr(unit, reg_val));
            
            for(threshold_index = 0; threshold_index < ARAD_STAT_IF_REPORT_THRESHOLD_LINES; threshold_index++) 
            {
                SOCDNX_IF_ERR_EXIT(READ_IQM_SCRBUFFTHm(unit, IQM_BLOCK(unit, 0), threshold_index, data_th));
                
                field_val = (ARAD_STAT_IF_REPORT_THRESHOLD_IGNORED == (stat_if->stat_if_info[0].if_scrubber_buffer_descr_th[threshold_index])) ? ARAD_STAT_IF_REPORT_DESC_THRESHOLD_MAX :stat_if->stat_if_info[0].if_scrubber_buffer_descr_th[threshold_index];
                soc_mem_field32_set(unit, IQM_SCRBUFFTHm, data_th, SCR_BD_THf, field_val);

                
                field_val = (ARAD_STAT_IF_REPORT_THRESHOLD_IGNORED == (stat_if->stat_if_info[0].if_scrubber_bdb_th[threshold_index])) ? ARAD_STAT_IF_REPORT_BDB_THRESHOLD_MAX :stat_if->stat_if_info[0].if_scrubber_bdb_th[threshold_index];
                soc_mem_field32_set(unit, IQM_SCRBUFFTHm, data_th, SCR_BDB_THf, field_val);

                
                field_val = (ARAD_STAT_IF_REPORT_THRESHOLD_IGNORED == (stat_if->stat_if_info[0].if_scrubber_uc_dram_buffer_th[threshold_index])) ? ARAD_STAT_IF_REPORT_UC_DRAM_THRESHOLD_MAX :stat_if->stat_if_info[0].if_scrubber_uc_dram_buffer_th[threshold_index];
                soc_mem_field32_set(unit, IQM_SCRBUFFTHm, data_th, SCR_MNMC_DB_THf, field_val);

                SOCDNX_IF_ERR_EXIT(WRITE_IQM_SCRBUFFTHm(unit, IQM_BLOCK(unit, 0), threshold_index, data_th));   
            }

            
            SOCDNX_IF_ERR_EXIT(_jer_stat_if_set_queue_range(unit, 0, 0, stat_if->stat_if_info[0].if_qsize_queue_min, stat_if->stat_if_info[0].if_qsize_queue_max));
            break;
            case SOC_TMC_STAT_IF_REPORT_MODE_NOT_CONFIGURE:               
                LOG_VERBOSE(BSL_LS_SOC_STAT,(BSL_META_U(unit, "stat_if mode for core #0 wasn't configured \n")));
            break;            
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("Invalid statistics interface mode %d "), stat_if->stat_if_info[0].mode));
            break;
            
    }
    
    switch (stat_if->stat_if_info[0].if_nof_reports_per_packet)
    {
        case SOC_TMC_STAT_IF_NOF_REPORTS_8:
            field_val = 0;
            break;
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
    soc_reg_field_set(unit, IQMT_STATISTICS_REPORT_PORT_0r, &reg_val_iqmt_stat, STP_0_ST_RPT_PKT_SIZEf, field_val);
    SOCDNX_IF_ERR_EXIT(WRITE_IQMT_STATISTICS_REPORT_PORT_0r(unit, reg_val_iqmt_stat)); 

    
    SOCDNX_SAND_IF_ERR_EXIT(arad_stat_if_rate_limit_prd_get(unit, &(stat_if->stat_if_info[0]), &limit_prd));
    SOCDNX_IF_ERR_EXIT(READ_IQMT_ST_RATE_LIMIT_PRDr(unit, &reg_val));
    soc_reg_field_set(unit, IQMT_ST_RATE_LIMIT_PRDr, &reg_val, IQM_0_ST_RATE_LIMIT_PRDf, limit_prd);
    SOCDNX_IF_ERR_EXIT(WRITE_IQMT_ST_RATE_LIMIT_PRDr(unit, reg_val));


    
    
    SOCDNX_IF_ERR_EXIT(_soc_jer_stat_if_report_info_verify(unit, &(stat_if->stat_if_info[1])));
    SOCDNX_IF_ERR_EXIT(READ_IQMT_STATISTICS_REPORT_PORT_1r(unit, &reg_val_iqmt_stat));
    switch (stat_if->stat_if_info[1].mode)
    {
        case SOC_TMC_STAT_IF_REPORT_MODE_BILLING:
        case SOC_TMC_STAT_IF_REPORT_MODE_BILLING_INGRESS:
            enable_per_port = 1;
            is_billing_sif_1 = 1;
            
            soc_reg_field_set(unit, IQMT_STATISTICS_REPORT_PORT_1r, &reg_val_iqmt_stat, STP_1_ST_RPT_MODEf, 0);
            
            field_val = (FALSE == (stat_if->stat_if_info[1].is_idle_reports_present)) ? ARAD_STAT_IF_REPORT_BILLING_NO_IDLE_PERIOD:ARAD_STAT_IF_REPORT_BILLING_MAX_IDLE_PERIOD;
            soc_reg_field_set(unit, IQMT_STATISTICS_REPORT_PORT_1r, &reg_val_iqmt_stat, STP_1_ST_EMPTY_RPT_PRDf, field_val);

            
            field_val = (stat_if->stat_if_info[1].mode == SOC_TMC_STAT_IF_REPORT_MODE_BILLING_INGRESS)? 1:0;
            SOCDNX_IF_ERR_EXIT(READ_IQMT_STATISTICS_GENERAL_CONFIGUARTIONr(unit, &reg_val));
            soc_reg_field_set(unit, IQMT_STATISTICS_GENERAL_CONFIGUARTIONr, &reg_val, DEVICE_1_ST_BILL_EGR_SRC_SELf, field_val);

            soc_reg_field_set(unit, IQMT_STATISTICS_GENERAL_CONFIGUARTIONr, &reg_val, IQM_1_ST_BILL_ING_ENQ_ENf, 1);
          
            if (stat_if->stat_if_info[1].mode == SOC_TMC_STAT_IF_REPORT_MODE_BILLING_INGRESS)
            {
                soc_reg_field_set(unit, IQMT_STATISTICS_GENERAL_CONFIGUARTIONr, &reg_val, IQM_1_ST_BILL_ING_DEQ_ENf, 1);
            }
            else
            {
                soc_reg_field_set(unit, IQMT_STATISTICS_GENERAL_CONFIGUARTIONr, &reg_val, EPNI_1_ST_BILL_EGR_ENf, 1);               
            }     
            
            if (stat_if->stat_if_info[1].if_billing_filter_reports_egress)
            {
                SOCDNX_IF_ERR_EXIT(READ_EPNI_STATISTICS_REPORTING_CONFIGURATIONr(unit, 1, &reg_val2));
                soc_reg_field_set(unit, EPNI_STATISTICS_REPORTING_CONFIGURATIONr, &reg_val2, STATISTICS_CONDITIONAL_REPORTING_CONFIGURATIONf, 0);
                SOCDNX_IF_ERR_EXIT(WRITE_EPNI_STATISTICS_REPORTING_CONFIGURATIONr(unit, 1, reg_val2));
                SOCDNX_IF_ERR_EXIT(READ_EPNI_GLOBAL_CONFIGr(unit, 1, &reg_val2));
                soc_reg_field_set(unit, EPNI_GLOBAL_CONFIGr, &reg_val2, STATISTICS_ENAf, 0xD);
                SOCDNX_IF_ERR_EXIT(WRITE_EPNI_GLOBAL_CONFIGr(unit, 1, reg_val2));
                soc_reg_field_set(unit, IQMT_STATISTICS_GENERAL_CONFIGUARTIONr, &reg_val, EPNI_1_ST_BILL_EGR_COND_RPT_ENf, 1);                    
            }
            
            SOCDNX_IF_ERR_EXIT(WRITE_IQMT_STATISTICS_GENERAL_CONFIGUARTIONr(unit, reg_val));

            SOCDNX_IF_ERR_EXIT(READ_IQM_STATISTICS_BILL_REPORT_CONFIGURATIONr(unit, 1, &reg_val));
            
            soc_reg_field_set(unit, IQM_STATISTICS_BILL_REPORT_CONFIGURATIONr, &reg_val, ST_RPT_MC_ONCEf, stat_if->stat_if_info[1].if_report_mc_once);
            
            soc_reg_field_set(unit, IQM_STATISTICS_BILL_REPORT_CONFIGURATIONr, &reg_val, ST_BILL_STAMP_QNUMf, stat_if->stat_if_info[1].if_billing_ingress_queue_stamp);
            
            soc_reg_field_set(unit, IQM_STATISTICS_BILL_REPORT_CONFIGURATIONr, &reg_val, ST_BILL_STAMP_DROP_REASONf, stat_if->stat_if_info[1].if_billing_ingress_drop_reason);
            
            if (stat_if->stat_if_info[1].if_billing_filter_reports_ingress)
            {
                soc_reg_field_set(unit, IQM_STATISTICS_BILL_REPORT_CONFIGURATIONr, &reg_val, ST_BILL_INGR_FILTER_DISf, 0);
            }
            SOCDNX_IF_ERR_EXIT(WRITE_IQM_STATISTICS_BILL_REPORT_CONFIGURATIONr(unit, 1, reg_val)); 
            
            SOCDNX_IF_ERR_EXIT(READ_IQMT_STATISTICS_REPORT_PORT_1r(unit, &reg_val));
            soc_reg_field_set(unit, IQMT_STATISTICS_REPORT_PORT_1r, &reg_val, STP_1_ST_RPT_SIZEf, 0);
            SOCDNX_IF_ERR_EXIT(WRITE_IQMT_STATISTICS_REPORT_PORT_1r(unit, reg_val));
                                       
            break;

        case SOC_TMC_STAT_IF_REPORT_QSIZE:
            
            soc_reg_field_set(unit, IQMT_STATISTICS_REPORT_PORT_1r, &reg_val_iqmt_stat, STP_1_ST_RPT_MODEf, 1);
            
            field_val = (FALSE == (stat_if->stat_if_info[1].is_idle_reports_present)) ? ARAD_STAT_IF_REPORT_BILLING_NO_IDLE_PERIOD:ARAD_STAT_IF_REPORT_BILLING_MAX_IDLE_PERIOD;
            soc_reg_field_set(unit, IQMT_STATISTICS_REPORT_PORT_1r, &reg_val_iqmt_stat, STP_1_ST_EMPTY_RPT_PRDf, field_val);
            
            field_val = (FALSE == (stat_if->stat_if_info[1].is_idle_reports_present)) ? ARAD_STAT_IF_REPORT_QSIZE_NO_IDLE_PERIOD:ARAD_STAT_IF_REPORT_QSIZE_MAX_IDLE_PERIOD;
            SOCDNX_IF_ERR_EXIT(READ_IQMT_STATISTICS_GENERAL_CONFIGUARTIONr(unit, &reg_val));
            soc_reg_field_set(unit, IQMT_STATISTICS_GENERAL_CONFIGUARTIONr, &reg_val, IQM_1_ST_QSZ_RPT_PRDf, field_val);
            SOCDNX_IF_ERR_EXIT(WRITE_IQMT_STATISTICS_GENERAL_CONFIGUARTIONr(unit, reg_val));

            
            if(stat_if->stat_if_info[1].report_size == SOC_TMC_STAT_IF_REPORT_SIZE_61b)
            {
                field_val = 1;
            }
            else if(stat_if->stat_if_info[1].report_size == SOC_TMC_STAT_IF_REPORT_SIZE_64b)
            {
                field_val = 0;
            }
            else
            {
                SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("Invalid report size mode %d "), stat_if->stat_if_info[1].report_size));            
            }
            soc_reg_field_set(unit, IQMT_STATISTICS_REPORT_PORT_1r, &reg_val_iqmt_stat, STP_1_ST_RPT_SIZEf, field_val);

            
            SOCDNX_IF_ERR_EXIT(_jer_stat_if_set_queue_range(unit, 1, 1, stat_if->stat_if_info[1].if_scrubber_queue_min, stat_if->stat_if_info[1].if_scrubber_queue_max));
            
            SOCDNX_SAND_IF_ERR_EXIT(arad_chip_time_to_ticks(unit, stat_if->stat_if_info[1].if_scrubber_rate_min, TRUE, 1, TRUE, &min_machine_clock_value));
            SOCDNX_SAND_IF_ERR_EXIT(arad_chip_time_to_ticks(unit, stat_if->stat_if_info[1].if_scrubber_rate_max, TRUE, 1, TRUE, &max_machine_clock_value));
            
            SOCDNX_IF_ERR_EXIT(READ_IQMT_IQM_1_SCR_PRDr(unit, &reg_val));
            soc_reg_field_set(unit, IQMT_IQM_1_SCR_PRDr, &reg_val, IQM_1_ST_SCR_MIN_PRDf, min_machine_clock_value/2);
            soc_reg_field_set(unit, IQMT_IQM_1_SCR_PRDr, &reg_val, IQM_1_ST_SCR_MAX_PRDf, max_machine_clock_value/2);
            SOCDNX_IF_ERR_EXIT(WRITE_IQMT_IQM_1_SCR_PRDr(unit, reg_val));
            
            for(threshold_index = 0; threshold_index < ARAD_STAT_IF_REPORT_THRESHOLD_LINES; threshold_index++) 
            {
                SOCDNX_IF_ERR_EXIT(READ_IQM_SCRBUFFTHm(unit, IQM_BLOCK(unit, 1), threshold_index, data_th));
                
                field_val = (ARAD_STAT_IF_REPORT_THRESHOLD_IGNORED == (stat_if->stat_if_info[1].if_scrubber_buffer_descr_th[threshold_index])) ? ARAD_STAT_IF_REPORT_DESC_THRESHOLD_MAX :stat_if->stat_if_info[1].if_scrubber_buffer_descr_th[threshold_index];
                soc_mem_field32_set(unit, IQM_SCRBUFFTHm, data_th, SCR_BD_THf, field_val);

                
                field_val = (ARAD_STAT_IF_REPORT_THRESHOLD_IGNORED == (stat_if->stat_if_info[1].if_scrubber_bdb_th[threshold_index])) ? ARAD_STAT_IF_REPORT_BDB_THRESHOLD_MAX :stat_if->stat_if_info[1].if_scrubber_bdb_th[threshold_index];
                soc_mem_field32_set(unit, IQM_SCRBUFFTHm, data_th, SCR_BDB_THf, field_val);

                
                field_val = (ARAD_STAT_IF_REPORT_THRESHOLD_IGNORED == (stat_if->stat_if_info[1].if_scrubber_uc_dram_buffer_th[threshold_index])) ? ARAD_STAT_IF_REPORT_UC_DRAM_THRESHOLD_MAX :stat_if->stat_if_info[1].if_scrubber_uc_dram_buffer_th[threshold_index];
                soc_mem_field32_set(unit, IQM_SCRBUFFTHm, data_th, SCR_MNMC_DB_THf, field_val);

                SOCDNX_IF_ERR_EXIT(WRITE_IQM_SCRBUFFTHm(unit, IQM_BLOCK(unit, 1), threshold_index, data_th));   
            }

            
            SOCDNX_IF_ERR_EXIT(_jer_stat_if_set_queue_range(unit, 1, 0, stat_if->stat_if_info[1].if_qsize_queue_min, stat_if->stat_if_info[1].if_qsize_queue_max));
            break;
        case SOC_TMC_STAT_IF_REPORT_MODE_NOT_CONFIGURE:               
            LOG_VERBOSE(BSL_LS_SOC_STAT,(BSL_META_U(unit, "stat_if mode for core #1 wasn't configured \n")));
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("Invalid statistics interface mode %d "), stat_if->stat_if_info[1].mode));
            break;
    }
    
    switch (stat_if->stat_if_info[1].if_nof_reports_per_packet)
    {
        case SOC_TMC_STAT_IF_NOF_REPORTS_8:
            field_val = 0;
            break;
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
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("Invalid statistics interface mode %d "), stat_if->stat_if_info[1].mode));
            break;
    }
    soc_reg_field_set(unit, IQMT_STATISTICS_REPORT_PORT_1r, &reg_val_iqmt_stat, STP_1_ST_RPT_PKT_SIZEf, field_val);
    SOCDNX_IF_ERR_EXIT(WRITE_IQMT_STATISTICS_REPORT_PORT_1r(unit, reg_val_iqmt_stat)); 

    
    SOCDNX_SAND_IF_ERR_EXIT(arad_stat_if_rate_limit_prd_get(unit, &(stat_if->stat_if_info[1]), &limit_prd));
    SOCDNX_IF_ERR_EXIT(READ_IQMT_ST_RATE_LIMIT_PRDr(unit, &reg_val));
    soc_reg_field_set(unit, IQMT_ST_RATE_LIMIT_PRDr, &reg_val, IQM_1_ST_RATE_LIMIT_PRDf, limit_prd);
    SOCDNX_IF_ERR_EXIT(WRITE_IQMT_ST_RATE_LIMIT_PRDr(unit, reg_val));


    
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
                        SOCDNX_IF_ERR_EXIT(READ_EGQ_PCTm(unit, EGQ_BLOCK(unit, core), curr_q_pair, data));
                        soc_mem_field32_set(unit, EGQ_PCTm, data, STATISTICS_ENf, 1);
                        SOCDNX_IF_ERR_EXIT(WRITE_EGQ_PCTm(unit, EGQ_BLOCK(unit,core), curr_q_pair, data));
                    }
                }
            }
        }
    }

    

    if (is_billing_sif_0 || is_billing_sif_1)
    {
        SOCDNX_IF_ERR_EXIT(READ_IDR_STATIC_CONFIGURATIONr(unit, &reg_val));
        soc_reg_field_set(unit, IDR_STATIC_CONFIGURATIONr, &reg_val, HEADER_DELTA_FOR_CRPS_NEGATEf, 1);
        SOCDNX_IF_ERR_EXIT(WRITE_IDR_STATIC_CONFIGURATIONr(unit, reg_val));

        SOCDNX_IF_ERR_EXIT(READ_IQMT_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, &reg_val));
        soc_reg_field_set(unit, IQMT_ENABLE_DYNAMIC_MEMORY_ACCESSr, &reg_val, ENABLE_DYNAMIC_MEMORY_ACCESSf, 1);
        SOCDNX_IF_ERR_EXIT(WRITE_IQMT_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, reg_val));
        for (index = 0; index < 256; index++)
        {
            SOCDNX_IF_ERR_EXIT(READ_IQMT_ING_RPT_PCMm(unit,IQMT_BLOCK(unit),index,&mem_val));
            if (is_billing_sif_0)
            {
                soc_mem_field32_set(unit, IQMT_ING_RPT_PCMm, &mem_val, IQM_0_ING_CMP_PROFILEf, 0);
            }
            if (is_billing_sif_1)
            {
                soc_mem_field32_set(unit, IQMT_ING_RPT_PCMm, &mem_val, IQM_1_ING_CMP_PROFILEf, 0);
            }
            SOCDNX_IF_ERR_EXIT(WRITE_IQMT_ING_RPT_PCMm(unit, IQMT_BLOCK(unit), index, &mem_val));
        }

        SOCDNX_IF_ERR_EXIT(READ_IQMT_ING_RPT_CPMm(unit, IQMT_BLOCK(unit), 0, &mem_val));
        soc_mem_field32_set(unit, IQMT_ING_RPT_CPMm, &mem_val, IQM_ING_PROFILE_CMPf, 0x2);
        SOCDNX_IF_ERR_EXIT(WRITE_IQMT_ING_RPT_CPMm(unit, IQMT_BLOCK(unit), 0, &mem_val));

        SOCDNX_IF_ERR_EXIT(READ_IQMT_RPT_PKTSIZE_COMPENSATION_SETTINGr(unit, &reg_val));
        if (is_billing_sif_0)
        {
            soc_reg_field_set(unit, IQMT_RPT_PKTSIZE_COMPENSATION_SETTINGr, &reg_val, ING_0_PKTSIZE_CMP_BITMAPf, 0x9);
        }
        if (is_billing_sif_1)
        {
            soc_reg_field_set(unit, IQMT_RPT_PKTSIZE_COMPENSATION_SETTINGr, &reg_val, ING_1_PKTSIZE_CMP_BITMAPf, 0x9);
        }
        SOCDNX_IF_ERR_EXIT(WRITE_IQMT_RPT_PKTSIZE_COMPENSATION_SETTINGr(unit, reg_val));

        for (i = is_billing_sif_0? 0:1 ; i <=  is_billing_sif_1? 1:0; i++)
        {
            for (index = 0 ; index < 256 ;index++)
            {
                SOCDNX_IF_ERR_EXIT(READ_EPNI_PP_COUNTER_TABLEm(unit, EPNI_BLOCK(unit, i), index, &mem_val64));
                soc_mem_field32_set(unit, EPNI_PP_COUNTER_TABLEm, &mem_val64, COUNTER_COMPENSATIONf, 0x2);
                SOCDNX_IF_ERR_EXIT(WRITE_EPNI_PP_COUNTER_TABLEm(unit, EPNI_BLOCK(unit, i), index, &mem_val64));
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t soc_jer_stat_if_queue_range_set(int unit, int core, int is_scrubber, int min_queue, int max_queue)
{
    SOCDNX_INIT_FUNC_DEFS;
    
    if (min_queue < 0 || 
        max_queue < min_queue || 
        (is_scrubber && (max_queue > ARAD_STAT_IF_REPORT_SCRUBBER_QUEUE_MAX)) || 
        ((!is_scrubber) && (max_queue > JER_STAT_IF_REPORT_QSIZE_QUEUE_MAX)))
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid queue range\n")));
    }

    SOCDNX_IF_ERR_EXIT(_jer_stat_if_set_queue_range(unit, core, is_scrubber, min_queue, max_queue));

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t soc_jer_stat_if_queue_range_get(int unit, int sif_port, int is_scrubber, int* min_queue, int* max_queue)
{
    uint64 reg_val64;
    SOCDNX_INIT_FUNC_DEFS;
    if (is_scrubber)
    {
        SOCDNX_IF_ERR_EXIT(READ_IQM_STATISTICS_SCR_QUEUE_RANGEr(unit, sif_port, &reg_val64));
        *min_queue = soc_reg64_field32_get(unit, IQM_STATISTICS_SCR_QUEUE_RANGEr, reg_val64, ST_SCR_LOW_QNUMf);
        *max_queue = soc_reg64_field32_get(unit, IQM_STATISTICS_SCR_QUEUE_RANGEr, reg_val64, ST_SCR_HIGH_QNUMf);
    }
    else
    {
        SOCDNX_IF_ERR_EXIT(READ_IQM_STATISTICS_REPORT_QUEUE_RANGE_0r(unit, sif_port, &reg_val64));
        *min_queue = soc_reg64_field32_get(unit, IQM_STATISTICS_REPORT_QUEUE_RANGE_0r, reg_val64, ST_RPT_QUEUE_RANGE_0_LOWf);
        *max_queue = soc_reg64_field32_get(unit, IQM_STATISTICS_REPORT_QUEUE_RANGE_0r, reg_val64, ST_RPT_QUEUE_RANGE_0_HIGHf);   
    }
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t soc_jer_stat_if_billing_filter_egress_port_set(int unit, int egr_port, int enable)
{
    uint32 flags, base_q_pair, tm_port, nof_pairs, curr_q_pair;
    int core;
    soc_reg_above_64_val_t data;
    ARAD_MGMT_INIT* init;
    ARAD_INIT_STAT_IF *stat_if;
    int enable_per_port = 0;
    SOCDNX_INIT_FUNC_DEFS;

    init = &(SOC_DPP_CONFIG(unit)->arad->init);
    stat_if = &(init->stat_if);

    if (stat_if->stat_if_info[0].mode == SOC_TMC_STAT_IF_REPORT_MODE_BILLING ||
        stat_if->stat_if_info[0].mode == SOC_TMC_STAT_IF_REPORT_MODE_BILLING_INGRESS ||
        stat_if->stat_if_info[1].mode == SOC_TMC_STAT_IF_REPORT_MODE_BILLING ||
        stat_if->stat_if_info[1].mode == SOC_TMC_STAT_IF_REPORT_MODE_BILLING_INGRESS ) {
        enable_per_port = 1;
    }
    if (init->stat_if.stat_if_enable && enable_per_port)
    {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, egr_port, &flags));
        if (!(SOC_PORT_IS_ELK_INTERFACE(flags) || SOC_PORT_IS_STAT_INTERFACE(flags)))
        {
            if (stat_if->stat_if_info[0].stat_if_port_enable[egr_port])
            {
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, egr_port, &tm_port, &core));
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, tm_port, &base_q_pair));
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_port_to_out_port_priority_get(unit, core, tm_port, &nof_pairs));
                for (curr_q_pair = base_q_pair; curr_q_pair - base_q_pair < nof_pairs; curr_q_pair++)
                {
                    SOCDNX_IF_ERR_EXIT(READ_EGQ_PCTm(unit, EGQ_BLOCK(unit, core), curr_q_pair, data));
                    soc_mem_field32_set(unit, EGQ_PCTm, data, STATISTICS_ENf, enable);
                    SOCDNX_IF_ERR_EXIT(WRITE_EGQ_PCTm(unit, EGQ_BLOCK(unit,core), curr_q_pair, data));
                }
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

#undef _ERR_MSG_MODULE_NAME

