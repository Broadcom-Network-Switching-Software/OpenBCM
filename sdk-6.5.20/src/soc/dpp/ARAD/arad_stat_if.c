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

#define _ERR_MSG_MODULE_NAME BSL_SOC_STAT



#include <shared/bsl.h>
#include <soc/mem.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>

#include <soc/dpp/ARAD/NIF/common_drv.h>
#include <soc/dpp/ARAD/arad_api_framework.h>
#include <soc/dpp/ARAD/arad_framework.h>
#include <soc/dpp/ARAD/arad_api_stat_if.h>
#include <soc/dpp/ARAD/arad_stat_if.h>
#include <soc/dpp/ARAD/arad_reg_access.h>
#include <soc/dpp/ARAD/arad_nif.h>
#include <soc/dpp/ARAD/arad_general.h>
#include <soc/dpp/ARAD/arad_sw_db.h>

#include <soc/dpp/port_sw_db.h>

#include <soc/dpp/drv.h>

#include <bcm_int/dpp/error.h>







#define ARAD_STAT_IF_REPORT_PKT_SIZE_MAX                         (SOC_TMC_NOF_STAT_IF_PKT_SIZES -1)
#define ARAD_STAT_IF_REPORT_BILLING_MODE_VALUE                   0
#define ARAD_STAT_IF_REPORT_QSIZE_MODE_VALUE                     1
#define ARAD_STAT_IF_REPORT_BILLING_ORIGINAL_PACKET_SIZE_DISABLE 0
#define ARAD_STAT_IF_REPORT_BILLING_ORIGINAL_PACKET_SIZE_ENABLE  1
#define ARAD_STAT_IF_REPORT_PREAMBLE_BYTES_NUM                   1
#define ARAD_STAT_IF_REPORT_BITS_IN_BYTES                        8






















