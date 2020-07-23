/* 
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        rate_calc.c
 * Purpose:     TBD
 */



#include <shared/shrextend/shrextend_debug.h>
#include <shared/bsl.h>

#include <appl/diag/dcmn/rate_calc.h>
#include <shared/swstate/access/sw_state_access.h>
#include <soc/mem.h>

#ifdef BCM_PETRA_SUPPORT
uint32  get_field32(uint32  in,  int  first,  int  last) {
   uint32 ret_val = 0;
   SHR_BITCOPY_RANGE(&ret_val, 0, &in, first, (last - first));
   return (ret_val);
}

void print_rate32(uint32 counter_result, char *counter_name, int clock_ratio, char* units) {
  cli_out("%-40s = %u K%s", counter_name, (counter_result) * clock_ratio / 1000, units);
  cli_out(" (ok)\n");
}

void print_packet_rate32(uint32 counter_result, char *counter_name, int clock_ratio) {
  print_rate32(counter_result, counter_name, clock_ratio, "pps");
}

void print_rate(uint64 val64, char *counter_name, int clock_ratio, char* units) {
    uint32 counter_result  = get_field32(COMPILER_64_LO(val64), 0, 31);
    uint32 counter_OVF     = get_field32(COMPILER_64_HI(val64), 0, 0);
    cli_out("%-40s = %u K%s", counter_name, (counter_result) * clock_ratio / 1000, units);
    if (counter_OVF == 0) {
        cli_out(" (ok)");
    }
    cli_out("\n");
}

void print_packet_rate(uint64 val64, char *counter_name, int clock_ratio) {
  print_rate(val64, counter_name, clock_ratio, "pps");
}

void print_bit_rate(uint64 val64, char *counter_name, int clock_ratio) {
    uint32 counter_result  = (get_field32(COMPILER_64_LO(val64), 0, 31) / (1000000/(8*clock_ratio))) + ((get_field32(COMPILER_64_HI(val64), 0, 13) / (1000000/(8*clock_ratio))));
    uint32 counter_OVF     = get_field32(COMPILER_64_HI(val64), 14, 14);
    cli_out("%-40s = %u Mbps", counter_name, (counter_result));
    if (counter_OVF == 0) {
        cli_out(" (ok)");
    }
    cli_out("\n");
}

int enable_sch_gtimer(int unit, int core, int nof_clocks_int) {
   uint32  val32 = 0;


   if (SOC_IS_JERICHO(unit)) {
       uint64  val64 ;
       COMPILER_64_ZERO(val64);

       soc_reg64_field32_set(unit, SCH_GTIMER_CONFIGURATIONr, &val64, GTIMER_CYCLEf, nof_clocks_int);
       soc_reg64_field32_set(unit, SCH_GTIMER_CONFIGURATIONr, &val64, GTIMER_ENABLEf, 1);
       soc_reg64_field32_set(unit, SCH_GTIMER_CONFIGURATIONr, &val64, GTIMER_RESET_ON_TRIGGERf, 1);
       if (WRITE_SCH_GTIMER_CONFIGURATIONr(unit, core, val64) != BCM_E_NONE) return BCM_E_FAIL;
   } else {
       soc_reg_field_set(unit, SCH_GLOBAL_TIMER_CONFIGURATION_REGISTERr, &val32, GTIMER_CNTf, nof_clocks_int);
       soc_reg_field_set(unit, SCH_GLOBAL_TIMER_CONFIGURATION_REGISTERr, &val32, GTIMER_CLR_CNTf, 1);   
       if (WRITE_SCH_GLOBAL_TIMER_CONFIGURATION_REGISTERr(unit, val32) != BCM_E_NONE) return BCM_E_FAIL;

       if (READ_SCH_SCHEDULER_CONFIGURATION_REGISTERr(unit, core, &val32) != BCM_E_NONE) return BCM_E_FAIL;
       soc_reg_field_set(unit, SCH_SCHEDULER_CONFIGURATION_REGISTERr, &val32, COUNT_BY_GTIMERf, 1);   
       if (WRITE_SCH_SCHEDULER_CONFIGURATION_REGISTERr(unit, core, val32) != BCM_E_NONE) return BCM_E_FAIL;
   }

   if (SOC_IS_QAX(unit)) {
     uint64  val64;
     COMPILER_64_ZERO(val64);
     if (READ_SCH_GTIMER_CONFIGURATIONr(unit, core, &val64) != BCM_E_NONE) return BCM_E_FAIL;
     soc_reg64_field32_set(unit, SCH_GTIMER_CONFIGURATIONr, &val64, GTIMER_TRIGGERf, 0);
     if (WRITE_SCH_GTIMER_CONFIGURATIONr(unit, core, val64) != BCM_E_NONE) return BCM_E_FAIL;
     soc_reg64_field32_set(unit, SCH_GTIMER_CONFIGURATIONr, &val64, GTIMER_TRIGGERf, 1);
     if (WRITE_SCH_GTIMER_CONFIGURATIONr(unit, core, val64) != BCM_E_NONE) return BCM_E_FAIL;
   }
   else {
     if (READ_SCH_GTIMER_TRIGGERr(unit, core, &val32) != BCM_E_NONE) return BCM_E_FAIL;
     soc_reg_field_set(unit, SCH_GTIMER_TRIGGERr, &val32, GTIMER_TRIGGERf, 0);
     if (WRITE_SCH_GTIMER_TRIGGERr(unit, core, val32) != BCM_E_NONE) return BCM_E_FAIL;
     soc_reg_field_set(unit, SCH_GTIMER_TRIGGERr, &val32, GTIMER_TRIGGERf, 1);
     if (WRITE_SCH_GTIMER_TRIGGERr(unit, core, val32) != BCM_E_NONE) return BCM_E_FAIL;
   }
   return BCM_E_NONE;
}

int disable_sch_gtimer(int unit, int core) {
   uint32  val32 = 0;


   if (SOC_IS_JERICHO(unit)) {
       uint64  val64;
       COMPILER_64_ZERO(val64);
       soc_reg64_field32_set(unit, SCH_GTIMER_CONFIGURATIONr, &val64, GTIMER_ENABLEf, 0);
       if (WRITE_SCH_GTIMER_CONFIGURATIONr(unit, core, val64) != BCM_E_NONE) return BCM_E_FAIL;
   } else {
       if (READ_SCH_SCHEDULER_CONFIGURATION_REGISTERr(unit, core, &val32) != BCM_E_NONE) return BCM_E_FAIL;
       soc_reg_field_set(unit, SCH_SCHEDULER_CONFIGURATION_REGISTERr, &val32, COUNT_BY_GTIMERf, 0);
       if (WRITE_SCH_SCHEDULER_CONFIGURATION_REGISTERr(unit, core, val32) != BCM_E_NONE) return BCM_E_FAIL;
   }

   if (READ_SCH_GTIMER_TRIGGERr(unit, core, &val32) != BCM_E_NONE) return BCM_E_FAIL;
   soc_reg_field_set(unit, SCH_GTIMER_TRIGGERr, &val32, GTIMER_TRIGGERf, 0);
   if (WRITE_SCH_GTIMER_TRIGGERr(unit, core, val32) != BCM_E_NONE) return BCM_E_FAIL;
   
   return BCM_E_NONE;
}

int enable_egq_gtimer(int unit, int core, int nof_clocks_int) {
   /*Enable gtimer for EGQ sub-block inside the EGQ and enable the gtimer for clk of 600MHz:*/
   if (WRITE_EGQ_GTIMER_CONFIGr(unit, core, nof_clocks_int) != BCM_E_NONE) return BCM_E_FAIL;
   if (WRITE_EGQ_GTIMER_CONFIG_CONTr(unit, core, 1) != BCM_E_NONE) return BCM_E_FAIL; /*CLEAR_GTIMER=0x1,ACTIVATE_GTIMER=0x0 */
   if (WRITE_EGQ_GTIMER_CONFIG_CONTr(unit, core, 3) != BCM_E_NONE) return BCM_E_FAIL; /*CLEAR_GTIMER=0x1,ACTIVATE_GTIMER=0x1 */

   /* Enable gtimer for EPNI sub-block inside the EPNI and enable the gtimer for clk of 600MHz:*/
   if (WRITE_EPNI_GTIMER_CONFIGr(unit, core, nof_clocks_int) != BCM_E_NONE) return BCM_E_FAIL;
   if (WRITE_EPNI_GTIMER_CONFIG_CONTr(unit, core, 1) != BCM_E_NONE) return BCM_E_FAIL;
   if (WRITE_EPNI_GTIMER_CONFIG_CONTr(unit, core, 3) != BCM_E_NONE) return BCM_E_FAIL;
   return CMD_OK;
}

int get_sch_otm_port(int unit, int core, int local_port_id, int *otm_port) {
   uint32  val32;
   if (READ_SCH_DSP_2_PORT_MAP_DSPPm(unit, SCH_BLOCK(unit, core), local_port_id, &val32) != BCM_E_NONE) {
       return BCM_E_FAIL;
   }

   *otm_port = soc_mem_field32_get(unit, SCH_DSP_2_PORT_MAP_DSPPm, &val32, DSP_2_PORT_MAP_DSPPf);

   return BCM_E_NONE;
}

int get_otm_port(int unit, int port, int *otm_port) {
   uint32  base_q_pair;
   int rv;

   rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.base_q_pair.get(unit, port, &base_q_pair);
   if (rv)
   {
       return CMD_FAIL;
   }

   *otm_port = base_q_pair;

   return CMD_OK;
}

int get_sch_ps_mode(int unit, int core, int local_port_id, uint32 *ps_mode) {
   int    otm_port;
   int    ps_num;
   uint32 val32_1, val32_2;
   uint32 ps_1p_mode = 0, ps_2p_mode = 0;

   if (get_sch_otm_port(unit, core, local_port_id, &otm_port) != CMD_OK) return BCM_E_FAIL;
   ps_num   = get_field32(otm_port, 3, 7);

   if (READ_SCH_PS_1P_PRIORITY_MODE_REGISTERr(unit, core, &val32_1) != BCM_E_NONE) return BCM_E_FAIL;
   if (READ_SCH_PS_2P_PRIORITY_MODE_REGISTERr(unit, core, &val32_2) != BCM_E_NONE) return BCM_E_FAIL;


   ps_1p_mode = soc_reg_field_get(unit, SCH_PS_1P_PRIORITY_MODE_REGISTERr, val32_1, PS_1P_PRIORITY_MODE_BITMAPf);
   ps_2p_mode = soc_reg_field_get(unit, SCH_PS_2P_PRIORITY_MODE_REGISTERr, val32_2, PS_2P_PRIORITY_MODE_BITMAPf);


   *ps_mode = 2 * get_field32(ps_2p_mode, ps_num, ps_num) + get_field32(ps_1p_mode, ps_num, ps_num);  

   return BCM_E_NONE;
}

int get_ps_mode(int unit, int core, int local_port_id, uint32 *ps_mode) {
   int    otm_port;
   int    ps_num;
   uint64 val64;

   COMPILER_64_ZERO(val64);
   if (get_otm_port(unit, local_port_id, &otm_port) != CMD_OK) return BCM_E_FAIL;
   ps_num   = get_field32(otm_port, 3, 7);
   if (READ_EGQ_PS_MODEr(unit, core, &val64) != BCM_E_NONE) return BCM_E_FAIL;
   if (ps_num < 16) *ps_mode = get_field32(COMPILER_64_LO(val64), ps_num * 2, ps_num * 2 + 1);
   else *ps_mode = get_field32(COMPILER_64_HI(val64), ps_num * 2 - 32, ps_num * 2 + 1 - 32);
   return CMD_OK;
}

int get_sch_credit_size(int unit, uint32 *credit_size) {
   uint32 val32;

   if (credit_size == NULL) return BCM_E_PARAM;

   if (SOC_IS_QAX(unit)) {
      if (READ_IPS_IPS_CREDIT_CONFIGr(unit, &val32) != BCM_E_NONE) return BCM_E_FAIL;
      *credit_size = soc_reg_field_get(unit, IPS_IPS_CREDIT_CONFIGr, val32, CREDIT_VALUE_0f);
   }
   else if (SOC_IS_JERICHO(unit)) {
      if (READ_IPST_CREDIT_CONFIG_1r(unit, &val32) != BCM_E_NONE) return BCM_E_FAIL;
      *credit_size = soc_reg_field_get(unit, IPST_CREDIT_CONFIG_1r, val32, CREDIT_VALUE_0f);
   }
   else if (SOC_IS_ARADPLUS(unit)) {
      if (READ_IPS_IPS_CREDIT_CONFIG_1r(unit, &val32) != BCM_E_NONE) return BCM_E_FAIL;
      *credit_size = soc_reg_field_get(unit, IPS_IPS_CREDIT_CONFIG_1r, val32, CREDIT_VALUE_1f);
   }
   else {
      if (READ_IPS_IPS_CREDIT_CONFIGr(unit, &val32) != BCM_E_NONE) return BCM_E_FAIL;
      *credit_size = soc_reg_field_get(unit, IPS_IPS_CREDIT_CONFIGr, val32, CREDIT_VALUEf);
   }

   return BCM_E_NONE;
}

