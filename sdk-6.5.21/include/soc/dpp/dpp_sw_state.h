/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * 
 * INFO: this module is the entry point the branch of the sw state that compose the dpp bcm 
 * layer's sw state 
 */
#ifndef _DPP_SW_STATE_H

#define _DPP_SW_STATE_H

#define DPP_SOC_CONFIG_VERSION_LENGTH_MAX 100


#if defined (BCM_PETRA_SUPPORT)

typedef struct soc_dpp_soc_sand_sw_state_s{
  PARSER_HINT_PTR soc_sand_sw_state_sorted_list_t *sorted_list ;
  PARSER_HINT_PTR soc_sand_sw_state_occ_bitmap_t  *occ_bitmap ;
  PARSER_HINT_PTR soc_sand_sw_state_hash_table_t  *hash_table ;
  PARSER_HINT_PTR soc_sand_sw_state_multi_set_t   *multi_set ;
} soc_dpp_soc_sand_sw_state_t ;

typedef struct soc_dpp_soc_arad_sw_state_s{
    PARSER_HINT_PTR soc_arad_sw_state_tm_t *tm;
    PARSER_HINT_PTR soc_arad_sw_state_pp_t *pp;
} soc_dpp_soc_arad_sw_state_t;

typedef struct soc_jericho_sw_state_tm_s {
    uint16 nof_remote_faps_with_remote_credit_value;
    PARSER_HINT_PTR SOC_TMC_FC_PFC_GEN_BMP_INFO *pfc_gen_bmp;
    PARSER_HINT_ARR SOC_TMC_ITM_INGRESS_RESERVED_RESOURCE *ingress_reserved_resource;
    soc_pbmp_t is_tdm_queuing_on; 
    PARSER_HINT_ARR SHR_BITDCL *is_fabric_link_allowed; 
    PARSER_HINT_ARR int *dram_rd_crc_interrupt_state;
    PARSER_HINT_ARR int *dram_wr_crc_interrupt_state;
    uint32  port_disabled_reassembly_context[SOC_DPP_DEFS_MAX(NOF_LOGICAL_PORTS)];


} soc_jericho_sw_state_tm_t;


typedef struct soc_jericho_sw_state_pp_s {
  PARSER_HINT_PTR jer_pp_mpls_term_t mpls_term;
} soc_jericho_sw_state_pp_t; 

typedef struct soc_qax_sw_state_pp_s {
  PARSER_HINT_PTR qax_pp_oam_mep_db_info_t mep_db;
} soc_qax_sw_state_pp_t; 


typedef struct soc_dpp_soc_jericho_sw_state_s {
    soc_jericho_sw_state_tm_t tm;
    soc_jericho_sw_state_pp_t pp;
} soc_dpp_soc_jericho_sw_state_t;

typedef struct soc_qax_sw_state_tm_s {
    soc_lb_info_t lb_info;
    SOC_TMC_ITM_GUARANTEED_RESOURCE guaranteed_info[SOC_DPP_DEFS_MAX(NOF_CORES)];
    int lossless_pool_id[SOC_DPP_DEFS_MAX(NOF_CORES)];
} soc_qax_sw_state_tm_t;

typedef struct soc_dpp_soc_qax_sw_state_s {
    soc_qax_sw_state_tm_t       tm;
    soc_qax_sw_state_pp_t       pp;
    soc_qax_sw_state_ipsec_t    ipsec;
} soc_dpp_soc_qax_sw_state_t;

typedef struct soc_dpp_soc_config_s{
    uint32      autosync;
    char        version[DPP_SOC_CONFIG_VERSION_LENGTH_MAX];
    int         nested_api_counter;
} soc_dpp_soc_config_t;

typedef struct soc_dpp_soc_sw_state_s{
    soc_dpp_soc_arad_sw_state_t arad;
	soc_dpp_soc_jericho_sw_state_t jericho;
    soc_dpp_soc_qax_sw_state_t qax;
	soc_dpp_soc_sand_sw_state_t sand;
    soc_dpp_soc_config_t config;
} soc_dpp_soc_sw_state_t;

typedef struct soc_dpp_shr_sw_state_s{
    _sw_state_res_unit_desc_t       *resmgr_info;
    sw_state_hash_table_db_t        htbls;
    sw_state_res_tag_bitmap_info_t  res_tag_bmp_info;
    _shr_template_unit_desc_t       *template_info;
} soc_dpp_shr_sw_state_t;

#endif

#endif 