uint32
 arad_stat_if_rate_limit_prd_get(
     SOC_SAND_IN  int                  unit,
     SOC_SAND_IN  ARAD_STAT_IF_REPORT_INFO   *info,
     SOC_SAND_OUT uint32                   *limit_prd_val
  )
{
    uint32 
     packet_size ,
     records_in_packet,
     if_rate_mbps,
     max_machine_clock_value,
     packet_rate, record_rate;
     SOC_TMC_STAT_IF_PACKET_INFO  
        billing_packet_info[SOC_TMC_NOF_STAT_IF_PKT_SIZES] = { { 68, 8 }, { 132, 16 }, { 260, 32 }, { 516, 64}, { 1028, 128} },
        qsize_packet_info[SOC_TMC_NOF_STAT_IF_PKT_SIZES] = { { 65, 8 }, { 126, 16 }, { 248, 32 }, { 492, 64 }, { 0, 0 } };
     uint8    
      found ;
     uint32
         is_hg,
         ipg;
     soc_error_t
         rv;
     soc_pbmp_t 
         stat_port_bm;
     soc_port_t
         port;
     int core_clock_speed;
     soc_dpp_config_arad_t 
         *dpp_arad;
        
       SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_STAT_IF_RATE_LIMIT_PRD_GET);
       
       SOC_SAND_CHECK_DRIVER_AND_DEVICE;
       SOC_SAND_CHECK_NULL_INPUT(info);
       SOC_SAND_CHECK_NULL_INPUT(limit_prd_val);

       if ((!SOC_IS_JERICHO(unit)) && info->if_pkt_size == SOC_TMC_NOF_STAT_IF_PKT_SIZES) {
           SOC_SAND_SET_ERROR_CODE(ARAD_STAT_IF_REPORT_PKT_SIZE_OUT_OF_RANGE_ERR, 5, exit);
       }
       
       
       if (SOC_IS_JERICHO(unit))
       {
           packet_size = info->if_nof_reports_per_packet * 8;
           if (info->report_size == SOC_TMC_STAT_IF_REPORT_SIZE_61b)
           {
               packet_size -= ((info->if_nof_reports_per_packet)/8)*3;
           }
       }
       else
       {
           packet_size = ((SOC_TMC_STAT_IF_REPORT_MODE_BILLING == info->mode) || (SOC_TMC_STAT_IF_REPORT_MODE_BILLING_QUEUE_NUMBER == info->mode)) ? billing_packet_info[info->if_pkt_size].packet_size : qsize_packet_info[info->if_pkt_size].packet_size;
       }

       
       if (SOC_IS_JERICHO(unit))
       {
           records_in_packet = info->if_nof_reports_per_packet;
       }
       else
       {
           records_in_packet = ((SOC_TMC_STAT_IF_REPORT_MODE_BILLING == info->mode) || (SOC_TMC_STAT_IF_REPORT_MODE_BILLING_QUEUE_NUMBER == info->mode)) ? billing_packet_info[info->if_pkt_size].records_in_packet : qsize_packet_info[info->if_pkt_size].records_in_packet;
       }
           
       if (info->rate == 0x0) {
           
           rv = soc_port_sw_db_valid_ports_get(unit, SOC_PORT_FLAGS_STAT_INTERFACE, &stat_port_bm);
           if(SOC_FAILURE(rv)) {
               SOC_SAND_SET_ERROR_CODE(ARAD_STAT_RATE_NOT_FOUND_ERR, 15, exit);
           }
           
           found = FALSE;
           SOC_PBMP_ITER(stat_port_bm, port){
               found = TRUE;
               break;
           }
           if (!found) {
               SOC_SAND_SET_ERROR_CODE(ARAD_STAT_RATE_NOT_FOUND_ERR, 19, exit);
           }
           
           
           rv = soc_port_sw_db_is_hg_get(unit, port, &is_hg);  
           if (SOC_FAILURE(rv)) {
               SOC_SAND_CHECK_FUNC_RESULT(ARAD_STAT_RATE_NOT_FOUND_ERR, 30, exit);
           }
          
           if(is_hg) {
               packet_size += ARAD_STAT_IF_REPORT_PREAMBLE_BYTES_NUM ;
           }
                      
            
           ipg = (is_hg ? NIF_AVERAGE_IPG_HIGIG : NIF_AVERAGE_IPG_DEFAULT);
           packet_size += ipg;                                 
       
           
           rv = soc_port_sw_db_interface_rate_get(unit, port, &if_rate_mbps);
           if(SOC_FAILURE(rv)) {
               SOC_SAND_CHECK_FUNC_RESULT(ARAD_STAT_RATE_NOT_FOUND_ERR, 20, exit);
           }
       } else {
           if_rate_mbps = info->rate;
       }
       
       
       packet_rate = if_rate_mbps / (packet_size * ARAD_STAT_IF_REPORT_BITS_IN_BYTES);
      
       
       record_rate = packet_rate * records_in_packet * 2;

       
       dpp_arad = SOC_DPP_CONFIG(unit)->arad;
       core_clock_speed = dpp_arad->init.core_freq.frequency/1000;
       
        if(record_rate >= core_clock_speed) {
            *limit_prd_val = 0;
        } else {
            
            max_machine_clock_value = (record_rate / (core_clock_speed - record_rate));
            if (max_machine_clock_value < 2) {
                max_machine_clock_value = 2;
            }

            *limit_prd_val = max_machine_clock_value;
        }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ports_regs_init()",0,0);       

}