int calc_sch_rate(int unit, uint32 num_of_credits, int clock_ratio, int *sch_rate_int, int *sch_rate_remainder) {
   uint32 credit_size;

   if ((sch_rate_int == NULL) || (sch_rate_remainder == NULL)) return BCM_E_PARAM;

   if (get_sch_credit_size(unit, &credit_size) != BCM_E_NONE) return BCM_E_FAIL;

   *sch_rate_int = (credit_size * num_of_credits ) / (1000000000/(8*clock_ratio));
   *sch_rate_remainder = ((credit_size * num_of_credits) / (1000000000 / (8*clock_ratio*1000))) % 1000;

   return BCM_E_NONE;
}

int calc_sch_ps_rate(int unit, int local_port_id) {
   uint32  nof_clocks;
   uint32  nof_clocks_int;
   uint32  clock_ratio;
   int     max_tc, i;
   int     core;

   int     otm_port;
   uint32  ps_mode;
   uint32  ps_num;
   int     hr_num;
   uint32  val32;
   uint64  val64;

   uint32  counter_data[SOC_TMC_NOF_FAP_PORTS_ARAD] = {0};
   uint32  fc_data[SOC_TMC_NOF_FAP_PORTS_ARAD] = {0};
   uint32  flowCreditCounterRes = 0;

   uint32  counter_OVF;
   uint32  counter_result, fc_result, qCreditRes; 
   int     sch_rate_in_gbps_int = 0, sch_rate_in_gbps_remainder = 0;
   int     qCreditRateInGbps_int = 0, qCreditRateInGbps_remainder = 0;   
   int     totalDvsRate_int = 0, totalDvsRate_remainder = 0;

   uint32   port = (local_port_id < 0) ? 0 : local_port_id;

   nof_clocks = SOC_DPP_CONFIG(unit)->arad->init.core_freq.frequency * 100; /* 60,000,000 clocks --> 0.1 sec*/
   nof_clocks_int = nof_clocks;
   clock_ratio = 10;

   if (soc_port_sw_db_core_get(unit, port, &core) != SOC_E_NONE) return BCM_E_FAIL;

   if ((local_port_id != -1) && (local_port_id < 256)) {
      if (get_sch_otm_port(unit, core, local_port_id, &otm_port) != BCM_E_NONE) return BCM_E_FAIL;
      ps_num   = get_field32(otm_port, 3, 7);
      if (get_sch_ps_mode(unit, core, local_port_id, &ps_mode) != BCM_E_NONE) return BCM_E_FAIL;
      max_tc = (ps_mode == 0) ? 8 : ((ps_mode == 2) ? 2: 1);
      cli_out("local_port_id=%d, otm_port=%d, ps_num=%d, ps_mode=%d, max_tc=%d\n", local_port_id, otm_port, ps_num, ps_mode, max_tc);
   }
   else {
      return BCM_E_FAIL;
   }

   for (i = 0; i < max_tc; i++) {
      hr_num = otm_port + i;

      val32 = 0;
      soc_reg_field_set(unit, SCH_DBG_DVS_CREDIT_COUNTER_CONFIGURATIONr, &val32, DVS_FILTER_PORTf, hr_num);
      soc_reg_field_set(unit, SCH_DBG_DVS_CREDIT_COUNTER_CONFIGURATIONr, &val32, CNT_BY_PORTf, 1);
      if (WRITE_SCH_DBG_DVS_CREDIT_COUNTER_CONFIGURATIONr(unit, core, val32) != BCM_E_NONE) return BCM_E_FAIL;

      val32 = 0;
      soc_reg_field_set(unit, SCH_DBG_DVS_FC_COUNTERS_CONFIGURATIONr, &val32, FC_CNT_PORTf, hr_num);
      soc_reg_field_set(unit, SCH_DBG_DVS_FC_COUNTERS_CONFIGURATIONr, &val32, CNT_PORT_FCf, 1);
      if (WRITE_SCH_DBG_DVS_FC_COUNTERS_CONFIGURATIONr(unit, core, val32) != BCM_E_NONE) return BCM_E_FAIL;
      
      if (READ_SCH_CREDIT_COUNTER_CONFIGURATION_REG_2r(unit, core, &val32) != BCM_E_NONE) return BCM_E_FAIL;
      soc_reg_field_set(unit, SCH_CREDIT_COUNTER_CONFIGURATION_REG_2r, &val32, FILTER_BY_FLOWf, 0);
      soc_reg_field_set(unit, SCH_CREDIT_COUNTER_CONFIGURATION_REG_2r, &val32, FILTER_BY_SUB_FLOWf, 0);
      soc_reg_field_set(unit, SCH_CREDIT_COUNTER_CONFIGURATION_REG_2r, &val32, FILTER_BY_DEST_FAPf, 0);      
      if (WRITE_SCH_CREDIT_COUNTER_CONFIGURATION_REG_2r(unit, core, val32) != BCM_E_NONE) return BCM_E_FAIL;

      enable_sch_gtimer(unit, core, nof_clocks_int);
      sal_usleep(150000); /* Wait more than 0.1sec, to make sure gtimer has finished */
      
      if (READ_SCH_DVS_CREDIT_COUNTERr(unit, core, &counter_data[hr_num]) != BCM_E_NONE) return BCM_E_FAIL;
      if (READ_SCH_DBG_DVS_FLOW_CONTROL_COUNTERr(unit, core, &fc_data[hr_num]) != BCM_E_NONE) return BCM_E_FAIL;
      if (READ_SCH_DBG_CREDIT_COUNTERr(unit, core, &flowCreditCounterRes) != BCM_E_NONE) return BCM_E_FAIL;
   }

   for (i = 0; i < max_tc; i++) {
      hr_num = otm_port + i;

      counter_result = soc_reg_field_get(unit, SCH_DVS_CREDIT_COUNTERr, counter_data[hr_num], DVS_CREDIT_CNTf);
      counter_OVF = soc_reg_field_get(unit, SCH_DVS_CREDIT_COUNTERr, counter_data[hr_num], DVS_CREDIT_CNT_OVF_OR_FINf);

      fc_result = soc_reg_field_get(unit, SCH_DBG_DVS_FLOW_CONTROL_COUNTERr, fc_data[hr_num], DVS_FC_CNTf);

      qCreditRes = soc_reg_field_get(unit, SCH_DBG_CREDIT_COUNTERr, flowCreditCounterRes, CREDIT_CNTf);

      if (calc_sch_rate(unit, counter_result, clock_ratio, &sch_rate_in_gbps_int, &sch_rate_in_gbps_remainder) != BCM_E_NONE) return BCM_E_FAIL;
      if (calc_sch_rate(unit, qCreditRes, clock_ratio, &qCreditRateInGbps_int, &qCreditRateInGbps_remainder) != BCM_E_NONE) return BCM_E_FAIL;
      totalDvsRate_int += sch_rate_in_gbps_int ;
      totalDvsRate_remainder += sch_rate_in_gbps_remainder;

      cli_out("HR:%5d Rate:%d.%dGbps, FlowControl:%d %%", hr_num, sch_rate_in_gbps_int, sch_rate_in_gbps_remainder, ((fc_result*100)/nof_clocks)); 
      if (counter_OVF != 0) {
         cli_out(" (ok)");
      }
      else {
         cli_out(" (NotOk, Gtimer hasn't finished!!)"); 
      }
      cli_out("\n");      
   }
   
   totalDvsRate_int += totalDvsRate_remainder/1000;
   totalDvsRate_remainder = totalDvsRate_remainder%1000;
   cli_out("totalDvsBw = %d.%d\n", totalDvsRate_int, totalDvsRate_remainder);
   cli_out("totalQBw   = %d.%d\n", qCreditRateInGbps_int, qCreditRateInGbps_remainder);

   if (READ_SCH_DBG_DVS_CREDIT_COUNTER_CONFIGURATIONr(unit, core, &val64) != BCM_E_NONE) return BCM_E_FAIL;
   soc_reg64_field32_set(unit, SCH_DBG_DVS_CREDIT_COUNTER_CONFIGURATIONr, &val64, CNT_BY_PORTf, 0);
   if (WRITE_SCH_DBG_DVS_CREDIT_COUNTER_CONFIGURATIONr(unit, core, val64) != BCM_E_NONE) return BCM_E_FAIL;
   
   disable_sch_gtimer(unit, core);
   
   return BCM_E_NONE;
}

int calc_sch_flow_bw(int unit, int flow_id) {
   uint32  reg_val1;

   uint32  nof_clocks_int;
   uint32  clock_ratio;

   uint32  counter_OVF;
   uint32  counter_result;
   int     sch_rate_in_gbps_int = 0, sch_rate_in_gbps_remainder = 0;

   int core;

   nof_clocks_int = SOC_DPP_CONFIG(unit)->arad->init.core_freq.frequency * 100; /* 60,000,000 clocks --> 0.1 sec*/
   clock_ratio = 10;

   SOC_DPP_CORES_ITER(SOC_CORE_ALL, core) {
       if (READ_SCH_CREDIT_COUNTER_CONFIGURATION_REG_1r(unit, core, &reg_val1) != BCM_E_NONE) return BCM_E_FAIL;
       soc_reg_field_set(unit, SCH_CREDIT_COUNTER_CONFIGURATION_REG_1r, &reg_val1, FILTER_FLOWf, flow_id);
       if (WRITE_SCH_CREDIT_COUNTER_CONFIGURATION_REG_1r(unit, core, reg_val1) != BCM_E_NONE) return BCM_E_FAIL;

       if (READ_SCH_CREDIT_COUNTER_CONFIGURATION_REG_2r(unit, core, &reg_val1) != BCM_E_NONE) return BCM_E_FAIL;
       soc_reg_field_set(unit, SCH_CREDIT_COUNTER_CONFIGURATION_REG_2r, &reg_val1, FILTER_BY_FLOWf, 1);
       soc_reg_field_set(unit, SCH_CREDIT_COUNTER_CONFIGURATION_REG_2r, &reg_val1, FILTER_BY_SUB_FLOWf, 1);
       soc_reg_field_set(unit, SCH_CREDIT_COUNTER_CONFIGURATION_REG_2r, &reg_val1, FILTER_BY_DEST_FAPf, 0);
       if (WRITE_SCH_CREDIT_COUNTER_CONFIGURATION_REG_2r(unit, core, reg_val1) != BCM_E_NONE) return BCM_E_FAIL;

       enable_sch_gtimer(unit, core, nof_clocks_int);
       sal_usleep(150000); /* Wait more than 0.1sec, to make sure gtimer has finished */

       if (READ_SCH_DBG_CREDIT_COUNTERr(unit, core, &reg_val1) != BCM_E_NONE) return BCM_E_FAIL;

       counter_result = soc_reg_field_get(unit, SCH_DBG_CREDIT_COUNTERr, reg_val1, CREDIT_CNTf);
       counter_OVF = soc_reg_field_get(unit, SCH_DBG_CREDIT_COUNTERr, reg_val1, CREDIT_OVFf);

       if (calc_sch_rate(unit, counter_result, clock_ratio, &sch_rate_in_gbps_int, &sch_rate_in_gbps_remainder) != BCM_E_NONE) return BCM_E_FAIL;

       cli_out("Core %d:     Flow:%5d Rate:%d.%dGbps", core, flow_id, sch_rate_in_gbps_int, sch_rate_in_gbps_remainder);
       if (counter_OVF != 0) {
          cli_out(" (ok)");
       }
       else {
          cli_out(" (NotOk, Gtimer hasn't finished!!)");
       }
       cli_out("\n");

       if (READ_SCH_CREDIT_COUNTER_CONFIGURATION_REG_2r(unit, core, &reg_val1) != BCM_E_NONE) return BCM_E_FAIL;
       soc_reg_field_set(unit, SCH_CREDIT_COUNTER_CONFIGURATION_REG_2r, &reg_val1, FILTER_BY_FLOWf, 0);
       soc_reg_field_set(unit, SCH_CREDIT_COUNTER_CONFIGURATION_REG_2r, &reg_val1, FILTER_BY_SUB_FLOWf, 0);
       soc_reg_field_set(unit, SCH_CREDIT_COUNTER_CONFIGURATION_REG_2r, &reg_val1, FILTER_BY_DEST_FAPf, 0);
       if (WRITE_SCH_CREDIT_COUNTER_CONFIGURATION_REG_2r(unit, core, reg_val1) != BCM_E_NONE) return BCM_E_FAIL;

       disable_sch_gtimer(unit, core);
   }

   
   return BCM_E_NONE;
}


