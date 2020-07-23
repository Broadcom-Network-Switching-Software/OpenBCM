/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
#include <shared/bsl.h>

#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_INIT



#include <shared/swstate/access/sw_state_access.h>
#include <soc/dcmn/error.h>
#include <soc/dcmn/dcmn_mem.h>
#include <soc/dcmn/dcmn_mbist.h>
#include <soc/mem.h>

#include <soc/dpp/TMC/tmc_api_ports.h>
#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/dpp_config_imp_defs.h>

#include <soc/dpp/drv.h>

#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Utils/sand_workload_status.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>

#include <soc/dpp/SAND/Management/sand_low_level.h>

#include <soc/dpp/ARAD/arad_general.h>
#include <soc/dpp/ARAD/arad_reg_access.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>

#include <soc/dpp/ARAD/arad_defs.h>
#include <soc/dpp/ARAD/arad_mgmt.h>
#include <soc/dpp/ARAD/arad_chip_defines.h>
#include <soc/dpp/ARAD/arad_init.h>
#include <soc/dpp/ARAD/arad_egr_queuing.h>
#include <soc/dpp/ARAD/arad_egr_prog_editor.h>
#include <soc/dpp/ARAD/arad_flow_control.h>
#include <soc/dpp/ARAD/arad_ingress_scheduler.h>
#include <soc/dpp/ARAD/arad_nif.h>
#include <soc/dpp/ARAD/arad_ports.h>
#include <soc/dpp/ARAD/arad_scheduler_end2end.h>
#include <soc/dpp/ARAD/arad_scheduler_ports.h>
#include <soc/dpp/ARAD/arad_serdes.h>
#include <soc/dpp/ARAD/arad_fabric.h>
#include <soc/dpp/ARAD/arad_ingress_packet_queuing.h>
#include <soc/dpp/ARAD/arad_ingress_traffic_mgmt.h>
#include <soc/dpp/multicast.h>
#include <soc/dpp/ARAD/arad_sw_db.h>
#include <soc/dpp/ARAD/arad_diagnostics.h>
#include <soc/dpp/ARAD/arad_dram.h>
#include <soc/dpp/ARAD/arad_stat_if.h>
#include <soc/dpp/ARAD/arad_parser.h>
#include <soc/dpp/ARAD/arad_cell.h>
#include <soc/dpp/ARAD/arad_cnt.h>
#include <soc/dpp/ARAD/arad_tdm.h>
#include <soc/dpp/ARAD/arad_api_mgmt.h>
#include <soc/dpp/ARAD/arad_pmf_low_level.h>
#include <soc/dpp/ARAD/arad_chip_regs.h>
#if defined(INCLUDE_KBP)
#include <soc/dpp/ARAD/arad_kbp.h>
#endif
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_flp_init.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_isem_access.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_trap_mgmt.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_eg_mirror.h>

#include <soc/dpp/ARAD/NIF/ports_manager.h>
#include <soc/dpp/port_sw_db.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_eg_encap_access.h>

#include <soc/dpp/JER/jer_regs.h>
#include <soc/dpp/JER/jer_ports.h>
#include <soc/dpp/JER/jer_tbls.h>
#include <soc/dpp/ARAD/arad_sim_em.h>
#include <soc/dpp/ARAD/arad_trunk.h>
#include <soc/dpp/QAX/qax_ports.h>







#define ARAD_MGMT_INIT_EGQ_MAX_FRG_VAR            127






#define ARAD_INIT_DRAM_BYTES_FOR_FBC               32
#define ARAD_INIT_DRAM_FBC_SEQUENCE_SIZE           11
#define ARAD_INIT_DRAM_BUFF_TO_FBC_DELTA_MIN       256
















#define ARAD_MGMT_INIT_DDR_PLL_CFG_I_NDIV_INT_START (3)
#define ARAD_MGMT_INIT_DDR_PLL_CFG_I_CH0_MDIV_START (13)
#define ARAD_MGMT_INIT_DDR_PLL_CFG_I_PDIV_START (43)
#define ARAD_MGMT_INIT_DDR_PLL_CFG_I_KP_START (27)
#define ARAD_MGMT_INIT_DDR_PLL_CFG_I_KI_START (31)
#define ARAD_MGMT_INIT_DDR_PLL_CFG_I_KA_START (34)
#define ARAD_MGMT_INIT_DDR_PLL_CFG_I_PLL_CTRL_START (46)
#define ARAD_MGMT_INIT_DDR_PLL_CFG_I_NDIV_INT_LENGTH (10)
#define ARAD_MGMT_INIT_DDR_PLL_CFG_I_CH0_MDIV_LENGTH (8)
#define ARAD_MGMT_INIT_DDR_PLL_CFG_I_PDIV_LENGTH (3)
#define ARAD_MGMT_INIT_DDR_PLL_CFG_I_KP_LENGTH (4)
#define ARAD_MGMT_INIT_DDR_PLL_CFG_I_KI_LENGTH (3)
#define ARAD_MGMT_INIT_DDR_PLL_CFG_I_KA_LENGTH (3)


#define ARAD_MGMT_INIT_DDR_PLL_CFG_I_BIT_VCO_DIV2 (ARAD_MGMT_INIT_DDR_PLL_CFG_I_PLL_CTRL_START + 26)

#define ARAD_MGMT_INIT_DDR_PLL_CFG_I_BIT_IF_FAST (ARAD_MGMT_INIT_DDR_PLL_CFG_I_PLL_CTRL_START + 27)
#define ARAD_MGMT_INIT_DDR_PLL_FREF_EFF (25)

#define ARAD_MGMT_INIT_DDR_PLL_CFG_I_NDIV_MIN (2800/ARAD_MGMT_INIT_DDR_PLL_FREF_EFF)
#define ARAD_MGMT_INIT_DDR_PLL_CFG_I_NDIV_MAX (4000/ARAD_MGMT_INIT_DDR_PLL_FREF_EFF)




#define ARAD_MGMT_INIT_DDR_LCPLL_CFG_I_NDIV_INT_START    (3)
#define ARAD_MGMT_INIT_DDR_LCPLL_CFG_I_CH0_MDIV_START    (13)
#define ARAD_MGMT_INIT_DDR_LCPLL_CFG_I_KP_START          (27)
#define ARAD_MGMT_INIT_DDR_LCPLL_CFG_I_KI_START          (31)
#define ARAD_MGMT_INIT_DDR_LCPLL_CFG_I_KA_START         (34)
#define ARAD_MGMT_INIT_DDR_LCPLL_CFG_I_PDIV_START        (43)
#define ARAD_MGMT_INIT_DDR_LCPLL_CFG_I_PLL_CTRL_START    (46)
#define ARAD_MGMT_INIT_DDR_LCPLL_CFG_I_NDIV_INT_LENGTH   (8)
#define ARAD_MGMT_INIT_DDR_LCPLL_CFG_I_CH0_MDIV_LENGTH   (8)
#define ARAD_MGMT_INIT_DDR_LCPLL_CFG_I_KP_LENGTH         (4)
#define ARAD_MGMT_INIT_DDR_LCPLL_CFG_I_KI_LENGTH         (3)
#define ARAD_MGMT_INIT_DDR_LCPLL_CFG_I_KA_LENGTH        (3)
#define ARAD_MGMT_INIT_DDR_LCPLL_CFG_I_PDIV_LENGTH       (3)
#define ARAD_MGMT_INIT_DDR_LCPLL_CFG_I_PLL_CTRL_LENGTH   (30)


#define ARAD_INIT_OCB_BUFFER_TYPE_NOF_ENTRIES               (320)

#define ARAD_INIT_OCB_BUF_TYPE_NO_OCB_BUF 0x0
#define ARAD_INIT_OCB_BUF_TYPE_UC_OCB_BUF 0x1
#define ARAD_INIT_OCB_BUF_TYPE_FLMC_OCB_BUF 0x2

#define ARAD_INIT_OCB_BUF_TYPE_FWD_UC 0x1
#define ARAD_INIT_OCB_BUF_TYPE_FWD_FMC 0x2
#define ARAD_INIT_OCB_BUF_TYPE_FWD_ING_MC_1_COPY 0x3
#define ARAD_INIT_OCB_BUF_TYPE_FWD_ING_MC_N_COPY 0x4

#define ARAD_INIT_OCB_BUF_TYPE_SNOOP_NO_SNOOP 0x0
#define ARAD_INIT_OCB_BUF_TYPE_MIRROR_NO_MIRROR 0x0
#define ARAD_INIT_OCB_BUF_TYPE_MIRROR_UC_FMC_MIRROR 0x1
#define ARAD_INIT_OCB_BUF_TYPE_MIRROR_ING_MC_MIRROR 0x2

#define ARAD_INIT_OCB_BUF_TYPE_QUEUE_ELIGIBILE 0x1

#define ARAD_INIT_OCB_BUF_TYPE_MCID_IN_RANGE 0x1

#define ARAD_INIT_DDR_FREQUENCY_25_MHZ    (25)
#define ARAD_INIT_DDR_FREQUENCY_125_MHZ   (125)


#define ARAD_INIT_LLFC_DUAL_PIPE_TH (39)
#define ARAD_INIT_LLFC_SINGLE_PIPE_TH (103)



#define ARAD_INIT_HM_NOF_ADDRS                      1024

#define ARAD_INIT_HM_TAKEN_FROM_EEI_EXT             0x1
#define ARAD_INIT_HM_TAKEN_FROM_EEP_EXT             0x2
#define ARAD_INIT_HM_TAKEN_FROM_LEARN_EXT           0x3
#define ARAD_INIT_HM_TAKEN_FROM_2b00_FTMH_EXT_13_0  0x4
#define ARAD_INIT_HM_TAKEN_FROM_2b01_FTMH_EXT_13_0  0x5
#define ARAD_INIT_HM_TAKEN_FROM_2b10_FTMH_EXT_13_0  0x6
#define ARAD_INIT_HM_TAKEN_FROM_PPH_SYSTEM_VSI      0x7


#define ARAD_INIT_HMI_EEI_EXT_EXISTS_LSB                    2
#define ARAD_INIT_HMI_EEP_EXT_EXISTS_LSB                    3
#define ARAD_INIT_HMI_FTMH_EXT_MSBS_LSB                     4
#define ARAD_INIT_HMI_FWD_CODE_LSB                          6

#define ARAD_INIT_HMI_EEI_EXT_EXISTS_NOF_BITS               1
#define ARAD_INIT_HMI_EEP_EXT_EXISTS_NOF_BITS               1
#define ARAD_INIT_HMI_FTMH_EXT_MSBS_NOF_BITS                2
#define ARAD_INIT_HMI_FWD_CODE_NOF_BITS                     4




#define ARAD_INIT_SYNCE_PADS_SEL_15                         2
#define ARAD_INIT_FC_PADS_SEL_33                            0
#define ARAD_INIT_FC_PADS_SEL_15                            2
#define ARAD_INIT_SYNCE_PADS_REF_INT_DS                     0
#define ARAD_INIT_SYNCE_PADS_AMP_EN                         1
#define ARAD_INIT_FC_PADS_REF_INT_EN                        1
#define ARAD_INIT_FC_PADS_REF_INT_DS                        0
#define ARAD_INIT_FC_PADS_AMP_EN                            1
#define ARAD_INIT_FC_PADS_AMP_DS                            0



#define MAX_TCAM_MODE_ARAD            4
#define INVALID_TCAM_MODE_ARAD        2
#define TCAM_BIST_CHECK_BANK_ARAD     6









#define ARAD_INIT_PRINT_ADVANCE(str)                                           \
{                                                                              \
  if (!silent)                                                                 \
  {                                                                            \
   LOG_VERBOSE(BSL_LS_SOC_INIT, \
               (BSL_META_U(unit, \
                           "    + %.2u: %s\n\r"), ++stage_id, str));      \
  }                                                                            \
}

#define ARAD_INIT_PRINT_INTERNAL_LEVEL_0_ADVANCE(str)                                                \
{                                                                                                    \
  if (!silent)                                                                                       \
  {                                                                                                  \
   LOG_VERBOSE(BSL_LS_SOC_INIT, \
               (BSL_META_U(unit, \
                           "        ++ %.2u: %s\n\r"), ++stage_internal_id, str));             \
  }                                                                                                  \
}

#define ARAD_INIT_PRINT_INTERNAL_LEVEL_1_ADVANCE(str)                                                \
{                                                                                                    \
  if (!silent)                                                                                       \
  {                                                                                                  \
   LOG_VERBOSE(BSL_LS_SOC_INIT, \
               (BSL_META_U(unit, \
                           "            ++ %.2u: %s\n\r"), ++stage_internal_id, str));         \
  }                                                                                                  \
}

#define ARAD_INIT_CLAC_M_N_AND_DIFF(dram_freq, freq_diff, m, n, freq_diff_best, m_best, n_best)\
  \
  freq_diff = ((ARAD_MGMT_INIT_DDR_PLL_FREF_EFF)*(n) - (dram_freq)*(m))/(m);\
  if(freq_diff < 0)\
  {\
    freq_diff = -freq_diff;\
  }\
\
  if(freq_diff < freq_diff_best)\
  {\
    m_best = (m);\
    n_best = n;\
    freq_diff_best = freq_diff;\
  }






typedef struct {
  uint32 start;
  uint32 end;
}ARAD_MGMT_DBUFF_BOUNDARIES;

typedef struct {
  ARAD_MGMT_DBUFF_BOUNDARIES  ocb_fmc;
  ARAD_MGMT_DBUFF_BOUNDARIES  fmc;
  ARAD_MGMT_DBUFF_BOUNDARIES  mmc;
  ARAD_MGMT_DBUFF_BOUNDARIES  ocb_uc;
  ARAD_MGMT_DBUFF_BOUNDARIES  uc;
  ARAD_MGMT_DBUFF_BOUNDARIES  fbc_ocb_uc;
  ARAD_MGMT_DBUFF_BOUNDARIES  fbc_uc;
  ARAD_MGMT_DBUFF_BOUNDARIES  fbc_mmc;
  ARAD_MGMT_DBUFF_BOUNDARIES  fbc_ocb_fmc;
  ARAD_MGMT_DBUFF_BOUNDARIES  fbc_fmc;
} ARAD_INIT_DBUFFS_BDRY;

typedef enum
{
  
  ARAD_INIT_SERDES_TYPE_FABRIC = 0,
  
  ARAD_INIT_SERDES_TYPE_NIF = 1,
  
  ARAD_INIT_NOF_SREDES_TYPES = 2
}ARAD_INIT_SERDES_TYPE;






static
soc_field_t
     Arad_pmf_fes_instruction_field[ARAD_PMF_LOW_LEVEL_NOF_FESS] = {
         FES_0_INSTRUCTIONf, FES_1_INSTRUCTIONf, FES_2_INSTRUCTIONf, FES_3_INSTRUCTIONf, FES_4_INSTRUCTIONf, FES_5_INSTRUCTIONf,
         FES_6_INSTRUCTIONf, FES_7_INSTRUCTIONf, FES_8_INSTRUCTIONf, FES_9_INSTRUCTIONf,
         FES_10_INSTRUCTIONf,FES_11_INSTRUCTIONf,FES_12_INSTRUCTIONf,FES_13_INSTRUCTIONf,FES_14_INSTRUCTIONf,FES_15_INSTRUCTIONf,
         FES_16_INSTRUCTIONf,FES_17_INSTRUCTIONf,FES_18_INSTRUCTIONf,FES_19_INSTRUCTIONf,
         FES_20_INSTRUCTIONf,FES_21_INSTRUCTIONf,FES_22_INSTRUCTIONf,FES_23_INSTRUCTIONf,FES_24_INSTRUCTIONf,FES_25_INSTRUCTIONf,
         FES_26_INSTRUCTIONf,FES_27_INSTRUCTIONf,FES_28_INSTRUCTIONf,FES_29_INSTRUCTIONf,
         FES_30_INSTRUCTIONf,FES_31_INSTRUCTIONf
     };

static
soc_reg_t arad_interrupts_mask_registers[] = {
    CFC_INTERRUPT_MASK_REGISTERr,
    CRPS_INTERRUPT_MASK_REGISTERr,
    CRPS_PAR_ERR_INTERRUPT_MASK_REGISTERr,
    CRPS_SRC_INVLID_ACCESS_INTERRUPT_REGISTER_MASKr,
    EGQ_ECC_INTERRUPT_REGISTER_MASKr,
    EGQ_ERPP_DISCARD_INT_REG_MASKr,
    EGQ_ERPP_DISCARD_INT_REG_MASK_2r,
    EGQ_INTERRUPT_MASK_REGISTERr,
    EGQ_PKT_REAS_INT_REG_MASKr,
    EPNI_ECC_INTERRUPT_REGISTER_MASKr,
    EPNI_ESEM_INTERRUPT_MASK_REGISTER_ONEr,
    EPNI_INTERRUPT_MASK_REGISTERr,
    EPNI_PP_INT_REG_MASKr,
    FCR_INTERRUPT_MASK_REGISTERr,
    FCT_INTERRUPT_MASK_REGISTERr,
    FDR_INTERRUPT_MASK_REGISTERr,
    FDR_INTERRUPT_MASK_REGISTER_1r,
    FDR_INTERRUPT_MASK_REGISTER_2r,
    FDR_INTERRUPT_MASK_REGISTER_3r,
    FDT_INTERRUPT_MASK_REGISTERr,
    FMAC_INTERRUPT_MASK_REGISTERr,
    FMAC_INTERRUPT_MASK_REGISTER_1r,
    FMAC_INTERRUPT_MASK_REGISTER_2r,
    FMAC_INTERRUPT_MASK_REGISTER_3r,
    FMAC_INTERRUPT_MASK_REGISTER_4r,
    FMAC_INTERRUPT_MASK_REGISTER_5r,
    FMAC_INTERRUPT_MASK_REGISTER_6r,
    FMAC_INTERRUPT_MASK_REGISTER_7r,
    FMAC_INTERRUPT_MASK_REGISTER_8r,
    FMAC_INTERRUPT_MASK_REGISTER_9r,
    FSRD_INTERRUPT_MASK_REGISTERr,
    FSRD_QUAD_INTERRUPT_MASK_REGISTERr,
    FSRD_QUAD_INTERRUPT_MASK_REGISTERr,
    FSRD_QUAD_INTERRUPT_MASK_REGISTERr,
    IDR_ECC_INTERRUPT_REGISTER_MASKr,
    IDR_INTERRUPT_MASK_REGISTERr,
    IDR_REASSEMBLY_INTERRUPT_REGISTER_MASKr,
    IHB_INTERRUPT_MASK_REGISTERr,
    IHB_OEMA_INTERRUPT_MASK_REGISTER_ONEr,
    IHB_OEMB_INTERRUPT_MASK_REGISTER_ONEr,
    IHP_INTERRUPT_MASK_REGISTERr,
    IHP_ISA_INTERRUPT_MASK_REGISTER_ONEr,
    IHP_ISB_INTERRUPT_MASK_REGISTER_ONEr,
    IHP_MACT_INTERRUPT_MASK_REGISTER_TWOr,
    PPDB_B_LARGE_EM_INTERRUPT_REGISTER_ONEr,
    IPS_INTERRUPT_MASK_REGISTERr,
    IPT_ECC_ERR_INTERRUPT_MASK_REGISTERr,
    IPT_INTERRUPT_MASK_REGISTERr,
    IQM_ECC_INTERRUPT_REGISTER_MASKr,
    IQM_INTERRUPT_MASK_REGISTERr,
    IRE_ECC_INTERRUPT_REGISTER_MASKr,
    IRE_INTERRUPT_MASK_REGISTERr,
    IRR_ECC_INTERRUPT_REGISTER_MASKr,
    IRR_INTERRUPT_MASK_REGISTERr,
    MMU_ECC_INTERRUPT_REGISTER_MASKr,
    MMU_INTERRUPT_MASK_REGISTERr,
    NBI_ECC_INTERRUPT_MASK_REGISTERr,
    NBI_ILKN_INTERRUPT_MASK_REGISTERr,
    NBI_INTERRUPT_MASK_REGISTERr,
    NBI_LINK_STATUS_CHANGE_INTERRUPT_MASK_REGISTERr,
    NBI_NBI_THROWN_BURSTS_COUNTERS_75P_INTERRUPT_MASK_REGISTERr,
    NBI_STAT_CNT_75P_INTERRUPT_MASK_REGISTERr,
    NBI_STAT_INTERRUPT_MASK_REGISTERr,
    OAMP_ECC_INTERRUPT_REGISTER_MASKr,
    OAMP_INTERRUPT_MASK_REGISTERr,
    OAMP_RMAPEM_INTERRUPT_MASK_REGISTER_ONEr,
    OLP_ECC_INTERRUPT_REGISTER_MASKr,
    OLP_INTERRUPT_MASK_REGISTERr,
    RTP_INTERRUPT_MASK_REGISTERr,
    SCH_ECC_1B_ERR_INTERRUPT_MASK_REGISTERr,
    SCH_ECC_2B_ERR_INTERRUPT_MASK_REGISTERr,
    SCH_INTERRUPT_MASK_REGISTERr,
    SCH_PAR_ERR_INTERRUPT_MASK_REGISTERr,
    NUM_SOC_REG
};

static
soc_reg_t ardon_interrupts_mask_registers[] = {
    CFC_INTERRUPT_MASK_REGISTERr,
    CRPS_INTERRUPT_MASK_REGISTERr,
    CRPS_SRC_INVLID_ACCESS_INTERRUPT_REGISTER_MASKr,
    CRPS_SRC_CMD_WAS_FILTERED_INTERRUPT_REGISTER_MASKr,
    CRPS_PAR_ERR_INTERRUPT_MASK_REGISTERr,
    DRCC_INTERRUPT_MASK_REGISTERr,
    
    
    
    
    
    
    
    
    EGQ_INTERRUPT_MASK_REGISTERr,
    EGQ_ERPP_DISCARD_INT_REG_MASKr,
    EGQ_ERPP_DISCARD_INT_REG_MASK_2r,
    EGQ_PKT_REAS_INT_REG_MASKr,
    EGQ_ECC_INTERRUPT_REGISTER_MASKr,
    EPNI_INTERRUPT_MASK_REGISTERr,
    EPNI_PP_INT_REG_MASKr,
    
    EPNI_ECC_INTERRUPT_REGISTER_MASKr,
    FCR_INTERRUPT_MASK_REGISTERr,
    FCT_INTERRUPT_MASK_REGISTERr,
    FDR_INTERRUPT_MASK_REGISTERr,
    FDR_INTERRUPT_MASK_REGISTER_1r,
    FDR_INTERRUPT_MASK_REGISTER_2r,
    FDR_INTERRUPT_MASK_REGISTER_3r,
    FDT_INTERRUPT_MASK_REGISTERr,
    FMAC_INTERRUPT_MASK_REGISTERr,
    FMAC_INTERRUPT_MASK_REGISTER_1r,
    FMAC_INTERRUPT_MASK_REGISTER_2r,
    FMAC_INTERRUPT_MASK_REGISTER_3r,
    FMAC_INTERRUPT_MASK_REGISTER_4r,
    FMAC_INTERRUPT_MASK_REGISTER_5r,
    FMAC_INTERRUPT_MASK_REGISTER_6r,
    FMAC_INTERRUPT_MASK_REGISTER_7r,
    FMAC_INTERRUPT_MASK_REGISTER_8r,
    FMAC_INTERRUPT_MASK_REGISTER_9r,
    FSRD_INTERRUPT_MASK_REGISTERr,
    FSRD_QUAD_INTERRUPT_MASK_REGISTERr,
    IDR_INTERRUPT_MASK_REGISTERr,
    IDR_REASSEMBLY_INTERRUPT_REGISTER_MASKr,
    IDR_ECC_INTERRUPT_REGISTER_MASKr,
    IHB_INTERRUPT_MASK_REGISTERr,
    IHP_INTERRUPT_MASK_REGISTERr,
    IPS_INTERRUPT_MASK_REGISTERr,
    IPT_INTERRUPT_MASK_REGISTERr,
    IPT_ECC_ERR_INTERRUPT_MASK_REGISTERr,
    IQM_INTERRUPT_MASK_REGISTERr,
    IQM_ECC_INTERRUPT_REGISTER_MASKr,
    IRE_INTERRUPT_MASK_REGISTERr,
    IRE_ECC_INTERRUPT_REGISTER_MASKr,
    IRR_INTERRUPT_MASK_REGISTERr,
    IRR_ECC_INTERRUPT_REGISTER_MASKr,
    MMU_INTERRUPT_MASK_REGISTERr,
    MMU_ECC_INTERRUPT_REGISTER_MASKr,
    OCB_INTERRUPT_MASK_REGISTERr,
    OCB_ECC_INTERRUPT_REGISTER_MASKr,
    RTP_INTERRUPT_MASK_REGISTERr,
    SCH_INTERRUPT_MASK_REGISTERr,
    SCH_ECC_1B_ERR_INTERRUPT_MASK_REGISTERr,
    SCH_ECC_2B_ERR_INTERRUPT_MASK_REGISTERr,
    SCH_PAR_ERR_INTERRUPT_MASK_REGISTERr,
    NUM_SOC_REG
};

static
soc_reg_t arad_interrupts_dram_mask_registers[] = {
    DRCA_INTERRUPT_MASK_REGISTERr,
    DRCB_INTERRUPT_MASK_REGISTERr,
    DRCC_INTERRUPT_MASK_REGISTERr,
    DRCD_INTERRUPT_MASK_REGISTERr,
    DRCE_INTERRUPT_MASK_REGISTERr,
    DRCF_INTERRUPT_MASK_REGISTERr,
    DRCG_INTERRUPT_MASK_REGISTERr,
    DRCH_INTERRUPT_MASK_REGISTERr,
    NUM_SOC_REG
};

static
soc_reg_t arad_interrupt_monitor_mem_reg[] = {
    CFC_PARITY_ERR_MONITOR_MEM_MASKr,
    CRPS_PAR_ERR_MEM_MASKr,
    EGQ_ECC_ERR_1B_MONITOR_MEM_MASKr,
    EGQ_ECC_ERR_2B_MONITOR_MEM_MASKr,
    EPNI_ECC_ERR_1B_MONITOR_MEM_MASKr,
    EPNI_ECC_ERR_2B_MONITOR_MEM_MASKr,
    FCR_ECC_1B_ERR_MONITOR_MEM_MASKr,
    FCR_ECC_2B_ERR_MONITOR_MEM_MASKr,
    FDR_ECC_1B_ERR_MONITOR_MEM_MASK_PRIMARYr,
    FDR_ECC_1B_ERR_MONITOR_MEM_MASK_SECONDARYr,
    FDR_ECC_2B_ERR_MONITOR_MEM_MASK_PRIMARYr,
    FDR_ECC_2B_ERR_MONITOR_MEM_MASK_SECONDARYr,
    FDT_ECC_1B_ERR_MONITOR_MEM_MASKr,
    FDT_ECC_2B_ERR_MONITOR_MEM_MASKr,
    FDT_PARITY_ERR_MONITOR_MEM_MASKr,
    FMAC_ECC_1B_ERR_MONITOR_MEM_MASKr,
    FMAC_ECC_2B_ERR_MONITOR_MEM_MASKr,
    IDR_ECC_ERR_1B_MONITOR_MEM_MASKr,
    IDR_ECC_ERR_2B_MONITOR_MEM_MASKr,
    IDR_PAR_ERR_MEM_MASKr,
    IHP_ECC_ERR_1B_MONITOR_MEM_MASKr,
    IHP_ECC_ERR_2B_MONITOR_MEM_MASKr,
    IPS_ECC_ERR_1B_MONITOR_MEM_MASKr,
    IPS_ECC_ERR_2B_MONITOR_MEM_MASKr,
    IPS_PAR_ERR_MEM_MASKr,
    IPT_ECC_ERR_1B_MONITOR_MEM_MASKr,
    IPT_ECC_ERR_2B_MONITOR_MEM_MASKr,
    IPT_PAR_ERR_MEM_MASKr,
    IQM_ECC_ERR_1B_MONITOR_MEM_MASKr,
    IQM_ECC_ERR_2B_MONITOR_MEM_MASKr,
    IQM_PAR_ERR_MEM_MASKr,
    IRE_ECC_ERR_1B_MONITOR_MEM_MASKr,
    IRE_ECC_ERR_2B_MONITOR_MEM_MASKr,
    IRE_PAR_ERR_MEM_MASKr,
    IRR_ECC_ERR_1B_MONITOR_MEM_MASKr,
    IRR_ECC_ERR_2B_MONITOR_MEM_MASKr,
    IRR_PAR_ERR_MEM_MASKr,
    MMU_ECC_ERR_1B_MONITOR_MEM_MASK_1r,
    MMU_ECC_ERR_1B_MONITOR_MEM_MASK_2r,
    MMU_ECC_ERR_2B_MONITOR_MEM_MASK_1r,
    MMU_ECC_ERR_2B_MONITOR_MEM_MASK_2r,
    OAMP_ECC_ERR_MONITOR_MEM_MASKr,
    OLP_PAR_ERR_MEM_MASKr,
    IHP_PAR_0_ERR_MEM_MASKr,
    IHP_PAR_1_ERR_MEM_MASKr,
    IHP_PAR_3_ERR_MEM_MASKr,
    IHP_PAR_ERR_MONITOR_MEM_MASKr,
    NBI_ECC_ERR_1B_MONITOR_MEM_MASKr,
    NBI_ECC_ERR_2B_MONITOR_MEM_MASKr,
    OAMP_PAR_0_ERR_MONITOR_MEM_MASKr,
    EGQ_PAR_ERR_MEM_MASKr,
    NBI_PARITY_ERR_MONITOR_MEM_MASKr,
    IHB_PAR_ERR_MEM_MASKr,
    EPNI_PAR_ERR_MEM_MASKr,
    INVALIDr
};

static
soc_reg_t ardon_interrupt_monitor_mem_reg[] = {
    CFC_PARITY_ERR_MONITOR_MEM_MASKr,
    CRPS_PAR_ERR_MEM_MASKr,
    EGQ_ECC_ERR_1B_MONITOR_MEM_MASKr,
    EGQ_ECC_ERR_2B_MONITOR_MEM_MASKr,
    EGQ_PAR_ERR_MEM_MASKr,
    EPNI_ECC_ERR_1B_MONITOR_MEM_MASKr,
    EPNI_ECC_ERR_2B_MONITOR_MEM_MASKr,
    EPNI_PAR_ERR_MEM_MASKr,
    FCR_ECC_1B_ERR_MONITOR_MEM_MASKr,
    FCR_ECC_2B_ERR_MONITOR_MEM_MASKr,
    FDR_ECC_1B_ERR_MONITOR_MEM_MASK_PRIMARYr,
    FDR_ECC_2B_ERR_MONITOR_MEM_MASK_PRIMARYr,
    FDR_ECC_1B_ERR_MONITOR_MEM_MASK_SECONDARYr,
    FDR_ECC_2B_ERR_MONITOR_MEM_MASK_SECONDARYr,
    FDT_ECC_1B_ERR_MONITOR_MEM_MASKr,
    FDT_ECC_2B_ERR_MONITOR_MEM_MASKr,
    FDT_PARITY_ERR_MONITOR_MEM_MASKr,
    FMAC_ECC_1B_ERR_MONITOR_MEM_MASKr,
    FMAC_ECC_2B_ERR_MONITOR_MEM_MASKr,
    IDR_ECC_ERR_1B_MONITOR_MEM_MASKr,
    IDR_ECC_ERR_2B_MONITOR_MEM_MASKr,
    IDR_PAR_ERR_MEM_MASKr,
    IHB_PAR_ERR_MEM_MASKr,
    IHP_PAR_ERR_MONITOR_MEM_MASKr,
    IPS_ECC_ERR_2B_MONITOR_MEM_MASKr,
    IPS_ECC_ERR_1B_MONITOR_MEM_MASKr,
    IPS_PAR_ERR_MEM_MASKr,
    IPT_ECC_ERR_2B_MONITOR_MEM_MASKr,
    IPT_ECC_ERR_1B_MONITOR_MEM_MASKr,
    IPT_PAR_ERR_MEM_MASKr,
    IQM_ECC_ERR_1B_MONITOR_MEM_MASKr,
    IQM_ECC_ERR_2B_MONITOR_MEM_MASKr,
    IQM_PAR_ERR_MEM_MASKr,
    IRE_ECC_ERR_1B_MONITOR_MEM_MASKr,
    IRE_ECC_ERR_2B_MONITOR_MEM_MASKr,
    IRE_PAR_ERR_MEM_MASKr,
    IRR_ECC_ERR_1B_MONITOR_MEM_MASKr,
    IRR_ECC_ERR_2B_MONITOR_MEM_MASKr,
    IRR_PAR_ERR_MEM_MASKr,
    MMU_ECC_ERR_2B_MONITOR_MEM_MASK_1r,
    MMU_ECC_ERR_2B_MONITOR_MEM_MASK_2r,
    MMU_ECC_ERR_1B_MONITOR_MEM_MASK_1r,
    MMU_ECC_ERR_1B_MONITOR_MEM_MASK_2r,
    NBI_ECC_ERR_1B_MONITOR_MEM_MASKr,
    NBI_ECC_ERR_2B_MONITOR_MEM_MASKr,
    NBI_PARITY_ERR_MONITOR_MEM_MASKr,
    OCB_ECC_ERR_2B_MONITOR_MEM_MASKr,
    OCB_ECC_ERR_1B_MONITOR_MEM_MASKr,
    INVALIDr
};








uint32
arad_init_operation_mode_set(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  ARAD_MGMT_INIT             *init
    )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  
  arad_sw_db_tdm_mode_set(
          unit,
          init->tdm_mode
        );

  arad_sw_db_ilkn_tdm_dedicated_queuing_set(
          unit,
          init->ilkn_tdm_dedicated_queuing
        );

  arad_sw_db_is_petrab_in_system_set(
          unit,
          init->is_petrab_in_system
        );


  ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_init_operation_mode_set()", 0, 0);
}

uint32
arad_init_pdm_nof_entries_calc(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  ARAD_INIT_PDM_MODE         pdm_mode,
    SOC_SAND_OUT uint32                     *pdm_nof_entries)
{
    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_INIT_DRAM_NOF_BUFFS_CALC);

    if(pdm_mode == ARAD_INIT_PDM_MODE_SIMPLE) {
        *pdm_nof_entries = SOC_DPP_DEFS_GET(unit, pdm_size); 
    } else { 
        *pdm_nof_entries = SOC_DPP_DEFS_GET(unit, pdm_size)*3/4; 
    }

    ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_init_pdm_nof_entries_calc()", 0, 0);
}

uint32
  arad_init_dram_fbc_buffs_get(
    SOC_SAND_IN  uint32  buffs_without_fbc,
    SOC_SAND_IN  uint32  buff_size_bytes,
    SOC_SAND_OUT uint32 *fbc_nof_bufs
  )
{
  uint32
    fbcs_for_buff,
    nof_fbc_buffs;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(ARAD_INIT_DRAM_FBC_BUFFS_GET);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(
    buff_size_bytes, ARAD_ITM_DBUFF_SIZE_BYTES_MIN, ARAD_ITM_DBUFF_SIZE_BYTES_MAX,
    ARAD_ITM_DRAM_BUF_SIZE_OUT_OF_RANGE_ERR, 10, exit
  );

  fbcs_for_buff = SOC_SAND_DIV_ROUND_UP(buff_size_bytes, ARAD_INIT_DRAM_BYTES_FOR_FBC);
  nof_fbc_buffs = SOC_SAND_DIV_ROUND_UP(buffs_without_fbc, fbcs_for_buff * ARAD_INIT_DRAM_FBC_SEQUENCE_SIZE);

  if (nof_fbc_buffs == 0) {
      nof_fbc_buffs = 1;
  }

  *fbc_nof_bufs = nof_fbc_buffs;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "arad_init_dram_fbc_buffs_get()", 0, 0);
}

STATIC uint32
  arad_init_dram_max_without_fbc_get(
    SOC_SAND_IN  uint32  buffs_with_fbc,
    SOC_SAND_IN  uint32  buff_size_bytes,
    SOC_SAND_OUT uint32 *buffs_without_fbc
  )
{
  SOC_SAND_U64
    dividend,
    buffs_no_fbc;
  uint32
    divisor;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(ARAD_INIT_DRAM_MAX_WITHOUT_FBC_GET);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(
    buff_size_bytes, ARAD_ITM_DBUFF_SIZE_BYTES_MIN, ARAD_ITM_DBUFF_SIZE_BYTES_MAX,
    ARAD_ITM_DRAM_BUF_SIZE_OUT_OF_RANGE_ERR, 10, exit
  );

  soc_sand_u64_multiply_longs(
    buffs_with_fbc,
    (ARAD_INIT_DRAM_FBC_SEQUENCE_SIZE * buff_size_bytes),
    &dividend
  );

  divisor = ARAD_INIT_DRAM_FBC_SEQUENCE_SIZE * buff_size_bytes + ARAD_INIT_DRAM_BYTES_FOR_FBC;

  soc_sand_u64_devide_u64_long(
    &dividend,
    divisor,
    &buffs_no_fbc
  );

  soc_sand_u64_to_long(&buffs_no_fbc, buffs_without_fbc);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "arad_init_dram_max_without_fbc_get()", 0, 0);
}

STATIC uint32 arad_init_dram_buff_boundaries_calc(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                    total_buffs,
    SOC_SAND_IN  ARAD_ITM_DBUFF_SIZE_BYTES   dbuff_size,
    SOC_SAND_IN  ARAD_INIT_OCB   ocb,
    SOC_SAND_OUT ARAD_INIT_DBUFFS_BDRY       *dbuffs
  )
{
  uint32
    res,
    fmc_size,
    uc_plus_fbc,
    uc_no_fbc = 0,
    uc_no_fbc_orig,
    ocb_fmc_fbc_size,
    ocb_uc_fbc_size,
    fmc_fbc_size,
    mmc_fbc_size,
    uc_fbc_size,
    ocb_fmc_size,
    ocb_uc_size,
    dbuff_ocb_size;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_INIT_DRAM_BUFF_BOUNDARIES_CALC);

  
  if (SOC_DPP_CONFIG(unit)->arad->init.dram.fmc_dbuff_mode == ARAD_INIT_FMC_64_REP_128K_DBUFF_MODE) {
      fmc_size = 128*1024;
  } else {
      
      fmc_size = 64*1024;
  }

  
  dbuff_ocb_size = (SOC_DPP_DEFS_GET(unit, ocb_memory_size) / 8) * (1024 * 1024/ SOC_DPP_CONFIG(unit)->arad->init.ocb.databuffer_size);

  if (ocb.ocb_enable != OCB_DISABLED) {
      if (ocb.repartition_mode == ARAD_OCB_REPARTITION_MODE_80_PRESENTS_UNICAST) {
        ocb_fmc_size = dbuff_ocb_size*2/10;
        ocb_uc_size = dbuff_ocb_size*8/10;
      } else {
        ocb_fmc_size = 1;
        ocb_uc_size = dbuff_ocb_size - 1;
      }
  } else {
      ocb_fmc_size = 1;
      ocb_uc_size = 1;
  }

  

  dbuffs->ocb_fmc.start  = 0;
  dbuffs->ocb_fmc.end    = SOC_SAND_RNG_LAST(dbuffs->ocb_fmc.start, ocb_fmc_size);

  dbuffs->fmc.start      = SOC_SAND_RNG_NEXT(dbuffs->ocb_fmc.end);
  dbuffs->fmc.end        = SOC_SAND_RNG_LAST(dbuffs->ocb_fmc.start, fmc_size - ocb_fmc_size);

  
  dbuffs->mmc.start  = SOC_SAND_RNG_NEXT(dbuffs->fmc.end);
  dbuffs->mmc.end    = SOC_SAND_RNG_LAST(dbuffs->mmc.start, ARAD_ITM_DBUFF_MMC_MAX);

  
  res = arad_init_dram_fbc_buffs_get(
          SOC_SAND_RNG_COUNT(dbuffs->ocb_fmc.end, dbuffs->ocb_fmc.start),
          dbuff_size,
          &ocb_fmc_fbc_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 9, exit);

  res = arad_init_dram_fbc_buffs_get(
          SOC_SAND_RNG_COUNT(dbuffs->fmc.end, dbuffs->fmc.start),
          dbuff_size,
          &fmc_fbc_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_init_dram_fbc_buffs_get(
          SOC_SAND_RNG_COUNT(dbuffs->mmc.end, dbuffs->mmc.start),
          dbuff_size,
          &mmc_fbc_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  
  dbuffs->ocb_uc.start   = SOC_SAND_RNG_NEXT(dbuffs->mmc.end);
  dbuffs->ocb_uc.end = SOC_SAND_RNG_LAST(dbuffs->ocb_uc.start, ocb_uc_size);

  res = arad_init_dram_fbc_buffs_get(
      SOC_SAND_RNG_COUNT(dbuffs->ocb_uc.end, dbuffs->ocb_uc.start),
      dbuff_size,
      &ocb_uc_fbc_size);
  SOC_SAND_CHECK_FUNC_RESULT(res, 21, exit);

  dbuffs->fbc_fmc.end    = total_buffs - 1;
  dbuffs->fbc_fmc.start  = SOC_SAND_RNG_FIRST(dbuffs->fbc_fmc.end, fmc_fbc_size);

  dbuffs->fbc_ocb_fmc.end    = SOC_SAND_RNG_PREV(dbuffs->fbc_fmc.start);
  dbuffs->fbc_ocb_fmc.start  = SOC_SAND_RNG_FIRST(dbuffs->fbc_ocb_fmc.end, ocb_fmc_fbc_size);

  dbuffs->fbc_mmc.end    = SOC_SAND_RNG_PREV(dbuffs->fbc_ocb_fmc.start);
  dbuffs->fbc_mmc.start  = SOC_SAND_RNG_FIRST(dbuffs->fbc_mmc.end, mmc_fbc_size);

  uc_plus_fbc = dbuffs->fbc_mmc.start - dbuffs->ocb_uc.end - ocb_uc_fbc_size - 1;

  res = arad_init_dram_max_without_fbc_get(
          uc_plus_fbc,
          dbuff_size,
          &uc_no_fbc
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = arad_init_dram_fbc_buffs_get(
          uc_no_fbc,
          dbuff_size,
          &uc_fbc_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  
  uc_no_fbc_orig = uc_no_fbc;
  uc_no_fbc -= ARAD_INIT_DRAM_BUFF_TO_FBC_DELTA_MIN;

  if (uc_no_fbc + uc_fbc_size > uc_plus_fbc)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_DRAM_BUFFERS_UC_FBC_OVERFLOW_ERR, 60, exit);
  }

  dbuffs->uc.start   = SOC_SAND_RNG_NEXT(dbuffs->ocb_uc.end);
  dbuffs->uc.end     = SOC_SAND_RNG_LAST(dbuffs->uc.start, uc_no_fbc);

  
  dbuffs->fbc_ocb_uc.start    = SOC_SAND_RNG_NEXT(SOC_SAND_RNG_LAST(dbuffs->uc.start, uc_no_fbc_orig));
  dbuffs->fbc_ocb_uc.end  = SOC_SAND_RNG_LAST(dbuffs->fbc_ocb_uc.start, ocb_uc_fbc_size);

  dbuffs->fbc_uc.start  = SOC_SAND_RNG_NEXT(dbuffs->fbc_ocb_uc.end);
  dbuffs->fbc_uc.end    = SOC_SAND_RNG_LAST(dbuffs->fbc_uc.start, uc_fbc_size);

  if (dbuffs->fbc_uc.end >= dbuffs->fbc_mmc.start)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_DRAM_BUFFERS_FBC_OVERFLOW_ERR, 70, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "arad_init_dram_buff_boundaries_calc()", 0, 0);
}

STATIC uint32 arad_init_dram_buff_boundaries_calc_ocb_only(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                    total_buffs,
    SOC_SAND_IN  ARAD_ITM_DBUFF_SIZE_BYTES   dbuff_size,
    SOC_SAND_IN  ARAD_OCB_REPARTITION_MODE   repartition_mode,
    SOC_SAND_OUT ARAD_INIT_DBUFFS_BDRY       *dbuffs)
{
    uint32
        dbuff_ocb_size,
        ocb_fmc_size,
        ocb_uc_size;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    dbuff_ocb_size = SOC_SAND_MIN((SOC_DPP_DEFS_GET(unit, ocb_memory_size) / 8) * (1024 * 1024/ SOC_DPP_CONFIG(unit)->arad->init.ocb.databuffer_size),
                                  ARAD_ITM_DBUFF_CACHE - 300);

    if (repartition_mode == ARAD_OCB_REPARTITION_MODE_80_PRESENTS_UNICAST) {
        ocb_fmc_size = (dbuff_ocb_size * 2) / 10;
        ocb_uc_size  = (dbuff_ocb_size * 8) / 10;
    } else {
        ocb_fmc_size = 1;
        ocb_uc_size = dbuff_ocb_size - 1;
    }

    
    dbuffs->ocb_fmc.start  = 0;
    dbuffs->ocb_fmc.end    = SOC_SAND_RNG_LAST(dbuffs->ocb_fmc.start, ocb_fmc_size);

    
    dbuffs->ocb_uc.start   = SOC_SAND_RNG_NEXT(dbuffs->ocb_fmc.end);
    dbuffs->ocb_uc.end = SOC_SAND_RNG_LAST(dbuffs->ocb_uc.start, ocb_uc_size);

    

    dbuffs->fbc_ocb_fmc.start   = SOC_SAND_RNG_NEXT(dbuffs->ocb_uc.end);
    dbuffs->fbc_ocb_fmc.end = SOC_SAND_RNG_LAST(dbuffs->fbc_ocb_fmc.start, 1);

    dbuffs->fbc_ocb_uc.start    = SOC_SAND_RNG_NEXT(dbuffs->fbc_ocb_fmc.end);
    dbuffs->fbc_ocb_uc.end  = SOC_SAND_RNG_LAST(dbuffs->fbc_ocb_uc.start, 1);

    SOC_SAND_EXIT_AND_SEND_ERROR( "arad_init_dram_buff_boundaries_calc_ocb_only()", unit, total_buffs);
}

STATIC uint32 arad_init_dram_buff_boundaries_calc_ocb_only_1_dram(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                    total_buffs,
    SOC_SAND_IN  ARAD_ITM_DBUFF_SIZE_BYTES   dbuff_size,
    SOC_SAND_IN  ARAD_OCB_REPARTITION_MODE   repartition_mode,
    SOC_SAND_OUT ARAD_INIT_DBUFFS_BDRY       *dbuffs)
{
    uint32
        res,
        ocb_fmc_fbc_size,
        ocb_uc_fbc_size,
        ocb_fmc_size,
        ocb_uc_size,
        dbuff_ocb_size;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    dbuff_ocb_size = (SOC_DPP_DEFS_GET(unit, ocb_memory_size) *  (1024 * 1024)) / (8 * SOC_DPP_CONFIG(unit)->arad->init.ocb.databuffer_size);

    if (repartition_mode == ARAD_OCB_REPARTITION_MODE_80_PRESENTS_UNICAST) {
        ocb_fmc_size = dbuff_ocb_size*2/10;
        ocb_uc_size = dbuff_ocb_size*8/10;
    } else {
        ocb_fmc_size = 1;
        ocb_uc_size = dbuff_ocb_size - 1;
    }


    
    dbuffs->ocb_fmc.start  = 0;
    dbuffs->ocb_fmc.end    = SOC_SAND_RNG_LAST(dbuffs->ocb_fmc.start, ocb_fmc_size);

    
    dbuffs->ocb_uc.start   = SOC_SAND_RNG_NEXT(dbuffs->ocb_fmc.end);
    dbuffs->ocb_uc.end = SOC_SAND_RNG_LAST(dbuffs->ocb_uc.start, ocb_uc_size);

    
    res = arad_init_dram_fbc_buffs_get(SOC_SAND_RNG_COUNT(dbuffs->ocb_fmc.end, dbuffs->ocb_fmc.start), dbuff_size, &ocb_fmc_fbc_size);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    dbuffs->fbc_ocb_fmc.start   = SOC_SAND_RNG_NEXT(dbuffs->ocb_uc.end);
    dbuffs->fbc_ocb_fmc.end = SOC_SAND_RNG_LAST(dbuffs->fbc_ocb_fmc.start, ocb_fmc_fbc_size);

    res = arad_init_dram_fbc_buffs_get(SOC_SAND_RNG_COUNT(dbuffs->ocb_uc.end, dbuffs->ocb_uc.start), dbuff_size, &ocb_uc_fbc_size);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    dbuffs->fbc_ocb_uc.start    = SOC_SAND_RNG_NEXT(dbuffs->fbc_ocb_fmc.end);
    dbuffs->fbc_ocb_uc.end  = SOC_SAND_RNG_LAST(dbuffs->fbc_ocb_uc.start, ocb_uc_fbc_size);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "arad_init_dram_buff_boundaries_calc_ocb_only_1_dram()", unit, total_buffs);
}


STATIC uint32 arad_init_serdes_pll_set(
    SOC_SAND_IN int                         unit,
    SOC_SAND_IN ARAD_INIT_SERDES_TYPE             serdes_type,
    SOC_SAND_IN soc_dcmn_init_serdes_ref_clock_t  ref_clock)
{

    uint32
        val,
        res;
    soc_reg_above_64_val_t
        reg_above_64,
        field_above_64;
    int32
        srd_ndx;
    const static soc_reg_t
        eci_srd_pll_config[] = {ECI_SRD_0_PLL_CONFIGr, ECI_SRD_1_PLL_CONFIGr};
    const static soc_reg_t
        eci_srd_pll_status[] = {ECI_SRD_0_PLL_STATUSr, ECI_SRD_1_PLL_STATUSr};
    const static soc_field_t
        eci_srd_pll_locked[] = {SRD_0_PLL_LOCKEDf, SRD_1_PLL_LOCKEDf};

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_INIT_SERDES_PLL_SET);

     if (serdes_type == ARAD_INIT_SERDES_TYPE_FABRIC) {
        srd_ndx = 0;
    } else {
        srd_ndx = 1;
    }

    
    SOC_REG_ABOVE_64_CLEAR(reg_above_64);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, soc_reg_above_64_set(unit, eci_srd_pll_config[srd_ndx], REG_PORT_ANY, 0, reg_above_64));

    
    SOC_REG_ABOVE_64_CLEAR(field_above_64);
    val = 25;
    SHR_BITCOPY_RANGE(field_above_64, ARAD_MGMT_INIT_DDR_LCPLL_CFG_I_NDIV_INT_START, &val, 0, ARAD_MGMT_INIT_DDR_LCPLL_CFG_I_NDIV_INT_LENGTH);

    if (ref_clock == soc_dcmn_init_serdes_ref_clock_125) {
        val = 25;
    } else if (ref_clock == soc_dcmn_init_serdes_ref_clock_156_25) {
        val = 20;
    }
    SHR_BITCOPY_RANGE(field_above_64, ARAD_MGMT_INIT_DDR_LCPLL_CFG_I_CH0_MDIV_START, &val, 0, ARAD_MGMT_INIT_DDR_LCPLL_CFG_I_CH0_MDIV_LENGTH);

    val = 8;
    SHR_BITCOPY_RANGE(field_above_64, ARAD_MGMT_INIT_DDR_LCPLL_CFG_I_KP_START, &val, 0, ARAD_MGMT_INIT_DDR_LCPLL_CFG_I_KP_LENGTH);

    val = 1;
    SHR_BITCOPY_RANGE(field_above_64, ARAD_MGMT_INIT_DDR_LCPLL_CFG_I_KI_START, &val, 0, ARAD_MGMT_INIT_DDR_LCPLL_CFG_I_KI_LENGTH);

    val = 4;
    SHR_BITCOPY_RANGE(field_above_64, ARAD_MGMT_INIT_DDR_LCPLL_CFG_I_KA_START, &val, 0, ARAD_MGMT_INIT_DDR_LCPLL_CFG_I_KA_LENGTH);

    val = 1;
    SHR_BITCOPY_RANGE(field_above_64, ARAD_MGMT_INIT_DDR_LCPLL_CFG_I_PDIV_START, &val, 0, ARAD_MGMT_INIT_DDR_LCPLL_CFG_I_PDIV_LENGTH);

    val = 0x15440000;
    SHR_BITCOPY_RANGE(field_above_64, ARAD_MGMT_INIT_DDR_LCPLL_CFG_I_PLL_CTRL_START, &val, 0, ARAD_MGMT_INIT_DDR_LCPLL_CFG_I_PLL_CTRL_LENGTH);

    soc_reg_above_64_field_set(unit, ECI_SRD_0_PLL_CONFIGr, reg_above_64, SRD_PLL_CTRLf, field_above_64);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, soc_reg_above_64_set(unit, eci_srd_pll_config[srd_ndx], REG_PORT_ANY, 0, reg_above_64));

    sal_usleep(10);

    
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, soc_reg_above_64_get(unit, eci_srd_pll_config[srd_ndx], REG_PORT_ANY, 0, reg_above_64));
    SOC_REG_ABOVE_64_CLEAR(field_above_64);
    SOC_REG_ABOVE_64_CREATE_MASK(field_above_64, 1, 0);
    soc_reg_above_64_field_set(unit, ECI_SRD_0_PLL_CONFIGr, reg_above_64, RESETBf, field_above_64);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, soc_reg_above_64_set(unit, eci_srd_pll_config[srd_ndx], REG_PORT_ANY, 0, reg_above_64));

    
    res = arad_polling(
        unit,
        ARAD_TIMEOUT,
        ARAD_MIN_POLLS,
        eci_srd_pll_status[srd_ndx],
        REG_PORT_ANY,
        0,
        eci_srd_pll_locked[srd_ndx],
        1);
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

    
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 70, exit, soc_reg_above_64_get(unit, eci_srd_pll_config[srd_ndx], REG_PORT_ANY, 0, reg_above_64));
    SOC_REG_ABOVE_64_CLEAR(field_above_64);
    SOC_REG_ABOVE_64_CREATE_MASK(field_above_64, 1, 0);
    soc_reg_above_64_field_set(unit, eci_srd_pll_config[srd_ndx], reg_above_64, POST_RESETBf, field_above_64);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 80, exit, soc_reg_above_64_set(unit, eci_srd_pll_config[srd_ndx], REG_PORT_ANY, 0, reg_above_64));

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_init_serdes_pll_set()", serdes_type, ref_clock);
}

STATIC uint32 arad_mgmt_init_ddr_configure(
    SOC_SAND_IN  int          unit,
    SOC_SAND_IN  int32           ddr_id,
    SOC_SAND_IN  uint32           dram_freq,
    SOC_SAND_IN  uint32           synt_dram_freq)
{
    uint32
        res,
        fld_val,
        m,
        m_best = 0,
        n,
        n_best = 0,
        ki,
        kp,
        vco_div2 = 0,
        freq_diff_best = SOC_SAND_U32_MAX,
        f_vco;
    int32
        freq_diff;
    static soc_reg_t
        eci_ddr_pll_config[] = {ECI_DDR_0_PLL_CONFIGr, ECI_DDR_1_PLL_CONFIGr, ECI_DDR_2_PLL_CONFIGr, ECI_DDR_3_PLL_CONFIGr};
    soc_reg_above_64_val_t
        reg_above_64;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_MGMT_INIT_DDR_CONFIGURE);

    SOC_REG_ABOVE_64_CLEAR(reg_above_64);

    

    

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, soc_reg_above_64_get(unit, eci_ddr_pll_config[ddr_id], REG_PORT_ANY, 0, reg_above_64));

    
    if (synt_dram_freq != ARAD_INIT_DDR_FREQUENCY_25_MHZ && synt_dram_freq != ARAD_INIT_DDR_FREQUENCY_125_MHZ) {
        
        SOC_SAND_SET_ERROR_CODE(ARAD_MGMT_INIT_INVALID_FREQUENCY, 20, exit);
    }

    fld_val = synt_dram_freq/ARAD_MGMT_INIT_DDR_PLL_FREF_EFF;
    SHR_BITCOPY_RANGE(reg_above_64, ARAD_MGMT_INIT_DDR_PLL_CFG_I_PDIV_START, &fld_val, 0, ARAD_MGMT_INIT_DDR_PLL_CFG_I_PDIV_LENGTH);

    
    
    for (n = ARAD_MGMT_INIT_DDR_PLL_CFG_I_NDIV_MIN; n <= ARAD_MGMT_INIT_DDR_PLL_CFG_I_NDIV_MAX; ++n) {
        m = ARAD_MGMT_INIT_DDR_PLL_FREF_EFF*n/dram_freq;
        if(m == 0) {
            continue;
        }
        if(m > SOC_SAND_U8_MAX) {
            break;
        }
        ARAD_INIT_CLAC_M_N_AND_DIFF(dram_freq, freq_diff, m, n, freq_diff_best, m_best, n_best);
        if(freq_diff_best == 0)
            break;
        if( (ARAD_MGMT_INIT_DDR_PLL_FREF_EFF*n)%dram_freq != 0) {
            ARAD_INIT_CLAC_M_N_AND_DIFF(dram_freq, freq_diff, m + 1, n, freq_diff_best, m_best, n_best);
            if(freq_diff_best == 0)
                break;
        }
    }

    
    fld_val = n_best;
    SHR_BITCOPY_RANGE(reg_above_64, ARAD_MGMT_INIT_DDR_PLL_CFG_I_NDIV_INT_START, &fld_val, 0, ARAD_MGMT_INIT_DDR_PLL_CFG_I_NDIV_INT_LENGTH);

    
    fld_val = m_best;
    SHR_BITCOPY_RANGE(reg_above_64, ARAD_MGMT_INIT_DDR_PLL_CFG_I_CH0_MDIV_START, &fld_val, 0, ARAD_MGMT_INIT_DDR_PLL_CFG_I_CH0_MDIV_LENGTH);

    f_vco = ARAD_MGMT_INIT_DDR_PLL_FREF_EFF*n_best;

    
    if (f_vco <= 3200) {
        ki = 0x3;
        kp = 0x7;
        vco_div2 = 0x0;
    } else {
        ki = 0x4;
        kp = 0x9;
        vco_div2 = 0x1;
    }
    SHR_BITCOPY_RANGE(reg_above_64, ARAD_MGMT_INIT_DDR_PLL_CFG_I_KI_START, &ki, 0, ARAD_MGMT_INIT_DDR_PLL_CFG_I_KI_LENGTH);
    SHR_BITCOPY_RANGE(reg_above_64, ARAD_MGMT_INIT_DDR_PLL_CFG_I_KP_START, &kp, 0, ARAD_MGMT_INIT_DDR_PLL_CFG_I_KP_LENGTH);
    SHR_BITCOPY_RANGE(reg_above_64, ARAD_MGMT_INIT_DDR_PLL_CFG_I_BIT_VCO_DIV2, &vco_div2, 0, 1);

    
    fld_val = 1;
    SHR_BITCOPY_RANGE(reg_above_64, ARAD_MGMT_INIT_DDR_PLL_CFG_I_BIT_IF_FAST, &fld_val, 0, 1);

    
    fld_val = 2;
    SHR_BITCOPY_RANGE(reg_above_64, ARAD_MGMT_INIT_DDR_PLL_CFG_I_KA_START, &fld_val, 0, ARAD_MGMT_INIT_DDR_PLL_CFG_I_KA_LENGTH);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, soc_reg_above_64_set(unit, eci_ddr_pll_config[ddr_id], REG_PORT_ANY, 0, reg_above_64));

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "arad_mgmt_init_ddr_configure()", 0, 0);
}

STATIC uint32 arad_init_ddr_pll_set(
    SOC_SAND_IN int                         unit,
    SOC_SAND_IN  ARAD_MGMT_INIT           *init)
{

    uint32
        synth_clock_freq = 0,
        res;
    int32
        ddr_id;
    static soc_reg_t
        eci_ddr_pll_config[] = {ECI_DDR_0_PLL_CONFIGr, ECI_DDR_1_PLL_CONFIGr, ECI_DDR_2_PLL_CONFIGr, ECI_DDR_3_PLL_CONFIGr},
        eci_ddr_pll_status[] = {ECI_DDR_0_PLL_STATUSr, ECI_DDR_1_PLL_STATUSr, ECI_DDR_2_PLL_STATUSr, ECI_DDR_3_PLL_STATUSr};
    static soc_field_t
        eci_ddr_pll_locked[] = {DDR_0_PLL_LOCKEDf, DDR_1_PLL_LOCKEDf, DDR_2_PLL_LOCKEDf, DDR_3_PLL_LOCKEDf};
    soc_reg_above_64_val_t
        reg_above_64,
        field_above_64;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_REG_ABOVE_64_CLEAR(reg_above_64);
    SOC_REG_ABOVE_64_CLEAR(field_above_64);

    switch(init->pll.synthesizer_clock_freq) {
    case ARAD_INIT_SYNTHESIZER_CLOCK_FREQUENCY_25_MHZ:
        synth_clock_freq = ARAD_INIT_DDR_FREQUENCY_25_MHZ;
        break;
    case ARAD_INIT_SYNTHESIZER_CLOCK_FREQUENCY_125_MHZ:
        synth_clock_freq = ARAD_INIT_DDR_FREQUENCY_125_MHZ;
        break;
    default:
        
        break;
    }

    for(ddr_id = 0; ddr_id < SOC_DPP_DEFS_GET(unit, hw_dram_interfaces_max)/2; ++ddr_id) {
        if(init->dram.is_valid[2*ddr_id] || init->dram.is_valid[2*ddr_id+1]) {

            
            SOC_REG_ABOVE_64_CLEAR(field_above_64);
            SOC_REG_ABOVE_64_CLEAR(reg_above_64);
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, soc_reg_above_64_set(unit, eci_ddr_pll_config[ddr_id], REG_PORT_ANY, 0, reg_above_64));

            
            res = arad_mgmt_init_ddr_configure(
                unit,
                ddr_id,
                init->dram.dram_conf.params_mode.dram_freq,
                synth_clock_freq);
            SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

            sal_usleep(10);

            
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, soc_reg_above_64_get(unit, eci_ddr_pll_config[ddr_id], REG_PORT_ANY, 0, reg_above_64));
            SOC_REG_ABOVE_64_CLEAR(field_above_64);
            SOC_REG_ABOVE_64_CREATE_MASK(field_above_64, 1, 0);
            soc_reg_above_64_field_set(unit, eci_ddr_pll_config[ddr_id], reg_above_64, RESETBf, field_above_64);
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 50, exit, soc_reg_above_64_set(unit, eci_ddr_pll_config[ddr_id], REG_PORT_ANY, 0, reg_above_64));

            
            res = arad_polling(
                unit,
                ARAD_TIMEOUT,
                ARAD_MIN_POLLS,
                eci_ddr_pll_status[ddr_id],
                REG_PORT_ANY,
                0,
                eci_ddr_pll_locked[ddr_id],
                1);
            SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

            
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 70, exit, soc_reg_above_64_get(unit, eci_ddr_pll_config[ddr_id], REG_PORT_ANY, 0, reg_above_64));
            SOC_REG_ABOVE_64_CLEAR(field_above_64);
            SOC_REG_ABOVE_64_CREATE_MASK(field_above_64, 1, 0);
            soc_reg_above_64_field_set(unit, eci_ddr_pll_config[ddr_id], reg_above_64, POST_RESETBf, field_above_64);
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 80, exit, soc_reg_above_64_set(unit, eci_ddr_pll_config[ddr_id], REG_PORT_ANY, 0, reg_above_64));
        }
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "arad_init_ddr_pll_set()", ddr_id, init->pll.synthesizer_clock_freq);
}

STATIC uint32 arad_init_ts_pll_set(
    SOC_SAND_IN int                         unit)
{

    uint32
        res,
        val;
    soc_reg_above_64_val_t
        reg_above_64,
        field_above_64;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

     
    SOC_REG_ABOVE_64_CLEAR(reg_above_64);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, soc_reg_above_64_set(unit, ECI_TS_PLL_CONFIGr, REG_PORT_ANY, 0, reg_above_64));

    
    SOC_REG_ABOVE_64_CLEAR(field_above_64);
    val = 0xb80183c0;
    SHR_BITCOPY_RANGE(field_above_64, 0, &val, 0, 32);
    val = 0x00000809;
    SHR_BITCOPY_RANGE(field_above_64, 32, &val, 0, 32);
    val = 0x00000200;
    SHR_BITCOPY_RANGE(field_above_64, 64, &val, 0, 32);
    val = 0x1e000;
    SHR_BITCOPY_RANGE(field_above_64, 128, &val, 0, 32);
    soc_reg_above_64_field_set(unit, ECI_TS_PLL_CONFIGr, reg_above_64, TS_PLL_CTRLf, field_above_64);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, soc_reg_above_64_set(unit, ECI_TS_PLL_CONFIGr, REG_PORT_ANY, 0, reg_above_64));

    sal_usleep(10);

    
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, soc_reg_above_64_get(unit, ECI_TS_PLL_CONFIGr, REG_PORT_ANY, 0, reg_above_64));
    SOC_REG_ABOVE_64_CLEAR(field_above_64);
    SOC_REG_ABOVE_64_CREATE_MASK(field_above_64, 1, 0);
    soc_reg_above_64_field_set(unit, ECI_TS_PLL_CONFIGr, reg_above_64, RESETBf, field_above_64);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, soc_reg_above_64_set(unit, ECI_TS_PLL_CONFIGr, REG_PORT_ANY, 0, reg_above_64));

    
    res = arad_polling(
        unit,
        ARAD_TIMEOUT,
        ARAD_MIN_POLLS,
        ECI_TS_PLL_STATUSr,
        REG_PORT_ANY,
        0,
        TS_PLL_LOCKEDf,
        1);
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

    
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 70, exit, soc_reg_above_64_get(unit, ECI_TS_PLL_CONFIGr, REG_PORT_ANY, 0, reg_above_64));
    SOC_REG_ABOVE_64_CLEAR(field_above_64);
    SOC_REG_ABOVE_64_CREATE_MASK(field_above_64, 1, 0);
    soc_reg_above_64_field_set(unit, ECI_TS_PLL_CONFIGr, reg_above_64, POST_RESETBf, field_above_64);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 80, exit, soc_reg_above_64_set(unit, ECI_TS_PLL_CONFIGr, REG_PORT_ANY, 0, reg_above_64));

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "arad_init_ts_pll_set()", 0, 0);
}


STATIC uint32 arad_init_bs_pll_set(
    SOC_SAND_IN int                         unit)
{

    uint32
        res;
    soc_reg_above_64_val_t
        reg_above_64,
        field_above_64;
    uint32 reg32_val;
    uint32 Ndiv_int = 120; 
    uint32 Ch1_Mdiv = 150; 

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

     
    SOC_REG_ABOVE_64_CLEAR(reg_above_64);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, soc_reg_above_64_set(unit, ECI_BS_PLL_CONFIGr, REG_PORT_ANY, 0, reg_above_64));



    
    
    
    

    if(soc_property_get(unit, spn_ARAD_20MHZ_BS, 0x0)) { 
        Ndiv_int = 140; 
        Ch1_Mdiv = 175;
    }

    SOC_REG_ABOVE_64_CLEAR(field_above_64);
    SOC_REG_ABOVE_64_WORD_SET(field_above_64,   1, 0);
    soc_reg_above_64_field_set(unit, ECI_BS_PLL_CONFIGr, reg_above_64, I_PDIVf,      field_above_64);
    
    SOC_REG_ABOVE_64_WORD_SET(field_above_64, Ndiv_int, 0);
    soc_reg_above_64_field_set(unit, ECI_BS_PLL_CONFIGr, reg_above_64, I_NDIV_INTf,  field_above_64);
    
    SOC_REG_ABOVE_64_WORD_SET(field_above_64, Ch1_Mdiv, 0);
    soc_reg_above_64_field_set(unit, ECI_BS_PLL_CONFIGr, reg_above_64, I_CH_1_MDIVf, field_above_64);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, soc_reg_above_64_set(unit, ECI_BS_PLL_CONFIGr, REG_PORT_ANY, 0, reg_above_64));

    sal_usleep(10);

    
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, soc_reg_above_64_get(unit, ECI_BS_PLL_CONFIGr, REG_PORT_ANY, 0, reg_above_64));
    SOC_REG_ABOVE_64_CLEAR(field_above_64);
    SOC_REG_ABOVE_64_CREATE_MASK(field_above_64, 1, 0);
    soc_reg_above_64_field_set(unit, ECI_BS_PLL_CONFIGr, reg_above_64, RESETBf, field_above_64);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, soc_reg_above_64_set(unit, ECI_BS_PLL_CONFIGr, REG_PORT_ANY, 0, reg_above_64));

    
    res = arad_polling(
        unit,
        ARAD_TIMEOUT,
        ARAD_MIN_POLLS,
        ECI_BS_PLL_STATUSr,
        REG_PORT_ANY,
        0,
        BS_PLL_LOCKEDf,
        1);
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

    
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 70, exit, soc_reg_above_64_get(unit, ECI_BS_PLL_CONFIGr, REG_PORT_ANY, 0, reg_above_64));
    SOC_REG_ABOVE_64_CLEAR(field_above_64);
    SOC_REG_ABOVE_64_CREATE_MASK(field_above_64, 1, 0);
    soc_reg_above_64_field_set(unit, ECI_BS_PLL_CONFIGr, reg_above_64, POST_RESETBf, field_above_64);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 80, exit, soc_reg_above_64_set(unit, ECI_BS_PLL_CONFIGr, REG_PORT_ANY, 0, reg_above_64));


    
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 90, exit, soc_reg_above_64_get(unit, ECI_BS_PLL_CONFIGr, REG_PORT_ANY, 0, reg_above_64));
    SOC_REG_ABOVE_64_CLEAR(field_above_64);
    SOC_REG_ABOVE_64_CREATE_MASK(field_above_64, 1, 0);
    soc_reg_above_64_field_set(unit, ECI_BS_PLL_CONFIGr, reg_above_64, BS_CLK_SELECTf, field_above_64);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 100, exit, soc_reg_above_64_set(unit, ECI_BS_PLL_CONFIGr, REG_PORT_ANY, 0, reg_above_64));

    
    reg32_val = 0x0;
    soc_reg_field_set(unit, ECI_GPIO_TS_SELr, &reg32_val, GPIO_0_SELf, 0x1);
    soc_reg_field_set(unit, ECI_GPIO_TS_SELr, &reg32_val, GPIO_1_SELf, 0x1);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 22, exit, soc_reg32_set(unit, ECI_GPIO_TS_SELr, REG_PORT_ANY, 0, reg32_val));

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "arad_init_bs_pll_set()", 0, 0);
}


uint32 arad_mgmt_init_pll_reset(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  ARAD_MGMT_INIT           *init)
{
    uint32
        res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_MGMT_INIT_PLL_RESET);

    if (!SOC_IS_ARDON(unit)) {

        res = arad_init_ddr_pll_set(unit, init);
        SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

        res = arad_init_serdes_pll_set(unit, ARAD_INIT_SERDES_TYPE_NIF, init->pll.nif_clk_freq);
        SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

        res = arad_init_serdes_pll_set(unit, ARAD_INIT_SERDES_TYPE_FABRIC, init->pll.fabric_clk_freq);
        SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);
    }

    
    if (init->pll.ts_clk_mode == 0x1) {
        res = arad_init_ts_pll_set(unit);
        SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
    }

    if (SOC_IS_ARADPLUS(unit) && (!SOC_IS_ARDON(unit))) {
        if (soc_property_get(unit, spn_PHY_1588_DPLL_FREQUENCY_LOCK, 0)) {
            res = arad_init_bs_pll_set(unit);
            SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);
        }
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "arad_mgmt_init_pll_reset()", 0, 0);
}




STATIC uint32
  arad_mgmt_init_before_blocks_oor(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  ARAD_INIT_DBUFFS_BDRY        *dbuffs_bdries,
    SOC_SAND_IN  ARAD_MGMT_INIT               *init
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    fld_val,
    reg_val;

  ARAD_MGMT_FTMH_LB_EXT_MODE
    ftmh_lb_ext_mode;
  uint32
    ftmh_lb_key_enable;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_MGMT_INIT_BEFORE_BLOCKS_OOR);

  SOC_SAND_CHECK_NULL_INPUT(init);
  
  
  

    res = arad_mgmt_init_pll_reset(
          unit,
          init
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  
  
  

  
  if (init->fabric.connect_mode == ARAD_FABRIC_CONNECT_MODE_MESH && !(SOC_DPP_CONFIG(unit)->tdm.is_bypass))
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  100,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_1r, REG_PORT_ANY, 0, MESH_MODEf,  0x1));
  }
  else
  {
    
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  110,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_1r, REG_PORT_ANY, 0, MESH_MODEf,  0x0));
  }

  
  fld_val = 0x1;
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  120,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_1r, REG_PORT_ANY, 0, ADD_DRAM_CRCf,  fld_val));

  
  fld_val = 0;
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  130,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_1r, REG_PORT_ANY, 0, NO_FAB_CRCf,  fld_val));

  res = arad_ports_ftmh_extension_set_unsafe(
          unit,
          init->fabric.ftmh_extension
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);

  
  ftmh_lb_ext_mode = init->fabric.ftmh_lb_ext_mode;
  ftmh_lb_key_enable = 0;
  if (ftmh_lb_ext_mode != ARAD_MGMT_FTMH_LB_EXT_MODE_DISABLED) {
    ftmh_lb_key_enable = 0x1;
  }

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  35,  exit, ARAD_REG_ACCESS_ERR,READ_ECI_GLOBALFr(unit, &reg_val));
  ARAD_FLD_TO_REG(ECI_GLOBALFr, FTMH_LB_KEY_EXT_ENf, ftmh_lb_key_enable, reg_val, 40, exit);
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  55,  exit, ARAD_REG_ACCESS_ERR,WRITE_ECI_GLOBALFr(unit,  reg_val));

  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  160,  exit, ARAD_REG_ACCESS_ERR,READ_ECI_GLOBALFr(unit, &reg_val));
  ARAD_FLD_TO_REG(ECI_GLOBALFr, FTMH_STACKING_EXT_ENABLEf, init->fabric.ftmh_stacking_ext_mode, reg_val, 170, exit);
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  180,  exit, ARAD_REG_ACCESS_ERR,WRITE_ECI_GLOBALFr(unit,  reg_val));

  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  190,  exit, ARAD_REG_ACCESS_ERR,READ_ECI_PAD_CONFIGURATION_REGISTERr(unit, &reg_val));
  ARAD_FLD_TO_REG(ECI_PAD_CONFIGURATION_REGISTERr, SYNCE_PADS_SELf, ARAD_INIT_SYNCE_PADS_SEL_15, reg_val, 210, exit);
  ARAD_FLD_TO_REG(ECI_PAD_CONFIGURATION_REGISTERr, SYNCE_PADS_REF_INT_ENf, ARAD_INIT_SYNCE_PADS_REF_INT_DS , reg_val, 220, exit);
  ARAD_FLD_TO_REG(ECI_PAD_CONFIGURATION_REGISTERr, SYNCE_PADS_AMP_ENf, ARAD_INIT_SYNCE_PADS_AMP_EN , reg_val, 230, exit);

  if (init->ex_vol_mod == ARAD_MGMT_EXT_VOL_MOD_HSTL_1p5V) {
      ARAD_FLD_TO_REG(ECI_PAD_CONFIGURATION_REGISTERr, FC_PADS_SELf, ARAD_INIT_FC_PADS_SEL_15, reg_val, 240, exit);
      ARAD_FLD_TO_REG(ECI_PAD_CONFIGURATION_REGISTERr, FC_PADS_REF_INT_ENf, ARAD_INIT_FC_PADS_REF_INT_DS, reg_val, 250, exit);
      ARAD_FLD_TO_REG(ECI_PAD_CONFIGURATION_REGISTERr, FC_PADS_AMP_ENf, ARAD_INIT_FC_PADS_AMP_EN, reg_val, 260, exit);
  } else if (init->ex_vol_mod == ARAD_MGMT_EXT_VOL_MOD_3p3V) {
      ARAD_FLD_TO_REG(ECI_PAD_CONFIGURATION_REGISTERr, FC_PADS_SELf, ARAD_INIT_FC_PADS_SEL_33, reg_val, 270, exit);
      ARAD_FLD_TO_REG(ECI_PAD_CONFIGURATION_REGISTERr, FC_PADS_REF_INT_ENf, ARAD_INIT_FC_PADS_REF_INT_DS, reg_val, 280, exit);
      ARAD_FLD_TO_REG(ECI_PAD_CONFIGURATION_REGISTERr, FC_PADS_AMP_ENf, ARAD_INIT_FC_PADS_AMP_DS, reg_val, 290, exit)
  } else if (init->ex_vol_mod == ARAD_MGMT_EXT_VOL_MOD_HSTL_1p5V_VDDO) {
      ARAD_FLD_TO_REG(ECI_PAD_CONFIGURATION_REGISTERr, FC_PADS_SELf, ARAD_INIT_FC_PADS_SEL_15, reg_val, 300, exit);
      ARAD_FLD_TO_REG(ECI_PAD_CONFIGURATION_REGISTERr, FC_PADS_REF_INT_ENf, ARAD_INIT_FC_PADS_REF_INT_EN, reg_val, 310, exit);
      ARAD_FLD_TO_REG(ECI_PAD_CONFIGURATION_REGISTERr, FC_PADS_AMP_ENf, ARAD_INIT_FC_PADS_AMP_EN, reg_val, 320, exit);
  }

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  330,  exit, ARAD_REG_ACCESS_ERR,WRITE_ECI_PAD_CONFIGURATION_REGISTERr(unit,  reg_val));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_mgmt_init_before_blocks_oor()", 0, 0);
}


STATIC uint32
  arad_mgmt_init_after_blocks_oor(
    SOC_SAND_IN int unit,
    SOC_SAND_IN  ARAD_MGMT_INIT        *hw_adj,
    SOC_SAND_IN  ARAD_INIT_DBUFFS_BDRY      *dbuffs_bdries
  )
{
  uint32
    fld_val,
    index,
    val32,
    res,
    nof_valid_ports;
  ARAD_ITM_DBUFF_SIZE_BYTES
    dbuff_size;
  uint64
      field_val,
      reg_val_64,
      val64;
  uint32
      table_entry[2] = {0},
      ocb_buf_type_fwd,
      ocb_buf_type_snoop,
      ocb_buf_type_mirror,
      ocb_buf_type_queue_e,
      ocb_buf_type_mc_id_range;

  soc_reg_above_64_val_t
     old_wr_mask_val,
     wr_mask_val;
  soc_pbmp_t
      port_bm;
  soc_reg_t
      disable_pp_reg;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_MGMT_INIT_AFTER_BLOCKS_OOR);

  if (SOC_IS_QAX(unit)) {
      SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("access to registers should be fixed for QAX at places we used _REG(32|64) access routines")));
  }


  
  for (index=0 ; index < SOC_DPP_DEFS_GET(unit, nof_instances_fsrd) ; index++) {
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  18,  exit, ARAD_REG_ACCESS_ERR,WRITE_FSRD_SBUS_BROADCAST_IDr(unit,  index,  60));
  }

  
  for (index=0 ; index < SOC_DPP_DEFS_GET(unit, nof_instances_fmac) ; index++) {
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  19,  exit, ARAD_REG_ACCESS_ERR,WRITE_FMAC_SBUS_BROADCAST_IDr(unit,  index,  61));
  }


  
  
  
  if ((hw_adj->dram.enable == TRUE) ||  (hw_adj->ocb.ocb_enable != OCB_DISABLED )){

    dbuff_size = hw_adj->dram.dbuff_size;
    res = arad_itm_dbuff_size2internal(dbuff_size, &fld_val);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  21,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_1r, REG_PORT_ANY, 0, DBUFF_SIZEf,  fld_val));

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  22,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_1r, REG_PORT_ANY, 0, ADD_DRAM_CRCf,  0x1));

    switch(dbuff_size) {
      case 256:
        fld_val = 1;
        break;
      case 512:
        fld_val = 3;
        break;
      case 1024:
        fld_val = 7;
        break;
      case 2048:
        fld_val = 15;
        break;
      default:
        SOC_SAND_SET_ERROR_CODE(ARAD_MGMT_INIT_INVALID_DBUFF_SIZE_ERR, 23, exit);
    }

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  23,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, MMU_OCB_CONFIGURATION_REGISTERr, REG_PORT_ANY, 0, OCB_DBUFF_MAX_OFFSETf,  fld_val));

    
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  27,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_4r, REG_PORT_ANY, 0, UC_DB_PTR_STARTf,  dbuffs_bdries->uc.start));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit , ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_5r, REG_PORT_ANY, 0, UC_DB_PTR_ENDf,  dbuffs_bdries->uc.end));

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit , ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_6r, REG_PORT_ANY, 0, MN_MUL_DB_PTR_STARTf,  dbuffs_bdries->mmc.start));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  50,  exit , ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_7r, REG_PORT_ANY, 0, MN_MUL_DB_PTR_ENDf,  dbuffs_bdries->mmc.end));

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  60,  exit , ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_8r, REG_PORT_ANY, 0, FL_MUL_DB_PTR_STARTf,  dbuffs_bdries->fmc.start));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  70,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_9r, REG_PORT_ANY, 0, FL_MUL_DB_PTR_ENDf,  dbuffs_bdries->fmc.end));

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  80,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, ECI_GLOBALAr, REG_PORT_ANY, 0, UC_OCB_PTR_STARTf,  dbuffs_bdries->ocb_uc.start));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  90,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, ECI_GLOBALBr, REG_PORT_ANY, 0, UC_OCB_PTR_ENDf,  dbuffs_bdries->ocb_uc.end));

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  100,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, ECI_GLOBALCr, REG_PORT_ANY, 0, FL_MUL_OCB_PTR_STARTf,  dbuffs_bdries->ocb_fmc.start));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  110,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, ECI_GLOBALDr, REG_PORT_ANY, 0, FL_MUL_OCB_PTR_ENDf,  dbuffs_bdries->ocb_fmc.end));

    
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  120,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IDR_FBC_FULL_MULTICAST_AUTOGEN_STARTr, REG_PORT_ANY, 0, FBC_FULL_MULTICAST_AUTOGEN_STARTf,  dbuffs_bdries->fmc.start));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  130,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IDR_FBC_FULL_MULTICAST_AUTOGEN_ENDr, REG_PORT_ANY, 0, FBC_FULL_MULTICAST_AUTOGEN_ENDf,  dbuffs_bdries->fmc.end));

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  140,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IDR_FBC_MINI_MULTICAST_AUTOGEN_STARTr, REG_PORT_ANY, 0, FBC_MINI_MULTICAST_AUTOGEN_STARTf,  dbuffs_bdries->mmc.start));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  150,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IDR_FBC_MINI_MULTICAST_AUTOGEN_ENDr, REG_PORT_ANY, 0, FBC_MINI_MULTICAST_AUTOGEN_ENDf,  dbuffs_bdries->mmc.end));

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  160,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IDR_FBC_UNICAST_AUTOGEN_STARTr, REG_PORT_ANY, 0, FBC_UNICAST_AUTOGEN_STARTf,  dbuffs_bdries->uc.start));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  170,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IDR_FBC_UNICAST_AUTOGEN_ENDr, REG_PORT_ANY, 0, FBC_UNICAST_AUTOGEN_ENDf,  dbuffs_bdries->uc.end));

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  180,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IDR_FBC_OCB_UNICAST_AUTOGEN_STARTr, REG_PORT_ANY, 0, FBC_OCB_UNICAST_AUTOGEN_STARTf,  dbuffs_bdries->ocb_uc.start));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  190,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IDR_FBC_OCB_UNICAST_AUTOGEN_ENDr, REG_PORT_ANY, 0, FBC_OCB_UNICAST_AUTOGEN_ENDf,  dbuffs_bdries->ocb_uc.end));

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  200,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IDR_FBC_OCB_MULTICAST_AUTOGEN_STARTr, REG_PORT_ANY, 0, FBC_OCB_MULTICAST_AUTOGEN_STARTf,  dbuffs_bdries->ocb_fmc.start));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  210,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IDR_FBC_OCB_MULTICAST_AUTOGEN_ENDr, REG_PORT_ANY, 0, FBC_OCB_MULTICAST_AUTOGEN_ENDf,  dbuffs_bdries->ocb_fmc.end));

    
    fld_val = (dbuffs_bdries->uc.start < dbuffs_bdries->uc.end)? 0x1:0x0;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  170,  exit , ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IDR_STATIC_CONFIGURATIONr, REG_PORT_ANY, 0, FBC_UNICAST_AUTOGEN_ENABLEf,  fld_val));

    fld_val = (dbuffs_bdries->mmc.start < dbuffs_bdries->mmc.end)?0x1:0x0;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  180,  exit , ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IDR_STATIC_CONFIGURATIONr, REG_PORT_ANY, 0, FBC_MINI_MULTICAST_AUTOGEN_ENABLEf,  fld_val ));

    fld_val = (dbuffs_bdries->fmc.start < dbuffs_bdries->fmc.end)?0x1:0x0;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  190,  exit , ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IDR_STATIC_CONFIGURATIONr, REG_PORT_ANY, 0, FBC_FULL_MULTICAST_AUTOGEN_ENABLEf,  fld_val));

    fld_val = (dbuffs_bdries->ocb_uc.start < dbuffs_bdries->ocb_uc.end)?0x1:0x0;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  200,  exit , ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IDR_STATIC_CONFIGURATIONr, REG_PORT_ANY, 0, FBC_OCB_UNICAST_AUTOGEN_ENABLEf,  fld_val));

    fld_val = (dbuffs_bdries->ocb_fmc.start < dbuffs_bdries->ocb_fmc.end)?0x1:0x0;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  210,  exit , ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IDR_STATIC_CONFIGURATIONr, REG_PORT_ANY, 0, FBC_OCB_MULTICAST_AUTOGEN_ENABLEf,  fld_val));

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  212,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IDR_STATIC_CONFIGURATIONr, REG_PORT_ANY, 0, ENABLE_BUFFER_CHANGEf,  0x1));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  213,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IDR_STATIC_CONFIGURATIONr, REG_PORT_ANY, 0, FBC_INTERNAL_REUSEf,  0x1));

    COMPILER_64_SET(reg_val_64, SOC_SAND_U32_MAX, SOC_SAND_U32_MAX);
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  215,  exit, ARAD_REG_ACCESS_ERR,WRITE_IRR_RESEQUENCER_FIFO_INDEXr(unit,  reg_val_64));

   
    SOC_SAND_CHECK_FUNC_RESULT( arad_mgmt_set_mru_by_dbuff_size(unit), 216, exit) ;

#ifdef BCM_88660_A0
    if (SOC_IS_ARADPLUS(unit)) {
        soc_reg_above_64_val_t
            fld_above_64_val,
            reg_above_64_val;
        SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);
        SOC_REG_ABOVE_64_CLEAR(fld_above_64_val);
        

        
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 220, exit, soc_port_sw_db_valid_ports_get(unit, 0, &port_bm));
        SOC_PBMP_COUNT(port_bm, nof_valid_ports);
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 222, exit, ARAD_REG_ACCESS_ERR, READ_IDR_OCCUPIED_DBUFF_THRESHOLDr(unit, SOC_CORE_ALL ,reg_above_64_val));

        ARAD_FLD_FROM_REG_ABOVE_64(IDR_OCCUPIED_DBUFF_THRESHOLDr, MMC_DBUFF_OCC_THf, fld_above_64_val ,reg_above_64_val, 214, exit);
        fld_val = (dbuffs_bdries->mmc.end - dbuffs_bdries->mmc.start + 1) -
                    ((SOC_DPP_CONFIG(unit))->arad->init.dram.enable ?
                     ((SOC_DPP_DEFS_GET(unit, ocb_memory_size) * 1024) / (SOC_DPP_CONFIG(unit))->arad->init.dram.dbuff_size) * nof_valid_ports + 50:
                     ((SOC_DPP_DEFS_GET(unit, ocb_memory_size) * 1024) / (SOC_DPP_CONFIG(unit))->arad->init.ocb.databuffer_size) * nof_valid_ports + 50);

        *fld_above_64_val = (fld_val < ((dbuffs_bdries->mmc.end - dbuffs_bdries->mmc.start + 1) * 9) / 10) ? fld_val :
                    (((dbuffs_bdries->mmc.end - dbuffs_bdries->mmc.start + 1) * 9) / 10);
        ARAD_FLD_TO_REG_ABOVE_64(IDR_OCCUPIED_DBUFF_THRESHOLDr, MMC_DBUFF_OCC_THf, fld_above_64_val , reg_above_64_val, 215, exit);
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 224, exit, ARAD_REG_ACCESS_ERR, WRITE_IDR_OCCUPIED_DBUFF_THRESHOLDr(unit, SOC_CORE_ALL ,reg_above_64_val));
    }
#endif 
    val32 = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "8865002", 0); 
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  60,  exit, ARAD_REG_ACCESS_ERR,READ_EGQ_EGRESS_REPLICATION_GENERAL_CONFIGr_REG32(unit, SOC_CORE_ALL, &fld_val));
    
    if (val32 > 2 || (val32 <= 0 && SOC_IS_ARADPLUS(unit))) { 
        ARAD_FLD_TO_REG(EGQ_EGRESS_REPLICATION_GENERAL_CONFIGr, EGRESS_MC_1_REP_ENABLEf, 0xf, fld_val, 71, exit);
        ARAD_FLD_TO_REG(EGQ_EGRESS_REPLICATION_GENERAL_CONFIGr, EGRESS_MC_2_REP_ENABLEf, 0xe, fld_val, 72, exit);
    } else if (val32 == 2) { 
        ARAD_FLD_TO_REG(EGQ_EGRESS_REPLICATION_GENERAL_CONFIGr, EGRESS_MC_1_REP_ENABLEf, 3, fld_val, 81, exit);
        ARAD_FLD_TO_REG(EGQ_EGRESS_REPLICATION_GENERAL_CONFIGr, EGRESS_MC_2_REP_ENABLEf, 12, fld_val, 82, exit);
    } else { 
        ARAD_FLD_TO_REG(EGQ_EGRESS_REPLICATION_GENERAL_CONFIGr, EGRESS_MC_1_REP_ENABLEf, 1, fld_val, 91, exit);
        ARAD_FLD_TO_REG(EGQ_EGRESS_REPLICATION_GENERAL_CONFIGr, EGRESS_MC_2_REP_ENABLEf, 2, fld_val, 92, exit);
    }
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  100,  exit, ARAD_REG_ACCESS_ERR,WRITE_EGQ_EGRESS_REPLICATION_GENERAL_CONFIGr_REG32(unit, SOC_CORE_ALL,  fld_val));

#ifdef BCM_88660_A0
    if (SOC_IS_ARADPLUS(unit)) {
        
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 115, exit, READ_IRR_STATIC_CONFIGURATIONr(unit, &val32));
        soc_reg_field_set(unit, IRR_STATIC_CONFIGURATIONr, &val32,
          MULTICAST_CUD_MODEf, SOC_DPP_CONFIG(unit)->tm.mc_ing_encoding_mode);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 116, exit, WRITE_IRR_STATIC_CONFIGURATIONr(unit, val32));

        if (!SOC_IS_ARDON(unit)) {
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 117, exit, READ_EGQ_CFG_BUG_FIX_CHICKEN_BITS_REG_2r(unit, REG_PORT_ANY, &val32));
            soc_reg_field_set(unit, EGQ_CFG_BUG_FIX_CHICKEN_BITS_REG_2r, &val32, CFG_CUD_MAPPING_ENABLEf,
              (SOC_DPP_CONFIG(unit)->tm.mc_mode & DPP_MC_EGR_17B_CUDS_127_PORTS_MODE) ? 1 : 0);
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 118, exit, WRITE_EGQ_CFG_BUG_FIX_CHICKEN_BITS_REG_2r(unit, REG_PORT_ANY, val32));

            SOC_SAND_SOC_IF_ERROR_RETURN(res, 119, exit, READ_EPNI_CFG_BUG_FIX_CHICKEN_BITS_REG_2r(unit, REG_PORT_ANY, &val32));
            soc_reg_field_set(unit, EPNI_CFG_BUG_FIX_CHICKEN_BITS_REG_2r, &val32, CFG_CUD_MAPPING_ENABLEf,
              (SOC_DPP_CONFIG(unit)->tm.mc_mode & DPP_MC_CUD_EXTENSION_MODE) ? 1 : 0); 
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 120, exit, WRITE_EPNI_CFG_BUG_FIX_CHICKEN_BITS_REG_2r(unit, REG_PORT_ANY, val32));
        }
        
        if ((SOC_DPP_CONFIG(unit)->tm.mc_mode) & DPP_MC_CUD_EXTENSION_MODE) {
            SOC_SAND_CHECK_FUNC_RESULT(soc_ppd_eg_encap_data_local_to_global_cud_init(unit), 221, exit);
        }
    }
#endif 
    
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 220, exit, WRITE_IDR_FBC_EXTERNAL_OCB_MULTICAST_LIMIT_LOWr(unit, dbuffs_bdries->fbc_ocb_fmc.start));
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 230, exit, WRITE_IDR_FBC_EXTERNAL_OCB_MULTICAST_LIMIT_HIGHr(unit, dbuffs_bdries->fbc_ocb_fmc.end));

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 240, exit, WRITE_IDR_FBC_EXTERNAL_FULL_MULTICAST_LIMIT_LOWr(unit, dbuffs_bdries->fbc_fmc.start));
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 250, exit, WRITE_IDR_FBC_EXTERNAL_FULL_MULTICAST_LIMIT_HIGHr(unit, dbuffs_bdries->fbc_fmc.end));

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 260, exit, WRITE_IDR_FBC_EXTERNAL_MINI_MULTICAST_LIMIT_LOWr(unit, dbuffs_bdries->fbc_mmc.start));
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 270, exit, WRITE_IDR_FBC_EXTERNAL_MINI_MULTICAST_LIMIT_HIGHr(unit, dbuffs_bdries->fbc_mmc.end));

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 280, exit, WRITE_IDR_FBC_EXTERNAL_OCB_UNICAST_LIMIT_LOWr(unit, dbuffs_bdries->fbc_ocb_uc.start));
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 290, exit, WRITE_IDR_FBC_EXTERNAL_OCB_UNICAST_LIMIT_HIGHr(unit, dbuffs_bdries->fbc_ocb_uc.end));

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 300, exit, WRITE_IDR_FBC_EXTERNAL_UNICAST_LIMIT_LOWr(unit, dbuffs_bdries->fbc_uc.start));
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 310, exit, WRITE_IDR_FBC_EXTERNAL_UNICAST_LIMIT_HIGHr(unit, dbuffs_bdries->fbc_uc.end));

    
    if (hw_adj->ocb.ocb_enable != OCB_DISABLED) {

        

        for(index = 0; index < ARAD_INIT_OCB_BUFFER_TYPE_NOF_ENTRIES; index++) {
            ocb_buf_type_fwd = SOC_SAND_GET_BITS_RANGE(index, 8, 6);
            ocb_buf_type_snoop = SOC_SAND_GET_BITS_RANGE(index, 5, 4);
            ocb_buf_type_mirror = SOC_SAND_GET_BITS_RANGE(index, 3, 2);
            ocb_buf_type_queue_e = SOC_SAND_GET_BITS_RANGE(index, 1, 1);
            ocb_buf_type_mc_id_range = SOC_SAND_GET_BITS_RANGE(index, 0, 0);

            if (ocb_buf_type_snoop != ARAD_INIT_OCB_BUF_TYPE_SNOOP_NO_SNOOP) {
                
                fld_val = ARAD_INIT_OCB_BUF_TYPE_NO_OCB_BUF;
            } else if (((ocb_buf_type_fwd == ARAD_INIT_OCB_BUF_TYPE_FWD_FMC) || (ocb_buf_type_fwd == ARAD_INIT_OCB_BUF_TYPE_FWD_UC)) &&
                (ocb_buf_type_queue_e == ARAD_INIT_OCB_BUF_TYPE_QUEUE_ELIGIBILE) &&
                (ocb_buf_type_mirror == ARAD_INIT_OCB_BUF_TYPE_MIRROR_NO_MIRROR)){
                
                fld_val = ARAD_INIT_OCB_BUF_TYPE_UC_OCB_BUF;

            } else if (((ocb_buf_type_fwd == ARAD_INIT_OCB_BUF_TYPE_FWD_FMC) || (ocb_buf_type_fwd == ARAD_INIT_OCB_BUF_TYPE_FWD_UC)) &&
                (ocb_buf_type_queue_e == ARAD_INIT_OCB_BUF_TYPE_QUEUE_ELIGIBILE) &&
                ((ocb_buf_type_mirror == ARAD_INIT_OCB_BUF_TYPE_MIRROR_UC_FMC_MIRROR) || (ocb_buf_type_mirror == ARAD_INIT_OCB_BUF_TYPE_MIRROR_ING_MC_MIRROR))) {
                
                fld_val = ARAD_INIT_OCB_BUF_TYPE_FLMC_OCB_BUF;

            } else if ((ocb_buf_type_fwd == ARAD_INIT_OCB_BUF_TYPE_FWD_ING_MC_1_COPY) &&
                (ocb_buf_type_mc_id_range == ARAD_INIT_OCB_BUF_TYPE_MCID_IN_RANGE) &&
                (ocb_buf_type_mirror == ARAD_INIT_OCB_BUF_TYPE_MIRROR_NO_MIRROR)) {
                
                fld_val = ARAD_INIT_OCB_BUF_TYPE_UC_OCB_BUF;

            } else if ((ocb_buf_type_fwd == ARAD_INIT_OCB_BUF_TYPE_FWD_ING_MC_1_COPY) &&
                (ocb_buf_type_mc_id_range == ARAD_INIT_OCB_BUF_TYPE_MCID_IN_RANGE) &&
                ((ocb_buf_type_mirror == ARAD_INIT_OCB_BUF_TYPE_MIRROR_UC_FMC_MIRROR) || (ocb_buf_type_mirror == ARAD_INIT_OCB_BUF_TYPE_MIRROR_ING_MC_MIRROR))) {
                
                fld_val = ARAD_INIT_OCB_BUF_TYPE_FLMC_OCB_BUF;

            } else if ((ocb_buf_type_fwd == ARAD_INIT_OCB_BUF_TYPE_FWD_ING_MC_N_COPY) &&
                (ocb_buf_type_mc_id_range == ARAD_INIT_OCB_BUF_TYPE_MCID_IN_RANGE)) {
                
                fld_val = ARAD_INIT_OCB_BUF_TYPE_FLMC_OCB_BUF;

            } else {
                fld_val = ARAD_INIT_OCB_BUF_TYPE_NO_OCB_BUF;
            }

            SOC_SAND_SOC_IF_ERROR_RETURN(res, 320, exit, WRITE_IDR_OCB_BUFFER_TYPEm(unit, MEM_BLOCK_ANY, index, &fld_val));
        }

        
        COMPILER_64_SET(field_val,0, 1);
        soc_mem_field64_set(unit, IQM_OCBPRMm, &table_entry, OCB_ENf, field_val);
        COMPILER_64_SET(field_val,0,SOC_TMC_MGMT_OCB_PRM_EN_TH_DEFAULT);
        soc_mem_field64_set(unit, IQM_OCBPRMm, &table_entry, OCB_QUE_SIZE_EN_TH_1f, field_val);
        soc_mem_field64_set(unit, IQM_OCBPRMm, &table_entry, OCB_QUE_SIZE_EN_TH_0f, field_val);
        soc_mem_field64_set(unit, IQM_OCBPRMm, &table_entry, OCB_QUE_BUFF_SIZE_EN_TH_1f, field_val);
        soc_mem_field64_set(unit, IQM_OCBPRMm, &table_entry, OCB_QUE_BUFF_SIZE_EN_TH_0f, field_val);
        res = arad_fill_table_with_entry(unit, IQM_OCBPRMm, MEM_BLOCK_ANY, &table_entry);
        SOC_SAND_CHECK_FUNC_RESULT(res, 345, exit);

        

        table_entry[0] = 0xffffffff;
        table_entry[1] = 0xffffffff;

        SOC_SAND_SOC_IF_ERROR_RETURN(res, 347, exit, READ_IDR_INDIRECT_WR_MASKr(unit, old_wr_mask_val));

        wr_mask_val[0] = 0xffffffff;
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 349, exit, WRITE_IDR_INDIRECT_WR_MASKr(unit, wr_mask_val));


        res = arad_fill_table_with_entry(unit, IDR_MEM_1F0000m, MEM_BLOCK_ANY, table_entry);
        SOC_SAND_CHECK_FUNC_RESULT(res, 7, exit);
        
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 349, exit, WRITE_IDR_INDIRECT_WR_MASKr(unit, old_wr_mask_val));

#ifdef BCM_88660_A0
        if (SOC_IS_ARADPLUS(unit)) {
            
            fld_val = 0x1;
            
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  350,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, MMU_OCB_CONFIGURATION_REGISTERr, REG_PORT_ANY, 0, FIELD_12_12f,  fld_val));


            if ((hw_adj->ocb.ocb_enable == OCB_ONLY) || (hw_adj->ocb.ocb_enable == OCB_ONLY_1_DRAM) || (hw_adj->ocb.ocb_enable == OCB_DRAM_SEPARATE)){
                fld_val = 0x1;
                SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  360,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IDR_STATIC_CONFIGURATIONr, REG_PORT_ANY, 0, ONE_WAY_BYPASS_ENABLEf,  fld_val));
                SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  365,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, MMU_GENERAL_CONFIGURATION_REGISTER_2r, REG_PORT_ANY, 0, ONE_WAY_BYPASS_ENf,      fld_val));
                SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  370,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_ONE_WAY_BYPASS_MODEr, REG_PORT_ANY, 0, ONE_WAY_BYPASS_MODEf,    fld_val));
                
                fld_val = 0x0;
                SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  380,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, MMU_OCB_CONFIGURATION_REGISTERr, REG_PORT_ANY, 0, FIELD_12_12f,  fld_val));
            } else if ((hw_adj->ocb.ocb_enable == OCB_ENABLED) && (hw_adj->dram.enable == 0x0)) {
               fld_val = 0x1;
               SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  390,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_ONE_WAY_BYPASS_MODEr, REG_PORT_ANY, 0, ONE_WAY_BYPASS_MODEf,    fld_val));
               fld_val = 0x0;
               SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  400,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, MMU_OCB_CONFIGURATION_REGISTERr, REG_PORT_ANY, 0, FIELD_12_12f,  fld_val));
            }


            if (hw_adj->ocb.ocb_enable == OCB_ONLY_1_DRAM){
                fld_val = 0x1;
                SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  405,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, MMU_GENERAL_CONFIGURATION_REGISTER_2r, REG_PORT_ANY, 0, ONE_DRAM_MODEf,  fld_val));
            }

        } else
#endif
        {
            if ((hw_adj->ocb.ocb_enable == OCB_ENABLED)&&(hw_adj->dram.nof_drams==0)){
                SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  410,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, MMU_OCB_CONFIGURATION_REGISTERr, REG_PORT_ANY, 0, FIELD_12_12f,  0));
            }
        }
    }

    
    fld_val = (hw_adj->ocb.ocb_enable != OCB_DISABLED) ? 0x1 : 0x0;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  415,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_1r, REG_PORT_ANY, 0, OCB_ENf,  fld_val));

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 420, exit, READ_IQM_IQM_ENABLERSr(unit, REG_PORT_ANY, &val32)); 
    
    if(hw_adj->dram.pdm_mode == ARAD_INIT_PDM_MODE_SIMPLE) {
        
        fld_val =  0x0;
    }else { 
        
        fld_val = 0x1;
    }
    soc_reg_field_set(unit, IQM_IQM_ENABLERSr, &val32, VSQ_CD_ENf, fld_val);

    
    if (hw_adj->dram.fmc_dbuff_mode == ARAD_INIT_FMC_64_REP_128K_DBUFF_MODE) {
        
        fld_val = 0x0;
    } else {
        
        fld_val = 0x1;
    }
    soc_reg_field_set(unit, IQM_IQM_ENABLERSr, &val32, FLMC_4K_REP_ENf, fld_val);

    
    soc_reg_field_set(unit, IQM_IQM_ENABLERSr, &val32, FWD_ACT_SELf,
      IS_ACTION_TYPE_FROM_QUEUE_SIGNATURE(unit) ? 1 : 0);

    
    soc_reg_field_set(unit, IQM_IQM_ENABLERSr, &val32, BUFF_SIZE_BD_SELf,
      SOC_DPP_CONFIG(unit)->tm.guaranteed_q_mode == SOC_DPP_GUARANTEED_Q_RESOURCE_BUFFERS ? 0 : 1);

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 423, exit, soc_dpp_prop_parse_admission_precedence_preference(unit, &fld_val));
    soc_reg_field_set(unit, IQM_IQM_ENABLERSr, &val32, IGNORE_DPf, fld_val);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 425, exit, WRITE_IQM_IQM_ENABLERSr(unit, REG_PORT_ANY, val32)); 
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 430, exit, WRITE_IQM_ECN_CONFIGURATIONr(unit, 255)); 

    if (soc_property_get(unit, spn_ECN_DM_ENABLE, 0)) {
        res = arad_pp_oam_ecn_init(unit);
        SOC_SAND_CHECK_FUNC_RESULT(res, 435, exit);
    }

#ifdef BCM_88660_A0
    

    if (SOC_IS_ARADPLUS(unit)) {

        uint32 use_sinle_lb_range = 1, override_msb=0;
        soc_reg_t reg;
        soc_field_t field;

        if(hw_adj->fabric.ftmh_lb_ext_mode == ARAD_MGMT_FTMH_LB_EXT_MODE_STANDBY_MC_LB) {
            use_sinle_lb_range = 0x0;
            override_msb = 0x1;
        }
        
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  1810,  exit, ARAD_REG_ACCESS_ERR,READ_EGQ_GLOBAL_CONFIGr(unit, SOC_CORE_ALL, &val32));
        ARAD_FLD_TO_REG(EGQ_GLOBAL_CONFIGr, USE_SINGLE_LB_RANGEf, use_sinle_lb_range, val32, 1820, exit);
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  1830,  exit, ARAD_REG_ACCESS_ERR,WRITE_EGQ_GLOBAL_CONFIGr(unit, SOC_CORE_ALL,  val32));

        if (!SOC_IS_ARDON(unit)) { 
            
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  1840,  exit, ARAD_REG_ACCESS_ERR,READ_IHB_LBP_GENERAL_CONFIG_1r(unit, SOC_CORE_ALL, &val64));
            soc_reg64_field32_set(unit, IHB_LBP_GENERAL_CONFIG_1r, &val64, OVERRIDE_FTMH_LB_KEY_MSBf, override_msb);

            
            if (override_msb == 0x1) {
                 soc_reg64_field32_set(unit, IHB_LBP_GENERAL_CONFIG_1r, &val64, OVERRIDE_FTMH_LB_KEY_MSB_VALUEf, 0);
            }

        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  1870,  exit, ARAD_REG_ACCESS_ERR,WRITE_IHB_LBP_GENERAL_CONFIG_1r(unit, SOC_CORE_ALL, val64));
        }

        
        if(hw_adj->fabric.ftmh_lb_ext_mode == ARAD_MGMT_FTMH_LB_EXT_MODE_FULL_HASH) {
            
            reg = SOC_IS_JERICHO(unit)? IHB_LBP_GENERAL_CONFIG_0r: IHB_REG_0092r;
            field = SOC_IS_JERICHO(unit)? STAMP_LAG_LB_KEY_ON_USER_HEADER_2f: ITEM_1_1f;
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  1880,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, reg, SOC_CORE_ALL, 0, field,  1));
            
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  1890,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IHB_LBP_GENERAL_CONFIG_0r, SOC_CORE_ALL, 0, LB_KEY_EXT_USE_MSB_BITSf,  1));
        }

        if(hw_adj->fabric.trunk_hash_format == ARAD_MGMT_TRUNK_HASH_FORMAT_INVERTED){
            
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  1900,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IHB_LBP_GENERAL_CONFIG_0r, SOC_CORE_ALL, 0, SWITCH_LAG_LB_KEY_BITSf,  1));

        } else if(hw_adj->fabric.trunk_hash_format == ARAD_MGMT_TRUNK_HASH_FORMAT_DUPLICATED){
           
            reg = SOC_IS_JERICHO(unit)? IHB_LBP_GENERAL_CONFIG_0r: IHB_REG_0092r;
            field = SOC_IS_JERICHO(unit)? DUPLICATE_LAG_LB_KEY_MSBS_TO_LSBSf: ITEM_0_0f;
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  1910,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, reg, SOC_CORE_ALL, 0, field,  1));
            
            
        }
    }

#endif 

    
    switch(hw_adj->ocb.databuffer_size) {
      case 128:
        fld_val = 0;
        break;
      case 256:
        fld_val = 1;
        break;
      case 512:
        fld_val = 2;
        break;
      case 1024:
        fld_val = 3;
        break;
      default:
        SOC_SAND_SET_ERROR_CODE(ARAD_MGMT_INIT_INVALID_DBUFF_SIZE_ERR, 370, exit);
    }
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  380,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, MMU_OCB_CONFIGURATION_REGISTERr, REG_PORT_ANY, 0, OCB_BUFF_SIZEf,  fld_val));
  }


  fld_val = SOC_DPP_CONFIG(unit)->arad->init.pp_enable;
  disable_pp_reg = SOC_IS_JERICHO(unit)? ECI_GLOBAL_PP_7r: ECI_GLOBAL_1r;
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 390, exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_modify(unit, disable_pp_reg, REG_PORT_ANY, 0, DISABLE_PPf,  fld_val));

  
  if (hw_adj->pll.ts_clk_mode == 0x1) {
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 400, exit, WRITE_CMIC_TS_FREQ_CTRL_LOWERr(unit, SOC_DPP_CONFIG(unit)->arad->init.pll.ts_pll_phase_initial_lo));
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 410, exit, WRITE_CMIC_TS_FREQ_CTRL_UPPERr(unit, SOC_DPP_CONFIG(unit)->arad->init.pll.ts_pll_phase_initial_hi));
  }

  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 420, exit, READ_ECI_BLOCKS_SOFT_INITr(unit, wr_mask_val));
  soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, wr_mask_val, IQM_INITf, 1);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 422, exit, WRITE_ECI_BLOCKS_SOFT_INITr(unit, wr_mask_val));
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 430, exit, READ_ECI_BLOCKS_SOFT_INITr(unit, wr_mask_val));
  soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, wr_mask_val, IQM_INITf, 0);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 432, exit, WRITE_ECI_BLOCKS_SOFT_INITr(unit, wr_mask_val));

    
    if (SOC_IS_ARAD_B0_AND_ABOVE(unit)) {
        res = soc_arad_mgmt_rev_b0_set(unit);
        SOC_SAND_CHECK_FUNC_RESULT(res, 435, exit);
    }

#ifdef BCM_88660_A0
    
    if (SOC_IS_ARADPLUS(unit)) {
        res = soc_arad_mgmt_rev_arad_plus_set(unit);
        SOC_SAND_CHECK_FUNC_RESULT(res, 437, exit);
    }
#endif

    
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  440,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, RTP_MC_TRAVERSE_RATEr, REG_PORT_ANY, 0, MC_TRAVERSE_RATEf,  0x1E00));

    
    if (SOC_IS_ARDON(unit)) {
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 480, exit, ardon_mgmt_drv_pvt_monitor_enable(unit));

    } else {

        COMPILER_64_SET(val64, 0, 0x01D0003);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 490, exit, WRITE_ECI_PVT_MON_A_CONTROL_REGr(unit, val64));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 500, exit, WRITE_ECI_PVT_MON_B_CONTROL_REGr(unit, val64));
        sal_usleep(20000);
        COMPILER_64_SET(val64, 0, 0x41D0003);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 510, exit, WRITE_ECI_PVT_MON_A_CONTROL_REGr(unit, val64));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 520, exit, WRITE_ECI_PVT_MON_B_CONTROL_REGr(unit, val64));
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_mgmt_init_after_blocks_oor()", 0, 0);
}


uint32
  arad_mgmt_init_finalize(
    SOC_SAND_IN int unit
  )
{
    int rv;
    soc_port_t port, link;
    uint32 reg_val,res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_MGMT_INIT_FINALIZE_ERR);

    
    PBMP_SFI_ITER(unit, port)
    {
        link = SOC_DPP_FABRIC_PORT_TO_LINK(unit, port);
        rv = arad_fabric_link_status_clear(unit, link);
        if (rv != SOC_E_NONE) {
            SOC_SAND_SET_ERROR_CODE(ARAD_MGMT_INIT_FINALIZE_ERR, 10, exit);
        }
    }

    
    reg_val = 0x0;
    soc_reg_field_set(unit, EPNI_INIT_TXI_CONFIGr, &reg_val, INIT_TXI_OLPf, 0x1);
    soc_reg_field_set(unit, EPNI_INIT_TXI_CONFIGr, &reg_val, INIT_TXI_OAMf, 0x1);
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,WRITE_EPNI_INIT_TXI_CONFIGr(unit, SOC_CORE_ALL,  reg_val));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_mgmt_init_finalize()", 0, 0);
}

int soc_dpp_arad_dma_init(int unit)
{
    uint32 reg_val;
    soc_reg_above_64_val_t above_64;

    SOCDNX_INIT_FUNC_DEFS;
    reg_val = 0x0;

    if (SOC_IS_JERICHO(unit)) {
        
        SOCDNX_IF_ERR_EXIT(READ_ECI_GP_CONTROL_9r(unit, above_64));
        soc_reg_above_64_field32_set(unit, ECI_GP_CONTROL_9r, above_64, PIR_OAMP_ENABLEf, 0);
        soc_reg_above_64_field32_set(unit, ECI_GP_CONTROL_9r, above_64, PIR_EGQ_0_ENABLEf, 0);
        soc_reg_above_64_field32_set(unit, ECI_GP_CONTROL_9r, above_64, PIR_EGQ_1_ENABLEf, 0);
        soc_reg_above_64_field32_set(unit, ECI_GP_CONTROL_9r, above_64, PIR_TXI_CREDITS_INIT_VALUEf, 32);
        soc_reg_above_64_field32_set(unit, ECI_GP_CONTROL_9r, above_64, PIR_TXI_CREDITS_INITf, 1);
        SOC_DPP_ALLOW_WARMBOOT_WRITE(WRITE_ECI_GP_CONTROL_9r(unit, above_64), _rv);
        soc_reg_above_64_field32_set(unit, ECI_GP_CONTROL_9r, above_64, PIR_TXI_CREDITS_INITf, 0);
        soc_reg_above_64_field32_set(unit, ECI_GP_CONTROL_9r, above_64, PIR_OAMP_ENABLEf, 1);
        soc_reg_above_64_field32_set(unit, ECI_GP_CONTROL_9r, above_64, PIR_EGQ_0_ENABLEf, 1);
        soc_reg_above_64_field32_set(unit, ECI_GP_CONTROL_9r, above_64, PIR_EGQ_1_ENABLEf, 1);
        SOC_DPP_ALLOW_WARMBOOT_WRITE(WRITE_ECI_GP_CONTROL_9r(unit, above_64), _rv);
    } else {
        
        soc_reg_field_set(unit, EPNI_INIT_TXI_CONFIGr, &reg_val, INIT_TXI_CMICMf, 0x1);
        SOC_DPP_ALLOW_WARMBOOT_WRITE(soc_reg32_set(unit, EPNI_INIT_TXI_CONFIGr, REG_PORT_ANY, 0, reg_val), _rv);
    }

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC uint32
  arad_mgmt_functional_init(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_MGMT_INIT           *init,
    SOC_SAND_IN  uint8                 silent
  )
{
  uint32
    res = SOC_SAND_OK;
  soc_error_t
    rc = SOC_E_NONE;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_MGMT_FUNCTIONAL_INIT);


  res = arad_egr_queuing_init(
        unit,
        init->eg_cgm_scheme
      );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  res = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_ipt_init,(unit));
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);


   res = arad_init_mesh_topology(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);

  res = arad_fabric_init(
          unit,
          &(init->fabric)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = arad_ofp_rates_init(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  res = arad_scheduler_end2end_init(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

  res = arad_ipq_init(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 95, exit);

  res = arad_itm_init(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  
  rc = arad_flow_control_init(
          unit
        );
  if (SOC_FAILURE(rc)) {
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 120, exit);
  }

  res = dpp_mc_init(unit);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 130, exit);

  res = arad_parser_init(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 180, exit);

  res = arad_pmf_low_level_init_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 170, exit);

  res = arad_ports_init(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 200, exit);

  res = arad_egr_prog_editor_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 210, exit);

  res = arad_tdm_init(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 230, exit);

  res = arad_cnt_init(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 220, exit);

  res = arad_stat_if_init(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 230, exit);

  if (SOC_DPP_CONFIG(unit)->arad->init.pp_enable == FALSE) {

      if (!SOC_IS_ARDON(unit)) {
          res = arad_pp_isem_access_init_unsafe(unit);
          SOC_SAND_CHECK_FUNC_RESULT(res, 370, exit);

          
          arad_pp_flp_prog_sel_cam_key_program_tm(unit);
          arad_pp_flp_process_key_program_tm(unit);
      }

      res = arad_pp_trap_mgmt_init_unsafe(unit);
      SOC_SAND_CHECK_FUNC_RESULT(res, 400, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_mgmt_functional_init()", 0, 0);
}


uint32
arad_mgmt_nbi_ecc_mask_get(
      SOC_SAND_IN int                unit,
      SOC_SAND_OUT uint64            *mask
      )
{
    soc_port_t port = 0, master_port = 0;
    uint32 res, offset;
    uint64 lanes_mask;
    uint32 lanes_count = 0;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    COMPILER_64_SET(*mask, 0, 0x07fffff);
    PBMP_IL_ITER(unit, port){
       SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, soc_port_sw_db_master_channel_get(unit, port, &master_port));
       if(master_port != port){
           continue;
       }
       SOC_SAND_SOC_IF_ERROR_RETURN(res, 11, exit, soc_port_sw_db_protocol_offset_get(unit, port, 0, &offset));
       SOC_SAND_SOC_IF_ERROR_RETURN(res, 12, exit, soc_port_sw_db_num_lanes_get(unit, port, &lanes_count));

       
       if (lanes_count == 0) {
           continue;
       }

       if (offset == 1) {
           COMPILER_64_MASK_CREATE(lanes_mask, lanes_count, 47 - lanes_count); 
       } else{
           COMPILER_64_MASK_CREATE(lanes_mask, lanes_count, 23); 
       }

       COMPILER_64_OR(*mask, lanes_mask);
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_mgmt_nbi_ecc_mask_get()", 0, 0);
}

uint32
arad_ser_init(int unit)
{
    uint32 inst_idx, reg32_val, res;
    uint64                 field64;
    soc_reg_above_64_val_t above_64;
    soc_block_types_t  block;
    int blk;
    int instance;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_MGMT_HW_SET_DEFAULTS);

    if (!SOC_UNIT_NUM_VALID(unit)) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_ILLEGAL_DEVICE_ID, 4, exit);
    }
    if (!SOC_WARM_BOOT(unit)) {
        if (!SOC_IS_ARDON(unit)) {
            
            SOC_REG_ABOVE_64_ALLONES(above_64);
            for(inst_idx=0; arad_interrupt_monitor_mem_reg[inst_idx] != INVALIDr; inst_idx++) {
                block = SOC_REG_INFO(unit, arad_interrupt_monitor_mem_reg[inst_idx]).block;
                SOC_BLOCKS_ITER(unit, blk, block) {
                    instance = (SOC_BLOCK_TYPE(unit, blk) == SOC_BLK_CLP || SOC_BLOCK_TYPE(unit, blk) == SOC_BLK_XLP) ? SOC_BLOCK_PORT(unit, blk) : SOC_BLOCK_NUMBER(unit, blk);
                    SOC_SAND_SOC_IF_ERROR_RETURN(res, 24, exit, soc_reg_above_64_set(unit, arad_interrupt_monitor_mem_reg[inst_idx], instance, 0, above_64));
                }
            }
            reg32_val = 0xFFFFFFFF;
            if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
                SOC_SAND_SOC_IF_ERROR_RETURN(res, 22, exit, soc_reg32_set(unit, IHB_ECC_ERR_1B_MONITOR_MEM_MASKr, REG_PORT_ANY, 0, reg32_val));
                SOC_SAND_SOC_IF_ERROR_RETURN(res, 22, exit, soc_reg32_set(unit, IHB_ECC_ERR_2B_MONITOR_MEM_MASKr, REG_PORT_ANY, 0, reg32_val));
            }
            if (SOC_IS_ARADPLUS_A0(unit)) {
                SOC_SAND_SOC_IF_ERROR_RETURN(res, 22, exit, soc_reg32_set(unit, IHB_REG_009A_1r, REG_PORT_ANY, 0, reg32_val));
                SOC_SAND_SOC_IF_ERROR_RETURN(res, 22, exit, soc_reg32_set(unit, IHB_REG_009A_2r, REG_PORT_ANY, 0, reg32_val));
            }
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 37, exit, soc_reg_above_64_field32_modify(unit, NBI_PARITY_ERR_MONITOR_MEM_MASKr, REG_PORT_ANY, 0, RLENG_MEM_PARITY_ERR_MASKf, 0));
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 38, exit, soc_reg_above_64_field32_modify(unit, NBI_PARITY_ERR_MONITOR_MEM_MASKr, REG_PORT_ANY, 0, TLENG_MEM_PARITY_ERR_MASKf, 0));
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 39, exit, soc_reg_above_64_field32_modify(unit, NBI_PARITY_ERR_MONITOR_MEM_MASKr, REG_PORT_ANY, 0, RTYPE_MEM_PARITY_ERR_MASKf, 0));
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, soc_reg_above_64_field32_modify(unit, NBI_PARITY_ERR_MONITOR_MEM_MASKr, REG_PORT_ANY, 0, TTYPE_MEM_PARITY_ERR_MASKf, 0));
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 41, exit, soc_reg_above_64_field32_modify(unit, NBI_PARITY_ERR_MONITOR_MEM_MASKr, REG_PORT_ANY, 0, RPKTS_MEM_PARITY_ERR_MASKf, 0));
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 42, exit, soc_reg_above_64_field32_modify(unit, NBI_PARITY_ERR_MONITOR_MEM_MASKr, REG_PORT_ANY, 0, TPKTS_MEM_PARITY_ERR_MASKf, 0));
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 43, exit, soc_reg_above_64_field32_modify(unit, NBI_PARITY_ERR_MONITOR_MEM_MASKr, REG_PORT_ANY, 0, RMON_MEM_PARITY_ERR_MASKf, 0));
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 44, exit, soc_reg_above_64_field32_modify(unit, NBI_PARITY_ERR_MONITOR_MEM_MASKr, REG_PORT_ANY, 0, TMON_MEM_PARITY_ERR_MASKf, 0));
        } else {
            
            SOC_REG_ABOVE_64_ALLONES(above_64);
            for(inst_idx=0; ardon_interrupt_monitor_mem_reg[inst_idx] != INVALIDr; inst_idx++) {
                block = SOC_REG_INFO(unit, ardon_interrupt_monitor_mem_reg[inst_idx]).block;
                SOC_BLOCKS_ITER(unit, blk, block) {
                    instance = (SOC_BLOCK_TYPE(unit, blk) == SOC_BLK_CLP || SOC_BLOCK_TYPE(unit, blk) == SOC_BLK_XLP) ? SOC_BLOCK_PORT(unit, blk) : SOC_BLOCK_NUMBER(unit, blk);
                    SOC_SAND_SOC_IF_ERROR_RETURN(res, 24, exit, soc_reg_above_64_set(unit, ardon_interrupt_monitor_mem_reg[inst_idx], instance, 0, above_64));
                }
            }
        }

        SOC_SAND_SOC_IF_ERROR_RETURN(res, 24, exit, arad_mgmt_nbi_ecc_mask_get(unit, &field64));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 25, exit, soc_reg_set(unit, NBI_ECC_ERR_1B_MONITOR_MEM_MASKr, REG_PORT_ANY, 0, field64));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 26, exit, soc_reg_set(unit, NBI_ECC_ERR_2B_MONITOR_MEM_MASKr, REG_PORT_ANY, 0, field64));

        
        if(!SOC_IS_JERICHO_PLUS_A0(unit)){
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 28, exit, soc_reg_field32_modify(unit, EGQ_ECC_ERR_1B_MONITOR_MEM_MASKr, REG_PORT_ANY, BUF_LINK_ECC_1B_ERR_MASKf, 0));
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 29, exit, soc_reg_field32_modify(unit, EGQ_ECC_ERR_2B_MONITOR_MEM_MASKr, REG_PORT_ANY, BUF_LINK_ECC_2B_ERR_MASKf, 0));
        }
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, soc_reg_field32_modify(unit, IDR_ECC_ERR_1B_MONITOR_MEM_MASKr, REG_PORT_ANY, CHUNK_STATUS_ECC__NB_ERR_MASKf, 0));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 31, exit, soc_reg_field32_modify(unit, IDR_ECC_ERR_2B_MONITOR_MEM_MASKr, REG_PORT_ANY, CHUNK_STATUS_ECC__NB_ERR_MASKf, 0));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 32, exit, soc_reg_field32_modify(unit, IDR_PAR_ERR_MEM_MASKr, REG_PORT_ANY, CONTEXT_STATUS_DATA_PARITY_ERR_MASKf, 0));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 33, exit, soc_reg_field32_modify(unit, IRR_PAR_ERR_MEM_MASKr, REG_PORT_ANY, SNOOP_MIRROR_TABLE_0_PARITY_ERR_MASKf, 0));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 34, exit, soc_reg_field32_modify(unit, IRR_PAR_ERR_MEM_MASKr, REG_PORT_ANY, SNOOP_MIRROR_TABLE_1_PARITY_ERR_MASKf, 0));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 35, exit, soc_reg_field32_modify(unit, IRE_PAR_ERR_MEM_MASKr, REG_PORT_ANY, NIF_PORT_TO_CTXT_BIT_MAP_PARITY_ERR_MASKf, 0));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 33, exit, soc_reg_field32_modify(unit, IRE_ECC_ERR_2B_MONITOR_MEM_MASKr, REG_PORT_ANY, CTXT_MEM_CONTROL_ECC__NB_ERR_MASKf, 0));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 34, exit, soc_reg_field32_modify(unit, IRE_ECC_ERR_1B_MONITOR_MEM_MASKr, REG_PORT_ANY, CTXT_MEM_CONTROL_ECC__NB_ERR_MASKf, 0));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 35, exit, soc_reg_field32_modify(unit, IRE_PAR_ERR_MEM_MASKr, REG_PORT_ANY, CTXT_MEM_DATA_PARITY_ERR_MASKf, 0));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 36, exit, soc_reg_above_64_field32_modify(unit, EPNI_PAR_ERR_MEM_MASKr, REG_PORT_ANY, 0, CNTX_PARITY_ERR_MASKf, 0));


    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_mgmt_hw_set_defaults()", 0, 0);
}

STATIC uint32
  arad_mgmt_hw_set_defaults(
    SOC_SAND_IN  int                 unit
  )
{
   uint32
    inst_idx = 0,
    reg32_val = 0,
    res;
   uint64                 field64;
   soc_reg_above_64_val_t above_64;
   soc_block_types_t  block;
   int blk;
   int instance;

   SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_MGMT_HW_SET_DEFAULTS);



    if (!SOC_UNIT_NUM_VALID(unit)) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_ILLEGAL_DEVICE_ID, 4, exit);
    }
    if (!SOC_WARM_BOOT(unit)) {
        SOC_REG_ABOVE_64_CLEAR(above_64);
        if (SOC_IS_ARDON(unit)) {
            for(inst_idx=0; ardon_interrupts_mask_registers[inst_idx] != NUM_SOC_REG; ++inst_idx) {
                block = SOC_REG_INFO(unit, ardon_interrupts_mask_registers[inst_idx]).block;
                SOC_BLOCKS_ITER(unit, blk, block) {
                    instance = (SOC_BLOCK_TYPE(unit, blk) == SOC_BLK_CLP || SOC_BLOCK_TYPE(unit, blk) == SOC_BLK_XLP) ? SOC_BLOCK_PORT(unit, blk) : SOC_BLOCK_NUMBER(unit, blk);
                    SOC_SAND_SOC_IF_ERROR_RETURN(res, 19, exit, soc_reg_above_64_set(unit, ardon_interrupts_mask_registers[inst_idx], instance, 0, above_64));
                }
            }
        } else {
            for(inst_idx=0; arad_interrupts_mask_registers[inst_idx] != NUM_SOC_REG; ++inst_idx) {
                block = SOC_REG_INFO(unit, arad_interrupts_mask_registers[inst_idx]).block;
                SOC_BLOCKS_ITER(unit, blk, block) {
                    instance = (SOC_BLOCK_TYPE(unit, blk) == SOC_BLK_CLP || SOC_BLOCK_TYPE(unit, blk) == SOC_BLK_XLP) ? SOC_BLOCK_PORT(unit, blk) : SOC_BLOCK_NUMBER(unit, blk);
                    SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, soc_reg_above_64_set(unit, arad_interrupts_mask_registers[inst_idx], instance, 0, above_64));
                }
            }
        }
        
        COMPILER_64_ZERO(field64);
        if(SOC_DPP_CONFIG(unit)->arad->init.dram.enable == TRUE){
            for(inst_idx=0; arad_interrupts_dram_mask_registers[inst_idx] != NUM_SOC_REG; ++inst_idx) {
                if(SOC_REG_IS_VALID(unit, arad_interrupts_dram_mask_registers[inst_idx])) {
                    SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, soc_reg_set(unit, arad_interrupts_dram_mask_registers[inst_idx], REG_PORT_ANY, 0, field64));
                }
            }
        }

        reg32_val = 0;
        
        soc_reg_field_set(unit, IHB_INTERRUPT_REGISTERr, &reg32_val, TCAM_PROTECTION_ERRORf, 1);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 45, exit, WRITE_IHB_INTERRUPT_REGISTERr(unit, SOC_CORE_ALL, reg32_val));
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_mgmt_hw_set_defaults()", 0, 0);
}





uint32
  arad_init_mesh_topology(
    SOC_SAND_IN int             unit
    )
{
    uint32 field_val;
    SOCDNX_INIT_FUNC_DEFS;

    field_val = SOC_DPP_IMP_DEFS_GET(unit, mesh_topology_register_config_4_val);

    if (!SOC_UNIT_NUM_VALID(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNIT, (_BSL_SOCDNX_MSG("Unit num %d is invalid\n"), unit));
    }

    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, MESH_TOPOLOGY_MESH_TOPOLOGYr, REG_PORT_ANY, 0, CONFIG_4f, field_val));

exit:
    SOCDNX_FUNC_RETURN;
}


uint32
    arad_mgmt_ipt_init(
      SOC_SAND_IN int             unit
      )
{
    uint32
        fabric_connect_mode,
        is_dual_mode,
        dtq_size0, dtq_size1, dtq_size2_6,
        dtq_th0, dtq_th1, dtq_th2_6,
        dpq_size0_5, dpq_size6_15,
        dpq_dqcq_th0_5, dpq_dqcq_th6_15,
        dpq_eir_th0_5, dpq_eir_th6_15,
        dpq_mc_th,
        reg32_val,
        fld;
    uint32 res;
    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_MGMT_IPT_INIT);

    if (!SOC_UNIT_NUM_VALID(unit)) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_ILLEGAL_DEVICE_ID, 4, exit);
    }

    
    fabric_connect_mode = SOC_DPP_CONFIG(unit)->arad->init.fabric.connect_mode;
    is_dual_mode = SOC_DPP_CONFIG(unit)->arad->init.fabric.dual_pipe_tdm_packet;

    

    
    if (fabric_connect_mode == SOC_TMC_FABRIC_CONNECT_MODE_FE || fabric_connect_mode == SOC_TMC_FABRIC_CONNECT_MODE_MULT_STAGE_FE) {
        
        if (is_dual_mode) {
            dtq_size0 = 0x1ea;
            dtq_size1 = 0x1ea;
            
            dtq_size2_6=0x2;
        } else {
            dtq_size0 = 0x3e8;
            
            dtq_size1 = 0x2;
            dtq_size2_6 = 0x2;
        }
    } else {
        dtq_size0 = 0x91;
        dtq_size1 = 0x91;
        dtq_size2_6 = 0x91;
    }
    reg32_val = 0;
    soc_reg_field_set(unit, IPT_TRANSMIT_DATA_QUEUE_SIZE_0_1r, &reg32_val, DTQ_SIZE_0f, dtq_size0);
    soc_reg_field_set(unit, IPT_TRANSMIT_DATA_QUEUE_SIZE_0_1r, &reg32_val, DTQ_SIZE_1f, dtq_size1);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 11, exit, WRITE_IPT_TRANSMIT_DATA_QUEUE_SIZE_0_1r(unit, reg32_val));

    reg32_val = 0;
    soc_reg_field_set(unit, IPT_TRANSMIT_DATA_QUEUE_SIZE_2_3r, &reg32_val, DTQ_SIZE_2f, dtq_size2_6);
    soc_reg_field_set(unit, IPT_TRANSMIT_DATA_QUEUE_SIZE_2_3r, &reg32_val, DTQ_SIZE_3f, dtq_size2_6);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 13, exit, WRITE_IPT_TRANSMIT_DATA_QUEUE_SIZE_2_3r(unit, reg32_val));

    reg32_val = 0;
    soc_reg_field_set(unit, IPT_TRANSMIT_DATA_QUEUE_SIZE_4_5r, &reg32_val, DTQ_SIZE_4f, dtq_size2_6);
    soc_reg_field_set(unit, IPT_TRANSMIT_DATA_QUEUE_SIZE_4_5r, &reg32_val, DTQ_SIZE_5f, dtq_size2_6);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 15, exit, WRITE_IPT_TRANSMIT_DATA_QUEUE_SIZE_4_5r(unit, reg32_val));

    reg32_val = 0;
    soc_reg_field_set(unit, IPT_TRANSMIT_DATA_QUEUE_SIZE_6r, &reg32_val, DTQ_SIZE_6f, dtq_size2_6);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 17, exit, WRITE_IPT_TRANSMIT_DATA_QUEUE_SIZE_6r(unit, reg32_val));

    
    reg32_val = 0;
    fld = 0;
    soc_reg_field_set(unit, IPT_TRANSMIT_DATA_QUEUE_START_ADRESS_0_1r, &reg32_val, DTQ_START_0f, fld);
    fld += dtq_size0 + 1;
    soc_reg_field_set(unit, IPT_TRANSMIT_DATA_QUEUE_START_ADRESS_0_1r, &reg32_val, DTQ_START_1f, fld);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 21, exit, WRITE_IPT_TRANSMIT_DATA_QUEUE_START_ADRESS_0_1r(unit, reg32_val));

    reg32_val = 0;
    fld += dtq_size1 + 1;
    soc_reg_field_set(unit, IPT_TRANSMIT_DATA_QUEUE_START_ADRESS_2_3r, &reg32_val, DTQ_START_2f, fld);
    fld += dtq_size2_6 + 1;
    soc_reg_field_set(unit, IPT_TRANSMIT_DATA_QUEUE_START_ADRESS_2_3r, &reg32_val, DTQ_START_3f, fld);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 23, exit, WRITE_IPT_TRANSMIT_DATA_QUEUE_START_ADRESS_2_3r(unit, reg32_val));

    reg32_val = 0;
    fld += dtq_size2_6 + 1;
    soc_reg_field_set(unit, IPT_TRANSMIT_DATA_QUEUE_START_ADRESS_4_5r, &reg32_val, DTQ_START_4f, fld);
    fld += dtq_size2_6 + 1;
    soc_reg_field_set(unit, IPT_TRANSMIT_DATA_QUEUE_START_ADRESS_4_5r, &reg32_val, DTQ_START_5f, fld);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 25, exit, WRITE_IPT_TRANSMIT_DATA_QUEUE_START_ADRESS_4_5r(unit, reg32_val));

    reg32_val = 0;
    fld += dtq_size2_6 + 1;
    soc_reg_field_set(unit, IPT_TRANSMIT_DATA_QUEUE_START_ADRESS_6r, &reg32_val, DTQ_START_6f, fld);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 27, exit, WRITE_IPT_TRANSMIT_DATA_QUEUE_START_ADRESS_6r(unit, reg32_val));

    
    dtq_th0 = dtq_size0/2;
    dtq_th1 = dtq_size1/2;
    dtq_th2_6 = dtq_size2_6/2;

    reg32_val = 0;
    soc_reg_field_set(unit, IPT_TRANSMIT_DATA_QUEUE_THRESHOLD_0_1r, &reg32_val, DTQ_TH_0f, dtq_th0);
    soc_reg_field_set(unit, IPT_TRANSMIT_DATA_QUEUE_THRESHOLD_0_1r, &reg32_val, DTQ_TH_1f, dtq_th1);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 31, exit, WRITE_IPT_TRANSMIT_DATA_QUEUE_THRESHOLD_0_1r(unit, reg32_val));

    reg32_val = 0;
    soc_reg_field_set(unit, IPT_TRANSMIT_DATA_QUEUE_THRESHOLD_2_3r, &reg32_val, DTQ_TH_2f, dtq_th2_6);
    soc_reg_field_set(unit, IPT_TRANSMIT_DATA_QUEUE_THRESHOLD_2_3r, &reg32_val, DTQ_TH_3f, dtq_th2_6);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 33, exit, WRITE_IPT_TRANSMIT_DATA_QUEUE_THRESHOLD_2_3r(unit, reg32_val));

    reg32_val = 0;
    soc_reg_field_set(unit, IPT_TRANSMIT_DATA_QUEUE_THRESHOLD_4_5r, &reg32_val, DTQ_TH_4f, dtq_th2_6);
    soc_reg_field_set(unit, IPT_TRANSMIT_DATA_QUEUE_THRESHOLD_4_5r, &reg32_val, DTQ_TH_5f, dtq_th2_6);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 35, exit, WRITE_IPT_TRANSMIT_DATA_QUEUE_THRESHOLD_4_5r(unit, reg32_val));

    reg32_val = 0;
    soc_reg_field_set(unit, IPT_TRANSMIT_DATA_QUEUE_THRESHOLD_6r, &reg32_val, DTQ_TH_6f, dtq_th2_6);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 37, exit, WRITE_IPT_TRANSMIT_DATA_QUEUE_THRESHOLD_6r(unit, reg32_val));


    

    
    if (fabric_connect_mode == SOC_TMC_FABRIC_CONNECT_MODE_FE || fabric_connect_mode == SOC_TMC_FABRIC_CONNECT_MODE_MULT_STAGE_FE) {
        
        
        
        
        dpq_size0_5 = 0x2a3;

        
        dpq_size6_15 = 0x2;

    } else {
        
        dpq_size0_5 = 0xff;
        dpq_size6_15 = 0xff;
    }

    reg32_val = 0;
    soc_reg_field_set(unit, IPT_DRAM_BUFFER_POINTER_QUEUE_SIZE_0_1r, &reg32_val, DPQ_SIZE_0f, dpq_size0_5);
    soc_reg_field_set(unit, IPT_DRAM_BUFFER_POINTER_QUEUE_SIZE_0_1r, &reg32_val, DPQ_SIZE_1f, dpq_size0_5);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 101, exit, WRITE_IPT_DRAM_BUFFER_POINTER_QUEUE_SIZE_0_1r(unit, reg32_val));

    reg32_val = 0;
    soc_reg_field_set(unit, IPT_DRAM_BUFFER_POINTER_QUEUE_SIZE_2_3r, &reg32_val, DPQ_SIZE_2f, dpq_size0_5);
    soc_reg_field_set(unit, IPT_DRAM_BUFFER_POINTER_QUEUE_SIZE_2_3r, &reg32_val, DPQ_SIZE_3f, dpq_size0_5);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 103, exit, WRITE_IPT_DRAM_BUFFER_POINTER_QUEUE_SIZE_2_3r(unit, reg32_val));

    reg32_val = 0;
    soc_reg_field_set(unit, IPT_DRAM_BUFFER_POINTER_QUEUE_SIZE_4_5r, &reg32_val, DPQ_SIZE_4f, dpq_size0_5);
    soc_reg_field_set(unit, IPT_DRAM_BUFFER_POINTER_QUEUE_SIZE_4_5r, &reg32_val, DPQ_SIZE_5f, dpq_size0_5);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 105, exit, WRITE_IPT_DRAM_BUFFER_POINTER_QUEUE_SIZE_4_5r(unit, reg32_val));

    reg32_val = 0;
    soc_reg_field_set(unit, IPT_DRAM_BUFFER_POINTER_QUEUE_SIZE_6_7r, &reg32_val, DPQ_SIZE_6f, dpq_size6_15);
    soc_reg_field_set(unit, IPT_DRAM_BUFFER_POINTER_QUEUE_SIZE_6_7r, &reg32_val, DPQ_SIZE_7f, dpq_size6_15);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 107, exit, WRITE_IPT_DRAM_BUFFER_POINTER_QUEUE_SIZE_6_7r(unit, reg32_val));


    reg32_val = 0;
    soc_reg_field_set(unit, IPT_DRAM_BUFFER_POINTER_QUEUE_SIZE_6_7r, &reg32_val, DPQ_SIZE_6f, dpq_size6_15);
    soc_reg_field_set(unit, IPT_DRAM_BUFFER_POINTER_QUEUE_SIZE_6_7r, &reg32_val, DPQ_SIZE_7f, dpq_size6_15);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 109, exit, WRITE_IPT_DRAM_BUFFER_POINTER_QUEUE_SIZE_6_7r(unit, reg32_val));


    reg32_val = 0;
    soc_reg_field_set(unit, IPT_DRAM_BUFFER_POINTER_QUEUE_SIZE_8_9r, &reg32_val, DPQ_SIZE_8f, dpq_size6_15);
    soc_reg_field_set(unit, IPT_DRAM_BUFFER_POINTER_QUEUE_SIZE_8_9r, &reg32_val, DPQ_SIZE_9f, dpq_size6_15);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 111, exit, WRITE_IPT_DRAM_BUFFER_POINTER_QUEUE_SIZE_8_9r(unit, reg32_val));


    reg32_val = 0;
    soc_reg_field_set(unit, IPT_DRAM_BUFFER_POINTER_QUEUE_SIZE_10_11r, &reg32_val, DPQ_SIZE_10f, dpq_size6_15);
    soc_reg_field_set(unit, IPT_DRAM_BUFFER_POINTER_QUEUE_SIZE_10_11r, &reg32_val, DPQ_SIZE_11f, dpq_size6_15);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 113, exit, WRITE_IPT_DRAM_BUFFER_POINTER_QUEUE_SIZE_10_11r(unit, reg32_val));


    reg32_val = 0;
    soc_reg_field_set(unit, IPT_DRAM_BUFFER_POINTER_QUEUE_SIZE_12_13r, &reg32_val, DPQ_SIZE_12f, dpq_size6_15);
    soc_reg_field_set(unit, IPT_DRAM_BUFFER_POINTER_QUEUE_SIZE_12_13r, &reg32_val, DPQ_SIZE_13f, dpq_size6_15);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 115, exit, WRITE_IPT_DRAM_BUFFER_POINTER_QUEUE_SIZE_12_13r(unit, reg32_val));


    reg32_val = 0;
    soc_reg_field_set(unit, IPT_DRAM_BUFFER_POINTER_QUEUE_SIZE_14_15r, &reg32_val, DPQ_SIZE_14f, dpq_size6_15);
    soc_reg_field_set(unit, IPT_DRAM_BUFFER_POINTER_QUEUE_SIZE_14_15r, &reg32_val, DPQ_SIZE_15f, dpq_size6_15);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 117, exit, WRITE_IPT_DRAM_BUFFER_POINTER_QUEUE_SIZE_14_15r(unit, reg32_val));

    
    reg32_val = 0;
    fld = 0;
    soc_reg_field_set(unit, IPT_DRAM_BUFFER_POINTER_QUEUE_START_ADDRESS_0_1r, &reg32_val, DPQ_START_0f, fld);
    fld += dpq_size0_5 + 1;
    soc_reg_field_set(unit, IPT_DRAM_BUFFER_POINTER_QUEUE_START_ADDRESS_0_1r, &reg32_val, DPQ_START_1f, fld);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 121, exit, WRITE_IPT_DRAM_BUFFER_POINTER_QUEUE_START_ADDRESS_0_1r(unit, reg32_val));

    reg32_val = 0;
    fld += dpq_size0_5 + 1;
    soc_reg_field_set(unit, IPT_DRAM_BUFFER_POINTER_QUEUE_START_ADDRESS_2_3r, &reg32_val, DPQ_START_2f, fld);
    fld += dpq_size0_5 + 1;
    soc_reg_field_set(unit, IPT_DRAM_BUFFER_POINTER_QUEUE_START_ADDRESS_2_3r, &reg32_val, DPQ_START_3f, fld);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 123, exit, WRITE_IPT_DRAM_BUFFER_POINTER_QUEUE_START_ADDRESS_2_3r(unit, reg32_val));

    reg32_val = 0;
    fld += dpq_size0_5 + 1;
    soc_reg_field_set(unit, IPT_DRAM_BUFFER_POINTER_QUEUE_START_ADDRESS_4_5r, &reg32_val, DPQ_START_4f, fld);
    fld += dpq_size0_5 + 1;
    soc_reg_field_set(unit, IPT_DRAM_BUFFER_POINTER_QUEUE_START_ADDRESS_4_5r, &reg32_val, DPQ_START_5f, fld);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 125, exit, WRITE_IPT_DRAM_BUFFER_POINTER_QUEUE_START_ADDRESS_4_5r(unit, reg32_val));

    reg32_val = 0;
    fld += dpq_size6_15 + 1;
    soc_reg_field_set(unit, IPT_DRAM_BUFFER_POINTER_QUEUE_START_ADDRESS_6_7r, &reg32_val, DPQ_START_6f, fld);
    fld += dpq_size6_15 + 1;
    soc_reg_field_set(unit, IPT_DRAM_BUFFER_POINTER_QUEUE_START_ADDRESS_6_7r, &reg32_val, DPQ_START_7f, fld);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 127, exit, WRITE_IPT_DRAM_BUFFER_POINTER_QUEUE_START_ADDRESS_6_7r(unit, reg32_val));

    reg32_val = 0;
    fld += dpq_size6_15 + 1;
    soc_reg_field_set(unit, IPT_DRAM_BUFFER_POINTER_QUEUE_START_ADDRESS_8_9r, &reg32_val, DPQ_START_8f, fld);
    fld += dpq_size6_15 + 1;
    soc_reg_field_set(unit, IPT_DRAM_BUFFER_POINTER_QUEUE_START_ADDRESS_8_9r, &reg32_val, DPQ_START_9f, fld);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 129, exit, WRITE_IPT_DRAM_BUFFER_POINTER_QUEUE_START_ADDRESS_8_9r(unit, reg32_val));

    reg32_val = 0;
    fld += dpq_size6_15 + 1;
    soc_reg_field_set(unit, IPT_DRAM_BUFFER_POINTER_QUEUE_START_ADDRESS_10_11r, &reg32_val, DPQ_START_10f, fld);
    fld += dpq_size6_15 + 1;
    soc_reg_field_set(unit, IPT_DRAM_BUFFER_POINTER_QUEUE_START_ADDRESS_10_11r, &reg32_val, DPQ_START_11f, fld);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 131, exit, WRITE_IPT_DRAM_BUFFER_POINTER_QUEUE_START_ADDRESS_10_11r(unit, reg32_val));

    reg32_val = 0;
    fld += dpq_size6_15 + 1;
    soc_reg_field_set(unit, IPT_DRAM_BUFFER_POINTER_QUEUE_START_ADDRESS_12_13r, &reg32_val, DPQ_START_12f, fld);
    fld += dpq_size6_15 + 1;
    soc_reg_field_set(unit, IPT_DRAM_BUFFER_POINTER_QUEUE_START_ADDRESS_12_13r, &reg32_val, DPQ_START_13f, fld);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 133, exit, WRITE_IPT_DRAM_BUFFER_POINTER_QUEUE_START_ADDRESS_12_13r(unit, reg32_val));

    reg32_val = 0;
    fld += dpq_size6_15 + 1;
    soc_reg_field_set(unit, IPT_DRAM_BUFFER_POINTER_QUEUE_START_ADDRESS_14_15r, &reg32_val, DPQ_START_14f, fld);
    fld += dpq_size6_15 + 1;
    soc_reg_field_set(unit, IPT_DRAM_BUFFER_POINTER_QUEUE_START_ADDRESS_14_15r, &reg32_val, DPQ_START_15f, fld);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 135, exit, WRITE_IPT_DRAM_BUFFER_POINTER_QUEUE_START_ADDRESS_14_15r(unit, reg32_val));

    
    dpq_dqcq_th0_5 = dpq_size0_5/2;
    dpq_dqcq_th6_15 = dpq_size6_15/2;

    reg32_val = 0;
    soc_reg_field_set(unit, IPT_DRAM_BUFFER_POINTER_QUEUE_DQCQ_THRESHOLD_01r, &reg32_val, DPQ_DQCQ_TH_0f, dpq_dqcq_th0_5);
    soc_reg_field_set(unit, IPT_DRAM_BUFFER_POINTER_QUEUE_DQCQ_THRESHOLD_01r, &reg32_val, DPQ_DQCQ_TH_1f, dpq_dqcq_th0_5);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 141, exit, WRITE_IPT_DRAM_BUFFER_POINTER_QUEUE_DQCQ_THRESHOLD_01r(unit, reg32_val));

    reg32_val = 0;
    soc_reg_field_set(unit, IPT_DRAM_BUFFER_POINTER_QUEUE_DQCQ_THRESHOLD_23r, &reg32_val, DPQ_DQCQ_TH_2f, dpq_dqcq_th0_5);
    soc_reg_field_set(unit, IPT_DRAM_BUFFER_POINTER_QUEUE_DQCQ_THRESHOLD_23r, &reg32_val, DPQ_DQCQ_TH_3f, dpq_dqcq_th0_5);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 143, exit, WRITE_IPT_DRAM_BUFFER_POINTER_QUEUE_DQCQ_THRESHOLD_23r(unit, reg32_val));

    reg32_val = 0;
    soc_reg_field_set(unit, IPT_DRAM_BUFFER_POINTER_QUEUE_DQCQ_THRESHOLD_4_5r, &reg32_val, DPQ_DQCQ_TH_4f, dpq_dqcq_th0_5);
    soc_reg_field_set(unit, IPT_DRAM_BUFFER_POINTER_QUEUE_DQCQ_THRESHOLD_4_5r, &reg32_val, DPQ_DQCQ_TH_5f, dpq_dqcq_th0_5);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 145, exit, WRITE_IPT_DRAM_BUFFER_POINTER_QUEUE_DQCQ_THRESHOLD_4_5r(unit, reg32_val));

    reg32_val = 0;
    soc_reg_field_set(unit, IPT_DRAM_BUFFER_POINTER_QUEUE_DQCQ_THRESHOLD_6_7r, &reg32_val, DPQ_DQCQ_TH_6f, dpq_dqcq_th6_15);
    soc_reg_field_set(unit, IPT_DRAM_BUFFER_POINTER_QUEUE_DQCQ_THRESHOLD_6_7r, &reg32_val, DPQ_DQCQ_TH_7f, dpq_dqcq_th6_15);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 147, exit, WRITE_IPT_DRAM_BUFFER_POINTER_QUEUE_DQCQ_THRESHOLD_6_7r(unit, reg32_val));

    reg32_val = 0;
    soc_reg_field_set(unit, IPT_DRAM_BUFFER_POINTER_QUEUE_DQCQ_THRESHOLD_8_9r, &reg32_val, DPQ_DQCQ_TH_8f, dpq_dqcq_th6_15);
    soc_reg_field_set(unit, IPT_DRAM_BUFFER_POINTER_QUEUE_DQCQ_THRESHOLD_8_9r, &reg32_val, DPQ_DQCQ_TH_9f, dpq_dqcq_th6_15);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 149, exit, WRITE_IPT_DRAM_BUFFER_POINTER_QUEUE_DQCQ_THRESHOLD_8_9r(unit, reg32_val));

    reg32_val = 0;
    soc_reg_field_set(unit, IPT_DRAM_BUFFER_POINTER_QUEUE_DQCQ_THRESHOLD_10_11r, &reg32_val, DPQ_DQCQ_TH_10f, dpq_dqcq_th6_15);
    soc_reg_field_set(unit, IPT_DRAM_BUFFER_POINTER_QUEUE_DQCQ_THRESHOLD_10_11r, &reg32_val, DPQ_DQCQ_TH_11f, dpq_dqcq_th6_15);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 151, exit, WRITE_IPT_DRAM_BUFFER_POINTER_QUEUE_DQCQ_THRESHOLD_10_11r(unit, reg32_val));

    reg32_val = 0;
    soc_reg_field_set(unit, IPT_DRAM_BUFFER_POINTER_QUEUE_DQCQ_THRESHOLD_12_13r, &reg32_val, DPQ_DQCQ_TH_12f, dpq_dqcq_th6_15);
    soc_reg_field_set(unit, IPT_DRAM_BUFFER_POINTER_QUEUE_DQCQ_THRESHOLD_12_13r, &reg32_val, DPQ_DQCQ_TH_13f, dpq_dqcq_th6_15);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 153, exit, WRITE_IPT_DRAM_BUFFER_POINTER_QUEUE_DQCQ_THRESHOLD_12_13r(unit, reg32_val));

    reg32_val = 0;
    soc_reg_field_set(unit, IPT_DRAM_BUFFER_POINTER_QUEUE_DQCQ_THRESHOLD_14_15r, &reg32_val, DPQ_DQCQ_TH_14f, dpq_dqcq_th6_15);
    soc_reg_field_set(unit, IPT_DRAM_BUFFER_POINTER_QUEUE_DQCQ_THRESHOLD_14_15r, &reg32_val, DPQ_DQCQ_TH_15f, dpq_dqcq_th6_15);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 155, exit, WRITE_IPT_DRAM_BUFFER_POINTER_QUEUE_DQCQ_THRESHOLD_14_15r(unit, reg32_val));

    
    dpq_eir_th0_5 = 2*dpq_size0_5/3;
    dpq_eir_th6_15 = 2*dpq_size6_15/3;

    reg32_val = 0;
    soc_reg_field_set(unit, IPT_DRAM_BUFFER_POINTER_QUEUE_EIR_CRDT_THRESHOLD_0_1r, &reg32_val, DPQ_EIR_CRDT_TH_0f, dpq_eir_th0_5);
    soc_reg_field_set(unit, IPT_DRAM_BUFFER_POINTER_QUEUE_EIR_CRDT_THRESHOLD_0_1r, &reg32_val, DPQ_EIR_CRDT_TH_1f, dpq_eir_th0_5);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 161, exit, WRITE_IPT_DRAM_BUFFER_POINTER_QUEUE_EIR_CRDT_THRESHOLD_0_1r(unit, reg32_val));

    reg32_val = 0;
    soc_reg_field_set(unit, IPT_DRAM_BUFFER_POINTER_QUEUE_EIR_CRDT_THRESHOLD_2_3r, &reg32_val, DPQ_EIR_CRDT_TH_2f, dpq_eir_th0_5);
    soc_reg_field_set(unit, IPT_DRAM_BUFFER_POINTER_QUEUE_EIR_CRDT_THRESHOLD_2_3r, &reg32_val, DPQ_EIR_CRDT_TH_3f, dpq_eir_th0_5);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 163, exit, WRITE_IPT_DRAM_BUFFER_POINTER_QUEUE_EIR_CRDT_THRESHOLD_2_3r(unit, reg32_val));

    reg32_val = 0;
    soc_reg_field_set(unit, IPT_DRAM_BUFFER_POINTER_QUEUE_EIR_CRDT_THRESHOLD_4_5r, &reg32_val, DPQ_EIR_CRDT_TH_4f, dpq_eir_th0_5);
    soc_reg_field_set(unit, IPT_DRAM_BUFFER_POINTER_QUEUE_EIR_CRDT_THRESHOLD_4_5r, &reg32_val, DPQ_EIR_CRDT_TH_5f, dpq_eir_th0_5);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 165, exit, WRITE_IPT_DRAM_BUFFER_POINTER_QUEUE_EIR_CRDT_THRESHOLD_4_5r(unit, reg32_val));

    reg32_val = 0;
    soc_reg_field_set(unit, IPT_DRAM_BUFFER_POINTER_QUEUE_EIR_CRDT_THRESHOLD_6_7r, &reg32_val, DPQ_EIR_CRDT_TH_6f, dpq_eir_th6_15);
    soc_reg_field_set(unit, IPT_DRAM_BUFFER_POINTER_QUEUE_EIR_CRDT_THRESHOLD_6_7r, &reg32_val, DPQ_EIR_CRDT_TH_7f, dpq_eir_th6_15);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 167, exit, WRITE_IPT_DRAM_BUFFER_POINTER_QUEUE_EIR_CRDT_THRESHOLD_6_7r(unit, reg32_val));

    reg32_val = 0;
    soc_reg_field_set(unit, IPT_DRAM_BUFFER_POINTER_QUEUE_EIR_CRDT_THRESHOLD_8_9r, &reg32_val, DPQ_EIR_CRDT_TH_8f, dpq_eir_th6_15);
    soc_reg_field_set(unit, IPT_DRAM_BUFFER_POINTER_QUEUE_EIR_CRDT_THRESHOLD_8_9r, &reg32_val, DPQ_EIR_CRDT_TH_9f, dpq_eir_th6_15);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 169, exit, WRITE_IPT_DRAM_BUFFER_POINTER_QUEUE_EIR_CRDT_THRESHOLD_8_9r(unit, reg32_val));

    reg32_val = 0;
    soc_reg_field_set(unit, IPT_DRAM_BUFFER_POINTER_QUEUE_EIR_CRDT_THRESHOLD_10_11r, &reg32_val, DPQ_EIR_CRDT_TH_10f, dpq_eir_th6_15);
    soc_reg_field_set(unit, IPT_DRAM_BUFFER_POINTER_QUEUE_EIR_CRDT_THRESHOLD_10_11r, &reg32_val, DPQ_EIR_CRDT_TH_11f, dpq_eir_th6_15);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 171, exit, WRITE_IPT_DRAM_BUFFER_POINTER_QUEUE_EIR_CRDT_THRESHOLD_10_11r(unit, reg32_val));

    reg32_val = 0;
    soc_reg_field_set(unit, IPT_DRAM_BUFFER_POINTER_QUEUE_EIR_CRDT_THRESHOLD_12_13r, &reg32_val, DPQ_EIR_CRDT_TH_12f, dpq_eir_th6_15);
    soc_reg_field_set(unit, IPT_DRAM_BUFFER_POINTER_QUEUE_EIR_CRDT_THRESHOLD_12_13r, &reg32_val, DPQ_EIR_CRDT_TH_13f, dpq_eir_th6_15);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 173, exit, WRITE_IPT_DRAM_BUFFER_POINTER_QUEUE_EIR_CRDT_THRESHOLD_12_13r(unit, reg32_val));

    reg32_val = 0;
    soc_reg_field_set(unit, IPT_DRAM_BUFFER_POINTER_QUEUE_EIR_CRDT_THRESHOLD_14_15r, &reg32_val, DPQ_EIR_CRDT_TH_14f, dpq_eir_th6_15);
    soc_reg_field_set(unit, IPT_DRAM_BUFFER_POINTER_QUEUE_EIR_CRDT_THRESHOLD_14_15r, &reg32_val, DPQ_EIR_CRDT_TH_15f, dpq_eir_th6_15);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 175, exit, WRITE_IPT_DRAM_BUFFER_POINTER_QUEUE_EIR_CRDT_THRESHOLD_14_15r(unit, reg32_val));

    
    dpq_mc_th =  2*dpq_size0_5/3;

    reg32_val = 0;
    soc_reg_field_set(unit, IPT_DRAM_BUFFER_POINTER_QUEUE_MULTICAST_THRESHOLDr, &reg32_val, DPQ_MC_TH_4f, dpq_mc_th);
    soc_reg_field_set(unit, IPT_DRAM_BUFFER_POINTER_QUEUE_MULTICAST_THRESHOLDr, &reg32_val, DPQ_MC_TH_5f, dpq_mc_th);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 200, exit, WRITE_IPT_DRAM_BUFFER_POINTER_QUEUE_MULTICAST_THRESHOLDr(unit, reg32_val));


    
    SOC_SAND_SOC_IF_ERROR_RETURN(res,  210, exit, READ_IPT_IPT_ENABLESr(unit, &reg32_val));
    soc_reg_field_set(unit, IPT_IPT_ENABLESr, &reg32_val, ZERO_ACT_LINKS_RATE_ENf, 1);
    SOC_SAND_SOC_IF_ERROR_RETURN(res,  210, exit, WRITE_IPT_IPT_ENABLESr(unit, reg32_val));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_mgmt_ipt_init()", 0, 0);
}

STATIC uint32
  arad_mgmt_fsrd_init(
      SOC_SAND_IN int             unit
      )
{
    uint32 reg_val,hv_disable, res;
    int blk_ins, quad, global_quad;
    int quad_index, quad_disabled[SOC_ARAD_NOF_QUADS_IN_FSRD*SOC_DPP_DEFS_MAX(NOF_INSTANCES_FSRD)];

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_MGMT_HW_ADJUST_FABRIC);



    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,READ_FSRD_SRD_QUAD_CTRLr(unit, 0,  0, &reg_val));

    
    for (quad=0; quad <(SOC_ARAD_NOF_QUADS_IN_FSRD*SOC_DPP_DEFS_GET(unit, nof_instances_fsrd)); quad++) {
        
        quad_disabled[quad] = TRUE;
        
        for (quad_index = 0; quad_index < 4; quad_index++) {
            
            if (!(SOC_PBMP_MEMBER(SOC_CONTROL(unit)->info.sfi.disabled_bitmap,
                            (FABRIC_LOGICAL_PORT_BASE(unit)+quad*4)+quad_index))) {
                quad_disabled[quad] = FALSE;
                break;
            }
        }
    }

    for(blk_ins=0 ; blk_ins<SOC_DPP_DEFS_GET(unit, nof_instances_fsrd) ; blk_ins++) {
        for(quad=0 ; quad<SOC_ARAD_NOF_QUADS_IN_FSRD ; quad++) {
            if (quad_disabled[blk_ins*SOC_DPP_DEFS_GET(unit, nof_instances_fsrd)+quad]) {
                continue;
            }
            if (!SOC_IS_ARDON(unit)) {
                soc_reg_field_set(unit, FSRD_SRD_QUAD_CTRLr, &reg_val, SRD_QUAD_N_POWER_DOWNf, 0);  
            }
            soc_reg_field_set(unit, FSRD_SRD_QUAD_CTRLr, &reg_val, SRD_QUAD_N_IDDQf, 0);
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  12,  exit, ARAD_REG_ACCESS_ERR,WRITE_FSRD_SRD_QUAD_CTRLr(unit,  blk_ins,  quad,  reg_val));
        }
    }
    sal_usleep(20);

    for(blk_ins=0 ; blk_ins<SOC_DPP_DEFS_GET(unit, nof_instances_fsrd) ; blk_ins++) {
        for(quad=0 ; quad<SOC_ARAD_NOF_QUADS_IN_FSRD ; quad++) {
            if (quad_disabled[blk_ins*SOC_DPP_DEFS_GET(unit, nof_instances_fsrd)+quad]) {
                continue;
            }
            soc_reg_field_set(unit, FSRD_SRD_QUAD_CTRLr, &reg_val, SRD_QUAD_N_RSTB_HWf, 1);
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  14,  exit, ARAD_REG_ACCESS_ERR,WRITE_FSRD_SRD_QUAD_CTRLr(unit,  blk_ins,  quad,  reg_val));
        }
    }
    sal_usleep(20);

    for(blk_ins=0 ; blk_ins<SOC_DPP_DEFS_GET(unit, nof_instances_fsrd) ; blk_ins++) {
        for(quad=0 ; quad<SOC_ARAD_NOF_QUADS_IN_FSRD ; quad++) {
            if (quad_disabled[blk_ins*SOC_DPP_DEFS_GET(unit, nof_instances_fsrd)+quad]) {
                continue;
            }
            soc_reg_field_set(unit, FSRD_SRD_QUAD_CTRLr, &reg_val, SRD_QUAD_N_MDIO_REGSf, 1);
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  16,  exit, ARAD_REG_ACCESS_ERR,WRITE_FSRD_SRD_QUAD_CTRLr(unit,  blk_ins,  quad,  reg_val));
        }
    }
    sal_usleep(20);

    for(blk_ins=0 ; blk_ins<SOC_DPP_DEFS_GET(unit, nof_instances_fsrd) ; blk_ins++) {
        for(quad=0 ; quad<SOC_ARAD_NOF_QUADS_IN_FSRD ; quad++) {
            if (quad_disabled[blk_ins*SOC_DPP_DEFS_GET(unit, nof_instances_fsrd)+quad]) {
                continue;
            }
            soc_reg_field_set(unit, FSRD_SRD_QUAD_CTRLr, &reg_val, SRD_QUAD_N_RSTB_PLLf, 1);
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  18,  exit, ARAD_REG_ACCESS_ERR,WRITE_FSRD_SRD_QUAD_CTRLr(unit,  blk_ins,  quad,  reg_val));
        }
    }
    sal_usleep(20);

    for(blk_ins=0 ; blk_ins<SOC_DPP_DEFS_GET(unit, nof_instances_fsrd) ; blk_ins++) {
        for(quad=0 ; quad<SOC_ARAD_NOF_QUADS_IN_FSRD ; quad++) {
            if (quad_disabled[blk_ins*SOC_DPP_DEFS_GET(unit, nof_instances_fsrd)+quad]) {
                continue;
            }
            soc_reg_field_set(unit, FSRD_SRD_QUAD_CTRLr, &reg_val, SRD_QUAD_N_RSTB_FIFOf, 1);
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,WRITE_FSRD_SRD_QUAD_CTRLr(unit,  blk_ins,  quad,  reg_val));
        }
    }
    sal_usleep(20);

    for(blk_ins=0 ; blk_ins<SOC_DPP_DEFS_GET(unit, nof_instances_fsrd) ; blk_ins++) {
        for(quad=0 ; quad<SOC_ARAD_NOF_QUADS_IN_FSRD ; quad++) {
            if (quad_disabled[blk_ins*SOC_DPP_DEFS_GET(unit, nof_instances_fsrd)+quad]) {
                continue;
            }
            global_quad = blk_ins*SOC_ARAD_NOF_QUADS_IN_FSRD + quad;
            hv_disable = soc_property_suffix_num_get(unit, global_quad, spn_SRD_TX_DRV_HV_DISABLE, "quad", 0);
            soc_reg_field_set(unit, FSRD_SRD_QUAD_CTRLr, &reg_val, SRD_QUAD_N_TX_DRV_HV_DISABLEf, hv_disable);
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  22,  exit, ARAD_REG_ACCESS_ERR,WRITE_FSRD_SRD_QUAD_CTRLr(unit,  blk_ins,  quad,  reg_val));
        }
    }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_mgmt_fsrd_init()", 0, 0);
}

STATIC uint32
arad_mgmt_init_header_map(
   SOC_SAND_IN int unit
   )
{
  uint32
    line,
    res,
  
    addr_idx,
  
      
    hmif_eei_ext_exists,
    hmif_eep_ext_exists,
    hmif_ftmh_ext_msbs,
    hmif_fwd_code,
  
  
    hmf_out_lif_src,
    hmf_vsi_or_vrf_src,
    hmf_in_lif_src,
    hmf_eei_valid,
    hmf_eei_src;
   
  

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_MGMT_IPT_INIT);

  
  for (addr_idx = 0; addr_idx < ARAD_INIT_HM_NOF_ADDRS; addr_idx++) {
     
    hmif_eei_ext_exists = 0;
    hmif_eep_ext_exists = 0;
    hmif_ftmh_ext_msbs = 0;
    hmif_fwd_code = 0;

    
     
    SHR_BITCOPY_RANGE(&hmif_eei_ext_exists,
                      0, &addr_idx, ARAD_INIT_HMI_EEI_EXT_EXISTS_LSB, ARAD_INIT_HMI_EEI_EXT_EXISTS_NOF_BITS);
    SHR_BITCOPY_RANGE(&hmif_eep_ext_exists,
                      0, &addr_idx, ARAD_INIT_HMI_EEP_EXT_EXISTS_LSB, ARAD_INIT_HMI_EEP_EXT_EXISTS_NOF_BITS);
    SHR_BITCOPY_RANGE(&hmif_ftmh_ext_msbs,
                      0, &addr_idx, ARAD_INIT_HMI_FTMH_EXT_MSBS_LSB, ARAD_INIT_HMI_FTMH_EXT_MSBS_NOF_BITS);
    SHR_BITCOPY_RANGE(&hmif_fwd_code,
                      0, &addr_idx, ARAD_INIT_HMI_FWD_CODE_LSB, ARAD_INIT_HMI_FWD_CODE_NOF_BITS);

      hmf_out_lif_src      = 0;
      hmf_vsi_or_vrf_src   = 0;
      hmf_in_lif_src       = 0;
      hmf_eei_valid        = 0;
      hmf_eei_src          = 0;

    if (hmif_fwd_code == SOC_TMC_PKT_FRWRD_TYPE_BRIDGE) {
      if (hmif_eep_ext_exists) {
        hmf_out_lif_src       = ARAD_INIT_HM_TAKEN_FROM_EEP_EXT;
      } else {
        hmf_out_lif_src       = (hmif_ftmh_ext_msbs == 0x01) ?
           ARAD_INIT_HM_TAKEN_FROM_2b00_FTMH_EXT_13_0 : 0;
      }
      hmf_vsi_or_vrf_src      = ARAD_INIT_HM_TAKEN_FROM_PPH_SYSTEM_VSI;
      hmf_in_lif_src          = ARAD_INIT_HM_TAKEN_FROM_LEARN_EXT;
      hmf_eei_valid           = 0;
      hmf_eei_src             = 0;
    } else if (hmif_fwd_code == SOC_TMC_PKT_FRWRD_TYPE_IPV4_UC) {
      hmf_out_lif_src         = ARAD_INIT_HM_TAKEN_FROM_PPH_SYSTEM_VSI;
      hmf_vsi_or_vrf_src      = 0;
      hmf_in_lif_src          = 0;
      hmf_eei_valid           = hmif_eep_ext_exists;
      hmf_eei_src             = ARAD_INIT_HM_TAKEN_FROM_EEP_EXT;
    } else if (hmif_fwd_code == SOC_TMC_PKT_FRWRD_TYPE_IPV4_MC) {
      hmf_out_lif_src         = ARAD_INIT_HM_TAKEN_FROM_EEP_EXT;
      hmf_vsi_or_vrf_src      = ARAD_INIT_HM_TAKEN_FROM_EEP_EXT;
      hmf_in_lif_src          = ARAD_INIT_HM_TAKEN_FROM_PPH_SYSTEM_VSI;
      hmf_eei_valid           = 0;
      hmf_eei_src             = 0;
    } else if (hmif_fwd_code == SOC_TMC_PKT_FRWRD_TYPE_MPLS) {
      hmf_out_lif_src         = ARAD_INIT_HM_TAKEN_FROM_EEP_EXT;
      hmf_vsi_or_vrf_src      = ARAD_INIT_HM_TAKEN_FROM_PPH_SYSTEM_VSI;
      hmf_in_lif_src          = 0;
      hmf_eei_valid           = hmif_eei_ext_exists;
      hmf_eei_src             = hmif_eei_ext_exists ? ARAD_INIT_HM_TAKEN_FROM_EEI_EXT : 0;
    } else if (hmif_fwd_code == SOC_TMC_PKT_FRWRD_TYPE_BRIDGE_AFTER_TERM) {
      if (hmif_eep_ext_exists) {
        hmf_out_lif_src   = ARAD_INIT_HM_TAKEN_FROM_EEP_EXT;
      } else {
        switch (hmif_ftmh_ext_msbs) {
        case 0x01:
          hmf_out_lif_src  = ARAD_INIT_HM_TAKEN_FROM_2b01_FTMH_EXT_13_0;
          break;
        case 0x02:
          hmf_out_lif_src  = ARAD_INIT_HM_TAKEN_FROM_2b10_FTMH_EXT_13_0;
          break;
        default:
          hmf_out_lif_src  = ARAD_INIT_HM_TAKEN_FROM_2b00_FTMH_EXT_13_0;
          break;
        }
      }

      hmf_vsi_or_vrf_src       = ARAD_INIT_HM_TAKEN_FROM_PPH_SYSTEM_VSI;
      hmf_in_lif_src           = ARAD_INIT_HM_TAKEN_FROM_LEARN_EXT;
      hmf_eei_valid            = hmif_eei_ext_exists;
      hmf_eei_src              = hmif_eei_ext_exists ? ARAD_INIT_HM_TAKEN_FROM_EEI_EXT : 0;
    } else if (hmif_fwd_code == SOC_TMC_PKT_FRWRD_TYPE_TM) {
      switch (hmif_ftmh_ext_msbs) {
        case 0x01:
          hmf_out_lif_src  = ARAD_INIT_HM_TAKEN_FROM_2b01_FTMH_EXT_13_0;
          break;
        case 0x02:
          hmf_out_lif_src  = ARAD_INIT_HM_TAKEN_FROM_2b10_FTMH_EXT_13_0;
          break;
        default:
          hmf_out_lif_src  = ARAD_INIT_HM_TAKEN_FROM_2b00_FTMH_EXT_13_0;
          break;
        }
      hmf_vsi_or_vrf_src   = 0;
      hmf_in_lif_src       = 0;
      hmf_eei_valid        = 0;
      hmf_eei_src          = 0;
    } else {
      hmf_out_lif_src      = 0;
      hmf_vsi_or_vrf_src   = 0;
      hmf_in_lif_src       = 0;
      hmf_eei_valid        = 0;
      hmf_eei_src          = 0;
    }

    line = 0;
    res = READ_EPNI_HEADER_MAPm(unit, MEM_BLOCK_ANY, addr_idx, &line);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 400000, exit);

    soc_mem_field32_set(unit, EPNI_HEADER_MAPm, &line, OUT_LIF_SRCf,    hmf_out_lif_src);
    soc_mem_field32_set(unit, EPNI_HEADER_MAPm, &line, VSI_OR_VRF_SRCf, hmf_vsi_or_vrf_src);
    soc_mem_field32_set(unit, EPNI_HEADER_MAPm, &line, IN_LIF_SRCf,     hmf_in_lif_src);
    soc_mem_field32_set(unit, EPNI_HEADER_MAPm, &line, EEI_VALIDf,      hmf_eei_valid);
    soc_mem_field32_set(unit, EPNI_HEADER_MAPm, &line, EEI_SRCf,        hmf_eei_src);
    res = WRITE_EPNI_HEADER_MAPm(unit, MEM_BLOCK_ANY, addr_idx, &line);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 500000, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_mgmt_init_header_map()", 0, 0);
}

STATIC uint32
  arad_mgmt_init_fabric(
    SOC_SAND_IN int             unit,
    SOC_SAND_IN ARAD_INIT_FABRIC*  init_fabric
  )
{
  uint32
    fld_val = 0,
    res = 0;

  ARAD_FABRIC_CELL_FORMAT cell_format;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_MGMT_HW_ADJUST_FABRIC);

  SOC_SAND_CHECK_NULL_INPUT(init_fabric);

  
  if (!SOC_IS_ARDON(unit)) {
      res = arad_mgmt_fsrd_init(unit);
      SOC_SAND_CHECK_FUNC_RESULT(res, 13, exit);
  }

  
  fld_val = ARAD_MGMT_INIT_EGQ_MAX_FRG_VAR;
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  15,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_MAXIMUM_AND_MINIMUM_PACKET_SIZEr, SOC_CORE_ALL, 0, MAX_FRG_NUMf,  fld_val));

  res = arad_fabric_connect_mode_set_unsafe(unit, init_fabric->connect_mode);
  SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);

  
  arad_ARAD_FABRIC_CELL_FORMAT_clear(&cell_format);
  cell_format.segmentation_enable = init_fabric->segmentation_enable;
  res = arad_fabric_cell_format_set_unsafe(
          unit,
          &cell_format
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 19, exit);
  

  fld_val = SOC_SAND_BOOL2NUM(init_fabric->is_fe600);
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  31,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, ECI_GLOBALEr, REG_PORT_ANY, 0, GLBL_VSC_128_MODEf,  fld_val));
  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  36,  exit, ARAD_REG_ACCESS_ERR,WRITE_BRDC_FMAC_CNTRL_INTRLVD_MODE_REGr(unit, init_fabric->is_fe600 ? 0 : 0xf));

  if(init_fabric->is_128_in_system) {
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  33,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, ECI_GLOBALEr, REG_PORT_ANY, 0, SYS_CONFIG_1f,  0x1));
  } else if (init_fabric->is_dual_mode_in_system) {
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  34,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, ECI_GLOBALEr, REG_PORT_ANY, 0, SYS_CONFIG_1f,  0x2));
  } else {
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  35,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, ECI_GLOBALEr, REG_PORT_ANY, 0, SYS_CONFIG_1f,  0x0));
  }

  if(init_fabric->is_128_in_system) {
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  50,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, FDR_REG_016Ar, REG_PORT_ANY, 0, FIELD_0_0f,  0x0));
  } else {
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  51,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, FDR_REG_016Ar, REG_PORT_ANY, 0, FIELD_0_0f,  0x1));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  52,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, FDR_REG_016Ar, REG_PORT_ANY, 0, FIELD_4_22f,  0x1800));
  }

  
  if (SOC_DPP_CONFIG(unit)->tm.is_petrab_in_system) {
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  37,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, ECI_GLOBALFr, REG_PORT_ANY, 0, SYSTEM_HEADERS_MODEf,  ARAD_PP_SYSTEM_HEADERS_MODE_PETRAB));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  38,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, FDT_MULTICAST_ID_MASKr, REG_PORT_ANY, 0, MULTICAST_ID_MASKf,  0x3fff));

      
      res = arad_mgmt_init_header_map(unit);
      SOC_SAND_CHECK_FUNC_RESULT(res, 54264, exit);
  }

  
  fld_val = init_fabric->dual_pipe_tdm_packet ? ARAD_INIT_LLFC_DUAL_PIPE_TH : ARAD_INIT_LLFC_SINGLE_PIPE_TH;
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  37,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, FDR_LINK_LEVEL_FLOW_CONTROLr, REG_PORT_ANY, 0, LNK_LVL_FC_THf,  fld_val));

  
  fld_val = init_fabric->dual_pipe_tdm_packet;
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  32,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, ECI_GLOBALEr, REG_PORT_ANY, 0, PARALLEL_DATA_PATHf,  fld_val));

  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  241,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_REG_0101r, REG_PORT_ANY, 0, FIELD_0_0f,  0));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  242,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, FDR_FDR_ENABLERS_REGISTER_2r, REG_PORT_ANY, 0, FIELD_1_1f,  1));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  244,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, FDT_FDT_ENABLER_REGISTERr, REG_PORT_ANY, 0, FIELD_12_12f,  1));
  }



exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_mgmt_init_fabric()", res, 0);
}


STATIC uint32
  arad_mgmt_init_nif_prep(
    SOC_SAND_IN int     unit,
    SOC_SAND_IN  uint8    silent
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_MGMT_HW_ADJUST_NIF);

  res = arad_nif_init(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_mgmt_init_nif_prep()", 0, 0);
}



STATIC
  uint32
    arad_mgmt_hw_interfaces_set_unsafe(
      SOC_SAND_IN  int                unit,
      SOC_SAND_IN  ARAD_MGMT_INIT*          init,
      SOC_SAND_IN  uint8                silent
    )
{
  uint32
    res,
    stage_id = 0,
    stage_internal_id = 0;
  int rv;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_MGMT_HW_INTERFACES_SET_UNSAFE);
  SOC_SAND_PCID_LITE_SKIP(unit);
  (void)stage_internal_id;
  (void)stage_id;
  SOC_SAND_CHECK_NULL_INPUT(init);

  if (SOC_IS_DPP_DRC_COMBO28(unit) == FALSE) {

  if (init->dram.enable) {
        ARAD_INIT_PRINT_INTERNAL_LEVEL_1_ADVANCE("DDR");

        res = arad_dram_init_ddr(unit, &(init->dram));
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);


    } else {
        res = arad_dram_init_power_down_unused_dprcs(unit, init->dram.is_valid);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    }
  } else {
      if (init->drc_info.enable) {
    rv =  soc_dpp_drc_combo28_dram_init(unit, (soc_dpp_drc_combo28_info_t*)(&(init->drc_info)));
        if (rv != SOC_E_NONE) {
          SOC_SAND_SET_ERROR_CODE(ARAD_MGMT_EGQ_INIT_FAILS_ERR, 5, exit);
        }
      }
  }

  res = arad_mgmt_init_nif_prep(
          unit,
          silent
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);

  if(init->synce.enable)
  {
    ARAD_INIT_PRINT_INTERNAL_LEVEL_1_ADVANCE("SyncE");
    res = arad_nif_synce_init(
            unit,
            &(init->synce)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  }

  if (init->fabric.enable) {
    ARAD_INIT_PRINT_INTERNAL_LEVEL_1_ADVANCE("Fabric");
    res = arad_mgmt_init_fabric(
            unit,
            &(init->fabric)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  }

#if defined(INCLUDE_KBP)
  if(init->elk.enable) {
    ARAD_INIT_PRINT_INTERNAL_LEVEL_1_ADVANCE("ELK");
    res = arad_kbp_init_arad_interface(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
  }
#endif 

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_mgmt_hw_interfaces_set_unsafe()", 0, 0);
}


STATIC
  uint32
    arad_bubble_configuration_set_unsafe(
      SOC_SAND_IN  int                unit
    )
{
  uint32
    res, conf_reg_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);



  if(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "bubble_egr", 1) == 1) {

   

    res = READ_EGQ_EHP_BUBBLE_CONFIGURATIONr(unit, REG_PORT_ANY, &conf_reg_val);
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    
    soc_reg_field_set(unit, EGQ_EHP_BUBBLE_CONFIGURATIONr, &conf_reg_val, EHP_CONST_BUBBLE_ENf, 1);

    
    soc_reg_field_set(unit, EGQ_EHP_BUBBLE_CONFIGURATIONr, &conf_reg_val, EHP_RQP_BUBBLE_REQ_ENf, 1);

    
    soc_reg_field_set(unit, EGQ_EHP_BUBBLE_CONFIGURATIONr, &conf_reg_val, EHP_BUBBLE_DELAYf, 31);

    res = WRITE_EGQ_EHP_BUBBLE_CONFIGURATIONr(unit, REG_PORT_ANY, conf_reg_val);
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);


  

    res = READ_EGQ_FQP_BUBBLE_CONFIGURATIONr(unit, REG_PORT_ANY, &conf_reg_val);
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    
    soc_reg_field_set(unit, EGQ_FQP_BUBBLE_CONFIGURATIONr, &conf_reg_val, FQP_CONST_BUBBLE_ENf, 1);

    
    soc_reg_field_set(unit, EGQ_FQP_BUBBLE_CONFIGURATIONr, &conf_reg_val, FQP_BUBBLE_DELAYf, 31);

    res = WRITE_EGQ_FQP_BUBBLE_CONFIGURATIONr(unit, REG_PORT_ANY, conf_reg_val);
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  }

    if(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "bubble_ing", 1) == 1) {

  

    
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IHP_IHP_ENABLERSr, SOC_CORE_ALL, 0, FORCE_BUBBLESf,  1));

    
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IHP_SYNC_COUNTERr, SOC_CORE_ALL, 0, SYNC_COUNTERf,  0x3ff));
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "arad_bubble_configuration_set_unsafe()", 0, 0);
}


STATIC
  uint32
    arad_mgmt_hw_interfaces_verify(
      SOC_SAND_IN  int                 unit,
      SOC_SAND_IN  ARAD_MGMT_INIT           *init
    )
{
  uint32
    res;
  uint32
    idx,
    dram_count = 0;
  ARAD_HW_PLL_PARAMS
    dram_pll_dflt_old;
  ARAD_INIT_DRAM_PLL
    dram_pll_dflt_new;
  

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_MGMT_HW_INTERFACES_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(init);
  SOC_SAND_MAGIC_NUM_VERIFY(init);

  if ((init->dram.enable)  && (!SOC_IS_DPP_DRC_COMBO28(unit)))
  {
    arad_ARAD_HW_PLL_PARAMS_clear(&dram_pll_dflt_old);
    ARAD_INIT_DRAM_PLL_clear(&dram_pll_dflt_new);

    SOC_SAND_ERR_IF_OUT_OF_RANGE(
      init->dram.conf_mode, ARAD_HW_DRAM_CONF_MODE_MIN, ARAD_HW_DRAM_CONF_MODE_MAX,
      SOC_SAND_ERR, 5, exit
    );

    if (init->dram.conf_mode == ARAD_HW_DRAM_CONF_MODE_BUFFER)
    {
      SOC_SAND_ERR_IF_ABOVE_MAX(
        init->dram.dram_conf.buffer_mode.buff_len, ARAD_HW_DRAM_CONF_SIZE_MAX-1,
        ARAD_HW_DRAM_CONF_LEN_OUT_OF_RANGE_ERR, 10, exit
      );
    }
    if (init->dram.conf_mode == ARAD_HW_DRAM_CONF_MODE_PARAMS)
    {
      res = arad_dram_info_verify(
              unit,
              init->dram.dram_conf.params_mode.dram_freq,
              init->dram.dram_type,
              &init->dram.dram_conf.params_mode.params
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);
    }
    for (idx = 0; idx < SOC_DPP_DEFS_GET(unit, hw_dram_interfaces_max); idx ++ )
    {
      if (init->dram.is_valid[idx])
      {
        dram_count++;
      }
    }
    if (init->dram.nof_banks !=  ARAD_DRAM_NUM_BANKS_8) {
      SOC_SAND_SET_ERROR_CODE(ARAD_HW_INVALID_NOF_BANKS_ERR, 20, exit);
    }

    SOC_SAND_ERR_IF_OUT_OF_RANGE(
      init->dram.nof_columns, ARAD_DRAM_NUM_COLUMNS_256, ARAD_DRAM_NUM_COLUMNS_8192,
      ARAD_HW_NOF_COLUMNS_OUT_OF_RANGE_ERR, 23, exit
    );
  }

  if(init->fabric.enable)
  {
    res = arad_fabric_connect_mode_verify(
            unit,
            init->fabric.connect_mode
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 88, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_mgmt_hw_interfaces_verify()", 0, 0);
}




STATIC uint32
  arad_mgmt_blocks_init_unsafe(
    SOC_SAND_IN  int                 unit
  )
{
    uint32
        reg32_val,
        res,
        fld_val;
    soc_reg_above_64_val_t
        data_above_64;
    int
        rv = 0;
    soc_reg_t
        idr_mishandling_reg;
    soc_field_t
        idr_mishandling_field;


    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_MGMT_BLOCKS_INIT_UNSAFE);

    rv = soc_dpp_device_reset(unit, SOC_DPP_RESET_MODE_BLOCKS_RESET, SOC_DPP_RESET_ACTION_OUT_RESET);
    if (rv != SOC_E_NONE) {
      SOC_SAND_SET_ERROR_CODE(ARAD_MGMT_EGQ_INIT_FAILS_ERR, 35, exit);
    }

    

        SOC_SAND_SOC_IF_ERROR_RETURN(res, 15, exit, READ_ECI_BLOCKS_SOFT_RESETr(unit, data_above_64));
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_RESETr, data_above_64, CLP_0_RESETf, 1);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 16, exit, WRITE_ECI_BLOCKS_SOFT_RESETr(unit, data_above_64));

        SOC_SAND_SOC_IF_ERROR_RETURN(res, 17, exit, READ_ECI_BLOCKS_SOFT_RESETr(unit, data_above_64));
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_RESETr, data_above_64, CLP_1_RESETf, 1);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 18, exit, WRITE_ECI_BLOCKS_SOFT_RESETr(unit, data_above_64));
    if (!SOC_IS_ARDON(unit)) {
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  24,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, NBI_NIF_PORTS_CFGr, REG_PORT_ANY,  0, CLP_N_OTP_PORT_BOND_OPTIONf,  0x3));
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  25,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, NBI_NIF_PORTS_CFGr, REG_PORT_ANY,  1, CLP_N_OTP_PORT_BOND_OPTIONf,  0x3));
    }
        SOC_REG_ABOVE_64_CLEAR(data_above_64);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_ECI_BLOCKS_SOFT_RESETr(unit, data_above_64));

    if (!SOC_IS_ARDON(unit)) {
        
        
        reg32_val = 0;
        soc_reg_field_set(unit, PORT_MLD_CTRL_REGr, &reg32_val, RST_B_HWf, 1);
        soc_reg_field_set(unit, PORT_MLD_CTRL_REGr, &reg32_val, RST_B_MDIOREGSf, 1);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 22, exit, WRITE_PORT_MLD_CTRL_REGr(unit, SOC_BLOCK_PORT(unit, CLP_BLOCK(unit, 0)) , reg32_val));

        
        reg32_val = 0;
        soc_reg_field_set(unit, PORT_MLD_CTRL_REGr, &reg32_val, RST_B_HWf, 1);
        soc_reg_field_set(unit, PORT_MLD_CTRL_REGr, &reg32_val, RST_B_MDIOREGSf, 1);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 24, exit, WRITE_PORT_MLD_CTRL_REGr(unit, SOC_BLOCK_PORT(unit, CLP_BLOCK(unit, 1))  , reg32_val));

        
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  26,  exit, ARAD_REG_ACCESS_ERR,WRITE_REG_2029700r(unit,  SOC_BLOCK_PORT(unit, CLP_BLOCK(unit, 0)),  0x6));
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  27,  exit, ARAD_REG_ACCESS_ERR,WRITE_REG_2029700r(unit,  SOC_BLOCK_PORT(unit, CLP_BLOCK(unit, 1)),  0x6));
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  28,  exit, ARAD_REG_ACCESS_ERR,WRITE_REG_2029700r(unit,  SOC_BLOCK_PORT(unit, XLP_BLOCK(unit, 0)),  0x6));
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  29,  exit, ARAD_REG_ACCESS_ERR,WRITE_REG_2029700r(unit,  SOC_BLOCK_PORT(unit, XLP_BLOCK(unit, 1)),  0x6));

       
       
       fld_val = 0x1;
       SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  34,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, PORT_ECC_CONTROLr,  SOC_BLOCK_PORT(unit, CLP_BLOCK(unit, 0)),  0, CDC_RXFIFO_MEM_ENf,  fld_val));
       SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  35,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, PORT_ECC_CONTROLr,  SOC_BLOCK_PORT(unit, CLP_BLOCK(unit, 1)),  0, CDC_RXFIFO_MEM_ENf,  fld_val));
       SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  36,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, PORT_ECC_CONTROLr,  SOC_BLOCK_PORT(unit, XLP_BLOCK(unit, 0)),  0, CDC_RXFIFO_MEM_ENf,  fld_val));
       SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  37,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, PORT_ECC_CONTROLr,  SOC_BLOCK_PORT(unit, XLP_BLOCK(unit, 1)),  0, CDC_RXFIFO_MEM_ENf,  fld_val));
       
       SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  38,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, PORT_ECC_CONTROLr,  SOC_BLOCK_PORT(unit, CLP_BLOCK(unit, 0)),  0, CDC_TXFIFO_MEM_ENf,  fld_val));
       SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  39,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, PORT_ECC_CONTROLr,  SOC_BLOCK_PORT(unit, CLP_BLOCK(unit, 1)),  0, CDC_TXFIFO_MEM_ENf,  fld_val));
       SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, PORT_ECC_CONTROLr,  SOC_BLOCK_PORT(unit, XLP_BLOCK(unit, 0)),  0, CDC_TXFIFO_MEM_ENf,  fld_val));
       SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  41,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, PORT_ECC_CONTROLr,  SOC_BLOCK_PORT(unit, XLP_BLOCK(unit, 1)),  0, CDC_TXFIFO_MEM_ENf,  fld_val));
       
       SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  42,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, PORT_ECC_CONTROLr,  SOC_BLOCK_PORT(unit, CLP_BLOCK(unit, 0)),  0, MIB_RSC_MEM_ENf,  fld_val));
       SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  43,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, PORT_ECC_CONTROLr,  SOC_BLOCK_PORT(unit, CLP_BLOCK(unit, 1)),  0, MIB_RSC_MEM_ENf,  fld_val));
       SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  44,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, PORT_ECC_CONTROLr,  SOC_BLOCK_PORT(unit, XLP_BLOCK(unit, 0)),  0, MIB_RSC_MEM_ENf,  fld_val));
       SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  45,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, PORT_ECC_CONTROLr,  SOC_BLOCK_PORT(unit, XLP_BLOCK(unit, 1)),  0, MIB_RSC_MEM_ENf,  fld_val));
       
       SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  46,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, PORT_ECC_CONTROLr,  SOC_BLOCK_PORT(unit, CLP_BLOCK(unit, 0)),  0, MIB_TSC_MEM_ENf,  fld_val));
       SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  47,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, PORT_ECC_CONTROLr,  SOC_BLOCK_PORT(unit, CLP_BLOCK(unit, 1)),  0, MIB_TSC_MEM_ENf,  fld_val));
       SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  48,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, PORT_ECC_CONTROLr,  SOC_BLOCK_PORT(unit, XLP_BLOCK(unit, 0)),  0, MIB_TSC_MEM_ENf,  fld_val));
       SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  49,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, PORT_ECC_CONTROLr,  SOC_BLOCK_PORT(unit, XLP_BLOCK(unit, 1)),  0, MIB_TSC_MEM_ENf,  fld_val));
       
       SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  50,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, PORT_ECC_CONTROLr,  SOC_BLOCK_PORT(unit, CLP_BLOCK(unit, 0)),  0, TXFIFO_MEM_ENf,  fld_val));
       SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  51,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, PORT_ECC_CONTROLr,  SOC_BLOCK_PORT(unit, CLP_BLOCK(unit, 1)),  0, TXFIFO_MEM_ENf,  fld_val));
       SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  52,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, PORT_ECC_CONTROLr,  SOC_BLOCK_PORT(unit, XLP_BLOCK(unit, 0)),  0, TXFIFO_MEM_ENf,  fld_val));
       SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  53,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, PORT_ECC_CONTROLr,  SOC_BLOCK_PORT(unit, XLP_BLOCK(unit, 1)),  0, TXFIFO_MEM_ENf,  fld_val));
    }

   if (SOC_IS_ARAD_B0_AND_ABOVE(unit)) {
       
       idr_mishandling_reg = SOC_IS_ARADPLUS(unit)? IDR_COMPATIBILITY_REGISTERr: IDR_SPARE_REGISTER_2r; 
       idr_mishandling_field = SOC_IS_ARADPLUS(unit)? FIELD_6_6f: SYS_CONFIG_1f; 
       SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  60,  exit, ARAD_REG_ACCESS_ERR,soc_reg32_get(unit, idr_mishandling_reg, REG_PORT_ANY,  0, &reg32_val));
       fld_val = 0x1;
       soc_reg_field_set(unit, idr_mishandling_reg, &reg32_val, idr_mishandling_field, fld_val);
       SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  70,  exit, ARAD_REG_ACCESS_ERR,soc_reg32_set(unit, idr_mishandling_reg, REG_PORT_ANY,  0,  reg32_val));
   }

  if (SOC_IS_ARDON(unit)) {
      
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 80, exit, ARAD_REG_ACCESS_ERR, soc_reg_field32_modify(unit, IQM_BDB_CONFIGURATIONr, REG_PORT_ANY, BDB_LIST_SIZEf, 7));
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_mgmt_blocks_init_unsafe()", 0, 0);
}



uint32 arad_mgmt_stk_init(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  ARAD_MGMT_INIT               *init)
{
    uint32
        res,
        reg_val,
        i,
        port_i,
        base_q_pair,
        flags;
    uint64
        reg_val64;
    soc_pbmp_t
        pbmp;
    ARAD_EGQ_PPCT_TBL_DATA
        ppct_tbl_data;
    int     core;
    uint32  tm_port;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,READ_IHB_LBP_GENERAL_CONFIG_0r(unit, SOC_CORE_ALL, &reg_val64));
    soc_reg64_field32_set(unit, IHB_LBP_GENERAL_CONFIG_0r, &reg_val64, TM_DOMAINf, init->ports.tm_domain);
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,WRITE_IHB_LBP_GENERAL_CONFIG_0r(unit, SOC_CORE_ALL,  reg_val64));

    if (SOC_IS_QAX(unit)) {
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,READ_TAR_STATIC_CONFIGURATIONr(unit, &reg_val));
        ARAD_FLD_TO_REG(TAR_STATIC_CONFIGURATIONr, USE_STACK_RESOLVEf, 0x1, reg_val, 50, exit);
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  60,  exit, ARAD_REG_ACCESS_ERR,WRITE_TAR_STATIC_CONFIGURATIONr(unit,  reg_val));
    } else {
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,READ_IRR_STATIC_CONFIGURATIONr(unit, &reg_val));
        ARAD_FLD_TO_REG(IRR_STATIC_CONFIGURATIONr, USE_STACK_RESOLVEf, 0x1, reg_val, 50, exit);
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  60,  exit, ARAD_REG_ACCESS_ERR,WRITE_IRR_STATIC_CONFIGURATIONr(unit,  reg_val));
    }

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 70, exit, soc_port_sw_db_valid_ports_get(unit, 0, &pbmp));

    SOC_PBMP_ITER(pbmp,port_i) {

        res = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.peer_tm_domain.get(unit, port_i, &ppct_tbl_data.peer_tm_domain_id);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 72, exit);

        if (ppct_tbl_data.peer_tm_domain_id == 0xffffffff) {
            continue;
        }

        res = soc_port_sw_db_flags_get(unit, port_i, &flags);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 73, exit);

        if (SOC_PORT_IS_ELK_INTERFACE(flags) || SOC_PORT_IS_STAT_INTERFACE(flags)) {
            continue;
        }

        res = soc_port_sw_db_local_to_tm_port_get(unit, port_i, &tm_port, &core);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 140, exit);

        
        res = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.base_q_pair.get(unit, port_i, &base_q_pair);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 74, exit);

        res = arad_egq_ppct_tbl_get_unsafe(unit, core, base_q_pair, &ppct_tbl_data);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 75, exit);

        ppct_tbl_data.is_stacking_port =  0x1;

        res = arad_egq_ppct_tbl_set_unsafe(unit, core, base_q_pair, &ppct_tbl_data);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 80, exit);
    }

    
    for (i=0; i < 64 ; i++) {

        res = arad_ipq_stack_fec_map_stack_lag_set_unsafe(unit, i , ARAD_IPQ_STACK_LAG_STACK_FEC_RESOLVE_ENTRY_ALL, i );
        SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);
    }

    
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  100,  exit, ARAD_REG_ACCESS_ERR,READ_IRE_STATIC_CONFIGURATIONr(unit, &reg_val));
    soc_reg_field_set(unit, IRE_STATIC_CONFIGURATIONr, &reg_val, STAMP_UC_DESTINATION_IN_FTMHf, 0x1);
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  110,  exit, ARAD_REG_ACCESS_ERR,WRITE_IRE_STATIC_CONFIGURATIONr(unit, reg_val));

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_mgmt_stk_init()", 0, 0);
}



STATIC uint32 arad_mgmt_system_red_init(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  ARAD_MGMT_INIT               *init)
{
    uint32
        res,
        val32 = 0;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (SOC_IS_QAX(unit)) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("access to registers should be fixed for QAX at places we used _REG(32|64) access routines")));
    }


    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_IPS_IPS_GENERAL_CONFIGURATIONSr_REG32(unit, REG_PORT_ANY, &val32));
    soc_reg_field_set(unit, IPS_IPS_GENERAL_CONFIGURATIONSr, &val32, ENABLE_SYSTEM_REDf, 0x1);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_IPS_IPS_GENERAL_CONFIGURATIONSr_REG32(unit, REG_PORT_ANY, val32));

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_mgmt_system_red_init()", 0, 0);
}


STATIC uint32 arad_mgmt_swap_init(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  ARAD_SWAP_INFO     *swap_info)
{
    uint32  res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_MGMT_SWAP_INIT);

    res = arad_swap_info_set_verify(unit, swap_info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    res = arad_swap_info_set_unsafe(unit, swap_info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_mgmt_swap_init()", swap_info->offset, 0);
}



uint32 arad_mgmt_pon_init(
    SOC_SAND_IN  int                                unit)
{
    uint32
        res, idx = 0;
    soc_pbmp_t
      pon_port_bm;
    soc_port_t
        port,
        pp_port = 0;
    ARAD_PORTS_SWAP_INFO ports_swap_info;
    ARAD_PORTS_PROGRAMS_INFO port_prog_info;
    int     core;

    SOC_TMC_PORT_SWAP_GLOBAL_INFO global_swap_info;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_PBMP_CLEAR(pon_port_bm);

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, soc_port_sw_db_valid_ports_get(unit, SOC_PORT_FLAGS_PON_INTERFACE, &pon_port_bm));

    arad_ARAD_PORTS_SWAP_INFO_clear(&ports_swap_info);

    sal_memset(&global_swap_info, 0, sizeof(SOC_TMC_PORT_SWAP_GLOBAL_INFO));

    
    if (SOC_IS_QAX(unit)) {
        global_swap_info.global_tag_swap_n_size = 0;    
        global_swap_info.tag_swap_n_offset_0 = 0xc;     
        global_swap_info.tag_swap_n_offset_1 = 0xc;     

        res = soc_qax_port_swap_global_info_set(unit, &global_swap_info);
        SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);
    }
    else if (SOC_IS_JERICHO(unit) && !SOC_IS_QAX(unit))  {
        global_swap_info.global_tag_swap_n_size = 0;    
        global_swap_info.tag_swap_n_offset_0 = 0xc;     
        global_swap_info.tag_swap_n_offset_1 = 0xc;     

        res = soc_jer_port_swap_global_info_set(unit, &global_swap_info);
        SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);
    }

    
    ports_swap_info.enable = TRUE;

    SOC_PBMP_ITER(pon_port_bm, port){
        res = soc_port_sw_db_core_get(unit, port, &core);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 140, exit);

        
        res = arad_ports_swap_set_unsafe(unit,
                                         port,
                                         &ports_swap_info);
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

        
        if (!_BCM_PPD_PON_PP_PORT_MAPPING_BY_PASS(unit)) {
            for (idx = 0; idx < 16; idx++)
            {
              pp_port = ((idx << 3) | port);
              res = arad_parser_pp_port_nof_bytes_to_remove_set(unit, core, pp_port, 4);
              SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
            }
        } else {
            pp_port = port;
            res = arad_parser_pp_port_nof_bytes_to_remove_set(unit, core, pp_port, 4);
            SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);
        }

        
        arad_ARAD_PORTS_PROGRAMS_INFO_clear(&port_prog_info);
        
        res = arad_ports_programs_info_get_unsafe(unit, port, &port_prog_info);
        SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
        port_prog_info.ptc_flp_profile = SOC_TMC_PORTS_FLP_PROFILE_PON;
        if (SOC_DPP_CONFIG(unit)->pp.compression_spoof_ip6_enable ||
            SOC_DPP_CONFIG(unit)->pp.compression_ip6_enable ||
            SOC_DPP_CONFIG(unit)->pp.custom_pon_ipmc) {
            port_prog_info.ptc_tt_profile = SOC_TMC_PORTS_TT_PROFILE_PON;
        }
        res = arad_ports_programs_info_set_unsafe(unit, port, &port_prog_info);
        SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
    }

    
    if(ARAD_PP_FLP_IBC_PORT_ENABLE(unit))
    {
        soc_pbmp_t valid_port_bm;
        char *propval;
        char propkey[SOC_PROPERTY_NAME_MAX];

        SOC_PBMP_CLEAR(valid_port_bm);
        sal_memset(propkey, 0, sizeof(propkey));
        if (sal_snprintf(propkey, SOC_PROPERTY_NAME_MAX, 
                         "%s_%s", spn_CUSTOM_FEATURE, "pon_ibc_port_enabled") >= SOC_PROPERTY_NAME_MAX)
        {
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 50, exit);
        }

        SOC_SAND_SOC_IF_ERROR_RETURN(res, 70, exit, soc_port_sw_db_valid_ports_get(unit, 0, &valid_port_bm));
        SOC_PBMP_ITER(valid_port_bm, port){ 
            propval = soc_property_port_num_get_str(unit, port, propkey);
            if (propval) {      
                if (sal_strcmp(propval, "TRUE") == 0) {
                    
                    arad_ARAD_PORTS_PROGRAMS_INFO_clear(&port_prog_info);
                    res = arad_ports_programs_info_get_unsafe(unit, port, &port_prog_info);
                    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
                    port_prog_info.ptc_flp_profile = SOC_TMC_PORTS_FLP_PROFILE_IBC;
                    res = arad_ports_programs_info_set_unsafe(unit, port, &port_prog_info);
                    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
                }
            }
        }
    }
    

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_mgmt_pon_init()", 0, 0);
}





uint32 arad_mgmt_coe_init(
    SOC_SAND_IN  int                                unit)
{
    uint32
        res;
    soc_pbmp_t
      coe_port_bm;
    soc_port_t
        port;
    ARAD_PORTS_SWAP_INFO ports_swap_info;
    SOC_TMC_PORT_SWAP_GLOBAL_INFO global_swap_info;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_PBMP_CLEAR(coe_port_bm);

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, soc_port_sw_db_valid_ports_get(unit, SOC_PORT_FLAGS_COE_PORT, &coe_port_bm));

    arad_ARAD_PORTS_SWAP_INFO_clear(&ports_swap_info);

    sal_memset(&global_swap_info, 0, sizeof(SOC_TMC_PORT_SWAP_GLOBAL_INFO));

    
    if (SOC_IS_QAX(unit)) {
        global_swap_info.global_tag_swap_n_size = 0;    
        global_swap_info.tag_swap_n_offset_0 = 0xc;     
        global_swap_info.tag_swap_n_offset_1 = 0xc;     

        res = soc_qax_port_swap_global_info_set(unit, &global_swap_info);
        SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);
    }
    else if (SOC_IS_JERICHO(unit)) {
        global_swap_info.global_tag_swap_n_size = 0; 
        global_swap_info.tag_swap_n_offset_0 = 0xc;  
        global_swap_info.tag_swap_n_offset_1 = 0xc;  

        res = soc_jer_port_swap_global_info_set(unit, &global_swap_info);
        SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);
    }

    
    ports_swap_info.enable = TRUE;

    SOC_PBMP_ITER(coe_port_bm, port){
        
        res = arad_ports_swap_set_unsafe(unit,
                                         port,
                                         &ports_swap_info);
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_mgmt_coe_init()", 0, 0);
}


#define BASE_QUEUE 0x17ff0 
#define NOF_QUEUES_TO_BLOCK 13 
#define NOF_PACKETS_PER_QUEUE 32
#define NUM_OF_REP (NOF_QUEUES_TO_BLOCK * NOF_PACKETS_PER_QUEUE)
#define TEMP_MC_ID 9000 
#define TEMP_FWD_ACTION_PROFILE 0x2a

uint32
  arad_iqm_workaround(
    SOC_SAND_IN  int                 unit
  )
{
  uint32 res;
  uint32 i;
  uint32 val32 = 0, intr_val, intr_mask;
  uint64 val64;
  SOC_TMC_MULT_ING_ENTRY mc_group;
  SOC_TMC_ERROR mc_err;
  SOC_TMC_MULT_ING_ENTRY *mc_members = NULL;
  uint32 entry[5] = {0}; 
  soc_reg_above_64_val_t reg_above_64_val = {0};
  uint32 nof_dram_buffs = 0;
  int should_disable_dynamic_mem_changes = 0;
  ARAD_INIT_DBUFFS_BDRY dbuffs_bdries = {{0}};
  uint32 bu_IHP_PINFO_LLR[3];
  uint32 bu_IHB_FWD_ACT_PROFILE[5];
  uint32 bu_IPS_CRWDTH[1];
  ARAD_MGMT_INIT *init = &(SOC_DPP_CONFIG(unit)->arad->init);
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  if (SOC_IS_ARAD_B0_AND_ABOVE(unit)) {
    goto exit;
  }

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_IQM_INTERRUPT_REGISTERr(unit, REG_PORT_ANY, &intr_val));
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, READ_IQM_INTERRUPT_MASK_REGISTERr(unit, REG_PORT_ANY, &intr_mask));
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, WRITE_IQM_INTERRUPT_MASK_REGISTERr(unit, REG_PORT_ANY, 0));

  
  SOC_TMC_MULT_ING_ENTRY_clear(&mc_group);
  res = dpp_mult_ing_group_open(unit, TEMP_MC_ID, &mc_group, 0, &mc_err);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  if (mc_err) {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_MALLOC_FAIL, 50, exit);
  }

  ARAD_ALLOC(mc_members, SOC_TMC_MULT_ING_ENTRY, NUM_OF_REP, "arad_iqm_workaround.mc_members");
  
  for (i = 0; i < NUM_OF_REP; ++i) {
      SOC_TMC_MULT_ING_ENTRY_clear(&(mc_members[i]));
      mc_members[i].destination.type = SOC_TMC_DEST_TYPE_QUEUE;
      mc_members[i].destination.id = (i / 32) + BASE_QUEUE;
      mc_members[i].cud = i + 1;
  }
  res = dpp_mult_ing_group_update(unit, TEMP_MC_ID, mc_members, NUM_OF_REP, &mc_err);
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
  if (mc_err) {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_MALLOC_FAIL, 70, exit);
  }


  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 80, exit, READ_IHP_PINFO_LLRm(unit, MEM_BLOCK_ANY, 1, entry));
  sal_memcpy(bu_IHP_PINFO_LLR, entry, sizeof(bu_IHP_PINFO_LLR));
  soc_mem_field32_set(unit, IHP_PINFO_LLRm, entry, DEFAULT_ACTION_PROFILE_FWDf, 7);
  soc_mem_field32_set(unit, IHP_PINFO_LLRm, entry, DEFAULT_CPU_TRAP_CODEf, TEMP_FWD_ACTION_PROFILE);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 90, exit, WRITE_IHP_PINFO_LLRm(unit, MEM_BLOCK_ANY, 1, entry));

  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 100, exit, READ_IHB_FWD_ACT_PROFILEm(unit, MEM_BLOCK_ANY, TEMP_FWD_ACTION_PROFILE, entry));
  sal_memcpy(bu_IHB_FWD_ACT_PROFILE, entry, sizeof(bu_IHB_FWD_ACT_PROFILE));
  soc_mem_field32_set(unit, IHB_FWD_ACT_PROFILEm, entry, FWD_ACT_DESTINATIONf, 0x50000 + TEMP_MC_ID);
  soc_mem_field32_set(unit, IHB_FWD_ACT_PROFILEm, entry, FWD_ACT_BYPASS_FILTERINGf, 1);
  soc_mem_field32_set(unit, IHB_FWD_ACT_PROFILEm, entry, FWD_ACT_DESTINATION_OVERWRITEf, 1);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 110, exit, WRITE_IHB_FWD_ACT_PROFILEm(unit, MEM_BLOCK_ANY, TEMP_FWD_ACTION_PROFILE, entry));
  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 120, exit, READ_IPS_CRWDTHm(unit, MEM_BLOCK_ANY, 0, entry));
  sal_memcpy(bu_IPS_CRWDTH, entry, sizeof(bu_IPS_CRWDTH));
  soc_mem_field32_set(unit, IPS_CRWDTHm, entry, WD_DELETE_Q_THf, 0);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 130, exit, WRITE_IPS_CRWDTHm(unit, MEM_BLOCK_ANY, 0, entry));


  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 140, exit, READ_IRE_STATIC_CONFIGURATIONr(unit, &val32));
  soc_reg_field_set(unit, IRE_STATIC_CONFIGURATIONr, &val32, ENABLE_DATA_PATHf, 1);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 150, exit, WRITE_IRE_STATIC_CONFIGURATIONr(unit, val32));

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 160, exit, READ_IDR_DYNAMIC_CONFIGURATIONr(unit, &val32));
  soc_reg_field_set(unit, IDR_DYNAMIC_CONFIGURATIONr, &val32, ENABLE_DATA_PATHf, 1);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 170, exit, WRITE_IDR_DYNAMIC_CONFIGURATIONr(unit, val32));

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 180, exit, READ_IRR_DYNAMIC_CONFIGURATIONr(unit, &val32));
  soc_reg_field_set(unit, IRR_DYNAMIC_CONFIGURATIONr, &val32, ENABLE_DATA_PATH_IQMf, 1);
  soc_reg_field_set(unit, IRR_DYNAMIC_CONFIGURATIONr, &val32, ENABLE_DATA_PATH_IDRf, 1);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 190, exit, WRITE_IRR_DYNAMIC_CONFIGURATIONr(unit, val32));

  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 200, exit, READ_IQM_IQM_ENABLERSr(unit, REG_PORT_ANY, &val32));
  soc_reg_field_set(unit, IQM_IQM_ENABLERSr, &val32, DSCRD_ALL_PKTf, 0);
  if (init->dram.pdm_mode != ARAD_INIT_PDM_MODE_SIMPLE ) {
    soc_reg_field_set(unit, IQM_IQM_ENABLERSr, &val32, VSQ_CD_ENf, 0);
  }
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 210, exit, WRITE_IQM_IQM_ENABLERSr(unit, REG_PORT_ANY, val32));

  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 220, exit, READ_IPS_IPS_GENERAL_CONFIGURATIONSr_REG32(unit, REG_PORT_ANY, &val32));
  soc_reg_field_set(unit, IPS_IPS_GENERAL_CONFIGURATIONSr, &val32, DISCARD_ALL_CRDTf, 0);
  soc_reg_field_set(unit, IPS_IPS_GENERAL_CONFIGURATIONSr, &val32, DIS_DEQ_CMDSf, 0);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 230, exit, WRITE_IPS_IPS_GENERAL_CONFIGURATIONSr_REG32(unit, REG_PORT_ANY, val32));


  val32 = 0;
  soc_reg_field_set(unit, IRE_REG_FAP_PORT_CONFIGURATIONr, &val32, REG_REASSEMBLY_CONTEXTf, 1);
  soc_reg_field_set(unit, IRE_REG_FAP_PORT_CONFIGURATIONr, &val32, REG_PORT_TERMINATION_CONTEXTf, 1);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 240, exit, WRITE_IRE_REG_FAP_PORT_CONFIGURATIONr(unit, val32));

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 250, exit, WRITE_IRE_REGI_PKT_DATAr(unit, reg_above_64_val));

  
  COMPILER_64_ZERO(val64);
  soc_reg64_field32_set(unit, IRE_REGISTER_INTERFACE_PACKET_CONTROLr, &val64, REGI_PKT_SEND_DATAf, 0);
  soc_reg64_field32_set(unit, IRE_REGISTER_INTERFACE_PACKET_CONTROLr, &val64, REGI_PKT_ERRf, 0);
  soc_reg64_field32_set(unit, IRE_REGISTER_INTERFACE_PACKET_CONTROLr, &val64, REGI_PKT_SOPf, 1);
  soc_reg64_field32_set(unit, IRE_REGISTER_INTERFACE_PACKET_CONTROLr, &val64, REGI_PKT_EOPf, 0);
  soc_reg64_field32_set(unit, IRE_REGISTER_INTERFACE_PACKET_CONTROLr, &val64, REGI_PKT_BEf, 31);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 260, exit, WRITE_IRE_REGISTER_INTERFACE_PACKET_CONTROLr(unit, val64));
  soc_reg64_field32_set(unit, IRE_REGISTER_INTERFACE_PACKET_CONTROLr, &val64, REGI_PKT_SEND_DATAf, 1);
  soc_reg64_field32_set(unit, IRE_REGISTER_INTERFACE_PACKET_CONTROLr, &val64, REGI_PKT_ERRf, 0);
  soc_reg64_field32_set(unit, IRE_REGISTER_INTERFACE_PACKET_CONTROLr, &val64, REGI_PKT_SOPf, 1);
  soc_reg64_field32_set(unit, IRE_REGISTER_INTERFACE_PACKET_CONTROLr, &val64, REGI_PKT_EOPf, 0);
  soc_reg64_field32_set(unit, IRE_REGISTER_INTERFACE_PACKET_CONTROLr, &val64, REGI_PKT_BEf, 31);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 270, exit, WRITE_IRE_REGISTER_INTERFACE_PACKET_CONTROLr(unit, val64));
  soc_reg64_field32_set(unit, IRE_REGISTER_INTERFACE_PACKET_CONTROLr, &val64, REGI_PKT_SEND_DATAf, 0);
  soc_reg64_field32_set(unit, IRE_REGISTER_INTERFACE_PACKET_CONTROLr, &val64, REGI_PKT_ERRf, 0);
  soc_reg64_field32_set(unit, IRE_REGISTER_INTERFACE_PACKET_CONTROLr, &val64, REGI_PKT_SOPf, 0);
  soc_reg64_field32_set(unit, IRE_REGISTER_INTERFACE_PACKET_CONTROLr, &val64, REGI_PKT_EOPf, 0);
  soc_reg64_field32_set(unit, IRE_REGISTER_INTERFACE_PACKET_CONTROLr, &val64, REGI_PKT_BEf, 31);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 280, exit, WRITE_IRE_REGISTER_INTERFACE_PACKET_CONTROLr(unit, val64));
  soc_reg64_field32_set(unit, IRE_REGISTER_INTERFACE_PACKET_CONTROLr, &val64, REGI_PKT_SEND_DATAf, 1);
  soc_reg64_field32_set(unit, IRE_REGISTER_INTERFACE_PACKET_CONTROLr, &val64, REGI_PKT_ERRf, 0);
  soc_reg64_field32_set(unit, IRE_REGISTER_INTERFACE_PACKET_CONTROLr, &val64, REGI_PKT_SOPf, 0);
  soc_reg64_field32_set(unit, IRE_REGISTER_INTERFACE_PACKET_CONTROLr, &val64, REGI_PKT_EOPf, 0);
  soc_reg64_field32_set(unit, IRE_REGISTER_INTERFACE_PACKET_CONTROLr, &val64, REGI_PKT_BEf, 31);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 290, exit, WRITE_IRE_REGISTER_INTERFACE_PACKET_CONTROLr(unit, val64));
  soc_reg64_field32_set(unit, IRE_REGISTER_INTERFACE_PACKET_CONTROLr, &val64, REGI_PKT_SEND_DATAf, 0);
  soc_reg64_field32_set(unit, IRE_REGISTER_INTERFACE_PACKET_CONTROLr, &val64, REGI_PKT_ERRf, 0);
  soc_reg64_field32_set(unit, IRE_REGISTER_INTERFACE_PACKET_CONTROLr, &val64, REGI_PKT_SOPf, 0);
  soc_reg64_field32_set(unit, IRE_REGISTER_INTERFACE_PACKET_CONTROLr, &val64, REGI_PKT_EOPf, 1);
  soc_reg64_field32_set(unit, IRE_REGISTER_INTERFACE_PACKET_CONTROLr, &val64, REGI_PKT_BEf, 31);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 300, exit, WRITE_IRE_REGISTER_INTERFACE_PACKET_CONTROLr(unit, val64));
  soc_reg64_field32_set(unit, IRE_REGISTER_INTERFACE_PACKET_CONTROLr, &val64, REGI_PKT_SEND_DATAf, 1);
  soc_reg64_field32_set(unit, IRE_REGISTER_INTERFACE_PACKET_CONTROLr, &val64, REGI_PKT_ERRf, 0);
  soc_reg64_field32_set(unit, IRE_REGISTER_INTERFACE_PACKET_CONTROLr, &val64, REGI_PKT_SOPf, 0);
  soc_reg64_field32_set(unit, IRE_REGISTER_INTERFACE_PACKET_CONTROLr, &val64, REGI_PKT_EOPf, 1);
  soc_reg64_field32_set(unit, IRE_REGISTER_INTERFACE_PACKET_CONTROLr, &val64, REGI_PKT_BEf, 31);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 310, exit, WRITE_IRE_REGISTER_INTERFACE_PACKET_CONTROLr(unit, val64));


  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 320, exit, READ_IQM_IQM_INITr(unit, REG_PORT_ANY, &val32));
  soc_reg_field_set(unit, IQM_IQM_INITr, &val32, PDM_INITf, 1);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 330, exit, WRITE_IQM_IQM_INITr(unit, REG_PORT_ANY, val32));

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 340, exit, READ_IQM_IQM_INITr(unit, REG_PORT_ANY, &val32));
  soc_reg_field_set(unit, IQM_IQM_INITr, &val32, PDM_INITf, 0);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 350, exit, WRITE_IQM_IQM_INITr(unit, REG_PORT_ANY, val32));


  
  for (i = 0; i < NOF_QUEUES_TO_BLOCK; ++i) {
    val32 = 0;
    soc_reg_field_set(unit, IPS_MANUAL_QUEUE_OPERATION_QUEUE_IDr, &val32, MAN_QUEUE_IDf, i + BASE_QUEUE);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 360, exit, WRITE_IPS_MANUAL_QUEUE_OPERATION_QUEUE_IDr(unit, REG_PORT_ANY, val32));

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 370, exit, READ_IPS_MANUAL_QUEUE_OPERATIONr(unit, REG_PORT_ANY, &val32));
    soc_reg_field_set(unit, IPS_MANUAL_QUEUE_OPERATIONr, &val32, GRANT_CREDITf, 1);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 380, exit, WRITE_IPS_MANUAL_QUEUE_OPERATIONr(unit, REG_PORT_ANY, val32));
  }

  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 390, exit, WRITE_IPS_CRWDTHm(unit, MEM_BLOCK_ANY, 0, bu_IPS_CRWDTH));
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 400, exit, WRITE_IHP_PINFO_LLRm(unit, MEM_BLOCK_ANY, 1, bu_IHP_PINFO_LLR));
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 410, exit, WRITE_IHB_FWD_ACT_PROFILEm(unit, MEM_BLOCK_ANY, 0x2a, bu_IHB_FWD_ACT_PROFILE));

  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 420, exit, READ_IRE_STATIC_CONFIGURATIONr(unit, &val32));
  soc_reg_field_set(unit, IRE_STATIC_CONFIGURATIONr, &val32, ENABLE_DATA_PATHf, 0);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 430, exit, WRITE_IRE_STATIC_CONFIGURATIONr(unit, val32));

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 440, exit, READ_IDR_DYNAMIC_CONFIGURATIONr(unit, &val32));
  soc_reg_field_set(unit, IDR_DYNAMIC_CONFIGURATIONr, &val32, ENABLE_DATA_PATHf, 0);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 450, exit, WRITE_IDR_DYNAMIC_CONFIGURATIONr(unit, val32));

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 460, exit, READ_IRR_DYNAMIC_CONFIGURATIONr(unit, &val32));
  soc_reg_field_set(unit, IRR_DYNAMIC_CONFIGURATIONr, &val32, ENABLE_DATA_PATH_IQMf, 0);
  soc_reg_field_set(unit, IRR_DYNAMIC_CONFIGURATIONr, &val32, ENABLE_DATA_PATH_IDRf, 0);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 470, exit, WRITE_IRR_DYNAMIC_CONFIGURATIONr(unit, val32));

  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 480, exit, READ_IQM_IQM_ENABLERSr(unit, REG_PORT_ANY, &val32));
  soc_reg_field_set(unit, IQM_IQM_ENABLERSr, &val32, DSCRD_ALL_PKTf, 1);
  if (init->dram.pdm_mode != ARAD_INIT_PDM_MODE_SIMPLE ) {
    soc_reg_field_set(unit, IQM_IQM_ENABLERSr, &val32, VSQ_CD_ENf, 1);
  }
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 490, exit, WRITE_IQM_IQM_ENABLERSr(unit, REG_PORT_ANY, val32));

  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 500, exit, READ_IPS_IPS_GENERAL_CONFIGURATIONSr_REG32(unit, REG_PORT_ANY, &val32));
  soc_reg_field_set(unit, IPS_IPS_GENERAL_CONFIGURATIONSr, &val32, DISCARD_ALL_CRDTf, 1);
  soc_reg_field_set(unit, IPS_IPS_GENERAL_CONFIGURATIONSr, &val32, DIS_DEQ_CMDSf, 1);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 510, exit, WRITE_IPS_IPS_GENERAL_CONFIGURATIONSr_REG32(unit, REG_PORT_ANY, val32));


  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 520, exit, READ_ECI_BLOCKS_SOFT_INITr(unit, reg_above_64_val));
  soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, IPS_INITf, 1);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 530, exit, WRITE_ECI_BLOCKS_SOFT_INITr(unit, reg_above_64_val));
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 540, exit, READ_ECI_BLOCKS_SOFT_INITr(unit, reg_above_64_val));
  soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, IPS_INITf, 0);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 550, exit, WRITE_ECI_BLOCKS_SOFT_INITr(unit, reg_above_64_val));

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 560, exit, READ_IQM_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, REG_PORT_ANY, &val32));
  if ((should_disable_dynamic_mem_changes = (val32 & 1) == 0)) {
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 570, exit, WRITE_IQM_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, REG_PORT_ANY, 1));
  } 
  
  for (i = 0; i < NOF_QUEUES_TO_BLOCK; ++i) {
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 580, exit, READ_IQM_PQDMDm(unit, MEM_BLOCK_ANY, i + BASE_QUEUE, entry));
    soc_mem_field32_set(unit, IQM_PQDMDm, entry, QUE_NOT_EMPTYf, 0);
    soc_mem_field32_set(unit, IQM_PQDMDm, entry, PQ_INST_QUE_SIZEf, 0);
    soc_mem_field32_set(unit, IQM_PQDMDm, entry, PQ_INST_QUE_BUFF_SIZEf, 0);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 590, exit, WRITE_IQM_PQDMDm(unit, MEM_BLOCK_ANY, i + BASE_QUEUE, entry));
  }

  res = dpp_mult_ing_group_close(unit, TEMP_MC_ID);
  SOC_SAND_CHECK_FUNC_RESULT(res, 600, exit);

  if ((init->dram.enable == TRUE) || (init->drc_info.enable)) {
    
    nof_dram_buffs = init->dram.nof_dram_buffers;
    SOC_SAND_CHECK_FUNC_RESULT(res, 610, exit);
    if ((init->ocb.ocb_enable == OCB_DISABLED) || (init->ocb.ocb_enable == OCB_ENABLED) || (init->ocb.ocb_enable == OCB_DRAM_SEPARATE)){
        res = arad_init_dram_buff_boundaries_calc(unit, nof_dram_buffs, init->dram.dbuff_size, init->ocb, &dbuffs_bdries);
        SOC_SAND_CHECK_FUNC_RESULT(res, 620, exit);
    }
    else if (init->ocb.ocb_enable == OCB_ONLY){ 
        res = arad_init_dram_buff_boundaries_calc_ocb_only(unit, nof_dram_buffs, init->dram.dbuff_size, init->ocb.repartition_mode, &dbuffs_bdries);
        SOC_SAND_CHECK_FUNC_RESULT(res, 630, exit);
        dbuffs_bdries.fmc.start = dbuffs_bdries.ocb_fmc.start;
    }
    else if (init->ocb.ocb_enable == OCB_ONLY_1_DRAM){
        res = arad_init_dram_buff_boundaries_calc_ocb_only_1_dram(unit, nof_dram_buffs, init->dram.dbuff_size, init->ocb.repartition_mode, &dbuffs_bdries);
        SOC_SAND_CHECK_FUNC_RESULT(res, 640, exit);
        dbuffs_bdries.fmc.start = dbuffs_bdries.ocb_fmc.start;
    }
  } else{ 
      if (init->ocb.ocb_enable != OCB_DISABLED) {
            
            res = arad_init_dram_buff_boundaries_calc_ocb_only(unit, nof_dram_buffs, init->dram.dbuff_size, init->ocb.repartition_mode, &dbuffs_bdries);
            SOC_SAND_CHECK_FUNC_RESULT(res, 650, exit);
            dbuffs_bdries.fmc.start = dbuffs_bdries.ocb_fmc.start;
      }
  }

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 660, exit, READ_NBI_STATISTICS_RX_BURSTS_ERR_CNTr(unit, &val32));
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 670, exit, READ_IRE_REGI_PACKET_COUNTERr(unit, &val64));
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 680, exit, READ_IQM_ENQUEUE_PACKET_COUNTERr(unit, REG_PORT_ANY, &val32));

#ifdef DEBUG_WORKAROUND
  LOG_INFO(BSL_LS_SOC_INIT,
           (BSL_META_U(unit,
                       "buffers to clear at 0x%lx\n"),(unsigned long)dbuffs_bdries.fmc.start));

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 690, exit, READ_IQM_FREE_FULL_MULTICAST_DBUFFS_COUNTERr(unit, REG_PORT_ANY, &val32));
  SOC_SAND_CHECK_FUNC_RESULT(res, 700, exit);
  LOG_INFO(BSL_LS_SOC_INIT,
           (BSL_META_U(unit,
                       "IQM_FREE_FULL_MULTICAST_DBUFFS_COUNTER is 0x%lx\n"),(unsigned long)val32));
#endif

  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 710, exit, READ_IQM_FLUSCNTm(unit, MEM_BLOCK_ANY, dbuffs_bdries.fmc.start, entry));
  soc_mem_field32_set(unit, IQM_FLUSCNTm, entry, FLUS_CNTf, 0);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 720, exit, WRITE_IQM_FLUSCNTm(unit, MEM_BLOCK_ANY, dbuffs_bdries.fmc.start, entry));

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 730, exit, READ_IPT_CPU_D_BUFF_RELEASE_CONTROLr(unit, &val32));
  soc_reg_field_set(unit, IPT_CPU_D_BUFF_RELEASE_CONTROLr, &val32, CPU_RELEASE_BUFFER_VALIDf, 1);
  soc_reg_field_set(unit, IPT_CPU_D_BUFF_RELEASE_CONTROLr, &val32, BUFFER_2_RELEASEf, dbuffs_bdries.fmc.start);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 740, exit, WRITE_IPT_CPU_D_BUFF_RELEASE_CONTROLr(unit, val32));

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 750, exit, WRITE_IQM_INTERRUPT_REGISTERr(unit, REG_PORT_ANY, 0x7fffffff & ~intr_val)); 
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 760, exit, WRITE_IQM_INTERRUPT_MASK_REGISTERr(unit, REG_PORT_ANY, intr_mask));

#ifdef DEBUG_WORKAROUND
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 770, exit, READ_IQM_FREE_FULL_MULTICAST_DBUFFS_COUNTERr(unit, REG_PORT_ANY, &val32));
  SOC_SAND_CHECK_FUNC_RESULT(res, 780, exit);
  LOG_INFO(BSL_LS_SOC_INIT,
           (BSL_META_U(unit,
                       "IQM_FREE_FULL_MULTICAST_DBUFFS_COUNTER is 0x%lx\n"),(unsigned long)val32));
#endif

exit:
  ARAD_FREE(mc_members);
  if (should_disable_dynamic_mem_changes) { 
    if(WRITE_IQM_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, REG_PORT_ANY, 0) != SOC_E_NONE) {
     LOG_ERROR(BSL_LS_SOC_INIT,
               (BSL_META_U(unit,
                           SOC_DPP_MSG("Failed to write to the register IQM_ENABLE_DYNAMIC_MEMORY_ACCESS\n"))));
    }
  }
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_iqm_workaround()", unit, 0);
}

#if ! (defined __KERNEL__) && ! (defined _STDLIB_H)
extern char *getenv(const char*);
#endif


#ifdef BCM_88660_A0

STATIC uint32
  arad_set_stat_if_tc_source_unsafe(
    SOC_SAND_IN  int unit
  )
{
    uint32 res;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    if (SOC_IS_ARADPLUS(unit)) {
        char *propkey = spn_STAT_IF_TC_SOURCE;
        char *propval = soc_property_get_str(unit, propkey);
        uint32 configuration = 0;

        if (propval) {
            if (sal_strcmp(propval, "ORIGINAL") == 0) { 
                configuration = 1;
            } else if (sal_strcmp(propval, "MAPPED") != 0) { 
                LOG_ERROR(BSL_LS_SOC_INIT,
                          (BSL_META_U(unit,
                                      "Unexpected property value (\"%s\") for %s\n\r"), propval, propkey));
                SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 50, exit);
            }
        }
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  60,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_STATISTICS_REPORT_CONFIGURATIONS_7r, REG_PORT_ANY, 0, ST_BILL_ING_USE_IRR_TCf,  configuration));
    }
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_set_stat_if_tc_source_unsafe()", unit, 0);
}
#endif 

int
soc_arad_tcam_bist_check(int unit)
{
    int i,j,k;
    uint32 value;
    uint64 reg_val64;

    SOCDNX_INIT_FUNC_DEFS;

    for (j=0; j<MAX_TCAM_MODE_ARAD; j++) {
        if (j != INVALID_TCAM_MODE_ARAD) {
            for (i=0; i<TCAM_BIST_CHECK_BANK_ARAD; i++) {
                SOCDNX_IF_ERR_EXIT(READ_IHB_REG_0307r(unit, &reg_val64));
                SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, IHB_REG_0307r, 0, 0, &reg_val64));
                soc_reg64_field32_set(unit, IHB_REG_0307r, &reg_val64, FIELD_55_61f, 0x3f);
                soc_reg64_field32_set(unit, IHB_REG_0307r, &reg_val64, FIELD_0_2f, i);
                soc_reg64_field32_set(unit, IHB_REG_0307r, &reg_val64, FIELD_3_4f, j);
                SOCDNX_IF_ERR_EXIT(WRITE_IHB_REG_0307r(unit, reg_val64));

                sal_usleep(10000);

                SOCDNX_IF_ERR_EXIT(READ_IHB_REG_0309r(unit, &value));
                if (value != 3 ) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Running TCAM BIST FAILED: got %u err in tcam bank %d bist mode %d test\n"),value,i,j));
                }
            }
            for (i=0; i<TCAM_BIST_CHECK_BANK_ARAD; i++) {
                k =  1 << i ;
                SOCDNX_IF_ERR_EXIT(READ_IHB_REG_0307r(unit, &reg_val64));
                SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, IHB_REG_0307r, 0, 0, &reg_val64));
                soc_reg64_field32_set(unit, IHB_REG_0307r, &reg_val64, FIELD_55_61f, k);
                soc_reg64_field32_set(unit, IHB_REG_0307r, &reg_val64, FIELD_0_2f, i);
                soc_reg64_field32_set(unit, IHB_REG_0307r, &reg_val64, FIELD_3_4f, j);
                SOCDNX_IF_ERR_EXIT(WRITE_IHB_REG_0307r(unit, reg_val64));

                sal_usleep(10000);

                SOCDNX_IF_ERR_EXIT(READ_IHB_REG_0309r(unit, &value));
                if (value != 3 ) {
                   SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Running TCAM BIST FAILED: got %u err in tcam bank %d bist mode %d test\n"),value,i,j));
                }
            }
        }
     }

exit:
    SOCDNX_FUNC_RETURN;
}


uint32
  arad_mgmt_init_sequence_phase1_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_MGMT_INIT         *init,
    SOC_SAND_IN  uint8                  silent
  )
{
  uint32
    nof_dram_buffs = 0,
    res,tcam_bist_enable;
  int
    core, rv;
  uint32
     stage_internal_id = 0,
     stage_id = 0,
     port_i,
     iqm_init_timeout;
  uint32
     tm_port, priority_mode,
     shaper_mode, base_q_pair, flags;
  ARAD_INIT_DBUFFS_BDRY
    dbuffs_bdries;
  soc_reg_above_64_val_t
      reg_above_64_val;
  soc_pbmp_t
      pbmp;
  const char *propval =
#ifdef __KERNEL__
    0;
#else
    getenv("DO_NOT_INITIALIZE_ALL_TABLES");
#endif
  int init_tables;
  int save_mbist_property;
  ARAD_PORT2IF_MAPPING_INFO mapping_info;
  ARAD_NIF_TYPE nif_type;
  uint32 phy_port;
  soc_port_if_t interface_type;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_MGMT_INIT_SEQUENCE_PHASE1_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(init);
  (void)stage_internal_id;
  (void)stage_id;
  sal_memset(&dbuffs_bdries, 0, sizeof(dbuffs_bdries));

  
  
  

  res = arad_init_operation_mode_set(unit, init);
  SOC_SAND_CHECK_FUNC_RESULT(res, 3, exit);

  if ((init->dram.enable == TRUE) || (init->drc_info.enable)) {
    
    nof_dram_buffs = init->dram.nof_dram_buffers;
    SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
    if ((init->ocb.ocb_enable == OCB_DISABLED) || (init->ocb.ocb_enable == OCB_ENABLED) || (init->ocb.ocb_enable == OCB_DRAM_SEPARATE)){
        res = arad_init_dram_buff_boundaries_calc(unit, nof_dram_buffs, init->dram.dbuff_size, init->ocb, &dbuffs_bdries);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    }
    else if (init->ocb.ocb_enable == OCB_ONLY){ 
        res = arad_init_dram_buff_boundaries_calc_ocb_only(unit, nof_dram_buffs, init->dram.dbuff_size, init->ocb.repartition_mode, &dbuffs_bdries);
        SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);
    }
    else if (init->ocb.ocb_enable == OCB_ONLY_1_DRAM){
        res = arad_init_dram_buff_boundaries_calc_ocb_only_1_dram(unit, nof_dram_buffs, init->dram.dbuff_size, init->ocb.repartition_mode, &dbuffs_bdries);
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    }
  } else{ 
      if (init->ocb.ocb_enable != OCB_DISABLED) {
            
            res = arad_init_dram_buff_boundaries_calc_ocb_only(unit, nof_dram_buffs, init->dram.dbuff_size, init->ocb.repartition_mode, &dbuffs_bdries);
            SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);
      }
  }
  
  
  
  ARAD_INIT_PRINT_ADVANCE("Mgmt");

  
  
  
  ARAD_INIT_PRINT_INTERNAL_LEVEL_0_ADVANCE("Initialize internal blocks, in-reset");
  res = arad_mgmt_init_before_blocks_oor(
          unit,
          &dbuffs_bdries,
          init
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);


  
  
  
  ARAD_INIT_PRINT_INTERNAL_LEVEL_0_ADVANCE("Take internal blocks out-of-reset");
  res = arad_mgmt_blocks_init_unsafe(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

#ifdef PLISIM
  if (!SAL_BOOT_PLISIM)
#endif
  {
      uint32 ser_test_iters = soc_property_suffix_num_get_only_suffix(unit, -1, spn_BIST_ENABLE, "set_test_iters_num", 0);
      if (ser_test_iters) { 
          uint32 ser_test_num = soc_property_suffix_num_get_only_suffix(unit, -1, spn_BIST_ENABLE, "ser_test_num", 1);
          uint32 time_to_wait = soc_property_suffix_num_get_only_suffix(unit, -1, spn_BIST_ENABLE, "ser_test_delay_sec", 0);
          if (time_to_wait == 0) { 
              time_to_wait = soc_property_suffix_num_get_only_suffix(unit, -1, spn_BIST_ENABLE, "ser_test_delay_us", 0);
              if (time_to_wait == 0) {
                  time_to_wait = 3600 | DCMN_MBIST_TEST_LONG_WAIT_DELAY_IS_SEC; 
              }
          } else {
              time_to_wait |= DCMN_MBIST_TEST_LONG_WAIT_DELAY_IS_SEC;
          }
          ARAD_INIT_PRINT_INTERNAL_LEVEL_0_ADVANCE("running MBIST based SER tests");
          SOC_SAND_SOC_IF_ERROR_RETURN(res, 4, exit, soc_bist_arad_ser_test(unit, 1, ser_test_iters, time_to_wait, ser_test_num));
      }

      res = soc_property_get(unit, spn_BIST_ENABLE, 0);
      if (!res && SOC_DPP_CONFIG(unit)->tm.various_bm & DPP_VARIOUS_BM_FORCE_MBIST_TEST) {
          res = 1;
      }
      tcam_bist_enable = soc_property_get(unit, spn_TCAM_BIST_ENABLE, 0);

      
      save_mbist_property = SOC_CONTROL(unit)->schanIntrEnb;
      SOC_CONTROL(unit)->schanIntrEnb=0;
      
      if (res || tcam_bist_enable) {
        if (res) {
            ARAD_INIT_PRINT_INTERNAL_LEVEL_0_ADVANCE("running internal memories BIST");
            if (SOC_IS_ARDON(unit)) {
                SOC_SAND_SOC_IF_ERROR_RETURN(res, 3, exit, soc_bist_all_ardon(unit, (res == 2 ? 1 : 0)));
            } else {
                SOC_SAND_SOC_IF_ERROR_RETURN(res, 4, exit, soc_bist_all_arad(unit, (res == 2 ? 1 : 0)));
            }
        }

        if (tcam_bist_enable) {
            ARAD_INIT_PRINT_INTERNAL_LEVEL_0_ADVANCE("running TCAM BIST");
                SOC_SAND_SOC_IF_ERROR_RETURN(res, 4, exit, soc_arad_tcam_bist_check(unit));
        }
      } else {
          if (SOC_IS_ARDON(unit)) 
          {
          } else {
              SOC_SAND_SOC_IF_ERROR_RETURN(res, 6, exit, soc_bist_irdp_arad(unit, 0)); 
          }
      }
      SOC_CONTROL(unit)->schanIntrEnb=save_mbist_property;
  }


  
  
  
  
  
  ARAD_INIT_PRINT_INTERNAL_LEVEL_0_ADVANCE("Initialize tables to zero");
  if (propval) {
    init_tables = !sal_strcmp(propval, "0");
  } else {
#ifdef PLISIM
    if (SAL_BOOT_PLISIM) {
        init_tables = 0;
    }
    else
#endif
    {
        init_tables = 1;
    }
  }
   
  if (init_tables) { 
    res = arad_mgmt_all_tbls_init(unit, silent);
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }

  
  reg_above_64_val[3] = reg_above_64_val[2] =
  reg_above_64_val[1] = reg_above_64_val[0] = 0xFFFFFFFF;
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 42, exit, WRITE_ECI_BLOCKS_SOFT_INITr(unit, reg_above_64_val));
  sal_usleep(10);

  reg_above_64_val[3] = reg_above_64_val[2] =
  reg_above_64_val[1] = reg_above_64_val[0] = 0;
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 45, exit, WRITE_ECI_BLOCKS_SOFT_INITr(unit, reg_above_64_val));




  
  
  
  sal_usleep(1000);

  res = arad_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, IPS_IPS_GENERAL_CONFIGURATIONSr, REG_PORT_ANY, 0, IPS_INIT_TRIGGERf, 0x0);
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

  iqm_init_timeout = (SOC_DPP_CONFIG(unit)->emulation_system == 1) ? ARAD_TIMEOUT * 1000 : ARAD_TIMEOUT;
  res = arad_polling(unit, iqm_init_timeout, ARAD_MIN_POLLS, IQM_IQM_INITr, REG_PORT_ANY, 0, PDM_INITf, 0x0);
  SOC_SAND_CHECK_FUNC_RESULT(res, 47, exit);

  res = arad_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, EGQ_EGQ_BLOCK_INIT_STATUSr, REG_PORT_ANY, 0, EGQ_BLOCK_INITf, 0x0);
  SOC_SAND_CHECK_FUNC_RESULT(res, 48, exit);


  
  
  
  if (init->core_freq.enable)
  {
      ARAD_INIT_PRINT_INTERNAL_LEVEL_0_ADVANCE("Set core clock frequency");
      res = arad_mgmt_init_set_core_clock_frequency(unit, init);
      SOC_SAND_CHECK_FUNC_RESULT(res, 28, exit);
  }

  
  
  
  ARAD_INIT_PRINT_INTERNAL_LEVEL_0_ADVANCE("Stop traffic");
  res = arad_mgmt_enable_traffic_set(
          unit,
          FALSE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  
  
  
  
  
  
  ARAD_INIT_PRINT_INTERNAL_LEVEL_0_ADVANCE("Stop control cells")
  res = arad_mgmt_all_ctrl_cells_enable_set(
          unit,
          FALSE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  
  
  
  ARAD_INIT_PRINT_INTERNAL_LEVEL_0_ADVANCE("Finalize internal blocks initialization");
  res = arad_mgmt_init_after_blocks_oor(
          unit,
          init,
          &dbuffs_bdries
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);

  {
      soc_dpp_guaranteed_q_resource_t guaranteed_q_resource;
   
      

      res = sw_state_access[unit].dpp.soc.arad.tm.guaranteed_q_resource.get(unit, 0, &guaranteed_q_resource);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 830, exit);

      guaranteed_q_resource.dram.total = 
          (dbuffs_bdries.uc.end - dbuffs_bdries.uc.start + 1);
      guaranteed_q_resource.ocb.total = 
          (dbuffs_bdries.ocb_uc.end - dbuffs_bdries.ocb_uc.start + 1);

      
      res = sw_state_access[unit].dpp.soc.arad.tm.guaranteed_q_resource.set(unit, 0, &guaranteed_q_resource);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 835, exit);
  }

  
  
  
  
  
  ARAD_INIT_PRINT_INTERNAL_LEVEL_0_ADVANCE("Configure tables defaults");

  res = arad_mgmt_tbls_init(
          unit,
          silent
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  
  
  
  
  ARAD_INIT_PRINT_INTERNAL_LEVEL_0_ADVANCE("Initialize registers defaults");
  res = arad_mgmt_hw_set_defaults(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  
  
  
  if (init != NULL)
  {
    ARAD_INIT_PRINT_INTERNAL_LEVEL_0_ADVANCE("Configure HW interfaces");
    res = arad_mgmt_hw_interfaces_set_unsafe(
            unit,
            init,
            silent
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
  }
  else
  {
    ARAD_INIT_PRINT_INTERNAL_LEVEL_0_ADVANCE("SKIPPING arad_mgmt_hw_interfaces_set");
  }
  
  
  

  res = arad_bubble_configuration_set_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  
  
  
    if (init->credit.credit_worth_enable)
    {
      ARAD_INIT_PRINT_INTERNAL_LEVEL_0_ADVANCE("Set credit worth");
      rv = arad_mgmt_credit_worth_set(
              unit,
              init->credit.credit_worth
            );
      SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 70, exit);
    }



  
  
  
  
  ARAD_INIT_PRINT_ADVANCE("Set default configuration");
  res = arad_mgmt_functional_init(
          unit,
          init,
          silent
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);


  
  
  
  ARAD_INIT_PRINT_ADVANCE("Port");
  
  ARAD_INIT_PRINT_INTERNAL_LEVEL_0_ADVANCE("Set the Egress editor");
  res = arad_egr_prog_editor_config_dut_by_queue_database(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 164, exit);

  
  
  
  ARAD_INIT_PRINT_INTERNAL_LEVEL_0_ADVANCE("Set OFP to Egress port information");

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 165, exit, soc_port_sw_db_valid_ports_get(unit, 0, &pbmp));

  SOC_PBMP_ITER(pbmp,port_i) {
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 166, exit, soc_port_sw_db_flags_get(unit, port_i, &flags));
      if (!(SOC_PORT_IS_ELK_INTERFACE(flags) || SOC_PORT_IS_STAT_INTERFACE(flags))) {
          res = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.base_q_pair.get(unit, port_i, &base_q_pair);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 120, exit);

          res = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.shaper_mode.get(unit, port_i, &shaper_mode);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 125, exit);

          res = soc_port_sw_db_local_to_out_port_priority_get(unit, port_i, &priority_mode);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 135, exit);

          res = soc_port_sw_db_local_to_tm_port_get(unit, port_i, &tm_port, &core);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 140, exit);

          
          res = arad_egr_dsp_pp_to_base_q_pair_set(unit, core, tm_port, base_q_pair);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 145, exit);

          
          res = arad_egr_dsp_pp_priorities_mode_set(unit, core, tm_port, priority_mode);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 145, exit);

          
          res = arad_egr_dsp_pp_shaper_mode_set_unsafe(unit, core, tm_port, shaper_mode);
          SOC_SAND_CHECK_FUNC_RESULT(res, 160, exit);
      }
  }

  ARAD_INIT_PRINT_INTERNAL_LEVEL_0_ADVANCE("Set port to interface mapping");

  
  if (SOC_DPP_CONFIG(unit)->arad->init.dynamic_port_enable)
  {
      res = arad_ports_init_interfaces_dynamic_context_map(unit);
      SOC_SAND_CHECK_FUNC_RESULT(res, 164, exit);
  }
  else
  {
      res = arad_ports_init_interfaces_context_map(
           unit,
           &(init->ports)
           );
      SOC_SAND_CHECK_FUNC_RESULT(res, 165, exit);
  }

  
  res = arad_ports_init_interfaces_erp_setting(
       unit,
       &(init->ports)
       );
  SOC_SAND_CHECK_FUNC_RESULT(res, 167, exit);

  SOC_PBMP_ITER(pbmp, port_i) {
      arad_ARAD_PORT2IF_MAPPING_INFO_clear(&mapping_info);
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 169, exit, soc_port_sw_db_interface_type_get(unit, port_i, &interface_type));

      res = soc_port_sw_db_local_to_tm_port_get(unit, port_i, &tm_port, &core);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 175, exit);

      switch (interface_type) {
      case SOC_PORT_IF_XFI:
      case SOC_PORT_IF_RXAUI:
      case SOC_PORT_IF_DNX_XAUI:
      case SOC_PORT_IF_SGMII:
      case SOC_PORT_IF_XLAUI:
      case SOC_PORT_IF_CAUI:
      case SOC_PORT_IF_ILKN:
      case SOC_PORT_IF_TM_INTERNAL_PKT:
        arad_port_to_nif_type(unit, tm_port, &nif_type);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 1050, exit, soc_port_sw_db_first_phy_port_get(unit, port_i, &phy_port));
        mapping_info.if_id = arad_nif_intern2nif_id(unit, nif_type, phy_port - 1);
        break;
      case SOC_PORT_IF_CPU:
        mapping_info.if_id = ARAD_IF_ID_CPU;
        break;
      case SOC_PORT_IF_RCY:
        mapping_info.if_id = ARAD_IF_ID_RCY;
        break;
      case SOC_PORT_IF_ERP:
        mapping_info.if_id = ARAD_IF_ID_ERP;
        break;
      case SOC_PORT_IF_OLP:
        mapping_info.if_id = ARAD_IF_ID_OLP;
        break;
      case SOC_PORT_IF_OAMP:
        mapping_info.if_id = ARAD_IF_ID_OAMP;
        break;
      default:
        SOC_SAND_SET_ERROR_CODE(ARAD_INCOMPATABLE_NIF_ID_ERR, 1060, exit);
        break;
      }

      SOC_SAND_SOC_IF_ERROR_RETURN(res, 1070, exit, soc_port_sw_db_channel_get(unit, port_i, &(mapping_info.channel_id)));

      if (mapping_info.if_id == ARAD_IF_ID_ERP) {
        res = arad_port_to_interface_map_set_unsafe(
           unit,
           core,
           tm_port,
           ARAD_PORT_DIRECTION_OUTGOING,
           &mapping_info,
           TRUE
           );
        SOC_SAND_CHECK_FUNC_RESULT(res, 125, exit);
      } else {
        res = arad_port_to_interface_map_set_unsafe(
           unit,
           core,
           tm_port,
           ARAD_PORT_DIRECTION_BOTH,
           &mapping_info,
           TRUE
           );
        SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);
      }
  }

  ARAD_INIT_PRINT_INTERNAL_LEVEL_0_ADVANCE("Set port header type");

  SOC_PBMP_ITER(pbmp, port_i) {
    res = arad_ports_header_type_update(unit, port_i);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 140, exit);
  }

  
  res = arad_ports_lag_mode_set_unsafe(unit, init->ports.lag_mode);
  SOC_SAND_CHECK_FUNC_RESULT(res, 152, exit);
  res = soc_arad_trunk_resolve_ingress_mc_destination_method(unit, init->ports.use_trunk_as_ingress_mc_dest);
  SOC_SAND_CHECK_FUNC_RESULT(res, 153, exit);

  
  if (init->ports.is_stacking_system == 0x1) {
      ARAD_INIT_PRINT_ADVANCE("Stacking");
      res = arad_mgmt_stk_init(unit, init);
      SOC_SAND_CHECK_FUNC_RESULT(res, 170, exit);
  }

  
  if (init->ports.is_system_red == 0x1) {
      ARAD_INIT_PRINT_ADVANCE("System RED");
      res = arad_mgmt_system_red_init(unit, init);
      SOC_SAND_CHECK_FUNC_RESULT(res, 175, exit);
  }

  
  res = arad_mgmt_swap_init(unit, &(init->ports.swap_info));
  SOC_SAND_CHECK_FUNC_RESULT(res, 180, exit);

  
  res = arad_mgmt_pon_init(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 190, exit);

  if ((SOC_DPP_CONFIG(unit))->pp.port_extender_map_lc_exists == 2) {
      
      res = arad_mgmt_coe_init(unit);
      SOC_SAND_CHECK_FUNC_RESULT(res, 200, exit);
  }

  
  
  

  if (init-> stat_if.stat_if_enable)
  {
      ARAD_INIT_PRINT_INTERNAL_LEVEL_0_ADVANCE("Configure statistic interfaces");
      res = arad_stat_if_report_info_set_unsafe(
            unit,
            &(init-> stat_if.stat_if_info[0])
      );
      SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
  }
#ifdef BCM_88660_A0
  if (SOC_IS_ARADPLUS(unit)) {
      SOC_SAND_CHECK_FUNC_RESULT(arad_set_stat_if_tc_source_unsafe(unit), 81, exit);
  }
#endif

#if defined(INCLUDE_KBP)
  arad_kbp_sw_init(unit);
#endif

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_mgmt_init_sequence_phase1_unsafe()", 0, 0);
}

uint32
  arad_mgmt_init_sequence_phase1_verify(
    SOC_SAND_IN     int              unit,
    SOC_SAND_IN     ARAD_MGMT_INIT           *init
  )
{
  uint32
    res = 0;


  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_MGMT_INIT_SEQUENCE_PHASE1_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(init);
  SOC_SAND_MAGIC_NUM_VERIFY(init);


  if ((init->dram.enable)  && (!SOC_IS_DPP_DRC_COMBO28(unit)))
  {
    if (init->dram.conf_mode == ARAD_HW_DRAM_CONF_MODE_BUFFER)
    {
      SOC_SAND_ERR_IF_ABOVE_MAX(
        init->dram.dram_conf.buffer_mode.buff_len, ARAD_HW_DRAM_CONF_SIZE_MAX-1,
        ARAD_HW_DRAM_CONF_LEN_OUT_OF_RANGE_ERR, 10, exit
        );
    }
    if (init->dram.conf_mode == ARAD_HW_DRAM_CONF_MODE_PARAMS)
    {
      res = arad_dram_info_verify(
              unit,
              init->dram.dram_conf.params_mode.dram_freq,
              init->dram.dram_type,
              &init->dram.dram_conf.params_mode.params
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);
    }
  }

  res = arad_mgmt_hw_interfaces_verify(
          unit,
          init
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  
  if (init->fabric.is_fe600 == TRUE) {
    

    
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_mgmt_init_sequence_phase1_verify()", 0, 0);
}



#define ARAD_NOF_NIF_BLOCKS (SOC_ARAD_NOF_INSTANCES_CLP + SOC_ARAD_NOF_INSTANCES_XLP)
#define ARAD_NOF_XMACS_IN_CLP SOC_ARAD_NOF_QUADS_IN_CLP
#define ARAD_NOF_CLP_SUB_BLOCKS (ARAD_NOF_XMACS_IN_CLP + 1)

#define ARAD_NOF_ARAD_FABRIC_QUADS(unit)   (SOC_ARAD_NOF_QUADS_IN_FSRD * SOC_DPP_DEFS_GET(unit, nof_instances_fsrd))
#define ARAD_MAX_NOF_ARAD_FABRIC_QUADS     (SOC_ARAD_NOF_QUADS_IN_FSRD * SOC_DPP_DEFS_MAX(NOF_INSTANCES_FSRD))
#define ARAD_NOF_ARAD_FABRIC_3QUAD_FMACS   (SOC_DPP_DEFS_MAX(NOF_INSTANCES_FSRD))


int
arad_activate_power_savings(int unit)
{
    uint32 val32;
    int i;
    soc_port_t port;
    pbmp_t ports;
    soc_reg_above_64_val_t fld_value, val_above64;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_REG_ABOVE_64_CREATE_MASK(fld_value, 1, 0);

    if (soc_property_suffix_num_get(unit, -1, spn_LOW_POWER, "nif_mac", 1)) {
        
        soc_port_if_t interface_type;
        int block_index, block, phy_port;
        int nif_blocks[ARAD_NOF_NIF_BLOCKS];
        char block_not_used[ARAD_NOF_NIF_BLOCKS] = {1, 1, 1, 1};
        char clp_sub_blocks_not_used[ARAD_NOF_CLP_SUB_BLOCKS * SOC_ARAD_NOF_INSTANCES_CLP] = {1, 1, 1, 1, 1, 1, 1, 1};
        nif_blocks[0] = CLP_BLOCK(unit, 0);
        nif_blocks[1] = CLP_BLOCK(unit, 1);
        nif_blocks[2] = XLP_BLOCK(unit, 0);
        nif_blocks[3] = XLP_BLOCK(unit, 1);

        
        BCM_PBMP_ASSIGN(ports, PBMP_PORT_ALL(unit));
        BCM_PBMP_REMOVE(ports, PBMP_SFI_ALL(unit));
        PBMP_ITER(ports, port) {
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port, &val32));
            if (!val32) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("Port %d is invalid"), port));
            }
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface_type));
            if (interface_type == SOC_PORT_IF_CPU) {
                continue;
            }
            if ((phy_port = SOC_INFO(unit).port_l2p_mapping[port]) < 0) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("Port %d is not mapped to a physical port"), port));
            }
            block = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);

            for (block_index = 0; ; ++block_index) {
                if (block_index >= ARAD_NOF_NIF_BLOCKS) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("Port %d has a non recognized block"), port));
                } else if (block == nif_blocks[block_index]) {
                    if (block_index < SOC_ARAD_NOF_INSTANCES_CLP) { 
                        if (interface_type == SOC_PORT_IF_ILKN) {
                            
                        } else if (interface_type == SOC_PORT_IF_CAUI) {
                            clp_sub_blocks_not_used[ARAD_NOF_CLP_SUB_BLOCKS * block_index + ARAD_NOF_XMACS_IN_CLP] = 0; 
                        } else {
                            uint32 xmac_index, in_block_index;
                            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_in_block_index_get(unit, port, &in_block_index));
                            xmac_index = in_block_index / LANES_IN_QUAD;
                            if (xmac_index >= ARAD_NOF_XMACS_IN_CLP) {
                                SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("Could not identify CMAC of port %d"), port));
                            }
                            clp_sub_blocks_not_used[ARAD_NOF_CLP_SUB_BLOCKS * block_index + xmac_index] = 0; 
                        }
                    } else { 
                    }
                    block_not_used[block_index] = 0;
                    break;
                }
            }
        }
#ifdef BCM_88660_A0
        if (SOC_IS_ARADPLUS(unit)) {
            uint8
                clp0_not_used = block_not_used[0],
                clp1_not_used = block_not_used[1],
                xlp0_not_used = block_not_used[2],
                xlp1_not_used = block_not_used[3];
           
            val32 = 0;
            SOCDNX_IF_ERR_EXIT(READ_NBI_NIF_PORTS_CFGr(unit,0, &val32));
            if (clp0_not_used) {
                soc_reg_field_set(unit, NBI_NIF_PORTS_CFGr, &val32, CLP_N_OTP_PORT_BOND_OPTIONf, 0x2);
            }
            if (xlp0_not_used) {
                soc_reg_field_set(unit, NBI_NIF_PORTS_CFGr, &val32, XLP_N_OTP_PORT_BOND_OPTIONf, 0x2);
            }
            SOCDNX_IF_ERR_EXIT(WRITE_NBI_NIF_PORTS_CFGr(unit,0, val32));

            val32 = 0;
            SOCDNX_IF_ERR_EXIT(READ_NBI_NIF_PORTS_CFGr(unit,1, &val32));
            if (clp1_not_used) {
                soc_reg_field_set(unit, NBI_NIF_PORTS_CFGr, &val32, CLP_N_OTP_PORT_BOND_OPTIONf, 0x2);
            }
            if (xlp1_not_used) {
                soc_reg_field_set(unit, NBI_NIF_PORTS_CFGr, &val32, XLP_N_OTP_PORT_BOND_OPTIONf, 0x2);
            }
            SOCDNX_IF_ERR_EXIT(WRITE_NBI_NIF_PORTS_CFGr(unit,1, val32));
        }
#endif 
        
        for (block_index = 0; block_index < ARAD_NOF_NIF_BLOCKS; ++block_index) {
            port = SOC_BLOCK_PORT(unit, nif_blocks[block_index]); 
            val32 = 0;
            if (block_index < SOC_ARAD_NOF_INSTANCES_CLP) { 
                
                if (block_not_used[block_index] || !SOC_DPP_CONFIG(unit)->arad->init.dynamic_port_enable) {
                    soc_reg_field_set(unit, PORT_POWER_SAVEr, &val32, XPORT_CORE_0f, clp_sub_blocks_not_used[ARAD_NOF_CLP_SUB_BLOCKS * block_index]);
                    soc_reg_field_set(unit, PORT_POWER_SAVEr, &val32, XPORT_CORE_1f, clp_sub_blocks_not_used[ARAD_NOF_CLP_SUB_BLOCKS * block_index + 1]);
                    soc_reg_field_set(unit, PORT_POWER_SAVEr, &val32, XPORT_CORE_2f, clp_sub_blocks_not_used[ARAD_NOF_CLP_SUB_BLOCKS * block_index + 2]);
                    soc_reg_field_set(unit, PORT_POWER_SAVEr, &val32, CPORT_COREf, clp_sub_blocks_not_used[ARAD_NOF_CLP_SUB_BLOCKS * block_index + 3]);
                    if (block_not_used[block_index]) {
                        SOC_INFO(unit).block_valid[nif_blocks[block_index]] = FALSE;
                    }
                }
            } else { 
                if (block_not_used[block_index]) {
                    soc_reg_field_set(unit, PORT_POWER_SAVEr, &val32, XPORT_CORE_0f, 1);
                    SOC_INFO(unit).block_valid[nif_blocks[block_index]] = FALSE;
                }
            }
            SOCDNX_IF_ERR_EXIT(WRITE_PORT_POWER_SAVEr(unit, port, val32));
        }
    }

    if (soc_property_suffix_num_get(unit, -1, spn_LOW_POWER, "fabric_mac", 1)) {
        
        
        int quad, quad_disabled, nof_disabled_quad = 0, nof_disabled_quad_before_this_3 = 0, disabling_3_quads = 0;
        uint64 val64;
        int blk;
        soc_info_t *si;
        soc_field_t one_quad_fmac_reset_fields[ARAD_MAX_NOF_ARAD_FABRIC_QUADS] = {
            FMAC_0_RESETf, FMAC_1_RESETf, FMAC_2_RESETf, FMAC_3_RESETf, FMAC_4_RESETf,
            FMAC_5_RESETf, FMAC_6_RESETf, FMAC_7_RESETf, FMAC_8_RESETf };
        soc_field_t three_quad_fmac_power_down_fields[ARAD_NOF_ARAD_FABRIC_3QUAD_FMACS] = {
            FMACA_POWER_DOWNf, FMACB_POWER_DOWNf, FMACC_POWER_DOWNf };
        COMPILER_64_ZERO(val64);
        si  = &SOC_INFO(unit);

        
        BCM_PBMP_ASSIGN(ports, SOC_PORT_DISABLED_BITMAP(unit, sfi));
        SOCDNX_IF_ERR_EXIT(READ_ECI_BLOCKS_SOFT_RESETr(unit, val_above64));
        for (quad = 0; quad < ARAD_NOF_ARAD_FABRIC_QUADS(unit); ++quad) {
            port = FABRIC_LOGICAL_PORT_BASE(unit) + quad * 4;
            quad_disabled = 1;
            for (i = 4; i; --i, ++port) {
                if (!(quad_disabled = quad_disabled && SOC_PBMP_MEMBER(ports, port))) {
                    break;
                }
            }
            if (quad_disabled) {
                ++nof_disabled_quad; 
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_RESETr, val_above64, one_quad_fmac_reset_fields[quad], 1);
                
                blk = si->fmac_block[quad];
                si->block_valid[blk] = 0;
            }
            if (quad % 3 == 2) { 
                if (nof_disabled_quad == nof_disabled_quad_before_this_3 + 3) {
                    if (!disabling_3_quads) {
                        disabling_3_quads = 1;
                        SOCDNX_IF_ERR_EXIT(READ_ECI_BLOCKS_POWER_DOWNr(unit, &val64)); 
                    }
                    soc_reg64_field32_set(unit, ECI_BLOCKS_POWER_DOWNr, &val64, three_quad_fmac_power_down_fields[quad / 3], 1);
                    
                    blk = si->fsrd_block[quad / 3];
                    si->block_valid[blk] = 0;
                }
                nof_disabled_quad_before_this_3 = nof_disabled_quad;
            }
        }
        
        if (nof_disabled_quad) {
            SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_RESETr(unit, val_above64));
            if (disabling_3_quads) {
                SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_POWER_DOWNr(unit, val64));
                if (nof_disabled_quad == ARAD_NOF_ARAD_FABRIC_QUADS(unit)) { 
                    SOCDNX_IF_ERR_EXIT(READ_ECI_SRD_0_PLL_CONFIGr(unit, val_above64));
                    soc_reg_above_64_field_set(unit, ECI_SRD_0_PLL_CONFIGr, val_above64, PWRDWNf, fld_value);
                    SOCDNX_IF_ERR_EXIT(WRITE_ECI_SRD_0_PLL_CONFIGr(unit, val_above64)); 
                }
            }
        }

    }

    if (soc_property_suffix_num_get(unit, -1, spn_LOW_POWER, "pll", 1)) {
        
        if (SOC_DPP_CONFIG(unit)->arad->init.pll.ts_clk_mode != 1) {
            SOCDNX_IF_ERR_EXIT(READ_ECI_TS_PLL_CONFIGr(unit, val_above64));
            soc_reg_above_64_field_set(unit, ECI_TS_PLL_CONFIGr, val_above64, PWRDWNf, fld_value);
            SOCDNX_IF_ERR_EXIT(WRITE_ECI_TS_PLL_CONFIGr(unit, val_above64));
            if (SOC_IS_ARADPLUS(unit) && (!SOC_IS_ARDON(unit))){
                SOCDNX_IF_ERR_EXIT(READ_ECI_BS_PLL_CONFIGr(unit, val_above64));
                soc_reg_above_64_field_set(unit, ECI_BS_PLL_CONFIGr, val_above64, I_PWRDWNf, fld_value);
                SOCDNX_IF_ERR_EXIT(WRITE_ECI_BS_PLL_CONFIGr(unit, val_above64));
            }
        }
    }
    
    if (!SOC_IS_ARADPLUS(unit)) {
        SOCDNX_IF_ERR_EXIT(READ_ECI_REG_0125r(unit, val_above64));
        soc_reg_above_64_field_set(unit, ECI_REG_0125r, val_above64, FIELD_148_148f, fld_value);
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_REG_0125r(unit, val_above64));
    }

    if (soc_property_suffix_num_get(unit, -1, spn_LOW_POWER, "dram", 1)) {
        char dram_plls_in_use[SOC_DPP_DEFS_MAX(HW_DRAM_INTERFACES_MAX) / 2] = {0};
        soc_reg_t dram_pll_regs[SOC_DPP_DEFS_MAX(HW_DRAM_INTERFACES_MAX) / 2] = {ECI_DDR_0_PLL_CONFIGr,
            ECI_DDR_1_PLL_CONFIGr, ECI_DDR_2_PLL_CONFIGr, ECI_DDR_3_PLL_CONFIGr};

        if (SOC_DPP_CONFIG(unit)->arad->init.dram.enable) { 
            for (i = 0; i < SOC_DPP_DEFS_GET(unit, hw_dram_interfaces_max); ++i) {
                if (SOC_DPP_CONFIG(unit)->arad->init.dram.is_valid[i]) {
                    dram_plls_in_use[i / 2] = 1;
                }
            }
        }

        
        for (i = 0; i < SOC_DPP_DEFS_GET(unit, hw_dram_interfaces_max) / 2; ++i) {
            if (dram_plls_in_use[i] == 0) {
                SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, dram_pll_regs[i], REG_PORT_ANY, 0, val_above64));
                soc_reg_above_64_field_set(unit, dram_pll_regs[i], val_above64, PWRDWNf, fld_value);
                SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, dram_pll_regs[i], REG_PORT_ANY, 0, val_above64));
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}


uint32
  arad_mgmt_init_sequence_phase2_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 silent
  )
{
  uint32
    res;
  uint32

    stage_id = 0,
    stage_internal_id = 0;


  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_MGMT_INIT_SEQUENCE_PHASE2_UNSAFE);

(void)stage_internal_id;
(void)stage_id;

  if (!SOC_UNIT_NUM_VALID(unit)) {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_ILLEGAL_DEVICE_ID, 10, exit);
  }

  if (!silent)
  {
   LOG_VERBOSE(BSL_LS_SOC_INIT,
               (BSL_META_U(unit,
                           "\n\r"
                           "   initialization: device %u"
                           "\n\r"),
                unit
                ));
  }

  ARAD_INIT_PRINT_INTERNAL_LEVEL_0_ADVANCE("Enable control cells");
  res = arad_mgmt_all_ctrl_cells_enable_set(
          unit,
          TRUE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);



  ARAD_INIT_PRINT_INTERNAL_LEVEL_0_ADVANCE("initialization done");

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_mgmt_init_sequence_phase2_unsafe()", 0, 0);
}




#if ARAD_DEBUG_IS_LVL1
#endif 

STATIC uint32
  arad_mgmt_ire_tbls_init(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 silent
  )
{
  uint32 res = SOC_SAND_OK;
  uint32 table_entry[3] = {0};

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_MGMT_IRE_TBLS_INIT);
  SOC_SAND_PCID_LITE_SKIP(unit);

  table_entry[2] = 0x40000; 
  res = arad_fill_table_with_entry(unit, IRE_TDM_CONFIGm, MEM_BLOCK_ANY, table_entry);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_mgmt_ire_tbls_init()",0,0);
}

int _arad_mgmt_irr_tbls_init_dma_callback(
    SOC_SAND_IN int unit,
    SOC_SAND_IN int copyno,
    SOC_SAND_IN int array_index,
    SOC_SAND_IN int index,
    SOC_SAND_OUT uint32 *value,
    SOC_SAND_IN int entry_sz,
    SOC_SAND_IN void *opaque)
{
  
  
  uint32 lag_size = 0;
  uint32 hash_index = 0;
  uint32 member;
  uint32 uindex = (uint32)index;
  soc_mem_t mem;

  *value = 0;
  SHR_BITCOPY_RANGE(&hash_index, 1, &uindex, 0, ARAD_IRR_GLAG_DEVISION_HASH_NOF_BITS - 1);
  SHR_BITCOPY_RANGE(&lag_size, 0, &uindex, ARAD_IRR_GLAG_DEVISION_HASH_NOF_BITS - 1, 4);

  mem = (SOC_IS_QAX(unit)) ?  TAR_SMOOTH_DIVISIONm : IRR_SMOOTH_DIVISIONm;

  member = ((hash_index >> 4) | (hash_index << 4)) % (lag_size+1);
  soc_mem_field32_set(unit, mem, value, MEMBER_0f, member);

  hash_index++;
  member = ((hash_index >> 4) | (hash_index << 4)) % (lag_size+1);
  soc_mem_field32_set(unit, mem, value, MEMBER_1f, member);

  return 0;
}

STATIC uint32
  arad_mgmt_irr_tbls_init(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 silent
  )
{
  uint32 res = SOC_SAND_OK;
  uint32 table_entry[2];
  soc_error_t new_rv;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_MGMT_IRR_TBLS_INIT);

  
  table_entry[0] = table_entry[1] = 0; 
  res = arad_fill_table_with_entry(unit, IDR_IRDBm, MEM_BLOCK_ANY, table_entry);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  
  SOC_SAND_SOC_IF_ERROR_RETURN(res,  20, exit, dpp_mult_rplct_tbl_entry_unoccupied_set_all(unit));
  SOC_SAND_SOC_IF_ERROR_RETURN(res,  30, exit, dpp_mcds_multicast_init2(unit));


  
  table_entry[0] = 0; 
  soc_mem_field32_set(unit, IRR_DESTINATION_TABLEm, table_entry, QUEUE_NUMBERf, 0x1ffff); 
  soc_mem_field32_set(unit, IRR_DESTINATION_TABLEm, table_entry, TC_PROFILEf, 0); 
  res = arad_fill_table_with_entry(unit, IRR_DESTINATION_TABLEm, MEM_BLOCK_ANY, table_entry); 
  SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);

  
  
  new_rv = arad_fill_table_with_variable_values_by_caching(unit, IRR_SMOOTH_DIVISIONm, 0, MEM_BLOCK_ANY, -1, -1,
                                                        _arad_mgmt_irr_tbls_init_dma_callback, NULL);
  if (new_rv != SOC_E_NONE) {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 112, exit);
  }

  
  table_entry[0] = 0; 
  soc_mem_field32_set(unit, IRR_SNOOP_MIRROR_DEST_TABLEm, table_entry, DESTINATIONf, 0x3ffff);
  res = arad_fill_table_with_entry(unit, IRR_SNOOP_MIRROR_DEST_TABLEm, MEM_BLOCK_ANY, table_entry); 
  SOC_SAND_CHECK_FUNC_RESULT(res, 121, exit);

  
  table_entry[0] = table_entry[1] = 0;
  res = arad_fill_table_with_entry(unit, IRR_SNOOP_MIRROR_TABLE_1m, MEM_BLOCK_ANY, table_entry); 
  SOC_SAND_CHECK_FUNC_RESULT(res, 122, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_mgmt_irr_tbls_init()",0,0);
}

uint32
  arad_mgmt_ihb_tbls_init(
    SOC_SAND_IN  int                 unit
  )
{
  uint32 idx;
  uint32 pmf_profile_data[2];
  uint32 res = SOC_SAND_OK;
  ARAD_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA
    ihb_fem0_4b_1st_pass_map_index_table_tbl_data;
  ARAD_IHB_FEM_MAP_TABLE_TBL_DATA
    ihb_fem0_4b_1st_pass_map_table_tbl_data;
    uint32
        fem_fes_ndx,
        table_line,
        table_entry[128] = {0},
        fld_val = 0,
        fes_tbl_data[(SOC_SAND_MAX(99, 430) / 32) + 1] = {0};

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_MGMT_IHB_TBLS_INIT);

  
  if (SOC_IS_JERICHO_PLUS(unit))
  {
      
      pmf_profile_data[0] = 0xE4FBEFBE;
      pmf_profile_data[1] = 0xBF;
  }
  else if (SOC_IS_JERICHO(unit))
  {
      pmf_profile_data[0] = 0x7FBEFBE;
  }
  else
  {
      pmf_profile_data[0] = 0xFBEFBE;
  }
  res = arad_fill_table_with_entry(unit, IHB_PMF_PASS_1_LOOKUPm, MEM_BLOCK_ANY, pmf_profile_data);
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
  res = arad_fill_table_with_entry(unit, IHB_PMF_PASS_2_LOOKUPm, MEM_BLOCK_ANY, pmf_profile_data);
  SOC_SAND_CHECK_FUNC_RESULT(res, 8, exit);



  res = soc_sand_os_memset(&ihb_fem0_4b_1st_pass_map_index_table_tbl_data, 0x0, sizeof(ihb_fem0_4b_1st_pass_map_index_table_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(res, 18, exit);

  ihb_fem0_4b_1st_pass_map_index_table_tbl_data.map_index = 0x3;
  ihb_fem0_4b_1st_pass_map_index_table_tbl_data.map_data = 0;
  ihb_fem0_4b_1st_pass_map_index_table_tbl_data.is_action_valid = 0;

  
  res = arad_ihb_fem_map_index_table_table_tbl_fill_unsafe(unit, &ihb_fem0_4b_1st_pass_map_index_table_tbl_data);
  SOC_SAND_CHECK_FUNC_RESULT(res, 152, exit);


  res = soc_sand_os_memset(&ihb_fem0_4b_1st_pass_map_table_tbl_data, 0x0, sizeof(ihb_fem0_4b_1st_pass_map_table_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(res, 19, exit);
  ihb_fem0_4b_1st_pass_map_table_tbl_data.action_type = 0x3c;
  for (idx = 0; idx < ARAD_IHB_FEM_MAX_OUTPUT_SIZE; idx++) {
      ihb_fem0_4b_1st_pass_map_table_tbl_data.field_select_map[idx] = 0x3e;
  }
  
  res = arad_ihb_fem_map_table_table_tbl_fill_unsafe(unit, &ihb_fem0_4b_1st_pass_map_table_tbl_data);
  SOC_SAND_CHECK_FUNC_RESULT(res, 152, exit);

  

  
  fld_val = 0;
  for (table_line = 0; table_line < arad_pmf_fem_action_type_array_size_get_unsafe(unit); ++table_line) {
      if (SOC_PPC_FP_ACTION_TYPE_NOP == arad_pmf_fem_action_type_array_element_get_unsafe(unit, table_line, INDEX_OF_ACTION)) {
          soc_mem_field32_set(unit, IHB_PMF_FES_PROGRAMm, &fld_val, FES_ACTION_TYPEf, arad_pmf_fem_action_type_array_element_get_unsafe(unit, table_line, INDEX_OF_HW_ID));
          break;
      }
  }

  for (fem_fes_ndx = 0; fem_fes_ndx < 32; fem_fes_ndx++) {
        soc_mem_field32_set(unit, IHB_PMF_FES_PROGRAMm, fes_tbl_data, Arad_pmf_fes_instruction_field[fem_fes_ndx], fld_val);
  }
  res = arad_fill_table_with_entry(unit, IHB_PMF_FES_PROGRAMm, MEM_BLOCK_ANY, fes_tbl_data); 
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);


  
  if (!SOC_IS_ARDON(unit)) {
      soc_mem_field32_set(unit, PPDB_A_TCAM_BANK_COMMANDm, table_entry, TCAM_CPU_CMD_WRf, SOC_IS_JERICHO(unit)? 0x3: 0x1);
      soc_mem_field32_set(unit, PPDB_A_TCAM_BANK_COMMANDm, table_entry, TCAM_CPU_CMD_VALIDf, 0x0);
      if (SOC_IS_JERICHO(unit) && !SOC_IS_QAX(unit)) {
          
          res = arad_fill_partial_table_with_entry(unit, PPDB_A_TCAM_BANK_COMMANDm, 0, 0, MEM_BLOCK_ANY,
                                                   2*1024*12 , 2*1024*14-1 , table_entry);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 47, exit);
      } else {
          res = arad_fill_table_with_entry(unit, PPDB_A_TCAM_BANK_COMMANDm, MEM_BLOCK_ANY, table_entry);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 47, exit);
      }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_mgmt_ihb_tbls_init()",0,0);
}

STATIC uint32
  arad_mgmt_iqm_tbls_init(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 silent
  )
{
  uint32
    res = SOC_SAND_OK,
    table_entry[1],
    cnred_table_entry[4] = {0};
  SOC_SAND_RET
    ret = SOC_SAND_OK;
  ARAD_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_TBL_DATA
    iqm_vsq_flow_control_parameters_table_group_tbl_data;
  ARAD_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_DATA
    iqm_packet_queue_red_parameters_table_tbl_data;
  ARAD_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_TBL_DATA
    iqm_vsq_queue_parameters_table_group_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_MGMT_IQM_TBLS_INIT);
  SOC_SAND_PCID_LITE_SKIP(unit);

  ret = soc_sand_os_memset(&iqm_packet_queue_red_parameters_table_tbl_data, 0x0, sizeof(iqm_packet_queue_red_parameters_table_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 19, exit);

  ret = soc_sand_os_memset(&iqm_vsq_queue_parameters_table_group_tbl_data, 0x0, sizeof(iqm_vsq_queue_parameters_table_group_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 36, exit);

  ret = soc_sand_os_memset(&iqm_vsq_flow_control_parameters_table_group_tbl_data, 0x0, sizeof(iqm_vsq_flow_control_parameters_table_group_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 37, exit);

  
  table_entry[0] = 0;
  soc_mem_field32_set(unit, IQM_PQWQm, table_entry, PQ_WEIGHTf, 2);
  soc_mem_field32_set(unit, IQM_PQWQm, table_entry, PQ_AVRG_ENf, 1); 
  res = arad_fill_table_with_entry(unit, IQM_PQWQm, MEM_BLOCK_ANY, table_entry); 
  SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);

  soc_mem_field32_set(unit, IQM_CNREDm, cnred_table_entry, PQ_MAX_QUE_SIZEf, 0x7FF);
  soc_mem_field32_set(unit, IQM_CNREDm, cnred_table_entry, PQ_MAX_QUE_BUFF_SIZEf, 0x7FF);
  soc_mem_field32_set(unit, IQM_CNREDm, cnred_table_entry, PQ_AVRG_MAX_THf, 0x7FF);
  res = arad_fill_table_with_entry(unit, IQM_CNREDm, MEM_BLOCK_ANY, cnred_table_entry); 
  SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);


  
  res = arad_iqm_packet_queue_red_parameters_table_tbl_fill_unsafe(unit, &iqm_packet_queue_red_parameters_table_tbl_data);
  SOC_SAND_CHECK_FUNC_RESULT(res, 152, exit);

  {

  res = arad_iqm_vsq_queue_parameters_table_group_tbl_fill_unsafe(unit, &iqm_vsq_queue_parameters_table_group_tbl_data);
         SOC_SAND_CHECK_FUNC_RESULT(res, 152, exit);

  iqm_vsq_flow_control_parameters_table_group_tbl_data.avrg_size_en = TRUE;
  res = arad_iqm_vsq_flow_control_parameters_table_group_tbl_fill_unsafe(unit, &iqm_vsq_flow_control_parameters_table_group_tbl_data);
                SOC_SAND_CHECK_FUNC_RESULT(res, 153, exit);
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_mgmt_iqm_tbls_init()",0,0);
}

uint32
  arad_mgmt_ips_tbls_init(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 silent
  )
{
  uint32 res = SOC_SAND_OK;
  uint32 table_entry[2] = {0};

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_MGMT_IPS_TBLS_INIT);
  SOC_SAND_PCID_LITE_SKIP(unit);

  

  
  
  if (ARAD_IS_VOQ_MAPPING_INDIRECT(unit)) { 

    
    
    soc_mem_field32_set(unit, IPS_QPM_1_SYS_REDm, table_entry, SYS_PHY_PORTf, SOC_TMC_MAX_SYSTEM_PHYSICAL_PORT_ID); 
    res = arad_fill_table_with_entry(unit, IPS_QPM_1_SYS_REDm, IPS_BLOCK(unit, SOC_CORE_ALL), table_entry); 
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    if (SOC_IS_JERICHO(unit)) {
        
        jer_mark_memory(unit, IPS_QPM_1_SYS_REDm);
    }

    table_entry[0] = 0; 
    soc_mem_field32_set(unit, IPS_QPM_2_SYS_REDm, table_entry, DEST_PORTf, ARAD_MAX_FAP_PORT_ID);
    soc_mem_field32_set(unit, IPS_QPM_2_SYS_REDm, table_entry, DEST_DEVf, ARAD_MAX_FAP_ID);
    if (SOC_IS_JERICHO(unit)) {
        
        jer_mark_memory(unit, IPS_QPM_2_SYS_REDm);
    }

  } else { 

    
    
    soc_mem_field32_set(unit, IPS_QPM_1_NO_SYS_REDm, table_entry, DEST_DEVf, ARAD_MAX_FAP_ID);
    soc_mem_field32_set(unit, IPS_QPM_1_NO_SYS_REDm, table_entry, DEST_PORT_MSBf, 1); 
    res = arad_fill_table_with_entry(unit, IPS_QPM_1_NO_SYS_REDm, IPS_BLOCK(unit, SOC_CORE_ALL), table_entry); 
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
    if (SOC_IS_JERICHO(unit)) {
        
        jer_mark_memory(unit, IPS_QPM_1_NO_SYS_REDm);
    }

    table_entry[0] = 0xfffffff; 

  }
  
  res = arad_fill_table_with_entry(unit, IPS_QPM_2_NO_SYS_REDm, IPS_BLOCK(unit, SOC_CORE_ALL), table_entry);
  SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
  if (SOC_IS_JERICHO(unit)) {
      
      jer_mark_memory(unit, IPS_QPM_2_NO_SYS_REDm);
  }
  
  table_entry[0] = ARAD_IPQ_INVALID_FLOW_QUARTET ;
  res = arad_fill_table_with_entry(unit, IPS_FLWIDm, IPS_BLOCK(unit, SOC_CORE_ALL), table_entry) ;
  SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);
  if (SOC_IS_JERICHO(unit)) {
      
      jer_mark_memory(unit, IPS_FLWIDm);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_mgmt_ips_tbls_init()",0,0);
}

uint32
  arad_mgmt_ipt_tbls_init(
      SOC_SAND_IN  int                 unit,
      SOC_SAND_IN  uint8                 silent
    )
{
    int i;
    uint32 res;

    uint32 fabric_priority;
    uint32 table_default[SOC_MAX_MEM_WORDS];

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_MGMT_IPT_TBLS_INIT);
    SOC_SAND_PCID_LITE_SKIP(unit);



    if (!SOC_UNIT_NUM_VALID(unit)) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_ILLEGAL_DEVICE_ID, 4, exit);
    }

    res = soc_sand_os_memset(&table_default, 0x0, sizeof(table_default));
    SOC_SAND_CHECK_FUNC_RESULT(res, 27, exit);

    
    for(i=0;i<ARAD_FBC_PRIORITY_NDX_NOF ; i++) {
        uint32 is_tdm, tc;
        
        
        is_tdm = (i & ARAD_FBC_PRIORITY_NDX_IS_TDM_MASK) >> ARAD_FBC_PRIORITY_NDX_IS_TDM_OFFSET;
        tc = (i & ARAD_FBC_PRIORITY_NDX_TC_MASK) >> ARAD_FBC_PRIORITY_NDX_TC_OFFSET;
        

        if(is_tdm) {
            fabric_priority = 3;
        } else {
            
            
            
            fabric_priority = tc/3;
            if ((!SOC_IS_JERICHO(unit)) && (SOC_DPP_CONFIG(unit)->tdm.is_tdm_over_primary_pipe) && (fabric_priority==2)) {
                
                fabric_priority = 1;
            }
        }
        if (SOC_IS_JERICHO(unit)) {
            res = WRITE_IPT_PRIORITY_BITS_MAPPING_2_FDTm(unit, MEM_BLOCK_ALL, i, &fabric_priority);
            SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 25, exit);
            res = WRITE_IPT_TDM_BIT_MAPPING_2_FDTm(unit, MEM_BLOCK_ALL, i, &is_tdm);
            SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 28, exit);
        } else {
            SHR_BITCOPY_RANGE(table_default, i*ARAD_FBC_PRIORITY_LENGTH, &fabric_priority, 0, ARAD_FBC_PRIORITY_LENGTH);
        }
    }
    if (!(SOC_IS_JERICHO(unit))) {
        res = WRITE_IPT_PRIORITY_BITS_MAPPING_2_FDTm(unit, MEM_BLOCK_ALL, 0, table_default);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_mgmt_ipt_tbls_init()",0,0);
}

uint32
  arad_mgmt_egq_tbls_init(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 silent
  )
{
  uint32 res = SOC_SAND_OK;
  uint32 table_entry[SOC_DPP_IMP_DEFS_MAX(EGQ_PPCT_NOF_LONGS)];
  soc_reg_above_64_val_t data;
  uint32 mtu_table_entry[4] = {0, 0, 0xFE000000, 0x7F}; 


  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_MGMT_EGQ_TBLS_INIT);

  sal_memset(table_entry, 0x0, sizeof(table_entry));

  if (SOC_DPP_CONFIG(unit)->emulation_system == 0) {

      table_entry[0] = 127; 
      res = arad_fill_table_with_entry(unit, EGQ_FQP_NIF_PORT_MUXm, MEM_BLOCK_ANY, table_entry);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      table_entry[0] = 32; 
      res = arad_fill_table_with_entry(unit, EGQ_PQP_NIF_PORT_MUXm, MEM_BLOCK_ANY, table_entry);
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

      res = arad_fill_table_with_entry(unit, EGQ_PP_PPCTm, MEM_BLOCK_ANY, &mtu_table_entry);
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }

  SOC_REG_ABOVE_64_CLEAR(data);
  soc_mem_field32_set(unit, EGQ_PPCTm, data, LB_KEY_MAXf, 0xff);
  soc_mem_field32_set(unit, EGQ_PPCTm, data, CGM_PORT_PROFILEf, ARAD_EGR_PORT_THRESH_TYPE_15);
  res = arad_fill_table_with_entry(unit, EGQ_PPCTm, MEM_BLOCK_ANY, data); 
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  SOC_REG_ABOVE_64_CLEAR(data);
  soc_mem_field32_set(unit, EGQ_PCTm, data, CGM_PORT_PROFILEf, ARAD_EGR_PORT_THRESH_TYPE_15);
  res = arad_fill_table_with_entry(unit, EGQ_PCTm, MEM_BLOCK_ANY, data); 
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

  SOC_REG_ABOVE_64_CLEAR(data);
  soc_mem_field32_set(unit, EGQ_DSP_PTR_MAPm, data, OUT_TM_PORTf, ARAD_EGR_INVALID_BASE_Q_PAIR);
  res  = arad_fill_table_with_entry(unit, EGQ_DSP_PTR_MAPm, MEM_BLOCK_ANY, data);
  SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_mgmt_egq_tbls_init()",0,0);
}

uint32
  arad_mgmt_epni_tbls_init(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 silent
  )
{
  uint32 res = SOC_SAND_OK;
  uint32 mtu_table_entry[4] = {0, 0, 0xFE000000, 0x7F}; 
  uint32 tx_tag_eable_entry[8] = {0};

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_MGMT_EPNI_TBLS_INIT);

  res = arad_pp_eg_mirror_init_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_fill_table_with_entry(unit, EPNI_PACKET_PROCESSING_PORT_CONFIGURATION_TABLE_PP_PCTm, MEM_BLOCK_ANY, &mtu_table_entry);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = soc_sand_bitstream_fill(tx_tag_eable_entry, 8);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  if (!SOC_IS_ARDON(unit)){
      res = arad_fill_table_with_entry(unit, EPNI_TX_TAG_TABLEm, MEM_BLOCK_ANY, &tx_tag_eable_entry);
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_mgmt_epni_tbls_init()",0,0);
}


STATIC uint32
  arad_mgmt_sch_tbls_init(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 silent
  )
{
  uint32
    table_entry[2] = {0},
    res = SOC_SAND_OK;
  ARAD_SCH_SCHEDULER_INIT_TBL_DATA
  sch_scheduler_init_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_MGMT_SCH_TBLS_INIT);
  SOC_SAND_PCID_LITE_SKIP(unit);

  res = arad_fill_table_with_entry(unit, SCH_SCHEDULER_ENABLE_MEMORY_SEMm, SCH_BLOCK(unit, SOC_CORE_ALL), table_entry);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (SOC_IS_JERICHO(unit) && !SOC_IS_QUX(unit))
  {
      res = arad_fill_table_with_entry(unit, SCH_SCHEDULER_ENABLE_MEMORY_SEM_Bm, SCH_BLOCK(unit, SOC_CORE_ALL), table_entry);
      SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);
  }

  
  
  res = arad_fill_table_with_entry(unit, SCH_HR_SCHEDULER_CONFIGURATION_SHCm, SCH_BLOCK(unit, SOC_CORE_ALL), table_entry);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  
  
  res = arad_fill_table_with_entry(unit, SCH_CL_SCHEDULERS_TYPE__SCTm, SCH_BLOCK(unit, SOC_CORE_ALL), table_entry);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  
  
  res = arad_fill_table_with_entry(unit, SCH_FLOW_GROUP_MEMORY_FGMm, SCH_BLOCK(unit, SOC_CORE_ALL), table_entry);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  
  soc_mem_field32_set(unit, SCH_DSP_2_PORT_MAP_DSPPm, table_entry, DSP_2_PORT_MAP_DSPPf, ARAD_EGR_INVALID_BASE_Q_PAIR);
  res = arad_fill_table_with_entry(unit, SCH_DSP_2_PORT_MAP_DSPPm, SCH_BLOCK(unit, SOC_CORE_ALL), table_entry);
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  soc_mem_field32_set(unit, SCH_DSP_2_PORT_MAP_DSPPm, table_entry, DSP_2_PORT_MAP_DSPPf, 0);

  
  sch_scheduler_init_tbl_data.schinit = 0x1;

  {
      res = arad_sch_scheduler_init_tbl_set_unsafe(
            unit,
            0,
            &sch_scheduler_init_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 132, exit);

  }

  
  
  soc_mem_field32_set(unit, SCH_FLOW_TO_FIP_MAPPING__FFMm, table_entry, DEVICE_NUMBERf, SOC_TMC_MAX_FAP_ID);
  res = arad_fill_table_with_entry(unit, SCH_FLOW_TO_FIP_MAPPING__FFMm, MEM_BLOCK_ANY, table_entry);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  soc_mem_field32_set(unit, SCH_FLOW_TO_FIP_MAPPING__FFMm, table_entry, DEVICE_NUMBERf, 0);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_mgmt_sch_tbls_init()",0,0);
}

uint32
  arad_mgmt_tbls_init(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 silent
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_MGMT_TBLS_INIT);



  res = arad_mgmt_sch_tbls_init(unit, silent);
  SOC_SAND_CHECK_FUNC_RESULT(res, 28, exit);

  res = arad_mgmt_irr_tbls_init(unit, silent);
  SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit);

  res = arad_mgmt_ire_tbls_init(unit, silent);
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

  res = arad_mgmt_iqm_tbls_init(unit, silent);
  SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);

  res = arad_mgmt_ips_tbls_init(unit, silent);
  SOC_SAND_CHECK_FUNC_RESULT(res, 19, exit);

  res = arad_mgmt_ipt_tbls_init(unit, silent);
  SOC_SAND_CHECK_FUNC_RESULT(res, 21, exit);

  res = arad_mgmt_egq_tbls_init(unit, silent);
  SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);

  res = arad_mgmt_epni_tbls_init(unit, silent);
  SOC_SAND_CHECK_FUNC_RESULT(res, 26, exit);


  ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_mgmt_tbls_init()", 0, 0);
}

STATIC
uint32
  arad_mgmt_all_tbls_init_enable_dynamic(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 enable
  )
{
  soc_reg_t
    enable_dynamic_memory_access[11] = {
      CRPS_ENABLE_DYNAMIC_MEMORY_ACCESSr, EGQ_ENABLE_DYNAMIC_MEMORY_ACCESSr,
      EPNI_ENABLE_DYNAMIC_MEMORY_ACCESSr, IDR_ENABLE_DYNAMIC_MEMORY_ACCESSr,
      IHB_ENABLE_DYNAMIC_MEMORY_ACCESSr, IPS_ENABLE_DYNAMIC_MEMORY_ACCESSr,
       IPT_ENABLE_DYNAMIC_MEMORY_ACCESSr, IQM_ENABLE_DYNAMIC_MEMORY_ACCESSr,
       IRE_ENABLE_DYNAMIC_MEMORY_ACCESSr, IRR_ENABLE_DYNAMIC_MEMORY_ACCESSr,
       OLP_ENABLE_DYNAMIC_MEMORY_ACCESSr
      };
  uint32
    res,
    memory_access_id = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  for(memory_access_id = 0; memory_access_id < 11; memory_access_id++)
  {
      if (SOC_IS_ARDON(unit) && enable_dynamic_memory_access[memory_access_id] == OLP_ENABLE_DYNAMIC_MEMORY_ACCESSr) {
          
          continue;
      }
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10+memory_access_id,  exit, ARAD_REG_ACCESS_ERR,soc_reg32_set(unit, enable_dynamic_memory_access[memory_access_id], REG_PORT_ANY,  0,  enable));
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_mgmt_all_tbls_init_enable_dynamic()",0,0);
}




uint32
  arad_mgmt_all_tbls_init(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 silent
  )
{
  uint32 res, errnum;
  uint32 table_entry[128] = {0};
  uint32 vsi_low_cfg[1] = {0};
  soc_mem_t mem_lcl;
  soc_error_t new_rv;


  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  new_rv = arad_tbl_access_init_unsafe(unit);
  if (new_rv != SOC_E_NONE) {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 55555, exit);
  }

  res = arad_mgmt_all_tbls_init_enable_dynamic(unit, 1);
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);


  errnum = 1000; 
  if (SOC_DPP_CONFIG(unit)->emulation_system == 0) {

      errnum = 2000; 
      res = arad_fill_table_with_entry(unit, CFC_CAT_2_TC_MAP_HCFCm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, CFC_CAT_2_TC_MAP_NIFm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, CFC_ILKN_RX_0_CALm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, CFC_ILKN_RX_1_CALm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, CFC_ILKN_TX_0_CALm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, CFC_ILKN_TX_1_CALm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, CFC_NIF_PFC_MAPm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, CFC_RCL_VSQ_MAPm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, CFC_SPI_OOB_RX_0_CALm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, CFC_SPI_OOB_RX_1_CALm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, CFC_SPI_OOB_TX_0_CALm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, CFC_SPI_OOB_TX_1_CALm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      if (SOC_IS_ARDON(unit)) {
          res = arad_fill_table_with_entry(unit, CFC_NIF_SHR_MAPm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      }

      errnum = 3000; 
      res = arad_fill_table_with_entry(unit, CRPS_CRPS_0_CNTS_MEMm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, CRPS_CRPS_0_OVTH_MEMm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, CRPS_CRPS_1_CNTS_MEMm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, CRPS_CRPS_1_OVTH_MEMm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, CRPS_CRPS_2_CNTS_MEMm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, CRPS_CRPS_2_OVTH_MEMm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, CRPS_CRPS_3_CNTS_MEMm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, CRPS_CRPS_3_OVTH_MEMm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, CRPS_MEM_0080000m, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, CRPS_MEM_0090000m, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, CRPS_MEM_00A0000m, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, CRPS_MEM_00B0000m, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);

#ifdef BCM_88660_A0
      if (SOC_IS_ARADPLUS(unit) && !SOC_IS_ARDON(unit)) {
          res = arad_fill_table_with_entry(unit, CRPS_IRPP_OFFSET_BMAP_Am, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, CRPS_IRPP_OFFSET_BMAP_Bm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, CRPS_EGQ_OFFSET_BMAPm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, CRPS_EPNI_OFFSET_BMAP_Am, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, CRPS_EPNI_OFFSET_BMAP_Bm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, CRPS_IQM_OFFSET_BMAP_Am, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, CRPS_IQM_OFFSET_BMAP_Bm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, CRPS_IQM_OFFSET_BMAP_Cm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, CRPS_IQM_OFFSET_BMAP_Dm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      } else
#endif
      {
          res = arad_fill_table_with_entry(unit, CRPS_MEM_00C0000m, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, CRPS_MEM_00D0000m, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, CRPS_MEM_00E0000m, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, CRPS_MEM_00F0000m, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, CRPS_MEM_0100000m, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, CRPS_MEM_0110000m, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, CRPS_MEM_0120000m, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, CRPS_MEM_0130000m, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, CRPS_MEM_0140000m, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
    }

      errnum = 5000; 
      res = arad_fill_table_with_entry(unit, EGQ_ACTION_PROFILE_TABLEm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EGQ_CBMm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EGQ_CCMm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EGQ_CH_0_SCMm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EGQ_CH_1_SCMm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EGQ_CH_2_SCMm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EGQ_CH_3_SCMm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EGQ_CH_4_SCMm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EGQ_CH_5_SCMm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EGQ_CH_6_SCMm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EGQ_CH_7_SCMm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EGQ_CH_8_SCMm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EGQ_CH_9_SCMm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EGQ_CNM_QUANTA_TO_FC_MAPm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EGQ_DCMm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EGQ_DSP_PTR_MAPm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      if (SOC_IS_JERICHO(unit)) {
          res = arad_fill_table_with_entry(unit, EGQ_DSP_PTR_MAPm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      }
      res = arad_fill_table_with_entry(unit, EGQ_DWMm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EGQ_DWM_8Pm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EGQ_EPS_PRIO_MAPm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EGQ_FBMm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EGQ_FDMm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EGQ_HEADER_MAPm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EGQ_MAP_OUTLIF_TO_DSPm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EGQ_MAP_PS_TO_IFCm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EGQ_MC_SP_TC_MAPm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EGQ_NONCH_SCMm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EGQ_PCTm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EGQ_PDCMm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EGQ_PDCMAXm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EGQ_PDCT_TABLEm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EGQ_PMCm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EGQ_PMF_KEY_GEN_LSBm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EGQ_PMF_KEY_GEN_MSBm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EGQ_PMF_PROGRAM_SELECTION_CAMm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EGQ_PPCTm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EGQ_PP_PPCTm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EGQ_PQSMm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EGQ_PQSMAXm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EGQ_PQST_TABLEm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EGQ_QDCMm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EGQ_QDCMAXm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EGQ_QDCT_TABLEm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EGQ_QM_0m, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EGQ_QM_1m, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EGQ_QM_2m, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EGQ_QM_3m, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EGQ_QP_CBMm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EGQ_QP_PMCm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EGQ_QP_SCMm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EGQ_QQSMm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EGQ_QQSMAXm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EGQ_QQST_TABLEm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EGQ_RCMMCm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EGQ_RCMUCm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EGQ_RPDMm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EGQ_RRDMm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EGQ_TCG_CBMm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EGQ_TCG_PMCm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EGQ_TCG_SCMm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EGQ_TC_DP_MAPm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EGQ_TTL_SCOPEm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EGQ_VLAN_TABLEm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      if (!SOC_IS_ARDON(unit)) {
          res = arad_fill_table_with_entry(unit, EGQ_VSI_MEMBERSHIPm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, EGQ_AUX_TABLEm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      }
      res = arad_fill_table_with_entry(unit, EGQ_VSI_PROFILEm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
  }
      res = arad_fill_table_with_entry(unit, EGQ_IVEC_TABLEm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);

  if (SOC_DPP_CONFIG(unit)->emulation_system == 0) {
      errnum = 6000; 
      res = arad_fill_table_with_entry(unit, EPNI_ACE_TABLEm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EPNI_ACE_TO_FHEIm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EPNI_ACE_TO_OUT_LIFm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EPNI_ACE_TO_OUT_PP_PORTm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EPNI_COUNTER_SOURCE_MAPm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EPNI_DSCP_EXP_TO_PCP_DEIm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      
      if (SOC_IS_JERICHO_PLUS(unit)) {
          res = arad_fill_table_with_entry(unit, EPNI_NATIVE_DSCP_EXP_TO_PCP_DEIm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      }
  if (!SOC_IS_ARDON(unit)) {
      res = arad_fill_table_with_entry(unit, EDB_ESEM_MANAGEMENT_REQUESTm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EDB_ESEM_STEP_TABLEm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
  }
      res = arad_fill_table_with_entry(unit, EPNI_ETH_OAM_OPCODE_MAPm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EPNI_EVEC_TABLEm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      
      if (SOC_IS_JERICHO_PLUS(unit)) {
          res = arad_fill_table_with_entry(unit, EPNI_NATIVE_EVEC_TABLEm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      }
      res = arad_fill_table_with_entry(unit, EPNI_EXP_REMARKm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EPNI_HEADER_MAPm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EPNI_LFEM_FIELD_SELECT_MAPm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EPNI_LINK_LAYER_VLAN_PROCESSING_LLVPm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
  }
  errnum += 10;
  res = arad_fill_table_with_entry(unit, EPNI_IVEC_TABLEm, MEM_BLOCK_ANY, table_entry);
  SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);

  if ((SOC_DPP_CONFIG(unit)->emulation_system == 0) && (!SOC_IS_ARDON(unit))) {

      res = arad_fill_table_with_entry(unit, EPNI_MEM_520000m, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EPNI_MEM_5A0000m, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EPNI_MEM_5B0000m, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EPNI_MEM_630000m, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EPNI_MEM_640000m, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EPNI_MEM_6C0000m, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EPNI_MEM_6D0000m, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EPNI_MEM_760000m, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EPNI_MEM_770000m, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
  }
  if (SOC_DPP_CONFIG(unit)->emulation_system == 0) {

      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EPNI_MIRROR_PROFILE_MAPm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EPNI_MIRROR_PROFILE_TABLEm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EPNI_MPLS_CMD_PROFILEm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EPNI_MY_CFM_MAC_TABLEm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EPNI_PCP_DEI_TABLEm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      
      if (SOC_IS_JERICHO_PLUS(unit)) {
          res = arad_fill_table_with_entry(unit, EPNI_NATIVE_PCP_DEI_TABLEm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      }
      res = arad_fill_table_with_entry(unit, EPNI_PP_COUNTER_TABLEm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EPNI_PACKET_PROCESSING_PORT_CONFIGURATION_TABLE_PP_PCTm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EPNI_PRGE_DATAm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EPNI_PRGE_INSTRUCTION_0m, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EPNI_PRGE_INSTRUCTION_1m, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EPNI_PRGE_INSTRUCTION_10m, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EPNI_PRGE_INSTRUCTION_11m, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EPNI_PRGE_INSTRUCTION_2m, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EPNI_PRGE_INSTRUCTION_3m, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EPNI_PRGE_INSTRUCTION_4m, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EPNI_PRGE_INSTRUCTION_5m, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EPNI_PRGE_INSTRUCTION_6m, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EPNI_PRGE_INSTRUCTION_7m, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EPNI_PRGE_INSTRUCTION_8m, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EPNI_PRGE_INSTRUCTION_9m, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EPNI_PRGE_PROGRAMm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EPNI_REMARK_MPLS_TO_DSCPm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EPNI_REMARK_MPLS_TO_EXPm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, EPNI_SPANNING_TREE_PROTOCOL_STATE_MEMORY_STPm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      if (!SOC_IS_ARDON(unit)) {
          res = arad_fill_table_with_entry(unit, EPNI_DSCP_REMARKm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, EPNI_EEDB_BANKm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, EPNI_ISID_TABLEm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, EPNI_REMARK_IPV4_TO_DSCPm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, EPNI_REMARK_IPV4_TO_EXPm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, EPNI_REMARK_IPV6_TO_DSCPm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, EPNI_REMARK_IPV6_TO_EXPm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, EPNI_TX_TAG_TABLEm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      }
  }

  errnum = 9000; 
  res = arad_fill_table_with_entry(unit, FDT_IN_BAND_MEMm, MEM_BLOCK_ANY, table_entry);
  errnum += 10;
  SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
  res = arad_fill_table_with_entry(unit, FDT_IRE_TDM_MASKSm, MEM_BLOCK_ANY, table_entry);
  errnum += 10;
  if (SOC_DPP_CONFIG(unit)->emulation_system == 0) {

      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, FDT_MEM_100000m, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
  }

  if (SOC_DPP_CONFIG(unit)->emulation_system == 0) {

      errnum = 11000; 
      res = arad_fill_table_with_entry(unit, FSRD_FSRD_WL_EXT_MEMm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);

      errnum = 12000; 
      res = arad_fill_table_with_entry(unit, IDR_CONTEXT_MRUm, MEM_BLOCK_ANY, table_entry); 
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IDR_DROP_PRECEDENCE_MAPPINGm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IDR_ETHERNET_METER_CONFIGm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IDR_ETHERNET_METER_PROFILESm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IDR_GLOBAL_METER_PROFILESm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IDR_GLOBAL_METER_STATUSm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      if (!SOC_IS_ARDON(unit)) {
          res = arad_fill_table_with_entry(unit, IDR_CONTEXT_COLORm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IDR_CONTEXT_SIZEm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IDR_MCDA_DYNAMICm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IDR_MCDA_PRFCFG_0m, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IDR_MCDA_PRFSELm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      }
  }
  if (!SOC_IS_ARDON(unit)) {
      if (SOC_DPP_CONFIG(unit)->emulation_system == 0) {
          res = arad_fill_table_with_entry(unit, IDR_MCDB_DYNAMICm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      }
      res = arad_fill_table_with_entry(unit, IDR_MCDB_PRFCFG_0m, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IDR_MCDB_PRFSELm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
  }
  if (SOC_DPP_CONFIG(unit)->emulation_system == 0) {
      res = arad_fill_table_with_entry(unit, IDR_MEM_00000m, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
  }

  res = arad_fill_table_with_entry(unit, IDR_MEM_10000m, MEM_BLOCK_ANY, table_entry);
  errnum += 10;
  SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);

  if (SOC_DPP_CONFIG(unit)->emulation_system == 0) {

      res = arad_fill_table_with_entry(unit, IDR_MEM_180000m, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IDR_MEM_1B0000m, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IDR_MEM_1F0000m, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IDR_MEM_30000m, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IDR_MEM_40000m, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IDR_MEM_50000m, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IDR_MEM_60000m, MEM_BLOCK_ANY, table_entry);
  }
  if (SOC_DPP_CONFIG(unit)->emulation_system == 0) {

      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IDR_OCB_BUFFER_TYPEm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);

      errnum = 13000; 
      res = arad_fill_table_with_entry(unit, IHB_CPU_TRAP_CODE_CTRm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);

      if (!SOC_IS_ARDON(unit)) {
          res = arad_fill_table_with_entry(unit, IHB_DESTINATION_STATUSm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHP_DSCP_EXP_MAPm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHP_DSCP_EXP_REMARKm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHP_ETHERNET_OAM_OPCODE_MAPm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHB_FEC_ECMPm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHB_FEC_ENTRYm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHB_FEC_ENTRY_ACCESSEDm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, SOC_IS_ARADPLUS_AND_BELOW(unit)?IHB_FEC_SUPER_ENTRYm:PPDB_A_FEC_SUPER_ENTRY_BANKm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      }
  }
  res = arad_fill_table_with_entry(unit, IHB_FEM_BIT_SELECTm, MEM_BLOCK_ANY, table_entry);
  errnum += 10;
  SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
  if (SOC_DPP_CONFIG(unit)->emulation_system == 0) {
      res = arad_fill_table_with_entry(unit, IHB_FWD_ACT_PROFILEm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IHB_HEADER_PROFILEm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IHB_IN_PORT_KEY_GEN_VARm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IHB_LB_PFC_PROFILEm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IHB_LB_VECTOR_PROGRAM_MAPm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      if (!SOC_IS_ARDON(unit)) {
          res = arad_fill_table_with_entry(unit, IHB_FLP_KEY_CONSTRUCTIONm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHP_FLP_LOOKUPSm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHB_FLP_PROCESSm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHP_FLP_PROGRAM_KEY_GEN_VARm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
#ifdef BCM_88675_SUPPORT
          if(SOC_IS_JERICHO(unit)) {
              res = arad_fill_table_with_entry(unit, IHP_FLP_PROGRAM_SELECTION_CAMm, MEM_BLOCK_ANY, table_entry);
          } else
#endif
          {
              res = arad_fill_table_with_entry(unit, IHB_FLP_PROGRAM_SELECTION_CAMm, MEM_BLOCK_ANY, table_entry);
          }
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHP_FLP_PTC_PROGRAM_SELECTm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHP_IEEE_1588_ACTIONm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHP_IEEE_1588_IDENTIFICATION_CAMm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHB_LPMm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHB_LPM_2m, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHB_LPM_3m, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHB_LPM_4m, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHB_LPM_5m, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHB_LPM_6m, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHB_MEM_1040000m, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHB_MEM_1050000m, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHB_MEM_10E0000m, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHB_MEM_10F0000m, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHB_MEM_14A0000m, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHB_MEM_1520000m, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHB_MEM_1530000m, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHB_MEM_15B0000m, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHB_MEM_15C0000m, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHB_MEM_1640000m, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHB_MEM_1650000m, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHB_MEM_16E0000m, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHB_MEM_16F0000m, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHB_MEM_250000m, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          if(SOC_IS_ARADPLUS_AND_BELOW(unit)) {
              res = arad_fill_table_with_entry(unit, IHB_MEM_260000m, MEM_BLOCK_ANY, table_entry);
              errnum += 10;
              SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
              res = arad_fill_table_with_entry(unit, IHB_MEM_270000m, MEM_BLOCK_ANY, table_entry);
              errnum += 10;
              SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          }
          res = arad_fill_table_with_entry(unit, IHB_MEM_E00000m, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHB_MEM_E10000m, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHB_MEM_EA0000m, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHB_MEM_F20000m, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHB_MEM_F30000m, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHB_MEM_FB0000m, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHB_MEM_FC0000m, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
      }
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IHB_MRR_ACT_PROFILEm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IHB_OAM_COUNTER_FIFOm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      if (!SOC_IS_ARDON(unit)) {
          res = arad_fill_table_with_entry(unit, ARAD_PP_FEC_PATH_SELECT_MEM(unit), MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      }
      res = arad_fill_table_with_entry(unit, IHB_L_4_OPSm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IHB_PFC_INFOm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IHB_PINFO_COUNTERSm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IHB_PINFO_FERm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IHB_PINFO_LBPm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IHB_PINFO_PMFm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IHB_PMF_FEM_PROGRAMm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IHB_PMF_INITIAL_KEY_2ND_PASSm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IHB_PMF_PASS_1_KEY_GEN_LSBm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IHB_PMF_PASS_1_KEY_GEN_MSBm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IHB_PMF_PASS_1_LOOKUPm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IHB_PMF_PASS_2_KEY_GEN_LSBm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IHB_PMF_PASS_2_KEY_GEN_MSBm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IHB_PMF_PASS_2_KEY_UPDATEm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IHB_PMF_PASS_2_LOOKUPm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IHB_PMF_PROGRAM_COUNTERSm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IHB_PMF_PROGRAM_GENERALm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IHB_PROGRAM_KEY_GEN_VARm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IHB_PTC_INFO_PMFm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IHB_PTC_KEY_GEN_VARm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IHB_SNOOP_ACTIONm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IHB_SNP_ACT_PROFILEm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, PPDB_A_TCAM_ACCESS_PROFILEm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, PPDB_A_TCAM_ACTIONm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      if (!SOC_IS_ARDON(unit)) {
          res = arad_fill_table_with_entry(unit, IHP_MY_BFD_DIPm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHB_OAMAm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHB_OAMBm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHP_OAM_CHANNEL_TYPEm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHP_OAM_MY_CFM_MACm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, PPDB_A_OEMA_MANAGEMENT_REQUESTm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, PPDB_A_OEMA_STEP_TABLEm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, PPDB_A_OEMB_MANAGEMENT_REQUESTm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, PPDB_A_OEMB_STEP_TABLEm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHB_OPCODE_MAP_RXm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHB_OPCODE_MAP_TXm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          if(SOC_IS_JERICHO(unit)) {
              res = arad_fill_table_with_entry(unit, IHP_PINFO_FLP_0m, MEM_BLOCK_ANY, table_entry);
              errnum += 10;
              SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
              res = arad_fill_table_with_entry(unit, IHP_PINFO_FLP_1m, MEM_BLOCK_ANY, table_entry);
              errnum += 10;
              SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          } else {
              res = arad_fill_table_with_entry(unit, IHB_PINFO_FLPm, MEM_BLOCK_ANY, table_entry);
              errnum += 10;
              SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          }
          if(SOC_IS_ARADPLUS_AND_BELOW(unit)) {
              res = arad_fill_table_with_entry(unit, IHB_TCAM_ACTION_24m, MEM_BLOCK_ANY, table_entry);
              errnum += 10;
              SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
              res = arad_fill_table_with_entry(unit, IHB_TCAM_ACTION_25m, MEM_BLOCK_ANY, table_entry);
              errnum += 10;
              SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
              res = arad_fill_table_with_entry(unit, IHB_TCAM_ACTION_26m, MEM_BLOCK_ANY, table_entry);
              errnum += 10;
              SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
              res = arad_fill_table_with_entry(unit, IHB_TCAM_ACTION_27m, MEM_BLOCK_ANY, table_entry);
              errnum += 10;
              SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          } else {
#ifdef BCM_88675_SUPPORT
              if(SOC_IS_JERICHO(unit)) {
                  res = arad_fill_table_with_entry(unit, PPDB_A_TCAM_ACTION_SMALL_24m, MEM_BLOCK_ANY, table_entry);
                  errnum += 10;
                  SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
                  res = arad_fill_table_with_entry(unit, PPDB_A_TCAM_ACTION_SMALL_25m, MEM_BLOCK_ANY, table_entry);
                  errnum += 10;
                  SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
                  res = arad_fill_table_with_entry(unit, PPDB_A_TCAM_ACTION_SMALL_26m, MEM_BLOCK_ANY, table_entry);
                  errnum += 10;
                  SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
                  res = arad_fill_table_with_entry(unit, PPDB_A_TCAM_ACTION_SMALL_27m, MEM_BLOCK_ANY, table_entry);
                  errnum += 10;
                  SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
                  res = arad_fill_table_with_entry(unit, PPDB_A_TCAM_ACTION_SMALL_28m, MEM_BLOCK_ANY, table_entry);
                  errnum += 10;
                  SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
                  res = arad_fill_table_with_entry(unit, PPDB_A_TCAM_ACTION_SMALL_29m, MEM_BLOCK_ANY, table_entry);
                  errnum += 10;
                  SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
                  res = arad_fill_table_with_entry(unit, PPDB_A_TCAM_ACTION_SMALL_30m, MEM_BLOCK_ANY, table_entry);
                  errnum += 10;
                  SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
                  res = arad_fill_table_with_entry(unit, PPDB_A_TCAM_ACTION_SMALL_31m, MEM_BLOCK_ANY, table_entry);
                  errnum += 10;
                  SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
              }
#endif
          }
          res = arad_fill_table_with_entry(unit, PPDB_A_TCAM_ACTION_HIT_INDICATIONm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          if(SOC_IS_ARADPLUS_AND_BELOW(unit)) {
              res = arad_fill_table_with_entry(unit, IHB_TCAM_ACTION_HIT_INDICATION_24m, MEM_BLOCK_ANY, table_entry);
              errnum += 10;
              SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
              res = arad_fill_table_with_entry(unit, IHB_TCAM_ACTION_HIT_INDICATION_25m, MEM_BLOCK_ANY, table_entry);
              errnum += 10;
              SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
              res = arad_fill_table_with_entry(unit, IHB_TCAM_ACTION_HIT_INDICATION_26m, MEM_BLOCK_ANY, table_entry);
              errnum += 10;
              SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
              res = arad_fill_table_with_entry(unit, IHB_TCAM_ACTION_HIT_INDICATION_27m, MEM_BLOCK_ANY, table_entry);
              errnum += 10;
              SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          } else {
#ifdef BCM_88675_SUPPORT
              if(SOC_IS_JERICHO(unit)) {
                  res = arad_fill_table_with_entry(unit, PPDB_A_TCAM_ACTION_HIT_INDICATION_SMALL_24m, MEM_BLOCK_ANY, table_entry);
                  errnum += 10;
                  SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
                  res = arad_fill_table_with_entry(unit, PPDB_A_TCAM_ACTION_HIT_INDICATION_SMALL_25m, MEM_BLOCK_ANY, table_entry);
                  errnum += 10;
                  SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
                  res = arad_fill_table_with_entry(unit, PPDB_A_TCAM_ACTION_HIT_INDICATION_SMALL_26m, MEM_BLOCK_ANY, table_entry);
                  errnum += 10;
                  SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
                  res = arad_fill_table_with_entry(unit, PPDB_A_TCAM_ACTION_HIT_INDICATION_SMALL_27m, MEM_BLOCK_ANY, table_entry);
                  errnum += 10;
                  SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
                  res = arad_fill_table_with_entry(unit, PPDB_A_TCAM_ACTION_HIT_INDICATION_SMALL_28m, MEM_BLOCK_ANY, table_entry);
                  errnum += 10;
                  SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
                  res = arad_fill_table_with_entry(unit, PPDB_A_TCAM_ACTION_HIT_INDICATION_SMALL_29m, MEM_BLOCK_ANY, table_entry);
                  errnum += 10;
                  SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
                  res = arad_fill_table_with_entry(unit, PPDB_A_TCAM_ACTION_HIT_INDICATION_SMALL_30m, MEM_BLOCK_ANY, table_entry);
                  errnum += 10;
                  SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
                  res = arad_fill_table_with_entry(unit, PPDB_A_TCAM_ACTION_HIT_INDICATION_SMALL_31m, MEM_BLOCK_ANY, table_entry);
                  errnum += 10;
                  SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
              }
#endif
          }
          if(SOC_IS_ARADPLUS_AND_BELOW(unit)) {
              res = arad_fill_table_with_entry(unit, IHB_TCAM_ENTRY_PARITY_12m, MEM_BLOCK_ANY, table_entry);
              errnum += 10;
              SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
              res = arad_fill_table_with_entry(unit, IHB_TCAM_ENTRY_PARITY_13m, MEM_BLOCK_ANY, table_entry);
              errnum += 10;
              SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          } else {
#ifdef BCM_88675_SUPPORT
              if(SOC_IS_JERICHO(unit)) {
                  res = arad_fill_table_with_entry(unit, PPDB_A_TCAM_ENTRY_PARITY_SMALL_12m, MEM_BLOCK_ANY, table_entry);
                  errnum += 10;
                  SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
                  res = arad_fill_table_with_entry(unit, PPDB_A_TCAM_ENTRY_PARITY_SMALL_13m, MEM_BLOCK_ANY, table_entry);
                  errnum += 10;
                  SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
                  res = arad_fill_table_with_entry(unit, PPDB_A_TCAM_ENTRY_PARITY_SMALL_14m, MEM_BLOCK_ANY, table_entry);
                  errnum += 10;
                  SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
                  res = arad_fill_table_with_entry(unit, PPDB_A_TCAM_ENTRY_PARITY_SMALL_15m, MEM_BLOCK_ANY, table_entry);
                  errnum += 10;
                  SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
              }
#endif
          }
          res = arad_fill_table_with_entry(unit, IHB_UNKNOWN_DA_MAPm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHB_VRF_CONFIGm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      }
      res = arad_fill_table_with_entry(unit, PPDB_A_TCAM_ENTRY_PARITYm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, PPDB_A_TCAM_PD_PROFILEm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IHB_TIME_STAMP_FIFOm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);

      errnum = 14000; 
      if (!SOC_IS_ARDON(unit)) {
          res = arad_fill_table_with_entry(unit, IHP_ACTION_PROFILE_MPLS_VALUEm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHP_BVD_CFGm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHP_BVD_FID_CLASSm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHP_BVD_TOPOLOGY_IDm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHP_DEFAULT_COUNTER_SOURCEm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHP_DESIGNATED_VLAN_TABLEm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHP_FID_CLASS_2_FIDm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHP_INGRESS_VLAN_EDIT_COMMAND_TABLEm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHP_IN_RIF_CONFIG_TABLEm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          if (SOC_IS_JERICHO(unit)) {
              res = arad_fill_table_with_entry(unit, IHB_ISEM_MANAGEMENT_REQUESTm, MEM_BLOCK_ANY, table_entry);
              errnum += 10;
              SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
              res = arad_fill_table_with_entry(unit, IHB_ISEM_STEP_TABLEm, MEM_BLOCK_ANY, table_entry);
              errnum += 10;
              SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          } else {
              res = arad_fill_table_with_entry(unit, IHP_ISA_MANAGEMENT_REQUESTm, MEM_BLOCK_ANY, table_entry);
              errnum += 10;
              SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
              res = arad_fill_table_with_entry(unit, IHP_ISA_STEP_TABLEm, MEM_BLOCK_ANY, table_entry);
              errnum += 10;
              SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
              res = arad_fill_table_with_entry(unit, IHP_ISB_MANAGEMENT_REQUESTm, MEM_BLOCK_ANY, table_entry);
              errnum += 10;
              SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
              res = arad_fill_table_with_entry(unit, IHP_ISB_STEP_TABLEm, MEM_BLOCK_ANY, table_entry);
              errnum += 10;
              SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          }
          res = arad_fill_table_with_entry(unit, IHP_LIF_ACCESSEDm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHP_LIF_TABLEm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, PPDB_B_LARGE_EM_AGING_CONFIGURATION_TABLEm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, PPDB_B_LARGE_EM_FID_COUNTER_DBm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, PPDB_B_LARGE_EM_FID_COUNTER_PROFILE_DBm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, PPDB_B_LARGE_EM_FID_PROFILE_DBm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, PPDB_B_LARGE_EM_FLUSH_DBm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, PPDB_B_LARGE_EM_FORMAT_0_TYPE_0m, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, PPDB_B_LARGE_EM_PORT_MINE_TABLE_LAG_PORTm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, PPDB_B_LARGE_EM_PORT_MINE_TABLE_PHYSICAL_PORTm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, PPDB_B_LARGE_EM_STEP_TABLEm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      }
      res = arad_fill_table_with_entry(unit, IHP_LLR_LLVPm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IHP_LL_MIRROR_PROFILEm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
  }
  if (SOC_DPP_CONFIG(unit)->emulation_system == 0) {
      if (!SOC_IS_ARDON(unit)) {
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
#ifdef BCM_88675_A0
          if(SOC_IS_JERICHO(unit)) {
              res = arad_fill_table_with_entry(unit, PPDB_B_LARGE_EM_AGET_Hm, MEM_BLOCK_ANY, table_entry);
          } else
#endif 
          {
              res = arad_fill_table_with_entry(unit, IHP_MEM_300000m, MEM_BLOCK_ANY, table_entry);
          }
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
#ifdef BCM_88675_A0
          if(SOC_IS_JERICHO(unit)) {
              res = arad_fill_table_with_entry(unit, PPDB_B_LARGE_EM_AGET_AUXm, MEM_BLOCK_ANY, table_entry);
          } else
#endif 
          {
              res = arad_fill_table_with_entry(unit, IHP_MEM_380000m, MEM_BLOCK_ANY, table_entry);
          }
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      }
  }
  if (SOC_DPP_CONFIG(unit)->emulation_system == 0) {
      if (!SOC_IS_ARDON(unit)) {
#ifdef BCM_88675_A0
          if(SOC_IS_JERICHO(unit)) {
              res = arad_fill_table_with_entry(unit, PPDB_B_LARGE_EM_KEYT_PLDT_Hm, MEM_BLOCK_ANY, table_entry);
          } else
#endif 
          {
              res = arad_fill_table_with_entry(unit, IHP_MEM_500000m, MEM_BLOCK_ANY, table_entry);
          }
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
#ifdef BCM_88675_A0
          if(SOC_IS_JERICHO(unit)) {
              res = arad_fill_table_with_entry(unit, PPDB_B_LARGE_EM_KEYT_AUXm, MEM_BLOCK_ANY, table_entry);
          } else
#endif 
          {
              res = arad_fill_table_with_entry(unit, IHP_MEM_580000m, MEM_BLOCK_ANY, table_entry);
          }
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
#ifdef BCM_88675_A0
          if(SOC_IS_JERICHO(unit)) {
              res = arad_fill_table_with_entry(unit, PPDB_B_LARGE_EM_PLDT_AUXm, MEM_BLOCK_ANY, table_entry);
          } else
#endif 
          {
              res = arad_fill_table_with_entry(unit, IHP_MEM_610000m, MEM_BLOCK_ANY, table_entry);
          }
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHP_MEM_620000m, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
#ifdef BCM_88675_A0
          if(SOC_IS_JERICHO(unit)) {
              res = arad_fill_table_with_entry(unit, PPDB_B_LARGE_EM_STEP_TABLEm, MEM_BLOCK_ANY, table_entry);
          } else
#endif 
          {
              res = arad_fill_table_with_entry(unit, IHP_MEM_6A0000m, MEM_BLOCK_ANY, table_entry);
          }
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
#ifdef BCM_88675_A0
          if(SOC_IS_JERICHO(unit)) {
              res = arad_fill_table_with_entry(unit, PPDB_B_LARGE_EM_STEP_TABLEm, MEM_BLOCK_ANY, table_entry);
          } else
#endif 
          {
              res = arad_fill_table_with_entry(unit, IHP_MEM_6B0000m, MEM_BLOCK_ANY, table_entry);
          }
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
#ifdef BCM_88675_A0
          if(SOC_IS_JERICHO(unit)) {
              res = arad_fill_table_with_entry(unit, PPDB_B_LARGE_EM_MAA_CAMm, MEM_BLOCK_ANY, table_entry);
          } else
#endif 
          {
              res = arad_fill_table_with_entry(unit, IHP_MEM_740000m, MEM_BLOCK_ANY, table_entry);
          }
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
#ifdef BCM_88675_A0
          if(SOC_IS_JERICHO(unit)) {
              res = arad_fill_table_with_entry(unit, PPDB_B_LARGE_EM_MAA_CAM_PAYLOADm, MEM_BLOCK_ANY, table_entry);
          } else
#endif 
          {
              res = arad_fill_table_with_entry(unit, IHP_MEM_750000m, MEM_BLOCK_ANY, table_entry);
          }
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHP_MEM_7A0000m, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHP_MEM_820000m, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHP_MEM_830000m, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHP_MEM_8B0000m, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHP_MEM_8C0000m, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHP_MEM_940000m, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHP_MEM_950000m, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHP_MEM_9E0000m, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHP_MEM_9F0000m, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHP_MEM_A10000m, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHP_MEM_A90000m, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHP_MEM_AA0000m, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHP_MEM_B20000m, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHP_MEM_B30000m, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHP_MEM_BB0000m, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHP_MEM_BC0000m, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHP_MEM_C50000m, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHP_MEM_C60000m, MEM_BLOCK_ANY, table_entry);
      }
  }
  if (SOC_DPP_CONFIG(unit)->emulation_system == 0) {

      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IHP_PACKET_FORMAT_TABLEm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IHP_PARSER_CUSTOM_MACRO_PARAMETERSm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IHP_PARSER_CUSTOM_MACRO_PROTOCOLSm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IHP_PARSER_CUSTOM_MACRO_WORD_MAPm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IHP_PARSER_ETH_PROTOCOLSm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IHP_PARSER_IPV4_NEXT_PROTOCOL_SIZEm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IHP_PARSER_IPV6_NEXT_PROTOCOL_SIZEm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IHP_PARSER_IP_PROTOCOLSm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IHP_PARSER_MPLS_NEXT_PROTOCOL_SPECULATE_MAPm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IHP_PARSER_PROGRAMm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IHP_PARSER_PROGRAM_POINTER_FEM_BIT_SELECT_TABLEm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IHP_PARSER_PROGRAM_POINTER_FEM_FIELD_SELECT_MAPm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IHP_PARSER_PROGRAM_POINTER_FEM_MAP_INDEX_TABLEm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IHP_PFQ_0_FEM_BIT_SELECT_TABLEm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IHP_PFQ_0_FEM_FIELD_SELECT_MAPm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IHP_PFQ_0_FEM_MAP_INDEX_TABLEm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IHP_PINFO_LLRm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IHP_PORT_PROTOCOLm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IHP_PP_PORT_INFOm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IHP_PTC_INFOm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IHP_PTC_PARSER_PROGRAM_POINTER_CONFIGm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IHP_PTC_PFQ_0_CONFIGm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IHP_PTC_SYS_PORT_CONFIGm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IHP_PTC_VIRTUAL_PORT_CONFIGm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IHP_RECYCLE_COMMANDm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IHP_RESERVED_MCm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IHP_SRC_SYSTEM_PORT_FEM_BIT_SELECT_TABLEm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IHP_SRC_SYSTEM_PORT_FEM_FIELD_SELECT_MAPm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IHP_SRC_SYSTEM_PORT_FEM_MAP_INDEX_TABLEm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IHP_SUBNET_CLASSIFYm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IHP_TOS_2_COSm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IHP_VIRTUAL_PORT_FEM_BIT_SELECT_TABLEm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IHP_VIRTUAL_PORT_FEM_FIELD_SELECT_MAPm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IHP_VIRTUAL_PORT_FEM_MAP_INDEX_TABLEm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IHP_VIRTUAL_PORT_TABLEm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      if (!SOC_IS_ARDON(unit)) {
          res = arad_fill_table_with_entry(unit, IHP_STP_TABLEm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHP_TC_DP_MAP_TABLEm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHP_TERMINATION_PROFILE_TABLEm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHP_VLAN_EDIT_PCP_DEI_MAPm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHP_VSI_PORT_MEMBERSHIPm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHP_VLAN_RANGE_COMPRESSION_TABLEm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHP_VRID_MY_MAC_MAPm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHP_VRID_TO_VRF_MAPm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHP_VSI_HIGH_DA_NOT_FOUND_DESTINATIONm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHP_VSI_HIGH_MY_MACm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHP_VSI_HIGH_PROFILEm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHP_VSI_LOW_CFG_1m, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          
          soc_mem_field32_set(unit, IHP_VSI_LOW_CFG_2m, vsi_low_cfg, FID_CLASSf, 7);
          res = arad_fill_table_with_entry(unit, IHP_VSI_LOW_CFG_2m, MEM_BLOCK_ANY, vsi_low_cfg);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHP_VTT_1ST_KEY_PROG_SEL_TCAMm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHP_VTT_1ST_LOOKUP_PROGRAM_0m, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHP_VTT_1ST_LOOKUP_PROGRAM_1m, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHP_VTT_2ND_KEY_PROG_SEL_TCAMm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHP_VTT_2ND_LOOKUP_PROGRAM_0m, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHP_VTT_2ND_LOOKUP_PROGRAM_1m, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHP_VTT_IN_PP_PORT_CONFIGm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHP_VTT_IN_PP_PORT_VLAN_CONFIGm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHP_VTT_LLVPm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHP_VTT_PP_PORT_TT_KEY_VARm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHP_VTT_PP_PORT_VSI_PROFILESm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHP_VTT_PP_PORT_VT_KEY_VARm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHP_VTT_PTC_CONFIGm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      }

      errnum = 15000; 
      res = arad_fill_table_with_entry(unit, IPS_CRBALm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IPS_CRBALTHm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit,  IPS_CRWDTHm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IPS_EMPTYQCRBALm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IPS_FLWIDm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IPS_MAXQSZm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
  }
  if (SOC_DPP_CONFIG(unit)->emulation_system == 0) {
      res = arad_fill_table_with_entry(unit, IPS_MEM_180000m, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IPS_MEM_1A0000m, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IPS_MEM_1E0000m, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IPS_MEM_200000m, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IPS_MEM_220000m, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IPS_MEM_240000m, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
  }
  res = arad_fill_table_with_entry(unit, IPS_QPM_1_SYS_REDm, MEM_BLOCK_ANY, table_entry);
  errnum += 10;
  SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
  res = arad_fill_table_with_entry(unit, IPS_QPRISELm, MEM_BLOCK_ANY, table_entry);
  errnum += 10;
  SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
  res = arad_fill_table_with_entry(unit, IPS_QSZm, MEM_BLOCK_ANY, table_entry);
  errnum += 10;
  SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
  res = arad_fill_table_with_entry(unit, IPS_QSZTHm, MEM_BLOCK_ANY, table_entry);
  errnum += 10;
  SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
  res = arad_fill_table_with_entry(unit, IPS_QTYPEm, MEM_BLOCK_ANY, table_entry);
  errnum += 10;
  SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
  res = arad_fill_table_with_entry(unit, IPS_Q_PRIORITY_BIT_MAPm, MEM_BLOCK_ANY, table_entry);
  errnum += 10;
  SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);

  errnum = 16000; 
  res = arad_fill_table_with_entry(unit, IPT_BDQm, MEM_BLOCK_ANY, table_entry);
  errnum += 10;
  SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
  res = arad_fill_table_with_entry(unit, IPT_EGQCTLm, MEM_BLOCK_ANY, table_entry);
  errnum += 10;
  SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
  res = arad_fill_table_with_entry(unit, IPT_EGQDATAm, MEM_BLOCK_ANY, table_entry);
  errnum += 10;
  SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
  res = arad_fill_table_with_entry(unit, IPT_FDTCTLm, MEM_BLOCK_ANY, table_entry);
  errnum += 10;
  SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
  res = arad_fill_table_with_entry(unit, IPT_FDTDATAm, MEM_BLOCK_ANY, table_entry);
  errnum += 10;
  SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
  res = arad_fill_table_with_entry(unit, IPT_GCI_BACKOFF_MASKm, MEM_BLOCK_ANY, table_entry);
  errnum += 10;
  SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
  res = arad_fill_table_with_entry(unit, IPT_ITM_TO_OTM_MAPm, MEM_BLOCK_ANY, table_entry);
  errnum += 10;
  SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
  res = arad_fill_table_with_entry(unit, IPT_MOP_MMUm, MEM_BLOCK_ANY, table_entry);
  errnum += 10;
  SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
  res = arad_fill_table_with_entry(unit, IPT_PCQm, MEM_BLOCK_ANY, table_entry);
  errnum += 10;
  SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
  res = arad_fill_table_with_entry(unit, IPT_PRIORITY_BITS_MAPPING_2_FDTm, MEM_BLOCK_ANY, table_entry);
  errnum += 10;
  SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
  res = arad_fill_table_with_entry(unit, IPT_SNP_MIR_CMD_MAPm, MEM_BLOCK_ANY, table_entry);
  errnum += 10;
  SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
  res = arad_fill_table_with_entry(unit, IPT_SOP_MMUm, MEM_BLOCK_ANY, table_entry);
  errnum += 10;
  SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);

  errnum = 17000; 
  if (SOC_DPP_CONFIG(unit)->emulation_system == 0) {
      res = arad_fill_table_with_entry(unit, IQM_BDBLLm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IQM_CPDMDm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IQM_CPDMSm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IQM_CPPRMm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IQM_CRDTDISm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IQM_DBFFMm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IQM_DELFFMm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IQM_FLUSCNTm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IQM_GRSPRMm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IQM_ITMPMm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IQM_MEM_7E00000m, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IQM_MNUSCNTm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IQM_NIFTCMm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IQM_OCBPRMm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
  }
  if (SOC_DPP_CONFIG(unit)->emulation_system == 0) {
      res = arad_fill_table_with_entry(unit, IQM_PDMm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
  }

  res = arad_fill_table_with_entry(unit, IQM_PQDMDm, MEM_BLOCK_ANY, table_entry);
  errnum += 10;
  SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
  res = arad_fill_table_with_entry(unit, IQM_PQDMSm, MEM_BLOCK_ANY, table_entry);
  errnum += 10;
  SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
  res = arad_fill_table_with_entry(unit, IQM_PQREDm, MEM_BLOCK_ANY, table_entry);
  errnum += 10;
  SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
  if (SOC_DPP_CONFIG(unit)->emulation_system == 0) {
      res = arad_fill_table_with_entry(unit, IQM_PQWQm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
  }
  res = arad_fill_table_with_entry(unit, IQM_SCRBUFFTHm, MEM_BLOCK_ANY, table_entry);
  errnum += 10;
  SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
  res = arad_fill_table_with_entry(unit, IQM_SPRDPRMm, MEM_BLOCK_ANY, table_entry);
  errnum += 10;
  SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
  res = arad_fill_table_with_entry(unit, IQM_SRCQRNGm, MEM_BLOCK_ANY, table_entry);
  errnum += 10;
  SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
  res = arad_fill_table_with_entry(unit, IQM_SRDPRBm, MEM_BLOCK_ANY, table_entry);
  errnum += 10;
  SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
  res = arad_fill_table_with_entry(unit, IQM_TAILm, MEM_BLOCK_ANY, table_entry);
  errnum += 10;
  SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
  if (SOC_DPP_CONFIG(unit)->emulation_system == 0) {

      res = arad_fill_table_with_entry(unit, IQM_VQFCPR_MAm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IQM_VQFCPR_MBm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IQM_VQFCPR_MCm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IQM_VQFCPR_MDm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IQM_VQFCPR_MEm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IQM_VQFCPR_MFm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IQM_VQPR_MAm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IQM_VQPR_MBm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IQM_VQPR_MCm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IQM_VQPR_MDm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IQM_VQPR_MEm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IQM_VQPR_MFm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IQM_VSQDRC_Am, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IQM_VSQDRC_Bm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IQM_VSQDRC_Cm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IQM_VSQDRC_Dm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IQM_VSQDRC_Em, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IQM_VSQDRC_Fm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);

      res = arad_fill_table_with_entry(unit, IQM_VSQ_A_MX_OCm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IQM_VSQ_B_MX_OCm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IQM_VSQ_C_MX_OCm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IQM_VSQ_D_MX_OCm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IQM_VSQ_E_MX_OCm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IQM_VSQ_F_MX_OCm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IQM_VS_QA_AVGm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IQM_VS_QA_QSZm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IQM_VS_QB_AVGm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IQM_VS_QB_QSZm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IQM_VS_QC_AVGm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IQM_VS_QC_QSZm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IQM_VS_QD_AVGm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IQM_VS_QD_QSZm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IQM_VS_QE_AVGm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IQM_VS_QE_QSZm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IQM_VS_QF_AVGm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IQM_VS_QF_QSZm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);

      errnum = 18000; 
  }
  res = arad_fill_table_with_entry(unit, IRE_CPU_CTXT_MAPm, MEM_BLOCK_ANY, table_entry);
  errnum += 10;
  SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
  res = arad_fill_table_with_entry(unit, IRE_CTXT_MEM_CONTROLm, MEM_BLOCK_ANY, table_entry);
  errnum += 10;
  SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
  res = arad_fill_table_with_entry(unit, IRE_NIF_CTXT_MAPm, MEM_BLOCK_ANY, table_entry);
  errnum += 10;
  SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
  res = arad_fill_table_with_entry(unit, IRE_NIF_PORT_MAPm, MEM_BLOCK_ANY, table_entry);
  errnum += 10;
  SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
  if (SOC_DPP_CONFIG(unit)->emulation_system == 0) {
      res = arad_fill_table_with_entry(unit, IRE_NIF_PORT_TO_CTXT_BIT_MAPm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);

  res = arad_fill_table_with_entry(unit, IRE_RCY_CTXT_MAPm, MEM_BLOCK_ANY, table_entry);
  errnum += 10;
  SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);

  errnum = 19000; 
  res = arad_fill_table_with_entry(unit, IRR_FLOW_TABLEm, MEM_BLOCK_ANY, table_entry);
  errnum += 10;
  SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
  res = arad_fill_table_with_entry(unit, IRR_FREE_PCB_MEMORYm, MEM_BLOCK_ANY, table_entry);
  errnum += 10;
  SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
  res = arad_fill_table_with_entry(unit, IDR_IRDBm, MEM_BLOCK_ANY, table_entry);
  errnum += 10;
  SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
  res = arad_fill_table_with_entry(unit, IRR_ISF_MEMORYm, MEM_BLOCK_ANY, table_entry);
  errnum += 10;
  SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
  res = arad_fill_table_with_entry(unit, IRR_IS_FREE_PCB_MEMORYm, MEM_BLOCK_ANY, table_entry);
  errnum += 10;
  SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
  res = arad_fill_table_with_entry(unit, IRR_IS_PCB_LINK_TABLEm, MEM_BLOCK_ANY, table_entry);
  errnum += 10;
  SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
  res = arad_fill_table_with_entry(unit, IRR_LAG_MAPPINGm, MEM_BLOCK_ANY, table_entry);
  errnum += 10;
  SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
  res = arad_fill_table_with_entry(unit, IRR_LAG_NEXT_MEMBERm, MEM_BLOCK_ANY, table_entry);
  errnum += 10;
  SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
  res = arad_fill_table_with_entry(unit, IRR_LAG_TO_LAG_RANGEm, MEM_BLOCK_ANY, table_entry);
  errnum += 10;
  SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
  res = arad_fill_table_with_entry(unit, IRR_MCR_MEMORYm, MEM_BLOCK_ANY, table_entry);
  errnum += 10;
  SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
  }

  
  if (!SOC_IS_ARADPLUS(unit)) {
      res = arad_fill_table_with_entry(unit, IRR_MEM_300000m, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);

      res = arad_fill_table_with_entry(unit, IRR_MEM_340000m, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IRR_MEM_3C0000m, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);

  }

  if (SOC_DPP_CONFIG(unit)->emulation_system == 0) {

      res = arad_fill_table_with_entry(unit, IRR_PCB_LINK_TABLEm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IRR_SMOOTH_DIVISIONm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IRR_STACK_FEC_RESOLVEm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IRR_STACK_TRUNK_RESOLVEm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, IRR_TRAFFIC_CLASS_MAPPINGm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit)

  }

  errnum = 20000; 
  res = arad_fill_table_with_entry(unit, MMU_FDFm, MEM_BLOCK_ANY, table_entry);
  errnum += 10;
  SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
  res = arad_fill_table_with_entry(unit, MMU_IDFm, MEM_BLOCK_ANY, table_entry);
  errnum += 10;
  SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);


  errnum = 21000; 
  if ((SOC_DPP_CONFIG(unit)->emulation_system == 0) && (!SOC_IS_ARDON(unit))) {

      res = arad_fill_table_with_entry(unit, NBI_MEM_92000m, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, NBI_MEM_93000m, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, NBI_MEM_94000m, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, NBI_MEM_95000m, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
  }

  if (!SOC_IS_ARDON(unit)) {
      res = arad_fill_table_with_entry(unit, NBI_MLF_RX_MEM_A_CTRLm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, NBI_MLF_RX_MEM_B_CTRLm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, NBI_MLF_TX_MEM_CTRLm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);

      errnum = 22000; 

      mem_lcl = SOC_IS_ARADPLUS(unit)? OAMP_MEM_20000m: OAMP_LOCAL_PORT_2_SYSTEM_PORTm;
      res = arad_fill_table_with_entry(unit, mem_lcl, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
  }

  if ((SOC_DPP_CONFIG(unit)->emulation_system == 0) && (!SOC_IS_ARDON(unit))) {

      res = arad_fill_table_with_entry(unit, OAMP_MEM_100000m, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, OAMP_MEM_180000m, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, OAMP_MEM_190000m, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, OAMP_MEM_210000m, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, OAMP_MEM_220000m, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, OAMP_MEM_2A0000m, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, OAMP_MEM_2B0000m, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, OAMP_MEM_340000m, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, OAMP_MEM_350000m, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, OAMP_MEM_40000m, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, OAMP_MEM_50000m, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, OAMP_MEM_60000m, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
          
          res = arad_fill_table_with_entry(unit, OAMP_MEM_70000m, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      }
      res = arad_fill_table_with_entry(unit, OAMP_MEM_80000m, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
  }
  if (!SOC_IS_ARDON(unit)) {
      if (SOC_DPP_CONFIG(unit)->emulation_system == 0) {
          res = arad_fill_table_with_entry(unit, OAMP_MEP_DBm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      }
      res = arad_fill_table_with_entry(unit, OAMP_RMAPEM_MANAGEMENT_REQUESTm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, OAMP_REMOTE_MEP_EXACT_MATCH_STEP_TABLEm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      if (SOC_DPP_CONFIG(unit)->emulation_system == 0) {
          res = arad_fill_table_with_entry(unit, OAMP_RMEP_DBm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      }
      res = arad_fill_table_with_entry(unit, OAMP_UMC_TABLEm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);

      errnum = 24000; 
      res = arad_fill_table_with_entry(unit, OLP_DSP_EVENT_ROUTEm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      if (SOC_DPP_CONFIG(unit)->emulation_system == 0) {
          res = arad_fill_table_with_entry(unit, OLP_MEM_00000m, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      }
  }
  if (SOC_DPP_CONFIG(unit)->emulation_system == 0) {

      errnum = 25000; 
      res = arad_fill_table_with_entry(unit, RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_CTRL_CELLSm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_DATA_CELLSm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, RTP_UNICAST_DISTRIBUTION_MEMORYm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);

      errnum = 26000; 
      res = arad_fill_table_with_entry(unit, SCH_CH_NIF_CALENDAR_CONFIGURATION__CNCCm, SCH_BLOCK(unit, SOC_CORE_ALL), table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, SCH_CH_NIF_RATES_CONFIGURATION__CNRCm, SCH_BLOCK(unit, SOC_CORE_ALL), table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, SCH_CIR_SHAPERS_STATIC_TABEL_CSSTm, SCH_BLOCK(unit, SOC_CORE_ALL), table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, SCH_CIR_SHAPER_CALENDAR__CSCm, SCH_BLOCK(unit, SOC_CORE_ALL), table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, SCH_CL_SCHEDULERS_CONFIGURATION_SCCm, SCH_BLOCK(unit, SOC_CORE_ALL), table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, SCH_CL_SCHEDULERS_TYPE__SCTm, SCH_BLOCK(unit, SOC_CORE_ALL), table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);

      res = arad_fill_table_with_entry(unit, SCH_SHARED_DEVICE_RATE_SHARED_DRMm, SCH_BLOCK(unit, SOC_CORE_ALL), table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      if(SOC_IS_JERICHO(unit)){
          
          res = arad_fill_table_with_entry(unit, SCH_DEVICE_RATE_MEMORY_DRMm, SCH_BLOCK(unit, SOC_CORE_ALL), table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      }
      res = arad_fill_table_with_entry(unit, SCH_DSP_2_PORT_MAP__DSPPm, SCH_BLOCK(unit, SOC_CORE_ALL), table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, SCH_DUAL_SHAPER_MEMORY_DSMm, SCH_BLOCK(unit, SOC_CORE_ALL), table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, SCH_FC_MAP_FCMm, SCH_BLOCK(unit, SOC_CORE_ALL), table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, SCH_FLOW_DESCRIPTOR_MEMORY_STATIC_FDMSm, SCH_BLOCK(unit, SOC_CORE_ALL), table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, SCH_FLOW_GROUP_MEMORY_FGMm, SCH_BLOCK(unit, SOC_CORE_ALL), table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, SCH_FLOW_SUB_FLOW_FSFm, SCH_BLOCK(unit, SOC_CORE_ALL), table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, SCH_FLOW_TO_FIP_MAPPING__FFMm, SCH_BLOCK(unit, SOC_CORE_ALL), table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, SCH_FLOW_TO_QUEUE_MAPPING_FQMm, SCH_BLOCK(unit, SOC_CORE_ALL), table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, SCH_FORCE_STATUS_MESSAGEm, SCH_BLOCK(unit, SOC_CORE_ALL), table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, SCH_HR_SCHEDULER_CONFIGURATION_SHCm, SCH_BLOCK(unit, SOC_CORE_ALL), table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
  }
  if (SOC_DPP_CONFIG(unit)->emulation_system == 0) {

      res = arad_fill_table_with_entry(unit, SCH_MEM_01F00000m, SCH_BLOCK(unit, SOC_CORE_ALL), table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, SCH_MEM_04700000m, SCH_BLOCK(unit, SOC_CORE_ALL), table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, SCH_MEM_04A00000m, SCH_BLOCK(unit, SOC_CORE_ALL), table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, SCH_MEM_04D00000m, SCH_BLOCK(unit, SOC_CORE_ALL), table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
  }

  if (SOC_DPP_CONFIG(unit)->emulation_system == 0) {

      res = arad_fill_table_with_entry(unit, SCH_ONE_PORT_NIF_CONFIGURATION_OPNCm, SCH_BLOCK(unit, SOC_CORE_ALL), table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, SCH_PIR_SHAPERS_STATIC_TABEL_PSSTm, SCH_BLOCK(unit, SOC_CORE_ALL), table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, SCH_PIR_SHAPER_CALENDAR__PSCm, SCH_BLOCK(unit, SOC_CORE_ALL), table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, SCH_PORT_ENABLE_PORTENm, SCH_BLOCK(unit, SOC_CORE_ALL), table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, SCH_PORT_GROUP_PFGMm, SCH_BLOCK(unit, SOC_CORE_ALL), table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, SCH_PORT_QUEUE_SIZE__PQSm, SCH_BLOCK(unit, SOC_CORE_ALL), table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, SCH_PORT_SCHEDULER_MAP_PSMm, SCH_BLOCK(unit, SOC_CORE_ALL), table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, SCH_PORT_SCHEDULER_WEIGHTS_PSWm, SCH_BLOCK(unit, SOC_CORE_ALL), table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR__CALm, SCH_BLOCK(unit, SOC_CORE_ALL), table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, SCH_SCHEDULER_ENABLE_MEMORY_SEMm, SCH_BLOCK(unit, SOC_CORE_ALL), table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      if (SOC_IS_JERICHO(unit) && !SOC_IS_QUX(unit))
      {
          res = arad_fill_table_with_entry(unit, SCH_SCHEDULER_ENABLE_MEMORY_SEM_Bm, SCH_BLOCK(unit, SOC_CORE_ALL), table_entry);
          errnum +=10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      }
      res = arad_fill_table_with_entry(unit, SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDSm, SCH_BLOCK(unit, SOC_CORE_ALL), table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      res = arad_fill_table_with_entry(unit, SCH_TOKEN_MEMORY_CONTROLLER__TMCm, SCH_BLOCK(unit, SOC_CORE_ALL), table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
  }

#ifdef BCM_88660_A0
  if ((SOC_DPP_CONFIG(unit)->emulation_system == 0) && (SOC_IS_ARADPLUS(unit))) {
      
      res = arad_fill_table_with_entry(unit, EGQ_PER_PORT_LB_RANGEm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      if (!SOC_IS_ARDON(unit)) {
          res = arad_fill_table_with_entry(unit, EPNI_IP_TOS_MARKINGm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, EPNI_PP_REMARK_PROFILEm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IDR_MCDA_PCUCm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IDR_MCDB_PCUCm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IDR_PCD_MAPm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHP_CONSISTENT_HASHING_PROGRAM_SEL_TCAMm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHB_CONSISTENT_HASHING_PROGRAM_VARIABLESm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHB_ELK_PAYLOAD_FORMATm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHB_FEC_ECMP_IS_STATEFULm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHP_FLP_CONSISTENT_HASHING_KEY_GENm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHB_IPP_LAG_TO_LAG_RANGEm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, IHP_VRID_MY_MAC_CAMm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      }
      res = arad_fill_table_with_entry(unit, IQM_FRDMTm, MEM_BLOCK_ANY, table_entry);
      errnum += 10;
      SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      if (!SOC_IS_ARDON(unit)) {
          res = arad_fill_table_with_entry(unit, OAMP_LMM_DA_NIC_TABLEm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, OAMP_MEM_20000m, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, OAMP_MEM_B0000m, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, OAMP_MEM_C0000m, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, OAMP_MEP_DB_DM_STATm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, OAMP_MEP_DB_LM_DBm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, OAMP_MEP_DB_LM_STATm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, OAMP_MEP_DB_RFC_6374_ON_MPLSTPm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, OAMP_PE_GEN_MEMm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, OAMP_PE_PROGRAMm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
          res = arad_fill_table_with_entry(unit, OAMP_PE_PROG_TCAMm, MEM_BLOCK_ANY, table_entry);
          errnum += 10;
          SOC_SAND_CHECK_FUNC_RESULT(res, errnum, exit);
      }
  }
#endif 

  res = arad_mgmt_all_tbls_init_enable_dynamic(unit, 0);
  SOC_SAND_CHECK_FUNC_RESULT(res, 11116, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_mgmt_all_tbls_init()",0,0);
}






#include <soc/dpp/SAND/Utils/sand_footer.h>



#endif 