uint32
  arad_stat_if_init(
    SOC_SAND_IN  int                 unit
  )
{
  uint32 res;
  int rv;
  soc_port_t port;
  soc_port_if_t interface;
  soc_pbmp_t stat_port_bm;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_MGMT_FUNCTIONAL_INIT);

  

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  5,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_STATISTICS_TAG_CONFIGURATIONr, SOC_CORE_ALL, 0, STAT_TAG_IN_PP_PORT_LSBf,  0x0));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_STATISTICS_TAG_CONFIGURATIONr, SOC_CORE_ALL, 0, STAT_TAG_ENf,  FALSE));

  
  if (!SOC_IS_ARDON(unit)) {
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_modify(unit, IQM_STATISTICS_REPORT_CONFIGURATIONSr, REG_PORT_ANY, 0, ST_BILL_STTAG_SELf,  FALSE)); 
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  22,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_STATISTICS_REPORT_CONFIGURATIONSr, REG_PORT_ANY, 0, ST_BILL_RPT_ON_ISPf,  FALSE));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  25,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_STATISTICS_REPORT_CONFIGURATIONSr, REG_PORT_ANY, 0, ST_BILL_INGR_ENf,  TRUE));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_STATISTICS_REPORT_CONFIGURATIONSr, REG_PORT_ANY, 0, ST_BILL_EGR_ENf,  TRUE));
  }

  

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  35,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_STATISTICS_REPORT_CONFIGURATIONS_2r, REG_PORT_ANY, 0, ST_HG_2_PREAMBf,  0xfb));
  
 
   
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  45,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_STATISTICS_REPORT_CONFIGURATIONS_4r, SOC_CORE_ALL, 0, ST_SCR_BUFF_SIZE_SELf,  FALSE));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  50,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_STATISTICS_REPORT_CONFIGURATIONS_4r, SOC_CORE_ALL, 0, SCR_UC_DB_CMP_SELf,  TRUE));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  55,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_STATISTICS_REPORT_CONFIGURATIONS_4r, SOC_CORE_ALL, 0, SCR_BD_CMP_SELf,  TRUE));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  60,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_STATISTICS_REPORT_CONFIGURATIONS_4r, SOC_CORE_ALL, 0, SCR_BDB_CMP_SELf,  TRUE));

  
  if (SOC_DPP_CONFIG(unit)->arad->init.stat_if.stat_if_enable) {
	  rv = soc_port_sw_db_valid_ports_get(unit, SOC_PORT_FLAGS_STAT_INTERFACE, &stat_port_bm);
      if(SOC_FAILURE(rv)) {
           SOC_SAND_SET_ERROR_CODE(ARAD_STAT_IF_INIT_FAILED_ERR, 80, exit);
      }

      PBMP_ITER(stat_port_bm, port) {
          rv = soc_port_sw_db_interface_type_get(unit, port, &interface);
          if(SOC_FAILURE(rv)) {
              SOC_SAND_SET_ERROR_CODE(ARAD_STAT_IF_INIT_FAILED_ERR, 80, exit);
          }

          if(interface == SOC_PORT_IF_RXAUI) {
              SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  31,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, NBI_NIF_STATISTIC_DUPLICATE_ENABLEr, REG_PORT_ANY, 0, STATISTIC_DUPLICATE_ENABLEf,  1));
              break;
          }

      }
  }

#ifdef BCM_88650_B0
  
    if (SOC_IS_ARAD_B0_AND_ABOVE(unit)) {
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_STATISTICS_REPORT_CONFIGURATIONS_7r, REG_PORT_ANY, 0, ST_BILL_INGR_FILTER_DISf,  0x0));
    }
#endif

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ports_regs_init()",0,0);
}





uint32
  arad_stat_if_report_info_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_STAT_IF_REPORT_INFO *info
  )
{
  uint32
    threshold_index,
    res = SOC_SAND_OK,
    fld_val,
    limit_prd =0,
    min_machine_clock_value,
    max_machine_clock_value,
    data[3] = {0, 0, 0};
  uint64
      field64;
  int core=0; 

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_STAT_IF_REPORT_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  SOC_SAND_CHECK_NULL_INPUT(info);


  switch(info->mode) {
#ifdef BCM_88650_B0
  case SOC_TMC_STAT_IF_REPORT_MODE_BILLING_QUEUE_NUMBER:
      if (SOC_IS_ARAD_B0_AND_ABOVE((unit))) {
          
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_STATISTICS_REPORT_CONFIGURATIONS_7r, REG_PORT_ANY, 0, ST_BILL_STAMP_QNUMf,  0x1));
      }