/*Arad - Measure EGQ and EPNI Rates:
input parameters:
 o q_num_to_check_bw  - the q-pair number to check BW to in PQP, if -1 then check total BW in PQP
 o if_num_to_check_bw - the interface number to check BW to in FQP, of -1 then check total BW in FQP
output parameters:
 ======================================================================================================================*/
int calc_egq_rate(int unit, int local_port_id, int tc_indx) {
   int     nof_clocks_int;
   int     otm_port;
   int     qp_num;
   int     core;
   uint32  ps_mode;
   uint32  ps_num;
   uint32  val32;
   uint64  val64;
   soc_reg_above_64_val_t data_above_64; 
   uint32   port = (local_port_id < 0) ? 0 : local_port_id;
   int      clock_ratio;

   COMPILER_64_ZERO(val64);
   SOC_REG_ABOVE_64_CLEAR(data_above_64);

   nof_clocks_int = SOC_DPP_CONFIG(unit)->arad->init.core_freq.frequency * 100; /* 60,000,000 clocks --> 0.1 sec*/
   clock_ratio = 10;

   if (soc_port_sw_db_core_get(unit, port, &core) != SOC_E_NONE) return BCM_E_FAIL;

   if (enable_egq_gtimer(unit, core, nof_clocks_int) != BCM_E_NONE) return BCM_E_FAIL;

   if (local_port_id != -1) {
	  if (get_otm_port(unit, local_port_id, &otm_port) != CMD_OK) return BCM_E_FAIL;
	  ps_num   = get_field32(otm_port, 3, 7);
	  if (get_ps_mode(unit, core, local_port_id, &ps_mode) != CMD_OK) return BCM_E_FAIL;
	  qp_num = (otm_port + tc_indx);
	  cli_out("local_port_id=%d, otm_port=%d, ps_num=%d, ps_mode=%d, qp_num=%d\n", local_port_id, otm_port, ps_num, ps_mode, qp_num);
	  if (WRITE_EGQ_CHECK_BW_TO_OFPr(unit, core, 0x100 + qp_num) != BCM_E_NONE) return BCM_E_FAIL;
   }
   else {
      if (WRITE_EGQ_CHECK_BW_TO_OFPr(unit, core, 0x0) != BCM_E_NONE) return BCM_E_FAIL;
   }
   
   sal_usleep(150000); /* Wait more than 0.1sec, to make sure gtimer has finished */

   /*packet rate*/
   if (SOC_IS_JERICHO(unit)) {
       if (READ_EGQ_CELL_DECODER_DEBUG_COUNTERSr(unit, core, data_above_64) != BCM_E_NONE) return BCM_E_FAIL; 
       val64 = soc_reg_above_64_field64_get(unit, EGQ_CELL_DECODER_DEBUG_COUNTERSr, data_above_64, CDC_FABRIC_PACKET_CNTf); 
       print_packet_rate(val64, "CDC_FABRIC_PACKET_CNT", clock_ratio);
       if (READ_EGQ_CELL_DECODER_DEBUG_COUNTERSr(unit, core, data_above_64) != BCM_E_NONE) return BCM_E_FAIL; 
       val32 = soc_reg_above_64_field32_get(unit, EGQ_CELL_DECODER_DEBUG_COUNTERSr, data_above_64, CDC_IPT_PACKET_CNTf); 
       print_packet_rate32(val32, "CDC_IPT_PACKET_CNT", clock_ratio);

       if (READ_EGQ_PRP_DEBUG_COUNTERSr(unit, core, data_above_64) != BCM_E_NONE) return BCM_E_FAIL; 
       val64 = soc_reg_above_64_field64_get(unit, EGQ_PRP_DEBUG_COUNTERSr, data_above_64, PRP_PACKET_IN_CNTf); 
       print_packet_rate(val64, "PRP_PACKET_IN_CNT", clock_ratio);
       if (READ_EGQ_PRP_DEBUG_COUNTERSr(unit, core, data_above_64) != BCM_E_NONE) return BCM_E_FAIL; 
       val64 = soc_reg_above_64_field64_get(unit, EGQ_PRP_DEBUG_COUNTERSr, data_above_64, PRP_PACKET_GOOD_UC_CNTf); 
       print_packet_rate(val64, "PRP_PACKET_GOOD_UC_CNT", clock_ratio);
       if (READ_EGQ_PRP_DEBUG_COUNTERSr(unit, core, data_above_64) != BCM_E_NONE) return BCM_E_FAIL; 
       val64 = soc_reg_above_64_field64_get(unit, EGQ_PRP_DEBUG_COUNTERSr, data_above_64, PRP_PACKET_GOOD_MC_CNTf); 
       print_packet_rate(val64, "PRP_PACKET_GOOD_MC_CNT", clock_ratio);
       if (READ_EGQ_PRP_DEBUG_COUNTERSr(unit, core, data_above_64) != BCM_E_NONE) return BCM_E_FAIL; 
       val64 = soc_reg_above_64_field64_get(unit, EGQ_PRP_DEBUG_COUNTERSr, data_above_64, PRP_PACKET_GOOD_TDM_CNTf); 
       print_packet_rate(val64, "PRP_PACKET_GOOD_TDM_CNT", clock_ratio);

       if (READ_EGQ_PRP_DEBUG_COUNTERSr(unit, core, data_above_64) != BCM_E_NONE) return BCM_E_FAIL; 
       val32 = soc_reg_above_64_field32_get(unit, EGQ_PRP_DEBUG_COUNTERSr, data_above_64, PRP_TDM_BYPASS_PACKET_CNTf); 
       print_packet_rate32(val32, "PRP_TDM_BYPASS_PACKET_CNT", clock_ratio);

       if (READ_EGQ_PRP_DEBUG_COUNTERSr(unit, core, data_above_64) != BCM_E_NONE) return BCM_E_FAIL; 
       val32 = soc_reg_above_64_field32_get(unit, EGQ_PRP_DEBUG_COUNTERSr, data_above_64, PRP_PACKET_DISCARD_UC_CNTf); 
       print_packet_rate32(val32, "PRP_PACKET_DISCARD_UC_CNT", clock_ratio);
       if (READ_EGQ_PRP_DEBUG_COUNTERSr(unit, core, data_above_64) != BCM_E_NONE) return BCM_E_FAIL; 
       val32 = soc_reg_above_64_field32_get(unit, EGQ_PRP_DEBUG_COUNTERSr, data_above_64, PRP_PACKET_DISCARD_MC_CNTf); 
       print_packet_rate32(val32, "PRP_PACKET_DISCARD_MC_CNT", clock_ratio);
       if (READ_EGQ_PRP_DEBUG_COUNTERSr(unit, core, data_above_64) != BCM_E_NONE) return BCM_E_FAIL; 
       val32 = soc_reg_above_64_field32_get(unit, EGQ_PRP_DEBUG_COUNTERSr, data_above_64, PRP_PACKET_DISCARD_TDM_CNTf); 
       print_packet_rate32(val32, "PRP_PACKET_DISCARD_TDM_CNT", clock_ratio);
       if (READ_EGQ_PRP_DEBUG_COUNTERSr(unit, core, data_above_64) != BCM_E_NONE) return BCM_E_FAIL; 
       val32 = soc_reg_above_64_field32_get(unit, EGQ_PRP_DEBUG_COUNTERSr, data_above_64, PRP_SOP_DISCARD_UC_CNTf); 
       print_packet_rate32(val32, "PRP_SOP_DISCARD_UC_CNT", clock_ratio);
       if (READ_EGQ_PRP_DEBUG_COUNTERSr(unit, core, data_above_64) != BCM_E_NONE) return BCM_E_FAIL; 
       val32 = soc_reg_above_64_field32_get(unit, EGQ_PRP_DEBUG_COUNTERSr, data_above_64, PRP_SOP_DISCARD_MC_CNTf); 
       print_packet_rate32(val32, "PRP_SOP_DISCARD_MC_CNT", clock_ratio);
       if (READ_EGQ_PRP_DEBUG_COUNTERSr(unit, core, data_above_64) != BCM_E_NONE) return BCM_E_FAIL; 
       val32 = soc_reg_above_64_field32_get(unit, EGQ_PRP_DEBUG_COUNTERSr, data_above_64, PRP_SOP_DISCARD_TDM_CNTf); 
       print_packet_rate32(val32, "PRP_SOP_DISCARD_TDM_CNT", clock_ratio);
   } else {
       if (READ_EGQ_CPU_PACKET_COUNTERr(unit, &val64) != BCM_E_NONE) return BCM_E_FAIL;
       print_packet_rate(val64, "EGQ_CPU_PACKET_COUNTER", clock_ratio);
       if (READ_EGQ_IPT_PACKET_COUNTERr(unit, &val64) != BCM_E_NONE) return BCM_E_FAIL;
       print_packet_rate(val64, "EGQ_IPT_PACKET_COUNTER", clock_ratio);
       if (READ_EGQ_FDR_PRIMARY_PACKET_COUNTERr(unit, &val64) != BCM_E_NONE) return BCM_E_FAIL;
       print_packet_rate(val64, "EGQ_FDR_PRIMARY_PACKET_COUNTER", clock_ratio);
       if (READ_EGQ_FDR_SECONDARY_PACKET_COUNTERr(unit, &val64) != BCM_E_NONE) return BCM_E_FAIL;
       print_packet_rate(val64, "EGQ_FDR_SECONDARY_PACKET_COUNTER", clock_ratio);
       if (READ_EGQ_RQP_PACKET_COUNTERr(unit, &val64) != BCM_E_NONE) return BCM_E_FAIL;
       print_packet_rate(val64, "EGQ_RQP_PACKET_COUNTER", clock_ratio);
       if (READ_EGQ_RQP_DISCARD_PACKET_COUNTERr(unit, &val64) != BCM_E_NONE) return BCM_E_FAIL;
       print_packet_rate(val64, "EGQ_RQP_DISCARD_PACKET_COUNTER", clock_ratio);
   }
   if (READ_EGQ_EHP_UNICAST_PACKET_COUNTERr(unit, core, &val64) != BCM_E_NONE) return BCM_E_FAIL;
   print_packet_rate(val64, "EGQ_EHP_UNICAST_PACKET_COUNTER", clock_ratio);
   if (READ_EGQ_EHP_MULTICAST_HIGH_PACKET_COUNTERr(unit, core, &val64) != BCM_E_NONE) return BCM_E_FAIL;
   print_packet_rate(val64, "EGQ_EHP_MULTICAST_HIGH_PACKET_COUNTER", clock_ratio);
   if (READ_EGQ_EHP_MULTICAST_LOW_PACKET_COUNTERr(unit, core, &val64) != BCM_E_NONE) return BCM_E_FAIL;
   print_packet_rate(val64, "EGQ_EHP_MULTICAST_LOW_PACKET_COUNTER", clock_ratio);
   if (READ_EGQ_EHP_DISCARD_PACKET_COUNTERr(unit, core, &val64) != BCM_E_NONE) return BCM_E_FAIL;
   print_packet_rate(val64, "EGQ_EHP_DISCARD_PACKET_COUNTER", clock_ratio);
   if (READ_EGQ_PQP_UNICAST_PACKET_COUNTERr(unit, core, &val64) != BCM_E_NONE) return BCM_E_FAIL;
   print_packet_rate(val64, "EGQ_PQP_UNICAST_PACKET_COUNTER", clock_ratio);
   if (READ_EGQ_PQP_MULTICAST_PACKET_COUNTERr(unit, core, &val64) != BCM_E_NONE) return BCM_E_FAIL;
   print_packet_rate(val64, "EGQ_PQP_MULTICAST_PACKET_COUNTER", clock_ratio);
   if (READ_EGQ_PQP_DISCARD_UNICAST_PACKET_COUNTERr(unit, core, &val64) != BCM_E_NONE) return BCM_E_FAIL;
   print_packet_rate(val64, "EGQ_PQP_DISCARD_UNICAST_PACKET_COUNTER", clock_ratio);
   if (READ_EGQ_PQP_DISCARD_MULTICAST_PACKET_COUNTERr(unit, core, &val64) != BCM_E_NONE) return BCM_E_FAIL;
   print_packet_rate(val64, "EGQ_PQP_DISCARD_MULTICAST_PACKET_COUNTER", clock_ratio);
   if (READ_EGQ_FQP_PACKET_COUNTERr(unit, core, &val64) != BCM_E_NONE) return BCM_E_FAIL;
   print_packet_rate(val64, "EGQ_FQP_PACKET_COUNTER", clock_ratio);
   if (READ_EPNI_EPE_PACKET_COUNTERr(unit, core, &val64) != BCM_E_NONE) return BCM_E_FAIL;
   print_packet_rate(val64, "EPNI_EPE_PACKET_COUNTER", clock_ratio);
   if (READ_EPNI_EPE_DISCARDED_PACKETS_COUNTERr(unit, core, &val64) != BCM_E_NONE) return BCM_E_FAIL;
   print_packet_rate(val64, "EPNI_EPE_DISCARDED_PACKETS_COUNTER", clock_ratio);

   if (READ_EGQ_PQP_UNICAST_BYTES_COUNTERr(unit, core, &val64) != BCM_E_NONE) return BCM_E_FAIL;
   print_bit_rate(val64, "EGQ_PQP_UNICAST_BYTES_COUNTER", clock_ratio);
   if (READ_EGQ_PQP_MULTICAST_BYTES_COUNTERr(unit, core, &val64) != BCM_E_NONE) return BCM_E_FAIL;
   print_bit_rate(val64, "EGQ_PQP_MULTICAST_BYTES_COUNTER", clock_ratio);
   if (READ_EPNI_EPE_BYTES_COUNTERr(unit, core, &val64) != BCM_E_NONE) return BCM_E_FAIL;
   print_bit_rate(val64, "EPNI_EPE_BYTES_COUNTER", clock_ratio);

   if (WRITE_EGQ_GTIMER_CONFIG_CONTr(unit, core, 0) != BCM_E_NONE) return BCM_E_FAIL;
   if (WRITE_EPNI_GTIMER_CONFIG_CONTr(unit, core, 0) != BCM_E_NONE) return BCM_E_FAIL;
   return CMD_OK;
}


