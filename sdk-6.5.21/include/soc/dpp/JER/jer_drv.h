/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This file contains structure and routine declarations for the
 * Switch-on-a-Chip Driver.
 *
 * This file also includes the more common include files so the
 * individual driver files don't have to include as much.
 */
#ifndef _SOC_DPP_JER_DRV_H
#define _SOC_DPP_JER_DRV_H


#include <shared/bitop.h>
#include <soc/dcmn/dcmn_defs.h>
#include <soc/dpp/dpp_defs.h>
#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/JER/jer_ocb_dram_buffers.h>
#include <soc/dpp/TMC/tmc_api_ingress_traffic_mgmt.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_flp_init.h>

#define SOC_JER_CORE_FREQ_KHZ_DEFAULT 720000


#define SOC_DPP_JER_CONFIG(unit)        (SOC_DPP_CONFIG(unit)->jer)

#define SOC_JER_NOF_FABRIC_PLL (2)
#define SOC_JER_NOF_SYNCE_PLL  (2)

typedef struct soc_jer_pll_config_s {
    soc_dcmn_init_serdes_ref_clock_t ref_clk_pmh_in;
    soc_dcmn_init_serdes_ref_clock_t ref_clk_pmh_out;
    soc_dcmn_init_serdes_ref_clock_t ref_clk_pml_in[SOC_MAX_NUM_NBIL_BLKS];
    soc_dcmn_init_serdes_ref_clock_t ref_clk_pml_out[SOC_MAX_NUM_NBIL_BLKS];
    soc_dcmn_init_serdes_ref_clock_t ref_clk_fabric_in[SOC_JER_NOF_FABRIC_PLL];
    soc_dcmn_init_serdes_ref_clock_t ref_clk_fabric_out[SOC_JER_NOF_FABRIC_PLL];
    soc_dcmn_init_serdes_ref_clock_t ref_clk_synce_out[SOC_JER_NOF_SYNCE_PLL];
    soc_dcmn_init_serdes_ref_clock_t ref_clk_pmx_in;
    soc_dcmn_init_serdes_ref_clock_t ref_clk_pmx_out;
    uint32 is_pll_binding_pml[SOC_MAX_NUM_NBIL_BLKS]; 
} soc_jer_pll_config_t;

typedef struct soc_jer_excluded_mem_bmap_s {
    SHR_BITDCL    excluded_mems_bmap[_SHR_BITDCLSIZE(NUM_SOC_MEM)];
} soc_jer_excluded_mem_bmap_t;



typedef struct soc_dpp_config_jer_pp_s {
  
  uint8   protection_ingress_coupled_mode;          
  uint8   protection_egress_coupled_mode;           
  uint8   protection_fec_accelerated_reroute_mode;  
  uint8   roo_host_arp_msbs;                        
  uint32  kaps_public_ip_frwrd_table_size;          
  uint32  kaps_private_ip_frwrd_table_size;         
  uint32  kaps_large_db_size;                       
  uint32  kaps_large_db_bits;                       
  uint8   truncate_delta_in_pp_counter_0;           
  uint8   truncate_delta_in_pp_counter_1;           
  uint32  res_sizes[ARAD_PP_FLP_KBP_MAX_NUMBER_OF_RESULTS]; 
  uint8   kaps_mgmt_advanced_mode;                  
} soc_dpp_config_jer_pp_t;



#define SOC_JERICHO_PM_4x25          6
#define SOC_JERICHO_PM_4x10          12
#define SOC_JERICHO_PM_QSGMII        4
#define SOC_JERICHO_PM_FABRIC        9
#define SOC_QMX_PM_FABRIC            4
#define SOC_QAX_PM_FABRIC            4
#define SOC_QAX_MESH_HYBRID_PM_FABRIC 2
#define SOC_JERICHO_PLUS_PM_FABRIC   12
#define SOC_JERICHO_PLUS_PM_4x25     12
#define SOC_JERICHO_PLUS_PM_4x10     6
#define SOC_JERICHO_PLUS_PM_QSGMII   2
#define SOC_JERICHO_CPU_PHY_CORE_0 0
#define SOC_JERICHO_CPU_PHY_CORE_1 500
#define SOC_JERICHO_MAX_ILKN_PORTS_OVER_FABRIC 2
#define SOC_JERICHO_NOF_QMLFS (18)


typedef struct soc_jer_ilkn_qmlf_resources_s{
    SHR_BITDCLNAME (serdes_qmlfs, SOC_JERICHO_NOF_QMLFS);
    SHR_BITDCLNAME (memory_qmlfs, SOC_JERICHO_NOF_QMLFS);
}soc_jer_ilkn_qmlf_resources_t;

typedef struct soc_jer_config_nif_s {
    int                                 fw_verify[SOC_DPP_DEFS_MAX(NOF_PMS)];
    int                                 ilkn_over_fabric[SOC_JERICHO_MAX_ILKN_PORTS_OVER_FABRIC];
    soc_jer_ilkn_qmlf_resources_t       ilkn_qmlf_resources[SOC_DPP_DEFS_MAX(NOF_INTERLAKEN_PORTS)];
    int                                 ilkn_burst_max[6];
    int                                 ilkn_burst_min[6];
    int                                 ilkn_burst_short[6];
    int                                 ilkn_nof_segments[SOC_DPP_DEFS_MAX(NOF_INTERLAKEN_PORTS)];
} soc_jer_config_nif_t;

typedef struct soc_jer_tm_config_s {
    uint16 nof_remote_faps_with_remote_credit_value;
    SOC_TMC_ITM_CGM_MGMT_GUARANTEE_MODE cgm_mgmt_guarantee_mode;
    int is_ilkn_big_cal;
    int single_reassembly_for_rcy;
    int truncate_delta_in_pp_counter[NUM_OF_COUNTERS_CMDS]; 
    int truncate_delta_in_tm_counter[NUM_OF_COUNTERS_CMDS]; 
    int shared_mem_reject_percentage_numerator[SOC_TMC_NOF_DROP_PRECEDENCE];
    int shared_mem_reject_percentage_denominator;
} soc_jer_tm_config_t;




typedef struct soc_dpp_config_jer_s {
    soc_jer_ocb_dram_dbuffs_t   dbuffs;
    soc_jer_pll_config_t        pll;
    soc_dpp_config_jer_pp_t     pp;
    soc_jer_excluded_mem_bmap_t excluded_mems;
    soc_jer_config_nif_t        nif;
    soc_jer_tm_config_t         tm;
} soc_dpp_config_jer_t;


void soc_restore_bcm88x7x(int unit, int core); 
int soc_getstatus_bcm88x7x(int unit, int core);
int soc_dpp_jericho_init(int unit, int reset_action);
int soc_jer_device_reset(int unit, int mode, int action);
int soc_jer_specific_info_config_direct(int unit);
int soc_jer_specific_info_config_derived(int unit);
int soc_jer_init_reset(int unit, int reset_action);
int soc_jer_ports_config(int unit);
int soc_jer_info_config_custom_reg_access(int unit);
int soc_jer_tcam_bist_check(int unit);

int pem_meminfo_init(const char *vmem_definition_file_name);  


int soc_jer_pll_info_get(int unit, soc_dpp_pll_info_t *pll_info);

#endif 