#endif
  
  
  case SOC_TMC_STAT_IF_REPORT_MODE_BILLING:
      
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_STATISTICS_REPORT_CONFIGURATIONSr, REG_PORT_ANY, 0, ST_RPT_MODEf,  ARAD_STAT_IF_REPORT_BILLING_MODE_VALUE));

      
      fld_val = (FALSE == (info->is_idle_reports_present)) ? ARAD_STAT_IF_REPORT_BILLING_NO_IDLE_PERIOD : ARAD_STAT_IF_REPORT_BILLING_MAX_IDLE_PERIOD;
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  25,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_STATISTICS_REPORT_CONFIGURATIONSr, REG_PORT_ANY, 0, ST_BILL_NULL_PRDf,   fld_val));

      
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  26,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_STATISTICS_REPORT_CONFIGURATIONSr, REG_PORT_ANY, 0, ST_RPT_MC_ONCEf,   info->if_report_mc_once));
      
      fld_val = (FALSE == (info->if_report_original_pkt_size)) ? ARAD_STAT_IF_REPORT_BILLING_ORIGINAL_PACKET_SIZE_DISABLE : ARAD_STAT_IF_REPORT_BILLING_ORIGINAL_PACKET_SIZE_ENABLE;
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_STATISTICS_REPORT_CONFIGURATIONSr, REG_PORT_ANY, 0, ST_BILL_ORIG_PKT_SIZE_ENf,   fld_val));

      
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  105,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_STATISTICS_REPORT_CONFIGURATIONSr, REG_PORT_ANY, 0, ST_RPT_PKT_SIZEf,  info->if_pkt_size));

#ifdef BCM_88650_B0
  
  if (SOC_IS_ARAD_B0_AND_ABOVE((unit))) {
      int i;
      int rv;
      uint32
        base_q_pair,
        nof_pairs,
        curr_q_pair,
        flags;
      uint32 entry[ARAD_EGQ_PCT_TBL_ENTRY_SIZE];
      soc_pbmp_t stat_port_bm;
      
      
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  70,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EPNI_STATISTICS_REPORTING_CONFIGURATIONr, SOC_CORE_ALL, 0, STAT_COND_REPORT_CHICKEN_ENf,  0x0));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  75,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_STATISTICS_REPORT_CONFIGURATIONS_7r, REG_PORT_ANY, 0, ST_BILL_EGR_COND_RPT_ENf,  0x1));

      rv = soc_port_sw_db_valid_ports_get(unit, SOC_PORT_FLAGS_NETWORK_INTERFACE, &stat_port_bm);
      if(SOC_FAILURE(rv)) {
           SOC_SAND_SET_ERROR_CODE(ARAD_STAT_IF_INIT_FAILED_ERR, 80, exit);
      }
      SOC_PBMP_OR(stat_port_bm, PBMP_RCY_ALL(unit));
      PBMP_ITER(stat_port_bm, i) {

          SOC_SAND_SOC_IF_ERROR_RETURN(res, 77, exit, soc_port_sw_db_flags_get(unit, i, &flags));
          if (!(SOC_PORT_IS_ELK_INTERFACE(flags) || SOC_PORT_IS_STAT_INTERFACE(flags))) {
              if (SOC_DPP_CONFIG(unit)->arad->init.stat_if.stat_if_info[0].stat_if_port_enable[i]) {
                  

                  res = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, i, &base_q_pair);
                  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 81, exit);
                  res = soc_port_sw_db_tm_port_to_out_port_priority_get(unit, core, i, &nof_pairs);
                  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 85, exit);

                  for (curr_q_pair = base_q_pair; curr_q_pair - base_q_pair < nof_pairs; curr_q_pair++)
                  {
                      rv = READ_EGQ_PCTm(unit, MEM_BLOCK_ANY, curr_q_pair, entry);
                      if(SOC_FAILURE(rv)) {
                          SOC_SAND_SET_ERROR_CODE(ARAD_STAT_IF_INIT_FAILED_ERR, 90, exit);
                      }
                      soc_mem_field32_set(unit, EGQ_PCTm, entry, STATISTICS_ENf, 0x1);
                      rv = WRITE_EGQ_PCTm(unit, MEM_BLOCK_ALL, curr_q_pair, entry);
                      if(SOC_FAILURE(rv)) {
                          SOC_SAND_SET_ERROR_CODE(ARAD_STAT_IF_INIT_FAILED_ERR, 95, exit);
                      }
                  }
              }
          }
      }

      
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  100,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_STATISTICS_REPORT_CONFIGURATIONS_7r, REG_PORT_ANY, 0, ST_BILL_INGR_FILTER_DISf,  0x0));

  }