/*======================================================================================================================*/
int calc_pqp_rate(int unit, int local_port_id, int tc_indx) {
   int     nof_clocks_int;
   uint64  val64;
   int     otm_port;
   int     qp_num;
   uint32	ps_mode;
   uint32     ps_num;
   int      core;
   uint32   port = (local_port_id < 0) ? 0 : local_port_id;
   int      clock_ratio;

   COMPILER_64_ZERO(val64);

   nof_clocks_int = SOC_DPP_CONFIG(unit)->arad->init.core_freq.frequency * 100; /* 60,000,000 clocks --> 0.1 sec*/
   clock_ratio = 10;

   if (soc_port_sw_db_core_get(unit, port, &core) != SOC_E_NONE) return BCM_E_FAIL;

   if ((local_port_id != -1)) {
	  if (get_otm_port(unit, local_port_id, &otm_port) != CMD_OK) return BCM_E_FAIL;
	  ps_num   = get_field32(otm_port, 3, 7);
	  if (get_ps_mode(unit, core, local_port_id, &ps_mode) != CMD_OK) return BCM_E_FAIL;
	  qp_num = (otm_port + tc_indx);
	  cli_out("local_port_id=%d, otm_port=%d, ps_num=%d, ps_mode=%d, qp_num=%d\n", local_port_id, otm_port, ps_num, ps_mode, qp_num);
	  if (WRITE_EGQ_CHECK_BW_TO_OFPr(unit, core, 0x100 + qp_num) != BCM_E_NONE) return BCM_E_FAIL;
   } else{
       if (WRITE_EGQ_CHECK_BW_TO_OFPr(unit, core, 0x0) != BCM_E_NONE) return BCM_E_FAIL;
   }

   if (enable_egq_gtimer(unit, core, nof_clocks_int) != CMD_OK) return BCM_E_FAIL;

   sal_usleep(150000); /* Wait more than 0.1sec, to make sure gtimer has finished */

   if (READ_EGQ_PQP_UNICAST_BYTES_COUNTERr(unit, core, &val64) != BCM_E_NONE) return BCM_E_FAIL;
   print_bit_rate(val64, "EGQ_PQP_UNICAST_BYTES_COUNTER", clock_ratio);
   if (READ_EGQ_PQP_MULTICAST_BYTES_COUNTERr(unit, core, &val64) != BCM_E_NONE) return BCM_E_FAIL;
   print_bit_rate(val64, "EGQ_PQP_MULTICAST_BYTES_COUNTER", clock_ratio);

   if (WRITE_EGQ_CHECK_BW_TO_OFPr(unit, core, 0) != BCM_E_NONE) return BCM_E_FAIL;
   if (WRITE_EGQ_GTIMER_CONFIG_CONTr(unit, core, 0) != BCM_E_NONE) return BCM_E_FAIL;
   if (WRITE_EPNI_GTIMER_CONFIG_CONTr(unit, core, 0) != BCM_E_NONE) return BCM_E_FAIL;
   return CMD_OK;
}
/*======================================================================================================================*/
/*
check_bw_scheme:
 0 - measure total
 1 - measure bw on interface number: check_bw_num
 2 - measure bw on port number: check_bw_num
 3 - measure bw on Q-pair number: check_bw_num
 4 - measure bw on channel number: check_bw_num
 5 - measure bw for mirror/not mirror according to check_bw_num
*/
int calc_epni_rate(int unit, int check_bw_scheme, int check_bw_num) {
   int     nof_clocks_int;
   uint64  val64;
   int     core;
   int     clock_ratio;

   if (check_bw_scheme>5 || check_bw_scheme<0) {
	  cli_out("ERROR in calc_epni_rate: scheme value %d is invalid.\n",check_bw_scheme);
	  return CMD_FAIL;
   }
   
   COMPILER_64_ZERO(val64);

   nof_clocks_int = SOC_DPP_CONFIG(unit)->arad->init.core_freq.frequency * 100; /* 60,000,000 clocks --> 0.1 sec*/
   clock_ratio = 10;

   if (enable_egq_gtimer(unit, BCM_CORE_ALL, nof_clocks_int) != CMD_OK) return BCM_E_FAIL;
   if (WRITE_EPNI_MASK_CHECK_BW_TO_PACKET_DESCRIPTORr(unit, BCM_CORE_ALL, 0x1fffbff) != BCM_E_NONE) return BCM_E_FAIL;

   /*calc nif port rate*/
   if (check_bw_scheme == 1) {
	  if (WRITE_EPNI_MASK_CHECK_BW_TO_PACKET_DESCRIPTORr(unit, BCM_CORE_ALL, 0x1ff03ff) != BCM_E_NONE) return BCM_E_FAIL;
	  if (WRITE_EPNI_CHECK_BW_TO_PACKET_DESCRIPTORr(unit, BCM_CORE_ALL, 2048 * check_bw_num) != BCM_E_NONE) return BCM_E_FAIL;
   }
   /*calc ofp rate*/
   else if (check_bw_scheme == 2) {
	  if (WRITE_EPNI_MASK_CHECK_BW_TO_PACKET_DESCRIPTORr(unit, BCM_CORE_ALL, 0x1fffb07) != BCM_E_NONE) return BCM_E_FAIL;
	  if (WRITE_EPNI_CHECK_BW_TO_PACKET_DESCRIPTORr(unit, BCM_CORE_ALL, check_bw_num) != BCM_E_NONE) return BCM_E_FAIL;
   }
   /*calc qp rate*/
   else if (check_bw_scheme == 3) {
	  if (WRITE_EPNI_MASK_CHECK_BW_TO_PACKET_DESCRIPTORr(unit, BCM_CORE_ALL, 0x1fffb00) != BCM_E_NONE) return BCM_E_FAIL;
	  if (WRITE_EPNI_CHECK_BW_TO_PACKET_DESCRIPTORr(unit, BCM_CORE_ALL, check_bw_num) != BCM_E_NONE) return BCM_E_FAIL;
   }
   /*calc channel rate*/
   else if (check_bw_scheme == 4) {
	  if (WRITE_EPNI_MASK_CHECK_BW_TO_PACKET_DESCRIPTORr(unit, BCM_CORE_ALL, 0x100fbff) != BCM_E_NONE) return BCM_E_FAIL;
	  if (WRITE_EPNI_CHECK_BW_TO_PACKET_DESCRIPTORr(unit, BCM_CORE_ALL, 65536 * check_bw_num) != BCM_E_NONE) return BCM_E_FAIL;
   }
   /*clac mirrored rate*/
   else if (check_bw_scheme == 5) {
	  if (WRITE_EPNI_MASK_CHECK_BW_TO_PACKET_DESCRIPTORr(unit, BCM_CORE_ALL, 0xfffbff) != BCM_E_NONE) return BCM_E_FAIL;
	  if (WRITE_EPNI_CHECK_BW_TO_PACKET_DESCRIPTORr(unit, BCM_CORE_ALL, 16777216 * check_bw_num) != BCM_E_NONE) return BCM_E_FAIL;
   }
   /*calc total BW*/
   else {
	  if (WRITE_EPNI_MASK_CHECK_BW_TO_PACKET_DESCRIPTORr(unit, BCM_CORE_ALL, 0x1fffbff) != BCM_E_NONE) return BCM_E_FAIL;
   }

   sal_usleep(150000); /* Wait more than 0.1sec, to make sure gtimer has finished */

   SOC_DPP_CORES_ITER(SOC_CORE_ALL, core) {
       cli_out("Core %d:\n", core);

       /*packet rate*/
       if (READ_EPNI_EPE_PACKET_COUNTERr(unit, core, &val64) != BCM_E_NONE) return BCM_E_FAIL;
       print_packet_rate(val64, "EPNI_EPE_PACKET_COUNTER", clock_ratio);

       /*bit rate*/
       if (READ_EPNI_EPE_BYTES_COUNTERr(unit, core, &val64) != BCM_E_NONE) return BCM_E_FAIL;
       print_bit_rate(val64, "EPNI_EPE_BYTES_COUNTER", clock_ratio);
   }

   /*mask all parameters*/
   if (WRITE_EPNI_MASK_CHECK_BW_TO_PACKET_DESCRIPTORr(unit, BCM_CORE_ALL, 0x1fffbff) != BCM_E_NONE) return BCM_E_FAIL;
   if (WRITE_EGQ_GTIMER_CONFIG_CONTr(unit, BCM_CORE_ALL, 0) != BCM_E_NONE) return BCM_E_FAIL;
   if (WRITE_EPNI_GTIMER_CONFIG_CONTr(unit, BCM_CORE_ALL, 0) != BCM_E_NONE) return BCM_E_FAIL;
   return CMD_OK;
}
/*======================================================================================*/