#endif

      break;

  case SOC_TMC_STAT_IF_REPORT_QSIZE:
      
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  35,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_STATISTICS_REPORT_CONFIGURATIONSr, REG_PORT_ANY, 0, ST_RPT_MODEf,  ARAD_STAT_IF_REPORT_QSIZE_MODE_VALUE));

      
      fld_val = (FALSE == (info->is_idle_reports_present)) ? ARAD_STAT_IF_REPORT_QSIZE_NO_IDLE_PERIOD:ARAD_STAT_IF_REPORT_QSIZE_MAX_IDLE_PERIOD;
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_STATISTICS_REPORT_CONFIGURATIONSr, REG_PORT_ANY, 0, ST_QSZ_IDLE_PRDf,   fld_val));

      
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  55,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_STATISTICS_REPORT_CONFIGURATIONSr, REG_PORT_ANY, 0, ST_RPT_PKT_SIZEf,  info->if_pkt_size));

      
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  60,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_STATISTICS_REPORT_CONFIGURATIONS_5r, REG_PORT_ANY, 0, ST_SCR_LOW_QNUMf,  info->if_scrubber_queue_min));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  65,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_STATISTICS_REPORT_CONFIGURATIONS_6r, REG_PORT_ANY, 0, ST_SCR_HIGH_QNUMf,  info->if_scrubber_queue_max));

      
      res = arad_chip_time_to_ticks(unit, info->if_scrubber_rate_min, TRUE, 1, TRUE, &min_machine_clock_value);
      SOC_SAND_CHECK_FUNC_RESULT(res, 95, exit);
    
      res = arad_chip_time_to_ticks(unit, info->if_scrubber_rate_max, TRUE, 1, TRUE, &max_machine_clock_value);
      SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
      
      
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  70,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_STATISTICS_REPORT_CONFIGURATIONS_4r, SOC_CORE_ALL, 0, ST_SCR_MIN_PRDf,  min_machine_clock_value / 2));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  75,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_STATISTICS_REPORT_CONFIGURATIONS_4r, SOC_CORE_ALL, 0, ST_SCR_MAX_PRDf,  max_machine_clock_value / 2));

      
      for(threshold_index = 0; threshold_index < ARAD_STAT_IF_REPORT_THRESHOLD_LINES; threshold_index++) {
         
          res = READ_IQM_SCRBUFFTHm(unit, MEM_BLOCK_ANY, threshold_index, data);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 80, exit);

          
          fld_val = (ARAD_STAT_IF_REPORT_THRESHOLD_IGNORED == (info->if_scrubber_buffer_descr_th[threshold_index])) ? ARAD_STAT_IF_REPORT_DESC_THRESHOLD_MAX :info->if_scrubber_buffer_descr_th[threshold_index];
          soc_IQM_SCRBUFFTHm_field32_set(unit, data, SCR_BD_THf, fld_val);

          
          fld_val = (ARAD_STAT_IF_REPORT_THRESHOLD_IGNORED == (info->if_scrubber_bdb_th[threshold_index])) ? ARAD_STAT_IF_REPORT_BDB_THRESHOLD_MAX :info->if_scrubber_bdb_th[threshold_index];
          soc_IQM_SCRBUFFTHm_field32_set(unit, data, SCR_BDB_THf, fld_val);

          
          fld_val = (ARAD_STAT_IF_REPORT_THRESHOLD_IGNORED == (info->if_scrubber_uc_dram_buffer_th[threshold_index])) ? ARAD_STAT_IF_REPORT_UC_DRAM_THRESHOLD_MAX :info->if_scrubber_uc_dram_buffer_th[threshold_index];
          soc_IQM_SCRBUFFTHm_field32_set(unit, data, SCR_UC_DB_THf, fld_val);

          res = WRITE_IQM_SCRBUFFTHm(unit, MEM_BLOCK_ANY, threshold_index, data);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 90, exit);
          
      }
#ifdef BCM_88650_B0
      if (SOC_IS_ARAD_B0_AND_ABOVE((unit))) {
          
          if (info->if_qsize_queue_min > ARAD_STAT_IF_REPORT_QSIZE_QUEUE_MAX || info->if_qsize_queue_max > ARAD_STAT_IF_REPORT_QSIZE_QUEUE_MAX) {
              SOC_SAND_SET_ERROR_CODE(ARAD_STAT_IF_INIT_FAILED_ERR, 80, exit);
          }
          if (info->if_qsize_queue_min > info->if_qsize_queue_max ) {
              SOC_SAND_SET_ERROR_CODE(ARAD_STAT_IF_INIT_FAILED_ERR, 85, exit);
          }
          COMPILER_64_SET(field64,0,info->if_qsize_queue_min);
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  90,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_modify(unit, IQM_STATISTICS_REPORT_CONFIGURATIONS_8r, REG_PORT_ANY, 0, ST_QSIZE_RPT_RANGE_0_LOWf,  field64));
          COMPILER_64_SET(field64,0,info->if_qsize_queue_max);
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  95,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_modify(unit, IQM_STATISTICS_REPORT_CONFIGURATIONS_8r, REG_PORT_ANY, 0, ST_QSIZE_RPT_RANGE_0_HIGHf,  field64));
      }
#endif

    if(SOC_IS_ARDON(unit)) {
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  95,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_STATISTICS_REPORT_CONFIGURATIONS_3r, REG_PORT_ANY, 0, ST_ENQ_RPT_ENf,  info->if_report_enqueue_enable));
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  95,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_STATISTICS_REPORT_CONFIGURATIONS_3r, REG_PORT_ANY, 0, ST_DEQ_RPT_ENf,  info->if_report_dequeue_enable));
    }

    break;

  default:
      SOC_SAND_SET_ERROR_CODE(ARAD_STAT_MODE_OUT_OF_RANGE_ERR, 100, exit);
  }

      
      res = arad_stat_if_rate_limit_prd_get(unit, info, &limit_prd);
      SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  125,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_STATISTICS_REPORT_CONFIGURATIONS_2r, REG_PORT_ANY, 0, ST_RATE_LIMIT_PRDf,  limit_prd));


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_stat_if_report_info_set_unsafe()",0,0);
}