int calc_epe_port_rate(int unit, int local_port_id, int tc_indx) {
   int     nof_clocks_int;
   uint64   val64;
   int     otm_port;
   int     qp_num;
   uint32     ps_mode;
   uint32   ps_num;
   int     qp_mask = 0x0;
   int      core;
   uint32   port = (local_port_id < 0) ? 0 : local_port_id;
   int      clock_ratio;

   COMPILER_64_ZERO(val64);

   nof_clocks_int = SOC_DPP_CONFIG(unit)->arad->init.core_freq.frequency * 100; /* 60,000,000 clocks --> 0.1 sec*/
   clock_ratio = 10;

   if (soc_port_sw_db_core_get(unit, port, &core) != SOC_E_NONE) return BCM_E_FAIL;

   if ((local_port_id >= 256) || (tc_indx >= 8)) {
	  cli_out("ERROR in calc_epe_port_rate local_port_id=%d, tc_indx=%d\n", local_port_id, tc_indx);
	  return CMD_FAIL;
   }

   /*qp rate calculation*/
   else if ((local_port_id != -1) && (tc_indx != -1)) {
	  if (get_otm_port(unit, local_port_id, &otm_port) != CMD_OK) return BCM_E_FAIL;
	  ps_num   = get_field32(otm_port, 3, 7);

	  if (get_ps_mode(unit, core, local_port_id, &ps_mode) != CMD_OK) return BCM_E_FAIL;
	  if ((ps_mode == 0) && (tc_indx > 0))      {
		 cli_out("ERROR, ps_mode=1P, tc_index=%d\n", tc_indx);
	  } else if ((ps_mode == 1) && (tc_indx > 1)) {
		 cli_out("ERROR, ps_mode=2P, tc_index=%d\n", tc_indx);
	  }

	  qp_num = (otm_port + tc_indx);
	  cli_out("calc qp rate: local_port_id=%d, otm_port=%d, ps_num=%d, ps_mode=%d, qp_num=%d\n", local_port_id, otm_port, ps_num, ps_mode, qp_num);

	  /*first step mask all parameters*/
	  if (WRITE_EPNI_MASK_CHECK_BW_TO_PACKET_DESCRIPTORr(unit, core, 0x1fffbff) != BCM_E_NONE) return BCM_E_FAIL;
	  /*second step unmask the relevant parameter*/
	  if (WRITE_EPNI_MASK_CHECK_BW_TO_PACKET_DESCRIPTORr(unit, core, 0x1fffb00) != BCM_E_NONE) return BCM_E_FAIL;
	  if (WRITE_EPNI_CHECK_BW_TO_PACKET_DESCRIPTORr(unit, core, qp_num) != BCM_E_NONE) return BCM_E_FAIL;
   }
   /*otm-port rate calculation*/
   else if (local_port_id != -1) {
	  if (get_otm_port(unit, local_port_id, &otm_port) != CMD_OK) return BCM_E_FAIL;
	  ps_num   = get_field32(otm_port, 3, 7);

	  if (get_ps_mode(unit, core, local_port_id, &ps_mode) != CMD_OK) return BCM_E_FAIL;
	  if (ps_mode == 0)      {qp_mask = 0x0;} /*1p port*/
	  else if (ps_mode == 1) {qp_mask = 0x1;} /*2p port*/
	  else              	 {qp_mask = 0x7;} /*8p port*/

	  cli_out("calc port rate: local_port_id=%d, otm_port=%d, ps_num=%d, ps_mode=%d, qp_mask=0x%x\n", local_port_id, otm_port, ps_num, ps_mode, qp_mask);
	  /*first step mask all parameters*/
	  if (WRITE_EPNI_MASK_CHECK_BW_TO_PACKET_DESCRIPTORr(unit, core, 0x1fffbff) != BCM_E_NONE) return BCM_E_FAIL;
	  /*second step unmask the relevant parameter*/
	  if (WRITE_EPNI_MASK_CHECK_BW_TO_PACKET_DESCRIPTORr(unit, core, 0x1fffb00 + qp_mask) != BCM_E_NONE) return BCM_E_FAIL;
	  if (WRITE_EPNI_CHECK_BW_TO_PACKET_DESCRIPTORr(unit, core, otm_port) != BCM_E_NONE) return BCM_E_FAIL;
   }
   /*total rate calculation*/
   else {
	  cli_out("calc total epni rate\n");
	  if (WRITE_EPNI_MASK_CHECK_BW_TO_PACKET_DESCRIPTORr(unit, core, 0x1fffbff) != BCM_E_NONE) return BCM_E_FAIL;
   }

   if (enable_egq_gtimer(unit, core, nof_clocks_int) != CMD_OK) return BCM_E_FAIL;

   sal_usleep(150000); /* Wait more than 0.1sec, to make sure gtimer has finished */

   /*packet rate*/
   if (READ_EPNI_EPE_PACKET_COUNTERr(unit, core, &val64) != BCM_E_NONE) return BCM_E_FAIL;
   print_packet_rate(val64, "EPNI_EPE_PACKET_COUNTER", clock_ratio);

   /*bit rate*/
   if (READ_EPNI_EPE_BYTES_COUNTERr(unit, core, &val64) != BCM_E_NONE) return BCM_E_FAIL;
   print_bit_rate(val64, "EPNI_EPE_BYTES_COUNTER", clock_ratio);

   /*mask all parameters*/
   if (WRITE_EPNI_MASK_CHECK_BW_TO_PACKET_DESCRIPTORr(unit, core, 0x1fffbff) != BCM_E_NONE) return BCM_E_FAIL;
   if (WRITE_EGQ_GTIMER_CONFIG_CONTr(unit, core, 0) != BCM_E_NONE) return BCM_E_FAIL;
   if (WRITE_EPNI_GTIMER_CONFIG_CONTr(unit, core, 0) != BCM_E_NONE) return BCM_E_FAIL;
   return CMD_OK;
}

/*======================================================================================================================*/
int calc_ire_rate(int unit, int local_port_id) {
    int     period;
    uint64  val64;
    int      core;
    uint32   port = (local_port_id < 0) ? 0 : local_port_id;
    int      clock_ratio;

    COMPILER_64_ZERO(val64);

    period = 100000;    /* Count 100000 usec == 0.1 sec */
    clock_ratio = 10;   /* We check counters after 0.1 sec, but we want packets per second */

    if (soc_port_sw_db_core_get(unit, port, &core) != SOC_E_NONE) return BCM_E_FAIL;

    if (gtimer_enable(unit, "IRE", period) != CMD_OK) return BCM_E_FAIL;
    if (gtimer_trigger(unit, "IRE") != CMD_OK) return BCM_E_FAIL;

    sal_usleep(150000); /* Wait more than 0.1sec, to make sure gtimer has finished */

    if (READ_IRE_NIF_PACKET_COUNTERr(unit, core, &val64) != BCM_E_NONE) return BCM_E_FAIL;
    print_packet_rate(val64, "IRE_NIF_PACKET_COUNTER", clock_ratio);
    if (SOC_IS_JERICHO(unit)) {
        /* rcy packet counter have 4 elements - EGQ-0 to even pipe, EGQ-0 to odd pipe, EGQ-1 to even pipe, EGQ-1 to odd pipe */
        if (READ_IRE_RCY_PACKET_COUNTERr(unit, core, &val64) != BCM_E_NONE) return BCM_E_FAIL;
        print_packet_rate(val64, "IRE_RCY_PACKET_COUNTER_0", clock_ratio);
        if (READ_IRE_RCY_PACKET_COUNTERr(unit, core + 2, &val64) != BCM_E_NONE) return BCM_E_FAIL;
        print_packet_rate(val64, "IRE_RCY_PACKET_COUNTER_1", clock_ratio);
    } else {
        if (READ_IRE_RCY_PACKET_COUNTERr(unit, core, &val64) != BCM_E_NONE) return BCM_E_FAIL;
        print_packet_rate(val64, "IRE_RCY_PACKET_COUNTER", clock_ratio);
    }
    if (READ_IRE_CPU_PACKET_COUNTERr(unit, &val64) != BCM_E_NONE) return BCM_E_FAIL;
    print_packet_rate(val64, "IRE_CPU_PACKET_COUNTER", clock_ratio);
    if (READ_IRE_OLP_PACKET_COUNTERr(unit, &val64) != BCM_E_NONE) return BCM_E_FAIL;
    print_packet_rate(val64, "IRE_OLP_PACKET_COUNTER", clock_ratio);
    if (READ_IRE_OAMP_PACKET_COUNTERr(unit, &val64) != BCM_E_NONE) return BCM_E_FAIL;
    print_packet_rate(val64, "IRE_OAMP_PACKET_COUNTER", clock_ratio);
    if (READ_IRE_REGISTER_INTERFACE_PACKET_COUNTERr(unit, &val64) != BCM_E_NONE) return BCM_E_FAIL;
    print_packet_rate(val64, "IRE_REGISTER_INTERFACE_PACKET_COUNTER", clock_ratio);

    if (gtimer_stop(unit, "IRE") != CMD_OK) return BCM_E_FAIL;
    return CMD_OK;
}

/*======================================================================================================================*/
int calc_iqm_rate(int unit, int local_port_id) {
    int      period;
    uint32   val;
    int      core;
    uint32   port = (local_port_id < 0) ? 0 : local_port_id;
    int      clock_ratio;

    period = 100000;    /* Count 100000 usec == 0.1 sec */
    clock_ratio = 10;   /* We check counters after 0.1 sec, but we want packets per second */

    if (soc_port_sw_db_core_get(unit, port, &core) != SOC_E_NONE) return BCM_E_FAIL;

    if (gtimer_enable(unit, "IQM", period) != CMD_OK) return BCM_E_FAIL;
    if (gtimer_trigger(unit, "IQM") != CMD_OK) return BCM_E_FAIL;

    sal_usleep(150000); /* Wait more than 0.1sec, to make sure gtimer has finished */

    if (READ_IQM_ENQUEUE_PACKET_COUNTERr(unit, core, &val) != BCM_E_NONE) return BCM_E_FAIL;
    print_packet_rate32(val, "IQM_ENQUEUE_PACKET_COUNTER", clock_ratio);
    if (READ_IQM_DEQUEUE_PACKET_COUNTERr(unit, core, &val) != BCM_E_NONE) return BCM_E_FAIL;
    print_packet_rate32(val, "IQM_DEQUEUE_PACKET_COUNTER", clock_ratio);
    if (SOC_IS_JERICHO(unit)) {
        if (READ_IQM_ENQUEUE_DISCARDED_PACKET_COUNTERr(unit, core, &val) != BCM_E_NONE) return BCM_E_FAIL;
        print_packet_rate32(val, "IQM_ENQUEUE_DISCARDED_PACKET_COUNTER", clock_ratio);
        if (READ_IQM_DEQUEUE_DELETED_PACKET_COUNTERr(unit, core, &val) != BCM_E_NONE) return BCM_E_FAIL;
        print_packet_rate32(val, "IQM_DEQUEUE_DELETED_PACKET_COUNTER", clock_ratio);
    } else {
        if (READ_IQM_TOTAL_DISCARDED_PACKET_COUNTERr(unit, &val) != BCM_E_NONE) return BCM_E_FAIL;
        print_packet_rate32(val, "IQM_TOTAL_DISCARDED_PACKET_COUNTER", clock_ratio);
        if (READ_IQM_DELETED_PACKET_COUNTERr(unit, &val) != BCM_E_NONE) return BCM_E_FAIL;
        print_packet_rate32(val, "IQM_DELETED_PACKET_COUNTER", clock_ratio);
    }

    if (gtimer_stop(unit, "IQM") != CMD_OK) return BCM_E_FAIL;
    return CMD_OK;
}

/*======================================================================================================================*/

typedef struct {
    int reg_id;
    char* reg_name;
    int field_id;
    char* field_name;
    char* units;
} reg_info_t;

/*======================================================================================================================*/
int calc_block_rate(int unit, char* block, reg_info_t* regs, int regs_size)
{
    int      period;
    uint32   val;
    uint64   val64;
    soc_reg_above_64_val_t data;
    int      clock_ratio;
    int i;

    period = 100000;    /* Count 100000 usec == 0.1 sec */
    clock_ratio = 10;   /* We check counters after 0.1 sec, but we want packets per second */

    if (gtimer_enable(unit, block, period) != CMD_OK) return BCM_E_FAIL;
    if (gtimer_trigger(unit, block) != CMD_OK) return BCM_E_FAIL;

    sal_usleep(150000); /* Wait more than 0.1sec, to make sure gtimer has finished */

    /* this function does not cover all possible combinations of register size/field size
       only those those which are currently in use. 
       additional combinations can be added as required
    */
    for (i = 0; i < regs_size; i++) {
        int reg_size_in_bytes = soc_reg_bytes(unit, regs[i].reg_id);
        if (reg_size_in_bytes <= 4) { /* 32 bit */
            if (soc_reg32_get(unit, regs[i].reg_id, REG_PORT_ANY, 0, &val) != BCM_E_NONE) return BCM_E_FAIL;
            if ( regs[i].field_id == INVALIDf) 
                print_rate32(val, regs[i].reg_name, clock_ratio, regs[i].units);
        }
        else if (reg_size_in_bytes <= 8) { /* 64 bit */
            if (soc_reg_get(unit, regs[i].reg_id, REG_PORT_ANY, 0, &val64) != BCM_E_NONE) return BCM_E_FAIL;
            if ( regs[i].field_id == INVALIDf) 
                print_rate(val64,  regs[i].reg_name, clock_ratio, regs[i].units);
        }
        else { /* above 64 bit */
            if (soc_reg_above_64_get(unit,  regs[i].reg_id, REG_PORT_ANY, 0, data) != BCM_E_NONE) return BCM_E_FAIL;
            if (soc_reg_field_length(unit, regs[i].reg_id, regs[i].field_id) <= 32) { /* bits */
                val = soc_reg_above_64_field32_get(unit, regs[i].reg_id, data, regs[i].field_id);
                print_rate32(val,regs[i].field_name , clock_ratio, regs[i].units);
            }
        }
    } 

    if (gtimer_stop(unit, block) != CMD_OK) return BCM_E_FAIL;
    return CMD_OK;

}