uint32
  arad_stat_if_report_info_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_STAT_IF_REPORT_INFO *info
  )
{

  uint32    
  threshold_index;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_STAT_IF_REPORT_INFO_VERIFY);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

  switch(info->mode) {
#ifdef BCM_88650_B0
  case SOC_TMC_STAT_IF_REPORT_MODE_BILLING_QUEUE_NUMBER:
#endif
  case SOC_TMC_STAT_IF_REPORT_MODE_BILLING:
  
    if(SOC_IS_ARDON(unit)) {
        SOC_SAND_SET_ERROR_CODE(ARAD_STAT_MODE_OUT_OF_RANGE_ERR, 5, exit);
    }  


      break;
  
  case SOC_TMC_STAT_IF_REPORT_QSIZE:
      SOC_SAND_ERR_IF_ABOVE_MAX(info->if_pkt_size, ARAD_STAT_IF_REPORT_PKT_SIZE_MAX, ARAD_STAT_IF_REPORT_PKT_SIZE_OUT_OF_RANGE_ERR, 25, exit);
      SOC_SAND_ERR_IF_ABOVE_MAX(info->if_scrubber_queue_min, ARAD_STAT_IF_REPORT_SCRUBBER_QUEUE_MIN, ARAD_STAT_IF_REPORT_SCRUBBER_QUEUE_MIN_OUT_OF_RANGE_ERR, 30, exit);
      SOC_SAND_ERR_IF_ABOVE_MAX(info->if_scrubber_queue_max, ARAD_STAT_IF_REPORT_SCRUBBER_QUEUE_MAX, ARAD_STAT_IF_REPORT_SCRUBBER_QUEUE_MAX_OUT_OF_RANGE_ERR, 35, exit);
      SOC_SAND_ERR_IF_ABOVE_MAX(info->if_scrubber_queue_min, info->if_scrubber_queue_max, ARAD_STAT_IF_REPORT_SCRUBBER_QUEUE_OUT_OF_RANGE_ERR, 75, exit);
      SOC_SAND_ERR_IF_ABOVE_MAX(info->if_scrubber_rate_min, info->if_scrubber_rate_min, ARAD_STAT_IF_REPORT_SCRUBBER_RATE_OUT_OF_RANGE_ERR, 40, exit);

      for(threshold_index = 0; threshold_index < SOC_TMC_STAT_NOF_THRESHOLD_RANGES; threshold_index++) {
          if (info->if_scrubber_buffer_descr_th[threshold_index] != ARAD_STAT_IF_REPORT_THRESHOLD_IGNORED) {
              SOC_SAND_ERR_IF_ABOVE_MAX(info->if_scrubber_buffer_descr_th[threshold_index], ARAD_STAT_IF_REPORT_DESC_THRESHOLD_MAX, ARAD_STAT_IF_REPORT_DESC_THRESHOLD_OUT_OF_RANGE_ERR, 50, exit);
          }
          if (info->if_scrubber_bdb_th[threshold_index] != ARAD_STAT_IF_REPORT_THRESHOLD_IGNORED) {
              SOC_SAND_ERR_IF_ABOVE_MAX(info->if_scrubber_bdb_th[threshold_index], ARAD_STAT_IF_REPORT_BDB_THRESHOLD_MAX, ARAD_STAT_IF_REPORT_BDB_THRESHOLD_OUT_OF_RANGE_ERR, 60, exit);
          }
          if (info->if_scrubber_buffer_descr_th[threshold_index] != ARAD_STAT_IF_REPORT_THRESHOLD_IGNORED) {
              SOC_SAND_ERR_IF_ABOVE_MAX(info->if_scrubber_buffer_descr_th[threshold_index], ARAD_STAT_IF_REPORT_UC_DRAM_THRESHOLD_MAX, ARAD_STAT_IF_REPORT_UC_DRAM_THRESHOLD_OUT_OF_RANGE_ERR, 70, exit);
          }
      }
      break;
  default:
      SOC_SAND_SET_ERROR_CODE(ARAD_STAT_MODE_OUT_OF_RANGE_ERR, 20, exit);
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_stat_if_report_info_verify()",0,0);
}



uint32
  arad_stat_if_report_info_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_STAT_IF_REPORT_INFO *info
  )
{
  uint32
    fld_get_val,
    threshold_index,
    data[3],
    res = SOC_SAND_OK;
   

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_STAT_IF_REPORT_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  SOC_SAND_CHECK_NULL_INPUT(info);

  
#ifdef BCM_88650_B0
  if (SOC_IS_ARAD_B0_AND_ABOVE((unit))) {
      uint32 is_billing_q_num;

      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_STATISTICS_REPORT_CONFIGURATIONSr, REG_PORT_ANY, 0, ST_RPT_MODEf, &fld_get_val));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  21,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_STATISTICS_REPORT_CONFIGURATIONS_7r, REG_PORT_ANY, 0, ST_BILL_STAMP_QNUMf, &is_billing_q_num));
      if (!is_billing_q_num) {
          info->mode = (ARAD_STAT_IF_REPORT_BILLING_MODE_VALUE == fld_get_val) ? SOC_TMC_STAT_IF_REPORT_MODE_BILLING : SOC_TMC_STAT_IF_REPORT_QSIZE;
      } else {
          info->mode = SOC_TMC_STAT_IF_REPORT_MODE_BILLING_QUEUE_NUMBER;
      }
  } else
#endif
  {
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_STATISTICS_REPORT_CONFIGURATIONSr, REG_PORT_ANY, 0, ST_RPT_MODEf, &fld_get_val));
      info->mode = (ARAD_STAT_IF_REPORT_BILLING_MODE_VALUE == fld_get_val) ? SOC_TMC_STAT_IF_REPORT_MODE_BILLING : SOC_TMC_STAT_IF_REPORT_QSIZE;
  }

  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  25,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_STATISTICS_REPORT_CONFIGURATIONSr, REG_PORT_ANY, 0, ST_RPT_MODEf, &info->if_pkt_size));

  switch(info->mode) {
#ifdef BCM_88650_B0
  case SOC_TMC_STAT_IF_REPORT_MODE_BILLING_QUEUE_NUMBER:
#endif
  case SOC_TMC_STAT_IF_REPORT_MODE_BILLING:

      
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_STATISTICS_REPORT_CONFIGURATIONSr, REG_PORT_ANY, 0, ST_BILL_NULL_PRDf, &fld_get_val));
      info->is_idle_reports_present = (ARAD_STAT_IF_REPORT_BILLING_NO_IDLE_PERIOD == fld_get_val) ? FALSE : TRUE;

      
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  31,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit,IQM_STATISTICS_REPORT_CONFIGURATIONSr,REG_PORT_ANY,0, ST_RPT_MC_ONCEf,(uint32*)&info->if_report_mc_once));
      
      
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  35,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_STATISTICS_REPORT_CONFIGURATIONSr, REG_PORT_ANY, 0, ST_BILL_ORIG_PKT_SIZE_ENf, &fld_get_val));
      info->if_report_original_pkt_size = (ARAD_STAT_IF_REPORT_BILLING_ORIGINAL_PACKET_SIZE_DISABLE == fld_get_val) ? FALSE : TRUE;
      
      break;

  case SOC_TMC_STAT_IF_REPORT_QSIZE:

      
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  35,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_STATISTICS_REPORT_CONFIGURATIONSr, REG_PORT_ANY, 0, ST_QSZ_IDLE_PRDf, &fld_get_val));
      info->is_idle_reports_present = ( ARAD_STAT_IF_REPORT_QSIZE_NO_IDLE_PERIOD== fld_get_val) ? FALSE : TRUE;
      
      
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_STATISTICS_REPORT_CONFIGURATIONS_5r, REG_PORT_ANY, 0, ST_SCR_LOW_QNUMf, &info->if_scrubber_queue_min));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  45,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_STATISTICS_REPORT_CONFIGURATIONS_5r, REG_PORT_ANY, 0, ST_SCR_HIGH_QNUMf, &info->if_scrubber_queue_max));

       
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  50,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_STATISTICS_REPORT_CONFIGURATIONS_4r, SOC_CORE_ALL, 0, ST_SCR_MIN_PRDf, &fld_get_val));
      res = arad_ticks_to_time(unit, fld_get_val, TRUE, 1, &info->if_scrubber_queue_min);
      SOC_SAND_CHECK_FUNC_RESULT(res, 55, exit);
    
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  60,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_STATISTICS_REPORT_CONFIGURATIONS_4r, SOC_CORE_ALL, 0, ST_SCR_MAX_PRDf, &fld_get_val));
      res = arad_ticks_to_time(unit, fld_get_val, TRUE, 1, &info->if_scrubber_queue_max);
      SOC_SAND_CHECK_FUNC_RESULT(res, 65, exit);

      
      for(threshold_index = 0; threshold_index < ARAD_STAT_IF_REPORT_THRESHOLD_LINES; threshold_index++) {
         
          res = READ_IQM_SCRBUFFTHm(unit, 0, threshold_index, data);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 70, exit);

          info->if_scrubber_buffer_descr_th[threshold_index] = soc_IQM_SCRBUFFTHm_field32_get(unit, data, SCR_BD_THf);
          info->if_scrubber_bdb_th[threshold_index] = soc_IQM_SCRBUFFTHm_field32_get(unit, data, SCR_BDB_THf);
          info->if_scrubber_uc_dram_buffer_th[threshold_index] = soc_IQM_SCRBUFFTHm_field32_get(unit, data, SCR_UC_DB_THf);
       
      }
      break;
  default:
      SOC_SAND_SET_ERROR_CODE(ARAD_STAT_MODE_OUT_OF_RANGE_ERR, 75, exit);

  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_stat_if_report_info_get_unsafe()",0,0);
}




#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif 