/*======================================================================================================================*/

int calc_cgm_rate(int unit) {

    reg_info_t regs[] = { {CGM_VOQ_SRAM_ENQ_PKT_CTRr, "CGM_VOQ_SRAM_ENQ_PKT_CTR", INVALIDf, NULL, "pps"} ,
                          {CGM_VOQ_SRAM_DEQ_PKT_CTRr, "CGM_VOQ_SRAM_DEQ_PKT_CTR", INVALIDf, NULL, "pps" },
                          {CGM_VOQ_SRAM_ENQ_RJCT_PKT_CTRr, "CGM_VOQ_SRAM_ENQ_RJCT_PKT_CTR", INVALIDf, NULL, "pps"},
                          {CGM_VOQ_SRAM_DEL_PKT_CTRr, "CGM_VOQ_SRAM_DEL_PKT_CTR", INVALIDf, NULL, "pps"},
                          {CGM_VOQ_SRAM_ENQ_BYTE_CTRr, "CGM_VOQ_SRAM_ENQ_BYTE_CTR", INVALIDf, NULL, "BPS"} ,
                          {CGM_VOQ_SRAM_DEQ_BYTE_CTRr, "CGM_VOQ_SRAM_DEQ_BYTE_CTR", INVALIDf, NULL, "BPS"},
                          {CGM_VOQ_SRAM_ENQ_RJCT_BYTE_CTRr, "CGM_VOQ_SRAM_ENQ_RJCT_BYTE_CTR", INVALIDf, NULL, "BPS"},
                          {CGM_VOQ_SRAM_DEL_BYTE_CTRr, "CGM_VOQ_SRAM_DEL_BYTE_CTR", INVALIDf, NULL, "BPS"}
    };

    int regs_size = sizeof(regs)/sizeof(regs[0]);

    return calc_block_rate(unit, "CGM", regs, regs_size);

}

/*======================================================================================================================*/
int calc_ipt_rate(int unit) {
    int      period;
    uint32   val;
    int      clock_ratio;

    period = 100000;    /* Count 100000 usec == 0.1 sec */
    clock_ratio = 10;   /* We check counters after 0.1 sec, but we want packets per second */

    if (gtimer_enable(unit, "IPT", period) != CMD_OK) return BCM_E_FAIL;
    if (gtimer_trigger(unit, "IPT") != CMD_OK) return BCM_E_FAIL;

    sal_usleep(150000); /* Wait more than 0.1sec, to make sure gtimer has finished */

    if (SOC_IS_JERICHO(unit)) {
        if (READ_IPT_ENQ_0_PKT_CNTr(unit, &val) != BCM_E_NONE) return BCM_E_FAIL;
        print_packet_rate32(val, "IPT_ENQ_0_PKT_CNT", clock_ratio);
        if (READ_IPT_ENQ_1_PKT_CNTr(unit, &val) != BCM_E_NONE) return BCM_E_FAIL;
        print_packet_rate32(val, "IPT_ENQ_1_PKT_CNT", clock_ratio);
        if (READ_IPT_EGQ_0_PKT_CNTr(unit, &val) != BCM_E_NONE) return BCM_E_FAIL;
        print_packet_rate32(val, "IPT_EGQ_0_PKT_CNT", clock_ratio);
        if (READ_IPT_EGQ_1_PKT_CNTr(unit, &val) != BCM_E_NONE) return BCM_E_FAIL;
        print_packet_rate32(val, "IPT_EGQ_1_PKT_CNT", clock_ratio);
        if (READ_IPT_FDT_0_PKT_CNTr(unit, &val) != BCM_E_NONE) return BCM_E_FAIL;
        print_packet_rate32(val, "IPT_FDT_0_PKT_CNT", clock_ratio);
        if (READ_IPT_FDT_1_PKT_CNTr(unit, &val) != BCM_E_NONE) return BCM_E_FAIL;
        print_packet_rate32(val, "IPT_FDT_1_PKT_CNT", clock_ratio);
    } else {
        if (READ_IPT_ENQ_PKT_CNTr(unit, &val) != BCM_E_NONE) return BCM_E_FAIL;
        print_packet_rate32(val, "IPT_ENQ_PKT_CNT", clock_ratio);
        if (READ_IPT_EGQ_PKT_CNTr(unit, &val) != BCM_E_NONE) return BCM_E_FAIL;
        print_packet_rate32(val, "IPT_EGQ_PKT_CNT", clock_ratio);
        if (READ_IPT_FDT_PKT_CNTr(unit, &val) != BCM_E_NONE) return BCM_E_FAIL;
        print_packet_rate32(val, "IPT_FDT_PKT_CNT", clock_ratio);
    }

    if (gtimer_stop(unit, "IPT") != CMD_OK) return BCM_E_FAIL;
    return CMD_OK;
}


/*======================================================================================================================*/
int calc_pts_rate(int unit) {
    reg_info_t regs[] = { {PTS_RXI_COUNTERr, "PTS_RXI_COUNTER", SQM_RXI_WORDSf, "SQM_RXI_WORDS", "WPS"},
                          {PTS_RXI_COUNTERr, "PTS_RXI_COUNTER", DQM_RXI_WORDSf, "DQM_RXI_WORDS", "WPS"}
    };

    int regs_size = sizeof(regs)/sizeof(regs[0]);

    return calc_block_rate(unit, "PTS", regs, regs_size);
}

/*======================================================================================================================*/
int calc_txq_rate(int unit) {
    reg_info_t regs[] = { {TXQ_RXI_COUNTERr, "TXQ_RXI_COUNTER", ITE_RXI_WORDSf, "ITE_RXI_WORDS", "WPS"},
                          {TXQ_RXI_COUNTERr, "TXQ_RXI_COUNTER", DDP_RXI_WORDSf, "DDP_RXI_WORDS", "WPS"}
    };

    int regs_size = sizeof(regs)/sizeof(regs[0]);

    return calc_block_rate(unit, "TXQ", regs, regs_size);
}

/*======================================================================================================================*/

#define CMD_DPP_GTIMER_BLOCKNAME       "block_name"

char cmd_dpp_gtimer_usage[] =
   "Usage (Gtimer):"
   "\n\tGtimer commands\n\t"
   "Usages:\n\t"
   "Gtimer <OPTION> <parameters> ..."
   "OPTION can be:"
   "\nEnable - Enable gtimer in block."
   "\n\t Parameters required:"
   "\n\t\t block - block name"
   "\n\t\t period - microsecond"

   "\nTrigger- Trigger gtimer."

   "\nStop   - Stop gtimer."
   "\n"
;

int gtimer_reg(int unit, char* block_name, soc_reg_t *cfg_reg, soc_reg_t *tri_reg) {
   int strnlen_block_name;

   if ((block_name == NULL) || ((cfg_reg == NULL) && (tri_reg == NULL)))
      return CMD_FAIL;

   /*
    * Make sure at least one of the strings is NULL terminated.
    */
   strnlen_block_name = sal_strnlen(block_name, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
   if (!sal_strncasecmp(block_name, "FMAC", strnlen_block_name)) {
      if (cfg_reg != NULL)
         *cfg_reg = FMAC_GTIMER_CONFIGURATIONr;
      if (tri_reg != NULL)
         *tri_reg = FMAC_GTIMER_TRIGGERr;
   } 
   else if (!sal_strncasecmp(block_name, "CFC", strnlen_block_name)) {
      if (cfg_reg != NULL)
         *cfg_reg = CFC_GTIMER_CONFIGURATIONr;
      if (tri_reg != NULL)
         *tri_reg = CFC_GTIMER_TRIGGERr;
   }
   else if (!sal_strncasecmp(block_name, "CRPS", strnlen_block_name)) {
      if (cfg_reg != NULL)
         *cfg_reg = CRPS_GTIMER_CONFIGURATIONr;
      if (tri_reg != NULL)
         *tri_reg = CRPS_GTIMER_TRIGGERr;
   }
   else if (!sal_strncasecmp(block_name, "ECI", strnlen_block_name)) {
      if (cfg_reg != NULL)
         *cfg_reg = ECI_GTIMER_CONFIGURATIONr;
      if (tri_reg != NULL)
         *tri_reg = ECI_GTIMER_TRIGGERr;
   }
   else if (!sal_strncasecmp(block_name, "EGQ", strnlen_block_name)) {
      if (cfg_reg != NULL)
         *cfg_reg = EGQ_GTIMER_CONFIGURATIONr;
      if (tri_reg != NULL)
         *tri_reg = EGQ_GTIMER_TRIGGERr;
   }
   else if (!sal_strncasecmp(block_name, "EPNI", strnlen_block_name)) {
      if (cfg_reg != NULL)
         *cfg_reg = EPNI_GTIMER_CONFIGURATIONr;
      if (tri_reg != NULL)
         *tri_reg = EPNI_GTIMER_TRIGGERr;
   }   
   else if (!sal_strncasecmp(block_name, "FCR", strnlen_block_name)) {
      if (cfg_reg != NULL)
         *cfg_reg = FCR_GTIMER_CONFIGURATIONr;
      if (tri_reg != NULL)
         *tri_reg = FCR_GTIMER_TRIGGERr;
   }
   else if (!sal_strncasecmp(block_name, "FCT", strnlen_block_name)) {
      if (cfg_reg != NULL)
         *cfg_reg = FCT_GTIMER_CONFIGURATIONr;
      if (tri_reg != NULL)
         *tri_reg = FCT_GTIMER_TRIGGERr;
   }
   else if (!sal_strncasecmp(block_name, "FDR", strnlen_block_name)) {
      if (cfg_reg != NULL)
         *cfg_reg = FDR_GTIMER_CONFIGURATIONr;
      if (tri_reg != NULL)
         *tri_reg = CFC_GTIMER_TRIGGERr;
   }
   else if (!sal_strncasecmp(block_name, "FDT", strnlen_block_name)) {
      if (cfg_reg != NULL)
         *cfg_reg = FDT_GTIMER_CONFIGURATIONr;
      if (tri_reg != NULL)
         *tri_reg = FDT_GTIMER_TRIGGERr;
   }
   else if (!sal_strncasecmp(block_name, "IDR", strnlen_block_name)) {
      if (cfg_reg != NULL)
         *cfg_reg = IDR_GTIMER_CONFIGURATIONr;
      if (tri_reg != NULL)
         *tri_reg = IDR_GTIMER_TRIGGERr;
   }
   else if (!sal_strncasecmp(block_name, "IHB", strnlen_block_name)) {
      if (cfg_reg != NULL)
         *cfg_reg = IHB_GTIMER_CONFIGURATIONr;
      if (tri_reg != NULL)
         *tri_reg = IHB_GTIMER_TRIGGERr;
   }
   else if (!sal_strncasecmp(block_name, "IHP", strnlen_block_name)) {
      if (cfg_reg != NULL)
         *cfg_reg = IHP_GTIMER_CONFIGURATIONr;
      if (tri_reg != NULL)
         *tri_reg = IHP_GTIMER_TRIGGERr;
   }
   else if (!sal_strncasecmp(block_name, "IPT", strnlen_block_name)) {
      if (cfg_reg != NULL)
         *cfg_reg = IPT_GTIMER_CONFIGURATIONr;
      if (tri_reg != NULL)
         *tri_reg = IPT_GTIMER_TRIGGERr;
   }
   else if (!sal_strncasecmp(block_name, "IQM", strnlen_block_name)) {
      if (cfg_reg != NULL)
         *cfg_reg = IQM_GTIMER_CONFIGURATIONr;
      if (tri_reg != NULL)
         *tri_reg = IQM_GTIMER_TRIGGERr;
   }
   else if (!sal_strncasecmp(block_name, "IRE", strnlen_block_name)) {
      if (cfg_reg != NULL)
         *cfg_reg = IRE_GTIMER_CONFIGURATIONr;
      if (tri_reg != NULL)
         *tri_reg = IRE_GTIMER_TRIGGERr;
   }
   else if (!sal_strncasecmp(block_name, "IRR", strnlen_block_name)) {
      if (cfg_reg != NULL)
         *cfg_reg = IRR_GTIMER_CONFIGURATIONr;
      if (tri_reg != NULL)
         *tri_reg = IRR_GTIMER_TRIGGERr;
   }   
   else if (!sal_strncasecmp(block_name, "MMU", strnlen_block_name)) {
      if (cfg_reg != NULL)
         *cfg_reg = MMU_GTIMER_CONFIGURATIONr;
      if (tri_reg != NULL)
         *tri_reg = MMU_GTIMER_TRIGGERr;
   }
   else if (!sal_strncasecmp(block_name, "NBI", strnlen_block_name)) {
      if (cfg_reg != NULL)
         *cfg_reg = NBI_GTIMER_CONFIGURATIONr;
      if (tri_reg != NULL)
         *tri_reg = NBI_GTIMER_TRIGGERr;
   }
   else if (!sal_strncasecmp(block_name, "NBIH", strnlen_block_name)) {
      if (cfg_reg != NULL)
         *cfg_reg = NBIH_GTIMER_CONFIGURATIONr;
      if (tri_reg != NULL)
         *tri_reg = NBIH_GTIMER_TRIGGERr;
   }
   else if (!sal_strncasecmp(block_name, "NBIL", strnlen_block_name)) {
      if (cfg_reg != NULL)
         *cfg_reg = NBIL_GTIMER_CONFIGURATIONr;
      if (tri_reg != NULL)
         *tri_reg = NBIL_GTIMER_TRIGGERr;
   }
   else if (!sal_strncasecmp(block_name, "NIF", strnlen_block_name)) {
      if (cfg_reg != NULL)
         *cfg_reg = NIF_GTIMER_CONFIGURATIONr;
   }
   else if (!sal_strncasecmp(block_name, "OAMP", strnlen_block_name)) {
      if (cfg_reg != NULL)
         *cfg_reg = OAMP_GTIMER_CONFIGURATIONr;
      if (tri_reg != NULL)
         *tri_reg = OAMP_GTIMER_TRIGGERr;
   }
   else if (!sal_strncasecmp(block_name, "OCB", strnlen_block_name)) {
      if (cfg_reg != NULL)
         *cfg_reg = OCB_GTIMER_CONFIGURATIONr;
      if (tri_reg != NULL)
         *tri_reg = OCB_GTIMER_TRIGGERr;
   }
   else if (!sal_strncasecmp(block_name, "OLP", strnlen_block_name)) {
      if (cfg_reg != NULL)
         *cfg_reg = OLP_GTIMER_CONFIGURATIONr;
      if (tri_reg != NULL)
         *tri_reg = OLP_GTIMER_TRIGGERr;
   }
   else if (!sal_strncasecmp(block_name, "SCH", strnlen_block_name)) {
      if (cfg_reg != NULL)
         *cfg_reg = SCH_GTIMER_CONFIGURATIONr;
      if (tri_reg != NULL)
         *tri_reg = SCH_GTIMER_TRIGGERr;
   }
   else if (!sal_strncasecmp(block_name, "CGM", strnlen_block_name)) {
      if (cfg_reg != NULL)
         *cfg_reg = CGM_GTIMER_CONFIGURATIONr;
   }
   else if (!sal_strncasecmp(block_name, "PTS", strnlen_block_name)) {
      if (cfg_reg != NULL)
         *cfg_reg = PTS_GTIMER_CONFIGURATIONr;
   }
   else if (!sal_strncasecmp(block_name, "TXQ", strnlen_block_name)) {
      if (cfg_reg != NULL)
         *cfg_reg = TXQ_GTIMER_CONFIGURATIONr;
   }
   else {
      cli_out("No gtimer resourse in block(%s)\n", block_name);
      return CMD_FAIL;
   }


   if ((cfg_reg !=NULL &&!SOC_REG_IS_VALID(unit,*cfg_reg)) ||
        (tri_reg !=NULL &&!SOC_REG_IS_VALID(unit,*tri_reg))
       ){
       cli_out("No gtimer resourse in block(%s)\n", block_name);
       return CMD_FAIL;
   }

   return CMD_OK;
}

int gtimer_enable(int unit, char* block_name, int period_num) {
   int strnlen_block_name;
   int nof_clocks_1us, nof_clocks;
   soc_reg_t cfg_reg, cfg_reg_2;
   uint32 val32;
   uint64 val64;
   int i, fmac_copy = 9;
   int rv = 0;

   /* determine gtimer cycle - units are: [core clock frequency(MHZ) * period_num(provided by user)] */
   nof_clocks_1us = SOC_DPP_CONFIG(unit)->arad->init.core_freq.frequency / 1000; /* for example: 600 clocks --> 1 usec*/
   nof_clocks = nof_clocks_1us*period_num;

   /* 1. check block_name */
   strnlen_block_name = sal_strnlen(block_name, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
   if (!sal_strncasecmp(block_name, "FMAC", strnlen_block_name)) {
       cfg_reg = FMAC_GTIMER_CONFIGURATIONr;
       for (i = 0; i < fmac_copy; i++) {
          if ((rv = soc_reg32_get(unit, cfg_reg, i, 0, &val32)) != SOC_E_NONE) {
             return CMD_FAIL;
          }
          soc_reg_field_set(unit, cfg_reg, &val32, GTIMER_CYCLEf, nof_clocks);   
          soc_reg_field_set(unit, cfg_reg, &val32, GTIMER_ENABLEf, 1);   
          soc_reg_field_set(unit, cfg_reg, &val32, GTIMER_RESET_ON_TRIGGERf, 1); 
          if ((rv = soc_reg32_set(unit, cfg_reg, i, 0, val32)) != SOC_E_NONE) {
             return CMD_FAIL;
          }
       }
   }
   else if (!sal_strncasecmp(block_name, "SCH", strnlen_block_name) && SOC_IS_ARADPLUS_AND_BELOW(unit)) {
       cfg_reg = SCH_GLOBAL_TIMER_CONFIGURATION_REGISTERr;
       if ((rv = soc_reg32_get(unit, cfg_reg, REG_PORT_ANY, 0, &val32)) != SOC_E_NONE) {
          return CMD_FAIL;
       }
       soc_reg_field_set(unit, cfg_reg, &val32, GTIMER_CNTf, nof_clocks);   
       soc_reg_field_set(unit, cfg_reg, &val32, GTIMER_CLR_CNTf, 1);   
       if ((rv = soc_reg32_set(unit, cfg_reg, REG_PORT_ANY, 0, val32)) != SOC_E_NONE) {
          return CMD_FAIL;
       }

       cfg_reg_2 = SCH_SCHEDULER_CONFIGURATION_REGISTERr;
       if ((rv = soc_reg32_get(unit, cfg_reg_2, REG_PORT_ANY, 0, &val32)) != SOC_E_NONE) {
          return CMD_FAIL;
       }
       soc_reg_field_set(unit, cfg_reg_2, &val32, COUNT_BY_GTIMERf, 1);     
       if ((rv = soc_reg32_set(unit, cfg_reg_2, REG_PORT_ANY, 0, val32)) != SOC_E_NONE) {
          return CMD_FAIL;
       }
   /*
    * Make sure at least one of the strings is NULL terminated.
    */
   } else if (!sal_strncasecmp(block_name, "IPS", strnlen_block_name)) {
       cfg_reg = IPS_GTIMER_CONFIGURATIONr;
       if ((rv = soc_reg32_get(unit, cfg_reg, REG_PORT_ANY, 0, &val32)) != SOC_E_NONE) {
             return CMD_FAIL;
      }
      soc_reg_field_set(unit, cfg_reg, &val32, GTIMER_CYCLEf, nof_clocks);   
      soc_reg_field_set(unit, cfg_reg, &val32, GTIMER_ENABLEf, 1);   
      soc_reg_field_set(unit, cfg_reg, &val32, GTIMER_RESET_ON_TRIGGERf, 1); 
      if ((rv = soc_reg32_set(unit, cfg_reg, REG_PORT_ANY, 0, val32)) != SOC_E_NONE) {
         return CMD_FAIL;
      }
   } else {    
       if ((rv = gtimer_reg(unit, block_name, &cfg_reg, NULL)) != CMD_OK) {
          return CMD_FAIL;
      }

       if (SOC_IS_QAX(unit)) {
          if ((rv = soc_reg64_get(unit, cfg_reg, REG_PORT_ANY, 0, &val64)) != SOC_E_NONE) {
               return CMD_FAIL;
          }
          soc_reg64_field32_set(unit, cfg_reg, &val64, GTIMER_CYCLEf, nof_clocks);   
          soc_reg64_field32_set(unit, cfg_reg, &val64, GTIMER_ENABLEf, 1);
          soc_reg64_field32_set(unit, cfg_reg, &val64, GTIMER_RESET_ON_TRIGGERf, 1);      
          if ((rv = soc_reg64_set(unit, cfg_reg, REG_PORT_ANY, 0, val64)) != SOC_E_NONE) {
             return CMD_FAIL;
          }
       } else {
           if ((rv = soc_reg32_get(unit, cfg_reg, REG_PORT_ANY, 0, &val32)) != SOC_E_NONE) {
              return CMD_FAIL;
          }
          soc_reg_field_set(unit, cfg_reg, &val32, GTIMER_CYCLEf, nof_clocks);   
          soc_reg_field_set(unit, cfg_reg, &val32, GTIMER_ENABLEf, 1);
          soc_reg_field_set(unit, cfg_reg, &val32, GTIMER_RESET_ON_TRIGGERf, 1);      
          if ((rv = soc_reg32_set(unit, cfg_reg, REG_PORT_ANY, 0, val32)) != SOC_E_NONE) {
             return CMD_FAIL;
          }
          /*
           * Make sure at least one of the strings is NULL terminated.
           */
          if (!sal_strncasecmp(block_name, "IQM", strnlen_block_name)&& SOC_IS_ARADPLUS_AND_BELOW(unit)) {
             cfg_reg_2 = IQM_GLOBAL_TIME_COUNTER_CONFIGURATIONr;
             if ((rv = soc_reg32_get(unit, cfg_reg_2, REG_PORT_ANY, 0, &val32)) != SOC_E_NONE) {
                return CMD_FAIL;
             }
             soc_reg_field_set(unit, cfg_reg_2, &val32, IQC_CNT_BY_GTf, 1); 
             soc_reg_field_set(unit, cfg_reg_2, &val32, IQC_PRG_CNT_BY_GTf, 1); 
             soc_reg_field_set(unit, cfg_reg_2, &val32, STE_CNT_BY_GTf, 1); 
             soc_reg_field_set(unit, cfg_reg_2, &val32, CRPS_CNT_BY_GTf, 1); 
             if ((rv = soc_reg32_set(unit, cfg_reg_2, REG_PORT_ANY, 0, val32)) != SOC_E_NONE) {
                return CMD_FAIL;
             }
          }
       }
   }
   
   return CMD_OK;
}

int gtimer_trigger(int unit, char* block_name) {
   int strnlen_block_name;
   soc_reg_t tri_reg;
   uint32 val32 = 0;
   uint64 val64 ;
   int i, fmac_copy = 9;
   int rv = 0;
    
   /* 1. check block_name */   
   /*
    * Make sure at least one of the strings is NULL terminated.
    */
   strnlen_block_name = sal_strnlen(block_name, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
   if (!sal_strncasecmp(block_name, "FMAC",strnlen_block_name)) {
      tri_reg = FMAC_GTIMER_TRIGGERr;
      for (i = 0; i < fmac_copy; i++) {
         soc_reg_field_set(unit, tri_reg, &val32, GTIMER_TRIGGERf, 0);   
         if ((rv = soc_reg32_set(unit, tri_reg, REG_PORT_ANY, 0, val32)) != SOC_E_NONE) {
            return CMD_FAIL;
         }
         soc_reg_field_set(unit, tri_reg, &val32, GTIMER_TRIGGERf, 1);   
         if ((rv = soc_reg32_set(unit, tri_reg, REG_PORT_ANY, 0, val32)) != SOC_E_NONE) {
            return CMD_FAIL;
         }
      }
   }
   else if (!sal_strncasecmp(block_name, "SCH", strnlen_block_name) && SOC_IS_ARADPLUS_AND_BELOW(unit)) {
      tri_reg = SCH_GLOBAL_TIMER_ACTIVATION_REGISTERr;
      soc_reg_field_set(unit, tri_reg, &val32, GTIMER_ACTf, 0);   
      if ((rv = soc_reg32_set(unit, tri_reg, REG_PORT_ANY, 0, val32)) != SOC_E_NONE) {
         return CMD_FAIL;
      }
      soc_reg_field_set(unit, tri_reg, &val32, GTIMER_ACTf, 1);   
      if ((rv = soc_reg32_set(unit, tri_reg, REG_PORT_ANY, 0, val32)) != SOC_E_NONE) {
         return CMD_FAIL;
      }
   } else if (!sal_strncasecmp(block_name, "IPS", strnlen_block_name)) {
      tri_reg = IPS_GTIMER_TRIGGERr;
      soc_reg_field_set(unit, tri_reg, &val32, GTIMER_TRIGGERf, 0);   
      if ((rv = soc_reg32_set(unit, tri_reg, REG_PORT_ANY, 0, val32)) != SOC_E_NONE) {
          return CMD_FAIL;
      }
      soc_reg_field_set(unit, tri_reg, &val32, GTIMER_TRIGGERf, 1);   
      if ((rv = soc_reg32_set(unit, tri_reg, REG_PORT_ANY, 0, val32)) != SOC_E_NONE) {
          return CMD_FAIL;
      }
   } else {
       if (SOC_IS_QAX(unit)) {
           if ((rv = gtimer_reg(unit, block_name, &tri_reg, NULL)) != CMD_OK) {
             return CMD_FAIL;
           }
           if ((rv = soc_reg64_get(unit, tri_reg, REG_PORT_ANY, 0, &val64)) != SOC_E_NONE) {
               return CMD_FAIL;
           }
           soc_reg64_field32_set(unit, tri_reg, &val64, GTIMER_TRIGGERf, 0);   
           if ((rv = soc_reg64_set(unit, tri_reg, REG_PORT_ANY, 0, val64)) != SOC_E_NONE) {
               return CMD_FAIL;
           }
           soc_reg64_field32_set(unit, tri_reg, &val64, GTIMER_TRIGGERf, 1);   
           if ((rv = soc_reg64_set(unit, tri_reg, REG_PORT_ANY, 0, val64)) != SOC_E_NONE) {
               return CMD_FAIL;
           }
       } else {
           if ((rv = gtimer_reg(unit, block_name, NULL, &tri_reg)) != CMD_OK) {
             return CMD_FAIL;
           }

           soc_reg_field_set(unit, tri_reg, &val32, GTIMER_TRIGGERf, 0);   
           if ((rv = soc_reg32_set(unit, tri_reg, REG_PORT_ANY, 0, val32)) != SOC_E_NONE) {
               return CMD_FAIL;
           }
           soc_reg_field_set(unit, tri_reg, &val32, GTIMER_TRIGGERf, 1);   
           if ((rv = soc_reg32_set(unit, tri_reg, REG_PORT_ANY, 0, val32)) != SOC_E_NONE) {
               return CMD_FAIL;
           }
       }
   }
    
   return CMD_OK;
}

int gtimer_stop(int unit, char* block_name) {
   int strnlen_block_name;
   soc_reg_t cfg_reg, cfg_reg_2, tri_reg;
   uint32 val32;
   uint64 val64;
   int i, fmac_copy = 9;
   int rv = 0;
    
   /* 1. check block_name */    
   /*
    * Make sure at least one of the strings is NULL terminated.
    */
   strnlen_block_name = sal_strnlen(block_name, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
   if (!sal_strncasecmp(block_name, "FMAC", strnlen_block_name)) {
      cfg_reg = FMAC_GTIMER_CONFIGURATIONr;
      tri_reg = FMAC_GTIMER_TRIGGERr;
      for (i = 0; i < fmac_copy; i++) {
         if ((rv = soc_reg32_get(unit, cfg_reg, i, 0, &val32) != SOC_E_NONE)) {
            return CMD_FAIL;
         }
         soc_reg_field_set(unit, cfg_reg, &val32, GTIMER_ENABLEf, 0);   
         if ((rv = soc_reg32_set(unit, cfg_reg, i, 0, val32) != SOC_E_NONE)) {
            return CMD_FAIL;
         }
         soc_reg_field_set(unit, tri_reg, &val32, GTIMER_TRIGGERf, 0);   
         if ((rv = soc_reg32_set(unit, tri_reg, REG_PORT_ANY, 0, val32)) != SOC_E_NONE) {
            return CMD_FAIL;
         }
      }
   }
   else if (!sal_strncasecmp(block_name, "SCH", strnlen_block_name) && SOC_IS_ARADPLUS_AND_BELOW(unit)) {
      cfg_reg = SCH_SCHEDULER_CONFIGURATION_REGISTERr;
      if ((rv = soc_reg32_get(unit, cfg_reg, REG_PORT_ANY, 0, &val32)) != SOC_E_NONE) {
         return CMD_FAIL;
      }
      soc_reg_field_set(unit, cfg_reg, &val32, COUNT_BY_GTIMERf, 0);   
      if ((rv = soc_reg32_set(unit, cfg_reg, REG_PORT_ANY, 0, val32)) != SOC_E_NONE) {
         return CMD_FAIL;
      }   

      tri_reg = SCH_GLOBAL_TIMER_ACTIVATION_REGISTERr;
      if ((rv = soc_reg32_get(unit, tri_reg, REG_PORT_ANY, 0, &val32)) != SOC_E_NONE) {
         return CMD_FAIL;
      }
      soc_reg_field_set(unit, tri_reg, &val32, GTIMER_ACTf, 0);   
      if ((rv = soc_reg32_set(unit, tri_reg, REG_PORT_ANY, 0, val32)) != SOC_E_NONE) {
         return CMD_FAIL;
      }     
   } else if (!sal_strncasecmp(block_name, "IPS", strnlen_block_name)) {
      cfg_reg = IPS_GTIMER_CONFIGURATIONr;
      tri_reg = IPS_GTIMER_TRIGGERr;
      if ((rv = soc_reg32_get(unit, cfg_reg, REG_PORT_ANY, 0, &val32) != SOC_E_NONE)) {
          return CMD_FAIL;
      }
      soc_reg_field_set(unit, cfg_reg, &val32, GTIMER_ENABLEf, 0);   
      if ((rv = soc_reg32_set(unit, cfg_reg, REG_PORT_ANY, 0, val32) != SOC_E_NONE)) {
          return CMD_FAIL;
      }
      soc_reg_field_set(unit, tri_reg, &val32, GTIMER_TRIGGERf, 0);   
      if ((rv = soc_reg32_set(unit, tri_reg, REG_PORT_ANY, 0, val32)) != SOC_E_NONE) {
          return CMD_FAIL;
      }
   } else {
       if (SOC_IS_QAX(unit)) {
          if ((rv = gtimer_reg(unit, block_name, &cfg_reg, NULL)) != CMD_OK) {
             return CMD_FAIL;
          }
          if ((rv = soc_reg64_get(unit, cfg_reg, REG_PORT_ANY, 0, &val64)) != SOC_E_NONE) {
             return CMD_FAIL;
          }   
          soc_reg64_field32_set(unit, cfg_reg, &val64, GTIMER_ENABLEf, 0);   
          soc_reg64_field32_set(unit, cfg_reg, &val64, GTIMER_TRIGGERf, 0);   
          if ((rv = soc_reg64_set(unit, cfg_reg, REG_PORT_ANY, 0, val64)) != SOC_E_NONE) {
             return CMD_FAIL;
          }

       } else {
           if ((rv = gtimer_reg(unit, block_name, &cfg_reg, &tri_reg)) != CMD_OK) {
             return CMD_FAIL;
          }
        
          if ((rv = soc_reg32_get(unit, cfg_reg, REG_PORT_ANY, 0, &val32)) != SOC_E_NONE){
             return CMD_FAIL;
          }   
          soc_reg_field_set(unit, cfg_reg, &val32, GTIMER_ENABLEf, 0);   
          if ((rv = soc_reg32_set(unit, cfg_reg, REG_PORT_ANY, 0, val32)) != SOC_E_NONE) {
             return CMD_FAIL;
          }

          soc_reg_field_set(unit, tri_reg, &val32, GTIMER_TRIGGERf, 0);   
          if ((rv = soc_reg32_set(unit, tri_reg, REG_PORT_ANY, 0, val32)) != SOC_E_NONE) {
             return CMD_FAIL;
          }

          if (!sal_strncasecmp(block_name, "IQM", strnlen_block_name) && SOC_IS_ARADPLUS_AND_BELOW(unit)) {
             cfg_reg_2 = IQM_GLOBAL_TIME_COUNTER_CONFIGURATIONr;
             if ((rv = soc_reg32_get(unit, cfg_reg_2, REG_PORT_ANY, 0, &val32)) != SOC_E_NONE) {
                return CMD_FAIL;
             }
             soc_reg_field_set(unit, cfg_reg_2, &val32, IQC_CNT_BY_GTf, 0); 
             soc_reg_field_set(unit, cfg_reg_2, &val32, IQC_PRG_CNT_BY_GTf, 0); 
             soc_reg_field_set(unit, cfg_reg_2, &val32, STE_CNT_BY_GTf, 0); 
             soc_reg_field_set(unit, cfg_reg_2, &val32, CRPS_CNT_BY_GTf, 0); 
             if ((rv = soc_reg32_set(unit, cfg_reg_2, REG_PORT_ANY, 0, val32)) != SOC_E_NONE) {
                return CMD_FAIL;
             }
          }      
       }
   }
    
   return CMD_OK;
}

cmd_result_t
cmd_dpp_gtimer(int unit, args_t *a) {
   char  *option = ARG_GET(a);
   int strnlen_option;
   char  *block_name = NULL;
   char  *var_name = NULL;
   int   period_num = -2;
   parse_table_t  pt;
   cmd_result_t   retCode = CMD_OK;
   int mode_given = 0;
   int rv = 0;  
      
   parse_table_init(unit, &pt);
   parse_table_add(&pt, "block", PQ_DFL|PQ_STRING, NULL, &block_name, NULL);
   parse_table_add(&pt, "period", PQ_DFL | PQ_INT, &period_num, &period_num, NULL);
   if (0 > parse_arg_eq(a, &pt)) {
      parse_arg_eq_done(&pt);
      return CMD_FAIL;
   }

   /*determine and apply*/
   while (NULL != option) {
      /*
       * Make sure at least one of the strings is NULL terminated.
       */
      strnlen_option = sal_strnlen(option, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
      if (!sal_strcasecmp(option, "?")) {
         mode_given++;
         parse_arg_eq_done(&pt);
         return CMD_USAGE;
      }      
      else if (!sal_strncasecmp(option, "enable", strnlen_option)) {
         mode_given++;
         if (block_name == NULL || period_num == -2) {
            cli_out("option enable requires block_name and period values\n");
            parse_arg_eq_done(&pt);
            return CMD_FAIL;
         }
         
         if ((rv = gtimer_enable(unit, block_name, period_num)) != CMD_OK) {
            cli_out("Failed(%d) enable gtimer\n", rv);
            retCode = CMD_FAIL;
         }
         else
         {
            var_set(CMD_DPP_GTIMER_BLOCKNAME, block_name, TRUE, FALSE);
         }
      } else if (!sal_strncasecmp(option, "trigger", strnlen_option)) {
         mode_given++;
         if ((var_name = var_get(CMD_DPP_GTIMER_BLOCKNAME)) == NULL) {
            cli_out("block_name is NULL\n");
            parse_arg_eq_done(&pt);
            return CMD_FAIL;
         }

         if (block_name != NULL) {
            if (sal_strncmp(var_name, block_name, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) != 0) {
                cli_out("block:%s was not enabled!\n", block_name);
                parse_arg_eq_done(&pt);
                return CMD_FAIL;
            }
         }
            
         if ((rv = gtimer_trigger(unit, var_name)) != CMD_OK) {
            cli_out("Failed(%d) trigger gtimer\n", rv);
            retCode = CMD_FAIL;
         }
      } else if (!sal_strncasecmp(option, "stop", strnlen_option)) {
         mode_given++;
         if ((var_name = var_get(CMD_DPP_GTIMER_BLOCKNAME)) == NULL) {
            cli_out("block_name is NULL\n");
            parse_arg_eq_done(&pt);
            return CMD_FAIL;
         }

         if (block_name != NULL) {
            if (sal_strncmp(var_name, block_name, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) != 0) {
                cli_out("block:%s was not enabled!\n", block_name);
                parse_arg_eq_done(&pt);
                return CMD_FAIL;
            }
         }

         var_unset(CMD_DPP_GTIMER_BLOCKNAME,  TRUE, FALSE, FALSE);

         if ((rv = gtimer_stop(unit, var_name)) != CMD_OK) {
            cli_out("Failed(%d) stop gtimer\n", rv);
            retCode = CMD_FAIL;
         }        
      }
      
      option = ARG_GET(a);
      if (retCode == CMD_FAIL) {
         parse_arg_eq_done(&pt);
         return CMD_FAIL;
      }
   }
   
   parse_arg_eq_done(&pt);
   if (mode_given == 0) {
      return CMD_USAGE;
   }
   return CMD_OK;
}

#endif /* BCM_PETRA_SUPPORT*/

