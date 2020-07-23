/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_PPC_API_OAM_INCLUDED__

#define __SOC_PPC_API_OAM_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPC/ppc_api_general.h>
#include <soc/register.h>
#include <bcm/oam.h>









#define SOC_PPC_OAM_IS_MEP_TYPE_BFD(mep_type) \
  ((mep_type==SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_1_HOP) || (mep_type==SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP) || \
   (mep_type==SOC_PPC_OAM_MEP_TYPE_BFD_O_MPLS) || (mep_type==SOC_PPC_OAM_MEP_TYPE_BFD_O_PWE)              || \
   (mep_type==SOC_PPC_OAM_MEP_TYPE_BFDCC_O_MPLSTP) || (mep_type==SOC_PPC_OAM_MEP_TYPE_BFD_O_PWE_GAL))

#define SOC_PPC_OAM_IS_MEP_TYPE_Y1731(mep_type) \
  ((mep_type==SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP) || (mep_type==SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE) || \
   (mep_type==SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE_GAL) || (mep_type==SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP_SECTION))


#define SOC_PPC_OAM_GROUP_NAME_TO_ICC_MAP_DATA(ma_name, data)              \
  COMPILER_64_SET(data, ma_name[1]+(ma_name[0]<<8),                        \
    ma_name[5]+(ma_name[4]<<8)+(ma_name[3]<<16)+(ma_name[2]<<24))


#define SOC_PPC_OAM_CCM_PERIOD_TO_CCM_INTERVAL_FIELD(ccm_period_ms, ccm_period_micro_s, ccm_interval) \
do {                                                                                               \
      if (ccm_period_ms==0) {                \
        ccm_interval = 0;\
      }\
      else if (ccm_period_ms > 5*60*1000 ) {                                                   \
        ccm_interval = 0x7;                                                                        \
      }                                                                                            \
      else if (ccm_period_ms > 35*1000 ) {                                               \
        ccm_interval = 0x6;                                                                        \
      }                                                                                            \
      else if (ccm_period_ms > 5 *1500 ) {                                            \
        ccm_interval = 0x5;                                                                        \
      }                                                                                            \
      else if (ccm_period_ms > 550 ) {                                                   \
        ccm_interval = 0x4;                                                                        \
      }                                                                                            \
      else if (ccm_period_ms > 55 ) {                                                     \
        ccm_interval = 0x3;                                                                        \
      }                                                                                            \
      else if (ccm_period_ms > 7 || (ccm_period_ms > 6 && ccm_period_micro_s > 660) ) { \
        ccm_interval = 0x2;                                                                        \
      }                                                                                            \
      else {                                                                                       \
        ccm_interval = 0x1;                                                                        \
      }                                                                                            \
} while (0)

#define SOC_PPC_OAM_CCM_PERIOD_FROM_CCM_INTERVAL_FIELD(ccm_period_ms, ccm_period_micro_s, ccm_interval)   \
do {                                                                               \
      ccm_period_micro_s = 0;                                                      \
      switch (ccm_interval) {                                                      \
      case 0x0:                                                             \
          ccm_period_ms = 0;                                                       \
          break;                                                                   \
      case 0x1:                                                         \
          ccm_period_ms = 3;                                                       \
          ccm_period_micro_s = 333;                                                \
          break;                                                                   \
      case 0x2:                                                            \
          ccm_period_ms = 10;                                                      \
          break;                                                                   \
      case 0x3:                                                           \
          ccm_period_ms = 100;                                                     \
          break;                                                                   \
      case 0x4:                                                              \
          ccm_period_ms = 1*1000;                                            \
          break;                                                                   \
      case 0x5:                                                             \
          ccm_period_ms = 10*1000;                                           \
          break;                                                                   \
      case 0x6:                                                            \
          ccm_period_ms = 60*1000;                                           \
          break;                                                                   \
      default:                                                            \
          ccm_period_ms = 10*60*1000;                                        \
      }                                                                            \
} while (0)


#define PPC_API_OAM_STORE_LOCAL_PORT_IN_MEP_DB(mep_type)       \
  ((mep_type == SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_1_HOP) ||       \
   (mep_type == SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP) ||       \
   (mep_type == SOC_PPC_OAM_MEP_TYPE_BFD_O_MPLS) ||             \
   (mep_type == SOC_PPC_OAM_MEP_TYPE_ETH_OAM))





typedef uint32   SOC_PPC_OAM_ETH_ACC_MEP_ID;

typedef enum
{
  
  SOC_PPC_OAM_ETH_ACC_FUNC_TYPE_0 = 0,
  
  SOC_PPC_OAM_ETH_ACC_FUNC_TYPE_1 = 1,
  
  SOC_PPC_OAM_ETH_ACC_FUNC_TYPE_2 = 2,
  
  SOC_PPC_OAM_ETH_ACC_FUNC_TYPE_3 = 3,
  
  SOC_PPC_OAM_ETH_ACC_FUNC_TYPE_4 = 4,
  
  SOC_PPC_OAM_ETH_ACC_FUNC_TYPE_5 = 5,
  
  SOC_PPC_OAM_NOF_ETH_ACC_FUNC_TYPES = 6
}SOC_PPC_OAM_ETH_ACC_FUNC_TYPE;

typedef enum
{
  
  SOC_PPC_OAM_ETH_MP_LEVEL_0 = 0,
  
  SOC_PPC_OAM_ETH_MP_LEVEL_1 = 1,
  
  SOC_PPC_OAM_ETH_MP_LEVEL_2 = 2,
  
  SOC_PPC_OAM_ETH_MP_LEVEL_3 = 3,
  
  SOC_PPC_OAM_ETH_MP_LEVEL_4 = 4,
  
  SOC_PPC_OAM_ETH_MP_LEVEL_5 = 5,
  
  SOC_PPC_OAM_ETH_MP_LEVEL_6 = 6,
  
  SOC_PPC_OAM_ETH_MP_LEVEL_7 = 7,
  
  SOC_PPC_OAM_NOF_ETH_MP_LEVELS = 8
}SOC_PPC_OAM_ETH_MP_LEVEL;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 valid;
  
  SOC_PPC_OAM_ETH_ACC_FUNC_TYPE func_type;
  
  SOC_PPC_OAM_ETH_ACC_MEP_ID acc_mep_id;

} SOC_PPC_OAM_ETH_MP_INFO;




#define SOC_PPC_BFD_PDU_OPCODE  0


#define SOC_PPC_OAM_ETHERNET_PDU_OPCODE_CCM  1
#define SOC_PPC_OAM_ETHERNET_PDU_OPCODE_LBR  2 
#define SOC_PPC_OAM_ETHERNET_PDU_OPCODE_LBM  3 
#define SOC_PPC_OAM_ETHERNET_PDU_OPCODE_LTR  4
#define SOC_PPC_OAM_ETHERNET_PDU_OPCODE_LTM  5
#define SOC_PPC_OAM_ETHERNET_PDU_OPCODE_AIS  33
#define SOC_PPC_OAM_ETHERNET_PDU_OPCODE_LCK  35
#define SOC_PPC_OAM_ETHERNET_PDU_OPCODE_TST  37
#define SOC_PPC_OAM_ETHERNET_PDU_OPCODE_LINEAR_APS  39
#define SOC_PPC_OAM_ETHERNET_PDU_OPCODE_LMR  42
#define SOC_PPC_OAM_ETHERNET_PDU_OPCODE_LMM  43 
#define SOC_PPC_OAM_ETHERNET_PDU_OPCODE_1DM  45 
#define SOC_PPC_OAM_ETHERNET_PDU_OPCODE_DMR  46
#define SOC_PPC_OAM_ETHERNET_PDU_OPCODE_DMM  47 
#define SOC_PPC_OAM_ETHERNET_PDU_OPCODE_EXR  48 
#define SOC_PPC_OAM_ETHERNET_PDU_OPCODE_EXM  49 
#define SOC_PPC_OAM_ETHERNET_PDU_OPCODE_CSF  52 
#define SOC_PPC_OAM_ETHERNET_PDU_OPCODE_SLR  54
#define SOC_PPC_OAM_ETHERNET_PDU_OPCODE_SLM  55
#define SOC_PPC_OAM_ETHERNET_PDU_OPCODE_PSEUDO_CCM  100
#define SOC_PPC_OAM_ETHERNET_PDU_OPCODE_COUNT 256



#define SOC_PPC_OAM_ETHERNET_PDU_DUMMY_MAX_DIAG_OPCODE SOC_PPC_OAM_ETHERNET_PDU_OPCODE_SLM


typedef enum
{

    SOC_PPC_OAM_OPCODE_MAP_BFD        = 0,
    SOC_PPC_OAM_OPCODE_MAP_CCM        = 1,
    SOC_PPC_OAM_OPCODE_MAP_LBR        = 2,
    SOC_PPC_OAM_OPCODE_MAP_LBM        = 3,
    SOC_PPC_OAM_OPCODE_MAP_LTR        = 4,
    SOC_PPC_OAM_OPCODE_MAP_LTM        = 5,
    SOC_PPC_OAM_OPCODE_MAP_LMR        = 6,
    SOC_PPC_OAM_OPCODE_MAP_LMM        = 7,
    SOC_PPC_OAM_OPCODE_MAP_DMR        = 8,
    SOC_PPC_OAM_OPCODE_MAP_DMM        = 9,
    SOC_PPC_OAM_OPCODE_MAP_1DM        = 10,
    SOC_PPC_OAM_OPCODE_MAP_SLM_SLR    = 11,
    SOC_PPC_OAM_OPCODE_MAP_SLM        = 11,
    SOC_PPC_OAM_OPCODE_MAP_SLR        = 11,
    SOC_PPC_OAM_OPCODE_MAP_AIS        = 12,
    SOC_PPC_OAM_OPCODE_MAP_LCK        = 13,
    SOC_PPC_OAM_OPCODE_MAP_LINEAR_APS = 14,
    SOC_PPC_OAM_OPCODE_MAP_DEFAULT    = 15,
    SOC_PPC_OAM_OPCODE_MAP_COUNT      = 16

}SOC_PPC_OAM_INTERNAL_OPCODE;

#define SOC_PPC_OAM_NON_ACC_PROFILES_NUM 4
#define SOC_PPC_OAM_NON_ACC_PROFILES_ARAD_PLUS_NUM 16
#define SOC_PPC_OAM_ACC_PROFILES_NUM     16
#define SOC_PPC_OAM_PROFILE_DEFAULT 0

#define SOC_PPC_OAM_MAX_PROTECTION_HEADER_SIZE 80
#define SOC_PPC_OAM_FULL_PROTECTION_HEADER_SIZE 120

#define SOC_PPC_OAM_MAX_NUMBER_OF_MAS(unit) (SOC_DPP_DEFS_GET(unit,oamp_number_of_meps)) 
#define SOC_PPC_OAM_MAX_NUMBER_OF_LOCAL_MEPS(unit) (SOC_DPP_DEFS_GET(unit,oamp_number_of_meps)) 
#define SOC_PPC_OAM_MAX_NUMBER_OF_REMOTE_MEPS(unit) (SOC_DPP_DEFS_GET(unit,oamp_number_of_rmeps)) 
#define SOC_PPC_OAM_MAX_NUMBER_OF_ETH1731_MEP_PROFILES(unit) (SOC_DPP_DEFS_GET(unit,oamp_number_of_eth_y1731_mep_profiles)) 
#define SOC_PPC_OAM_SIZE_OF_UMC_TABLE(unit) (SOC_DPP_DEFS_GET(unit,oamp_umc_table_size)) 
#define SOC_PPC_OAM_SIZE_OF_LOCAL_PORT_2_SYS_PORT_TABLE(unit) (SOC_DPP_DEFS_GET(unit,oamp_local_port_2_sys_port_size))

#define SOC_PPC_OAM_SIZE_OF_OAM_KEY_IN_BITS(unit)              (SOC_DPP_DEFS_GET(unit,size_of_oam_key))



#define SOC_PPC_OAM_ICC_MAP_DATA_NOF_BITS (48) 

#define SOC_PPC_OAM_NUMBER_OF_OUTLIF_BITS_USED_BY_PCP               (SOC_DPP_CONFIG(unit)->pp.oam_pcp_egress_prof_num_bits)
#define SOC_PPC_OAM_NUMBER_OF_OUTLIF_BITS_USED_BY_DEFAULT           (SOC_DPP_CONFIG(unit)->pp.oam_default_egress_prof_num_bits)

#define SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ACCELERATED                0x1
#define SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ENDPOINT                   0x2
#define SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_PCP                        0x4
#define SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_DM_1588                    0x8
#define SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_UPMEP                      0x10
#define SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_HAS_LOOPBACK           0x20
#define SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_HAS_LOOPBACK_REPLY     0x40
#define SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_SERVER                     0x80

#define SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ONLY_ON_DEMAND_DMM_ACTIVE  0x100
#define SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_USING_ASYMETRIC_MPLS_LIF   0x200
#define SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_BFD_TIMEOUT_SET_EXPLICITLY 0x400
#define SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_DOWNMEP_TX_GPORT_IS_LAG    0x400
#define SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_LOOPBACK_PERIOD_IN_PPS     0x800
#define SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_LOOPBACK_JER           0x1000
#define SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_TST_TX_JER             0x2000
#define SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_TST_RX_JER             0x4000
#define SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_EGRESS_ONLY            0x8000
#define SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_SD_SF                  0x10000
#define SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MEP_1711_ALARM             0x20000
#define SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_TTL_1                      0x40000
#define SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_RA                         0x80000
#define SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MPLS_OUTLIF_EGRESS_ONLY    0x100000
#define SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_USE_DOUBLE_OUTLIF_INJ      0x200000
#define SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MPLS_OUTLIF_INGRESS_ONLY   0x400000
#define SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MPLS_TP                    0x800000 
#define SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_DOWN_MEP_INJECTION  0x1000000 
#define SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_BFD_SEAMLESS_BFD_INITIATOR  0x2000000 
#define SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_BFD_SEAMLESS_BFD_REFLECOTR  0x4000000 
#define SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_BFD_MPLS_PHP                0x8000000 
#define SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MPLS_LM_DM_ILM              0x10000000 
#define SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_MPLS_TP_CC_CV                0x20000000 


#define SOC_PPC_OAM_Y1711_LM_MPLS          0x01        
#define SOC_PPC_OAM_Y1711_LM_PWE           0x02         



#define SOC_PPC_OAM_LIF_PROFILE_FLAG_COUNTED                0x01        
#define SOC_PPC_OAM_LIF_PROFILE_FLAG_SAT_LOOPBACK           0x02        

typedef enum
{
    
    SOC_PPC_OAM_OAMP_TRAP_ERR,
    
    SOC_PPC_OAM_OAMP_TYPE_ERR,
    
    SOC_PPC_OAM_OAMP_RMEP_ERR,
    
    SOC_PPC_OAM_OAMP_MAID_ERR,
    
    SOC_PPC_OAM_OAMP_MDL_ERR,
    
    SOC_PPC_OAM_OAMP_CCM_INTERVAL_ERR,
    
    SOC_PPC_OAM_OAMP_MY_DISC_ERR,
    
    SOC_PPC_OAM_OAMP_SRC_IP_ERR,
    
    SOC_PPC_OAM_OAMP_YOUR_DISC_ERR,
    
    SOC_PPC_OAM_OAMP_SRC_PORT_ERR,
    
    SOC_PPC_OAM_OAMP_RMEP_STATE_CHANGE,
    
    SOC_PPC_OAM_OAMP_PARITY_ERR,
    
    SOC_PPC_OAM_OAMP_TIMESTAMP_ERR,
    
    SOC_PPC_OAM_OAMP_PROTECTION,
    
    SOC_PPC_OAM_OAMP_CHANNEL_TYPE_ERR,
    
    SOC_PPC_OAM_OAMP_FLEX_CRC_ERR,
    
    SOC_PPC_OAM_OAMP_RFC_PUNT_ERR,
    SOC_PPC_OAM_OAMP_TRAP_TYPE_COUNT
} SOC_PPC_OAM_OAMP_TRAP_TYPE;

typedef enum
{
    
    SOC_PPC_OAM_EVENT_NULL=0,
    
    SOC_PPC_OAM_EVENT_LOC_SET=1,
    
    SOC_PPC_OAM_EVENT_ALMOST_LOC_SET=2,
    
    SOC_PPC_OAM_EVENT_LOC_CLR=3,
    
    SOC_PPC_OAM_EVENT_RDI_SET=4,
    
    SOC_PPC_OAM_EVENT_RDI_CLR=5,
    
    SOC_PPC_OAM_EVENT_RMEP_STATE_CHANGE=6,
    
    SOC_PPC_OAM_EVENT_REPORT_RX_LM=7,
    
    SOC_PPC_OAM_EVENT_REPORT_RX_DM=8,
    
    SOC_PPC_OAM_EVENT_SD_SET=9,
    
    SOC_PPC_OAM_EVENT_SD_CLR=10,
    
    SOC_PPC_OAM_EVENT_SF_SET=11,
    
    SOC_PPC_OAM_EVENT_SF_CLR=12,
    
    SOC_PPC_OAM_EVENT_DEXCESS_SET=13,
    
    SOC_PPC_OAM_EVENT_DMISSMATCH=14,
    
    SOC_PPC_OAM_EVENT_DMISSMERGE=15,
    
    SOC_PPC_OAM_EVENT_DALL_CLR=16,
    
    SOC_PPC_OAM_EVENT_SAT_FLOW_ID=17,
    SOC_PPC_OAM_EVENT_COUNT
} SOC_PPC_OAM_EVENT;



typedef enum {
  SOC_PPC_OAM_MP_TYPE_MATCH = 0,
  SOC_PPC_OAM_MP_TYPE_ABOVE,
  SOC_PPC_OAM_MP_TYPE_BELOW,
  SOC_PPC_OAM_MP_TYPE_BETWEEN,
  SOC_PPC_OAM_MP_TYPE_COUNT
} SOC_PPC_OAM_MP_TYPE;


typedef enum {
  SOC_PPC_OAM_MP_TYPE_MEP_OR_ACTIVE_MATCH_PLUS = 0,
  SOC_PPC_OAM_MP_TYPE_BELOW_PLUS,
  SOC_PPC_OAM_MP_TYPE_MIP_OR_PASSIVE_MATCH_PLUS,
  SOC_PPC_OAM_MP_TYPE_ABOVE_PLUS,
  SOC_PPC_OAM_MP_TYPE_COUNT_PLUS
} SOC_PPC_OAM_MP_TYPE_PLUS;


typedef enum {
    SOC_PPC_OAM_MP_TYPE_JERICHO_MIP_MATCH = 0,
    SOC_PPC_OAM_MP_TYPE_JERICHO_ACTIVE_MATCH = 1,
    SOC_PPC_OAM_MP_TYPE_JERICHO_PASSIVE_MATCH = 2,
    SOC_PPC_OAM_MP_TYPE_JERICHO_BELLOW_HIGHEST_MEP = 3,
    SOC_PPC_OAM_MP_TYPE_JERICHO_ABOVE_ALL = 4,
    SOC_PPC_OAM_MP_TYPE_JERICHO_BFD = 5,
    SOC_PPC_OAM_MP_TYPE_JERICHO_COUNT = 6 
} SOC_PPC_OAM_MP_TYPE_JERICHO; 


typedef enum {
    SOC_PPC_OAM_MP_TYPE_QAX_MIP_MATCH = 0,
    SOC_PPC_OAM_MP_TYPE_QAX_ACTIVE_MATCH = 1,
    SOC_PPC_OAM_MP_TYPE_QAX_PASSIVE_MATCH = 2,
    SOC_PPC_OAM_MP_TYPE_QAX_BETWEEN_MEPS = 3,
    SOC_PPC_OAM_MP_TYPE_QAX_BELOW_ALL = 4,
    SOC_PPC_OAM_MP_TYPE_QAX_ABOVE_ALL = 5,
    SOC_PPC_OAM_MP_TYPE_QAX_BFD = 6,

    SOC_PPC_OAM_MP_TYPE_QAX_COUNT
} SOC_PPC_OAM_MP_TYPE_QAX;



typedef enum {
    SOC_PPC_OAM_MP_TYPE_FOR_DEFAULT_EP_JERICHO_INGRESS_MATCH = 0,
    SOC_PPC_OAM_MP_TYPE_FOR_DEFAULT_EP_JERICHO_EGRESS_MATCH = 1,
    SOC_PPC_OAM_MP_TYPE_FOR_DEFAULT_EP_JERICHO_INGRESS_EGRESS_MATCH = 2,
    SOC_PPC_OAM_MP_TYPE_FOR_DEFAULT_EP_JERICHO_PASS = 3,
    SOC_PPC_OAM_MP_TYPE_FOR_DEFAULT_EP_JERICHO_ABOVE_ALL = 4,
    SOC_PPC_OAM_MP_TYPE_FOR_DEFAULT_EP_JERICHO_INVALID = 5,
    SOC_PPC_OAM_MP_TYPE_FOR_DEFAULT_EP_JERICHO_COUNT = 6
} SOC_PPC_OAM_MP_TYPE_FOR_DEFAULT_EP_JERICHO;


typedef enum {
  SOC_PPC_OAM_MEP_TYPE_ETH_OAM = 0,
  SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP = 1,
  SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE = 2,
  SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_1_HOP = 3,
  SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP = 4,
  SOC_PPC_OAM_MEP_TYPE_BFD_O_MPLS = 5,
  SOC_PPC_OAM_MEP_TYPE_BFD_O_PWE = 6,
  SOC_PPC_OAM_MEP_TYPE_BFDCC_O_MPLSTP = 7,
  SOC_PPC_OAM_MEP_TYPE_LM = 8,
  SOC_PPC_OAM_MEP_TYPE_LM_STAT = 9,
  SOC_PPC_OAM_MEP_TYPE_DM = 10,
  SOC_PPC_OAM_MEP_TYPE_EXT_DATA_HDR = 11, 
  SOC_PPC_OAM_MEP_TYPE_BFD_O_PWE_GAL = 12,  
  SOC_PPC_OAM_MEP_TYPE_EXT_DATA_PLD = 12, 
  SOC_PPC_OAM_MEP_TYPE_DM_ONE_WAY = 13, 


  
  SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE_GAL = 16,
  SOC_PPC_OAM_MEP_TYPE_Y1711_MPLS = 17, 
  SOC_PPC_OAM_MEP_TYPE_Y1711_PWE = 18,  
  
  SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP_SECTION = 19,
  SOC_PPC_OAM_MEP_TYPE_RFC6374_MPLS_LM_DM_PWE = 20,
  SOC_PPC_OAM_MEP_TYPE_RFC6374_MPLS_LM_DM_LSP = 21,
  SOC_PPC_OAM_MEP_TYPE_RFC6374_MPLS_LM_DM_SECTION = 22,
  SOC_PPC_OAM_MEP_TYPE_COUNT 
} SOC_PPC_OAM_MEP_TYPE;


#define SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV6_M_HOP SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP

typedef enum {
  SOC_PPC_OAM_LM_DM_ENTRY_TYPE_LM = 0,
  SOC_PPC_OAM_LM_DM_ENTRY_TYPE_LM_STAT,
    SOC_PPC_OAM_LM_DM_ENTRY_TYPE_DM,
  SOC_PPC_OAM_LM_DM_ENTRY_TYPE_NONE,
  SOC_PPC_OAM_LM_DM_ENTRY_TYPE_COUNT
} SOC_PPC_OAM_LM_DM_ENTRY_TYPE;

typedef enum {
  SOC_PPC_OAM_TRAP_ID_OAMP = 0,
  SOC_PPC_OAM_TRAP_ID_CPU,
  SOC_PPC_OAM_TRAP_ID_RECYCLE,
  SOC_PPC_OAM_TRAP_ID_SNOOP,
  SOC_PPC_OAM_TRAP_ID_ERR_LEVEL,
  SOC_PPC_OAM_TRAP_ID_ERR_PASSIVE,
  SOC_PPC_OAM_TRAP_ID_OAMP_Y1731_MPLS,
  SOC_PPC_OAM_TRAP_ID_OAMP_Y1731_PWE,
  SOC_PPC_OAM_TRAP_ID_SAT0_TST,
  SOC_PPC_OAM_TRAP_ID_SAT0_LB,
  SOC_PPC_OAM_TRAP_ID_RECYCLE_LBM,
  SOC_PPC_OAM_TRAP_ID_Y1711_MPLS,
  SOC_PPC_OAM_TRAP_ID_Y1711_PWE,
  SOC_PPC_OAM_TRAP_ID_COUNT
} SOC_PPC_OAM_TRAP_ID;

typedef enum {
  SOC_PPC_OAM_UPMEP_TRAP_ID_OAMP = 0,
  SOC_PPC_OAM_UPMEP_TRAP_ID_CPU,
  SOC_PPC_OAM_UPMEP_TRAP_ID_RECYCLE,
  SOC_PPC_OAM_UPMEP_TRAP_ID_SNOOP,
  SOC_PPC_OAM_UPMEP_TRAP_ID_ERR_LEVEL,
  SOC_PPC_OAM_UPMEP_TRAP_ID_ERR_PASSIVE,
  SOC_PPC_OAM_UPMEP_TRAP_ID_COUNT
} SOC_PPC_OAM_UPMEP_TRAP_ID;

typedef enum {
  SOC_PPC_OAM_MIRROR_ID_OAMP = 0,
  SOC_PPC_OAM_MIRROR_ID_CPU,
  SOC_PPC_OAM_MIRROR_ID_RECYCLE,
  SOC_PPC_OAM_MIRROR_ID_SNOOP,
  SOC_PPC_OAM_MIRROR_ID_CPU_RAW,
  SOC_PPC_OAM_MIRROR_ID_ERR_LEVEL,
  SOC_PPC_OAM_MIRROR_ID_ERR_PASSIVE,
  SOC_PPC_OAM_MIRROR_ID_RECYCLE_LBM,
  SOC_PPC_OAM_MIRROR_ID_COUNT
} SOC_PPC_OAM_MIRROR_ID;

typedef enum {
  SOC_PPC_BFD_TRAP_ID_CPU = 0,
  SOC_PPC_BFD_TRAP_ID_OAMP_IPV4,
  SOC_PPC_BFD_TRAP_ID_OAMP_MPLS,
  SOC_PPC_BFD_TRAP_ID_OAMP_PWE,
  SOC_PPC_BFD_TRAP_ID_OAMP_CC_MPLS_TP,
  SOC_PPC_BFD_TRAP_ID_UC_IPV6,
  SOC_PPC_BFD_TRAP_ID_COUNT
} SOC_PPC_BFD_TRAP_ID;


typedef enum {
    SOC_PPC_OAM_REPORT_MODE_NORMAL = 0,
    SOC_PPC_OAM_REPORT_MODE_COMPACT = 1,
    SOC_PPC_OAM_REPORT_MODE_RAW = 2,

    SOC_PPC_OAM_REPORT_MODE_COUNT
} SOC_PPC_OAM_REPORT_MODE;

typedef uint64 SOC_PPC_OAM_ICC_MAP_DATA;

typedef struct {
    SOC_SAND_MAGIC_NUM_VAR
    uint8 loc;
    uint8 rdi_set;
    uint8 rdi_clear;
    uint8 rmep_state_change;
    uint8 sd_set;
    uint8 sd_clear;
    uint8 sf_set;
    uint8 sf_clear;
    uint8 dExcess_set;
    uint8 dMissmatch;
    uint8 dMissmerge;
    uint8 dAll_clear;
    uint32 rmeb_db_ndx;
    uint32 rmep_state;
} SOC_PPC_OAM_EVENT_DATA;


typedef struct {
    
   int  last_interrupt_message_num; 
   
   uint32 interrupt_message[SOC_REG_ABOVE_64_MAX_SIZE_U32]; 

   
    
   uint32    * buffer_copied_from_dma_host_memory; 
   uint8 internal_buffer_is_allocated;
   
   int num_entries_available_in_local_buffer;
   
   int num_entries_read_in_local_buffer;


} SOC_PPC_OAM_INTERRUPT_GLOBAL_DATA; 


typedef enum {
    SOC_PPC_OAM_DMA_EVENT_TYPE_EVENT,
    SOC_PPC_OAM_DMA_EVENT_TYPE_STAT_EVENT,

    SOC_PPC_OAM_DMA_EVENT_TYPE_NOF
} SOC_PPC_OAM_DMA_EVENT_TYPE;


typedef enum {
    QAX_OAMP_FLEX_CRC_TCAM_48_BYTE_MAID = 0,
    QAX_OAMP_FLEX_CRC_TCAM_48_BYTE_MAID_CCM_COUNT = 1,
    QAX_OAMP_FLEX_CRC_TCAM_48_BYTE_MAID_EGRESS_INJ = 2,
    QAX_OAMP_FLEX_CRC_TCAM_RSRV3 = 3,
    QAX_OAMP_FLEX_CRC_TCAM_RSRV4 = 4,
    QAX_OAMP_FLEX_CRC_TCAM_RSRV5 = 5,
    QAX_OAMP_FLEX_CRC_TCAM_RSRV6 = 6,
    QAX_OAMP_FLEX_CRC_TCAM_RSRV7 = 7,
    QAX_OAMP_FLEX_CRC_TCAM_RSRV8 = 8,
    QAX_OAMP_FLEX_CRC_TCAM_RSRV9 = 9,
    QAX_OAMP_FLEX_CRC_TCAM_RSRV10 = 10,
    QAX_OAMP_FLEX_CRC_TCAM_RSRV11 = 11,
    QAX_OAMP_FLEX_CRC_TCAM_RSRV12 = 12,
    QAX_OAMP_FLEX_CRC_TCAM_RSRV13 = 13,
    QAX_OAMP_FLEX_CRC_TCAM_RSRV14 = 14,
    QAX_OAMP_FLEX_CRC_TCAM_RSRV15 = 15
} QAX_OAMP_FLEX_CRC_TCAM_t;


typedef enum {
    QAX_OAMP_FLEX_VER_MASK_TEMP_48_BYTE_MAID = 0,
    QAX_OAMP_FLEX_VER_MASK_TEMP_RSRV1 = 1,
    QAX_OAMP_FLEX_VER_MASK_TEMP_RSRV2 = 2,
    QAX_OAMP_FLEX_VER_MASK_TEMP_RSRV3 = 3,
    QAX_OAMP_FLEX_VER_MASK_TEMP_RSRV4 = 4,
    QAX_OAMP_FLEX_VER_MASK_TEMP_RSRV5 = 5,
    QAX_OAMP_FLEX_VER_MASK_TEMP_RSRV6 = 6,
    QAX_OAMP_FLEX_VER_MASK_TEMP_RSRV7 = 7
} QAX_OAMP_FLEX_VER_MASK_TEMP_t;


typedef int (*dma_event_handler_cb_t)(int, SOC_PPC_OAM_DMA_EVENT_TYPE, SOC_PPC_OAM_INTERRUPT_GLOBAL_DATA *) ;



typedef struct {
    int lif;
    int level;
    int opcode;
    int ing;
    int inj;
    int mymac;
    int bfd;

    int your_disc; 

} SOC_PPC_OAM_ACTION_KEY_PARAMS;


typedef struct {
    int ing; 
    int olo; 
    int oli; 
    
    int inj; 
    int pio; 
    int cp;  
    
    int ydv; 
    int leo; 
    int lei; 
} SOC_PPC_OAM_KEY_SELECT_PARAMS;




typedef struct {
    SOC_SAND_MAGIC_NUM_VAR
    uint32 event_size; 
    uint8 event_type; 
} SOC_PPC_OAM_RX_REPORT_EVENT_DATA;


typedef struct {
    SOC_SAND_MAGIC_NUM_VAR
    
    uint32             punt_rate;  
    
    uint32             punt_enable;
    
    uint32             rx_state_update_enable;
          
    uint32             scan_state_update_enable;  
    
    uint32             mep_rdi_update_loc_enable; 
    
    uint32             mep_rdi_update_loc_clear_enable; 
    
    uint32             mep_rdi_update_rx_enable;  
} SOC_PPC_OAM_OAMP_PUNT_PROFILE_DATA;

typedef struct {
    SOC_SAND_MAGIC_NUM_VAR
    SHR_BITDCL             counter_disable[_SHR_BITDCLSIZE(SOC_PPC_OAM_ETHERNET_PDU_OPCODE_COUNT)];
    SHR_BITDCL             meter_disable[_SHR_BITDCLSIZE(SOC_PPC_OAM_ETHERNET_PDU_OPCODE_COUNT)];
    uint32                 opcode_to_trap_code_unicast_map[SOC_PPC_OAM_OPCODE_MAP_COUNT];
    uint32                 opcode_to_trap_code_multicast_map[SOC_PPC_OAM_OPCODE_MAP_COUNT];
    uint8                  opcode_to_trap_strength_unicast_map[SOC_PPC_OAM_OPCODE_MAP_COUNT];
    uint8                  opcode_to_trap_strength_multicast_map[SOC_PPC_OAM_OPCODE_MAP_COUNT];
    uint8                  opcode_to_snoop_strength_unicast_map[SOC_PPC_OAM_OPCODE_MAP_COUNT];
    uint8                  opcode_to_snoop_strength_multicast_map[SOC_PPC_OAM_OPCODE_MAP_COUNT];
} SOC_PPC_OAM_MEP_PROFILE_DATA;

typedef struct {
    SOC_SAND_MAGIC_NUM_VAR
    uint8                  is_1588;
    SOC_PPC_OAM_MEP_PROFILE_DATA                  mep_profile_data;
    SOC_PPC_OAM_MEP_PROFILE_DATA                  mip_profile_data;           
    uint8                  flags;
    
    uint8                  mp_type_passive_active_mix;
    uint8                  is_piggybacked;
    uint8                  is_slm;
    
    uint8                  is_default;
} SOC_PPC_OAM_LIF_PROFILE_DATA;

typedef uint8 SOC_PPC_OAM_CPU_TRAP_CODE_TO_MIRROR_PROFILE_MAP[SOC_PPC_NOF_TRAP_CODES];

typedef uint8 SOC_PPC_OAM_MA_NAME[13];

typedef struct {
    SOC_SAND_MAGIC_NUM_VAR
    uint16             rmep_id;
    uint32             mep_index;           
} SOC_PPC_OAM_RMEP_INFO_DATA;

typedef struct {
    uint32 trap_ids[SOC_PPC_OAM_TRAP_ID_COUNT];
    uint32 upmep_trap_ids[SOC_PPC_OAM_UPMEP_TRAP_ID_COUNT];
    uint32 mirror_ids[SOC_PPC_OAM_MIRROR_ID_COUNT];
} SOC_PPC_OAM_INIT_TRAP_INFO;

typedef struct {
    uint32 trap_ids[SOC_PPC_BFD_TRAP_ID_COUNT];
} SOC_PPC_BFD_INIT_TRAP_INFO;

typedef struct {
    SOC_SAND_MAGIC_NUM_VAR
    uint8              rdi_received;     
    uint8              loc; 
    uint8              is_state_auto_handle;    
    uint8              is_event_mask;            
    uint8              punt_profile;    
    uint32             ccm_period;      
    uint32                loc_clear_threshold;  
    uint32                rmep_state;
    uint8               last_ccm_lifetime_valid_on_create; 
} SOC_PPC_OAM_OAMP_RMEP_DB_ENTRY;

typedef struct {
    SOC_SAND_MAGIC_NUM_VAR
    uint8              last_prd_pkt_cnt_1731;     
    uint8              last_prd_pkt_cnt_1711;     
    uint8              rx_err;                    
    uint8              loc;                        
} SOC_PPC_OAM_OAMP_RMEP_DB_EXT_ENTRY;

typedef struct {
    SOC_SAND_MAGIC_NUM_VAR
    uint8              ccm_tx_rate;             
    uint8              entry_format;             
    uint8              thresh_profile;          
    uint8              sf;                      
    uint8              sd;                      
    uint16             rmep_db_ptr;             
    uint16             sum_cnt;                  
    uint32             sliding_wnd_cntr[BCM_OAM_MAX_NUM_SLINDING_WINDOWS];      
} SOC_PPC_OAM_OAMP_SD_SF_DB_ENTRY;

typedef struct {
    SOC_SAND_MAGIC_NUM_VAR
    uint8            prd_pkt_cnt_0;       
    uint8            prd_pkt_cnt_1;       
    uint8            prd_pkt_cnt_2;       
    uint8            prd_err_ind_0;       
    uint8            prd_err_ind_1;       
    uint8            prd_err_ind_2;       
    uint8            d_excess ;           
    uint8            d_mismerge;          
    uint8            d_mismatch;          
    uint8            ccm_tx_rate;         
    uint8            allert_method ;      
   uint16            rmep_db_ptr;        
} SOC_PPC_OAM_OAMP_SD_SF_Y_1711_DB_ENTRY;

typedef struct {
    SOC_SAND_MAGIC_NUM_VAR
    uint8           wnd_lngth;          
    uint8           alert_method;       
    uint8           supress_alerts;     
    uint16         sd_set_thresh;      
    uint16         sd_clr_thresh;      
    uint16         sf_set_thresh;      
    uint16         sf_clr_thresh;      
} SOC_PPC_OAM_OAMP_SD_SF_PROFILE_DB;

typedef struct {
  SOC_SAND_MAGIC_NUM_VAR
  uint8                                 d_excess_thresh;
  uint8                                 clr_low_thresh;
  uint8                                 clr_high_thresh;
  uint8                                 num_entry;
} SOC_PPC_OAM_OAMP_SD_SF_1711_config;


#define SOC_PPC_OAM_FLEX_TOS_M_HOP_OR_FEAT_NOT_SET  0
#define SOC_PPC_OAM_FLEX_TOS_S_HOP  1
#define SOC_PPC_OAM_FLEX_TOS_MICRO_BFD  2


#define SOC_PPC_OAM_S_BFD_NONE       0
#define SOC_PPC_OAM_S_BFD_INITIATOR  1
#define SOC_PPC_OAM_S_BFD_REFLECTOR  2

typedef struct {
    SOC_SAND_MAGIC_NUM_VAR
    uint8 do_not_set_interval;
    uint8 ccm_interval; 
    uint8 is_upmep; 
    uint8 priority; 
    uint32 system_port;
    
    
    uint32 local_port; 
    SOC_PPC_OAM_MEP_TYPE  mep_type;
    uint16 remote_recycle_port; 

    
    SOC_SAND_PP_MAC_ADDRESS   src_mac_address; 
    uint8 mdl; 
    uint8 icc_ndx; 
    uint16 mep_id; 
    uint8 inner_tpid; 
    uint16 inner_vid_dei_pcp; 
    uint8 outer_tpid; 
    uint16 outer_vid_dei_pcp; 
    uint8 rdi; 
    uint8 tags_num;
    
    uint16 counter_pointer; 
    
    uint8 port_status_tlv_en;
    uint8 port_status_tlv_val; 
    uint8 interface_status_tlv_control; 
    uint8 src_mac_lsb; 
    uint8 src_mac_msb_profile; 
    uint8 is_ais_entry; 
    
    uint32 session_identifier_id; 
    uint32 timestamp_format; 
    uint8  is_ilm;           
    
    uint32 dst_ip_add;
    uint32 egress_if;       
    uint32 label;           
    uint32 remote_discr;    
    uint8  src_ip_add_ptr;
    uint8  push_profile;
    uint8  local_detect_mult; 
    uint8  min_rx_interval_ptr;    
    uint8  min_tx_interval_ptr;    
    uint8  tunnel_is_mpls;
    uint8  tos_ttl_profile;
    uint8 ip_subnet_len; 
    uint8 micro_bfd; 
    uint8 flex_tos_s_hop_m_bfd_flag; 
    uint8 s_bfd_flag;

    
    uint8  pbit;
    uint8  fbit;

    
    uint8  sta;
    uint8  diag_profile;
    uint8  flags_profile;
    uint8  bfd_pwe_router_alert; 
    uint8  bfd_pwe_ach; 
    uint8  bfd_pwe_gal; 

    
    uint8 is_11b_maid;

    uint32 maid_48_index; 
    uint8 is_maid_48;    
    uint8 is_mc;         

    
    uint32 lm_dm_ptr; 
    uint32 flex_data_ptr; 
    uint32 out_lif; 
    uint8 is_lsp;    
    uint8 is_vccv;    
    uint8 disc_type_update;
} SOC_PPC_OAM_OAMP_MEP_DB_ENTRY;


typedef struct {
    
    SOC_SAND_MAGIC_NUM_VAR
    SOC_PPC_OAM_LM_DM_ENTRY_TYPE entry_type; 
    uint8 name_type;
    uint32 mep_id; 
    uint32 allocated_id;  
    uint8 is_update; 

    uint8 is_1DM; 
    uint8 is_piggyback_down; 
    uint8 is_jumbo_dm; 
    uint8   is_slm; 
    uint32 lm_dm_id; 
} SOC_PPC_OAM_OAMP_LM_DM_MEP_DB_ENTRY;

typedef struct {
    SOC_SAND_MAGIC_NUM_VAR
        
    uint32 entry_id; 
    uint32 my_tx;
    uint32 my_rx;
    uint32 peer_tx;
    uint32 peer_rx;
        
    uint8 is_extended;
    uint32 last_lm_far;
    uint32 last_lm_near;
    uint32 acc_lm_far;
    uint32 acc_lm_near;
    uint32 max_lm_far;
    uint32 max_lm_near;

} SOC_PPC_OAM_OAMP_LM_INFO_GET;

typedef struct {
    SOC_SAND_MAGIC_NUM_VAR
    uint32 entry_id; 
    uint32 last_delay_sub_seconds; 
    uint32 last_delay_second; 
    uint32 max_delay_sub_seconds; 
    uint32 max_delay_second;
    uint32 min_delay_sub_seconds; 
    uint32 min_delay_second;
    int    Y1731_profile_index; 
} SOC_PPC_OAM_OAMP_DM_INFO_GET;


#define SOC_PPC_OAM_OAMP_ETH1731_MEP_PROFILE_MAX_CCM_CNT 0x7ffff

typedef struct {
    unsigned int piggy_back_lm : 1;
    unsigned int slm_lm :1;
	
    
    unsigned int maid_check_dis:1;
	
    
    unsigned int report_mode :2; 
    unsigned int rdi_gen_method :2;
    unsigned int lmm_da_oui_prof :3;
    unsigned int dmm_rate : 3;
    unsigned int lmm_rate : 3;
    
    unsigned int opcode_0_rate : 3;
    unsigned int opcode_1_rate : 3;
    
    unsigned int lmm_offset :8;
    unsigned int lmr_offset :8;
    unsigned int dmm_offset :8;
    unsigned int dmr_offset :8;
    
    unsigned int ccm_cnt : 20;
    unsigned int dmm_cnt : 20;
    unsigned int lmm_cnt : 20;
    unsigned int op_0_cnt : 20;
    unsigned int op_1_cnt : 20;
    
    unsigned int dmm_measure_one_way : 3;

}SOC_PPC_OAM_ETH1731_MEP_PROFILE_ENTRY;



typedef struct {
    uint32  *buffer;
    uint8 use_dma; 
    uint8 is_allocated;
} _oam_oam_a_b_table_buffer_t;


typedef struct {
    
    SOC_PPC_OAM_MEP_TYPE mep_type;
    SOC_PPC_LIF_ID lif;
    SOC_PPC_LIF_ID passive_side_lif; 
    uint32 tx_gport;                 
    uint32 your_discriminator;
    uint32 port; 
    uint8 port_core; 
    uint8 non_acc_profile; 
    uint8 non_acc_profile_passive; 
    uint8 acc_profile;
    uint32 flags; 
    uint8 src_mac_lsb; 
    
    uint8 md_level;
    SOC_SAND_PP_MAC_ADDRESS dst_mac_address;
    uint32 ma_index; 
    uint32 counter; 
    
    uint32 dip_profile_ndx; 
    uint32 remote_gport; 
    uint32 session_map_flag;  
    uint16 sd_sf_id;  
    uint16 y1711_sd_sf_id;  
    int sbfd_non_acc_src_ip_add_ptr; 
    uint8 is_rfc_6374_entry; 
} SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY;

typedef struct {
    uint8 flag;  
    bcm_oam_endpoint_t mepid;              
    int lb_tlv_num;
    bcm_oam_tlv_t tlv; 
    uint8                     pkt_pri;              
    uint8                     inner_pkt_pri;        
    bcm_cos_t                 int_pri;              
} SOC_PPC_OAM_SAT_GTF_ENTRY;

typedef struct {
    uint8 flag;  
    bcm_oam_endpoint_t mepid;              
    uint32 identifier_tc;          
    uint32 identifier_color;       
    uint32 identifier_trap_id;     
    uint32 identifier_session_id;  
    uint32 session_map_flag;  
    bcm_oam_tlv_t tlv; 
} SOC_PPC_OAM_SAT_CTF_ENTRY;


typedef struct {
    SOC_SAND_MAGIC_NUM_VAR
    uint8 ingress; 
    uint8 your_discr; 
     uint32 oam_lif;
} SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_KEY;

typedef struct {
    SOC_SAND_MAGIC_NUM_VAR
    uint8 mep_bitmap;  
    uint8 mip_bitmap;  
    uint8 mp_profile;
    uint16 mp_type_vector; 
    uint32 counter_ndx;
} SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD;

typedef struct {
    SOC_SAND_MAGIC_NUM_VAR
    uint8 ingress; 
    uint8 mdl;
    uint8 your_disc;
    uint32 oam_lif;
} SOC_PPC_OAM_CLASSIFIER_OEM2_ENTRY_KEY;

typedef struct {
    SOC_SAND_MAGIC_NUM_VAR
    uint8 mp_profile;
    uint16 oam_id;
} SOC_PPC_OAM_CLASSIFIER_OEM2_ENTRY_PAYLOAD;

typedef struct {
    SOC_SAND_MAGIC_NUM_VAR
    uint8 my_cfm_mac; 
    uint8 is_bfd;
    uint8 opcode;
    
    SOC_PPC_OAM_MP_TYPE mip_type;
    
    SOC_PPC_OAM_MP_TYPE mep_type;
    
    SOC_PPC_OAM_MP_TYPE_PLUS mp_type;
    
    SOC_PPC_OAM_MP_TYPE_JERICHO mp_type_jr;
    uint8 mp_profile;
    uint8 inject;
    uint8 ingress;
} SOC_PPC_OAM_CLASSIFIER_OAM1_ENTRY_KEY;

typedef struct {
    SOC_SAND_MAGIC_NUM_VAR
    uint8 my_cfm_mac; 
    uint8 is_bfd;
    uint8 opcode;
    uint8 mp_profile;
    uint8 inject;
    uint8 ingress;
} SOC_PPC_OAM_CLASSIFIER_OAM2_ENTRY_KEY;

typedef struct {
    SOC_SAND_MAGIC_NUM_VAR
    uint8 snoop_strength;
    uint8 forward_disable; 
    uint8 mirror_profile; 
    uint8 mirror_strength; 
    uint8 mirror_enable; 
    uint32 cpu_trap_code; 
    uint8 forwarding_strength;
    uint8 up_map;
    uint8 sub_type;
    uint8 meter_disable; 
    uint8 counter_disable;
} SOC_PPC_OAM_CLASSIFIER_OAM_ENTRY_PAYLOAD;

typedef struct {
    SOC_SAND_MAGIC_NUM_VAR
    SOC_PPC_PKT_TERM_TYPE ttc;
    uint32 fwd_code;
    uint32 mpls_label;
    uint32 channel_type;
    uint32 mdl;
    uint32 opcode;
    uint32 your_disc;
    uint32 ip_ttl;
} SOC_PPC_OAM_TCAM_ENTRY_KEY;

typedef struct {
    SOC_SAND_MAGIC_NUM_VAR
    uint32 type;
    uint32 is_oam;
    uint32 is_bfd;
    uint32 opcode;
    uint32 mdl;
    uint32 your_discr;
    uint32 oam_lif_tcam_result;
    uint32 oam_lif_tcam_result_valid;
    uint32 my_cfm_mac;
    uint32 oam_offset;
    uint32 oam_stamp_offset;
    uint32 oam_pcp;
} SOC_PPC_OAM_TCAM_ENTRY_ACTION;


typedef struct {
    SOC_SAND_MAGIC_NUM_VAR
    uint8 data[80]; 
    int len;
} SOC_PPC_OAM_OAMP_PROTECTION_HEADER;



#define SOC_PPC_OAMP_LENGTH_TTL                             8
#define SOC_PPC_OAMP_IPV4_TOS_TTL_LENGTH_TOS                 8
#define SOC_PPC_OAMP_IPV4_TOS_TTL_DATA_NOF_BITS             16
#define SOC_PPC_OAMP_IPV4_SRC_ADDR_DATA_NOF_BITS             32
#define SOC_PPC_OAMP_BFD_REQ_INTERVAL_DATA_NOF_BITS         32
#define SOC_PPC_OAMP_BFD_DIAG_PROFILE_NOF_BITS                5       
#define SOC_PPC_OAMP_BFD_FLAGS_PROFILE_NOF_BITS                6
#define SOC_PPC_OAMP_MPLS_PWE_PROFILE_NOF_BITS                 11
#define SOC_PPC_OAMP_MPLS_PWE_PROFILE_LENGTH_EXP             3

#define SOC_PPC_BFD_TX_MY_DISCRIMINATOR_RANGE_BIT_START           (SOC_DPP_DEFS_GET(unit,oam_2_id_nof_bits))

#define SOC_PPC_BFD_RX_YOUR_DISCRIMINATOR_RANGE_BIT_START           ( 16 + 2 *SOC_IS_JERICHO(unit)  )
#define SOC_PPC_BFD_DISCRIMINATOR_TO_LIF_START_MASK        ((1 <<SOC_PPC_OAM_SIZE_OF_OAM_KEY_IN_BITS(unit)) -1)
#define SOC_PPC_BFD_DISCRIMINATOR_TO_ACC_MEP_ID_START_MASK         ((1<<SOC_DPP_DEFS_GET(unit,oam_2_id_nof_bits)) -1) 

#define SOC_PPC_BFD_PDU_LENGTH  0x18
#define SOC_PPC_BFD_PDU_VERSION 0x1


#define SOC_PPC_OAM_OAMP_NUMBER_OF_PUNT_PROFILES(_unit)  SOC_DPP_DEFS_GET(_unit,oamp_number_of_punt_profiles)




#define SOC_PPC_OAM_AIS_PERIOD_ONE_SECOND_OPCODE_ENTRY 0
#define SOC_PPC_OAM_AIS_PERIOD_ONE_MINUTE_OPCODE_ENTRY 1


#define SOC_PPC_BFD_ACH_TYPE_PWE_CW     1
#define SOC_PPC_BFD_ACH_TYPE_GACH_CC    2
typedef struct {
    SOC_SAND_MAGIC_NUM_VAR
    uint8 bfd_vers;
    uint8 bfd_diag;
    uint8 bfd_sta;
    uint8 bfd_flags;
    uint8 bfd_length;
    uint32 bfd_req_min_echo_rx_interval;
} SOC_PPC_BFD_PDU_STATIC_REGISTER;

typedef struct {
    SOC_SAND_MAGIC_NUM_VAR
    SOC_PPC_BFD_PDU_STATIC_REGISTER bfd_static_reg_fields;
    uint32 bfd_my_discr;
    uint32 bfd_your_discr;
} SOC_PPC_BFD_CC_PACKET_STATIC_REGISTER;

typedef struct {
    SOC_SAND_MAGIC_NUM_VAR
    uint8 ttl;
    uint8 exp;
} SOC_PPC_MPLS_PWE_PROFILE_DATA;

typedef struct {
    SOC_SAND_MAGIC_NUM_VAR
    uint8 tos;
    uint8 ttl;
} SOC_PPC_BFD_IP_MULTI_HOP_TOS_TTL_DATA;

typedef struct {
    SOC_SAND_MAGIC_NUM_VAR
    uint8 tc;
    uint8 dp;
} SOC_PPC_OAMP_TX_ITMH_ATTRIBUTES;


#define SOC_PPC_OAM_OAMP_CRC_MASK_MSB_BIT_SIZE  (64)
#define SOC_PPC_OAM_OAMP_CRC_MASK_MSB_BYTE_SIZE (SOC_PPC_OAM_OAMP_CRC_MASK_MSB_BIT_SIZE / SOC_SAND_NOF_BITS_IN_BYTE)
#define SOC_PPC_OAM_OAMP_CRC_MASK_LSB_BYTE_SIZE (120)


typedef struct {
    uint8       msb_mask[SOC_PPC_OAM_OAMP_CRC_MASK_MSB_BYTE_SIZE];
    SHR_BITDCL  lsbyte_mask[_SHR_BITDCLSIZE(SOC_PPC_OAM_OAMP_CRC_MASK_LSB_BYTE_SIZE)];
} SOC_PPC_OAM_OAMP_CRC_MASK;

typedef struct {
    uint16                      crc16_val1;      
    uint16                      crc16_val2;      
    SOC_PPC_OAM_OAMP_CRC_MASK   mask;           
} SOC_PPC_OAM_OAMP_CRC_INFO;

typedef struct {
    int                         opcode_bmp;          
    int                         opcode_bmp_mask;     
    int                         mep_pe_profile;      
    int                         mep_pe_profile_mask; 
    uint8                       oam_bfd;             
    uint8                       oam_bfd_mask;        
    int                         mask_tbl_index;      
    uint8                       crc_select;          
} SOC_PPC_CRC_SELECT_INFO;



typedef struct {
    int                           mep_idx;        
    QAX_OAMP_FLEX_VER_MASK_TEMP_t mask_tbl_index; 
    QAX_OAMP_FLEX_CRC_TCAM_t      crc_tcam_index; 
    SOC_PPC_OAM_OAMP_CRC_INFO     crc_info;       
    SOC_PPC_CRC_SELECT_INFO       crc_tcam_info;  
} SOC_PPC_OAM_BFD_FLEXIBLE_VERIFICATION_INFO;


#define SOC_PPC_OAM_BFD_MEP_DB_EXT_DATA_FIRST_HDR_SIZE_BITS     (124)
#define SOC_PPC_OAM_BFD_MEP_DB_EXT_DATA_EXTRA_ENTRY_SIZE_BITS   (172)
#define SOC_PPC_OAM_BFD_MEP_DB_EXT_DATA_MAX_NOF_EXTRA_ENTRIES   (3)
#define SOC_PPC_OAM_BFD_MEP_DB_EXT_DATA_MAX_SIZE_BITS       \
        (SOC_PPC_OAM_BFD_MEP_DB_EXT_DATA_FIRST_HDR_SIZE_BITS \
         + SOC_PPC_OAM_BFD_MEP_DB_EXT_DATA_EXTRA_ENTRY_SIZE_BITS * SOC_PPC_OAM_BFD_MEP_DB_EXT_DATA_MAX_NOF_EXTRA_ENTRIES)
#define SOC_PPC_OAM_BFD_MEP_DB_EXT_DATA_MAX_SIZE_UINT32 (SOC_PPC_OAM_BFD_MEP_DB_EXT_DATA_MAX_SIZE_BITS / SOC_SAND_NOF_BITS_IN_UINT32 + 1)


#define SOC_PPC_OAM_BFD_MEP_DB_EXT_OPCODE_BITMAP_CCM_CCM_LM         (1 << 0)
#define SOC_PPC_OAM_BFD_MEP_DB_EXT_OPCODE_BITMAP_SLM_LMM            (1 << 1)
#define SOC_PPC_OAM_BFD_MEP_DB_EXT_OPCODE_BITMAP_DMM                (1 << 2)
#define SOC_PPC_OAM_BFD_MEP_DB_EXT_OPCODE_BITMAP_GEN_PDU_OPCODE_0   (1 << 3)
#define SOC_PPC_OAM_BFD_MEP_DB_EXT_OPCODE_BITMAP_GEN_PDU_OPCODE_1   (1 << 4)
#define SOC_PPC_OAM_BFD_MEP_DB_EXT_OPCODE_BITMAP_GEN_PDU_OPCODE_2   (1 << 5)
#define SOC_PPC_OAM_BFD_MEP_DB_EXT_OPCODE_BITMAP_GEN_PDU_OPCODE_3   (1 << 6)
#define SOC_PPC_OAM_BFD_MEP_DB_EXT_OPCODE_BITMAP_GEN_PDU_OPCODE_4   (1 << 7)
#define SOC_PPC_OAM_BFD_MEP_DB_EXT_OPCODE_BITMAP_GEN_PDU_OPCODE_5   (1 << 8)
#define SOC_PPC_OAM_BFD_MEP_DB_EXT_OPCODE_BITMAP_GEN_PDU_OPCODE_6   (1 << 9)
#define SOC_PPC_OAM_BFD_MEP_DB_EXT_OPCODE_BITMAP_GEN_PDU_OPCODE_7   (1 << 10)
#define SOC_PPC_OAM_BFD_MEP_DB_EXT_OPCODE_BITMAP_BFD                (1 << 11)
#define SOC_PPC_OAM_BFD_MEP_DB_EXT_OPCODE_BITMAP_RFC_LM             (1 << 12)
#define SOC_PPC_OAM_BFD_MEP_DB_EXT_OPCODE_BITMAP_RFC_DM             (1 << 13)

#define SOC_PPC_OAM_BFD_MEP_DB_EXT_OPCODE_BITMAP_LEGAL_OPCODE       ((1 << 14) - 1)

typedef struct {
    int         mep_idx;
    int         extension_idx;
    uint32      data[SOC_PPC_OAM_BFD_MEP_DB_EXT_DATA_MAX_SIZE_UINT32];
    int         data_size_in_bits;
    int         opcode_bmp; 
} SOC_PPC_OAM_BFD_MEP_DB_EXT_DATA_INFO;









void
  SOC_PPC_OAM_ETH_MP_INFO_clear(
    SOC_SAND_OUT SOC_PPC_OAM_ETH_MP_INFO *info
  );

#if SOC_PPC_DEBUG_IS_LVL1

const char*
  SOC_PPC_OAM_ETH_ACC_FUNC_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_OAM_ETH_ACC_FUNC_TYPE enum_val
  );

const char*
  SOC_PPC_OAM_ETH_MP_LEVEL_to_string(
    SOC_SAND_IN  SOC_PPC_OAM_ETH_MP_LEVEL enum_val
  );

void
  SOC_PPC_OAM_ETH_MP_INFO_print(
    SOC_SAND_IN  SOC_PPC_OAM_ETH_MP_INFO *info
  );

#endif 



void
  SOC_PPC_OAM_INIT_TRAP_INFO_clear(
    SOC_SAND_OUT SOC_PPC_OAM_INIT_TRAP_INFO *init_trap_info
  );

void
  SOC_PPC_OAM_MEP_PROFILE_DATA_clear(
    SOC_SAND_OUT SOC_PPC_OAM_MEP_PROFILE_DATA *profile_data
  );

void
  SOC_PPC_OAM_LIF_PROFILE_DATA_clear(
    SOC_SAND_OUT SOC_PPC_OAM_LIF_PROFILE_DATA *profile_data
  );

void
  SOC_PPC_OAM_RMEP_INFO_DATA_clear(
    SOC_SAND_OUT SOC_PPC_OAM_RMEP_INFO_DATA *rmep_info_data
  );

void
  SOC_PPC_OAM_OAMP_RMEP_DB_ENTRY_clear(
    SOC_SAND_OUT SOC_PPC_OAM_OAMP_RMEP_DB_ENTRY *rmep_db_entry
  );

void
  SOC_PPC_OAM_OAMP_MEP_DB_ENTRY_clear(
    SOC_SAND_OUT SOC_PPC_OAM_OAMP_MEP_DB_ENTRY *mep_db_entry
  );

void
  SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_clear(
    SOC_SAND_OUT SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY *classifier_entry
  );
void
  SOC_PPC_OAM_SAT_CTF_ENTRY_clear(
    SOC_SAND_OUT SOC_PPC_OAM_SAT_CTF_ENTRY *info
  );

void
  SOC_PPC_OAM_SAT_GTF_ENTRY_clear(
    SOC_SAND_OUT SOC_PPC_OAM_SAT_GTF_ENTRY *info
  );

void
  SOC_PPC_OAM_OAMP_LM_DM_MEP_DB_ENTRY_clear(
    SOC_SAND_OUT SOC_PPC_OAM_OAMP_LM_DM_MEP_DB_ENTRY *mep_db_entry
  );

void
  SOC_PPC_OAM_OAMP_DM_INFO_GET_clear(
    SOC_SAND_OUT SOC_PPC_OAM_OAMP_DM_INFO_GET *mep_db_entry
  );

void
  SOC_PPC_OAM_OAMP_LM_INFO_GET_clear(
    SOC_SAND_OUT SOC_PPC_OAM_OAMP_LM_INFO_GET *mep_db_entry
  );

void
  SOC_PPC_OAM_ETH1731_MEP_PROFILE_ENTRY_clear(
    SOC_SAND_OUT SOC_PPC_OAM_ETH1731_MEP_PROFILE_ENTRY *entry
  );


void
  SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD_clear(
    SOC_SAND_OUT SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD *oem1_payload
  );

void
  SOC_PPC_OAM_CLASSIFIER_OEM2_ENTRY_PAYLOAD_clear(
    SOC_SAND_OUT SOC_PPC_OAM_CLASSIFIER_OEM2_ENTRY_PAYLOAD *oem2_payload
  );

void
  SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_KEY_clear(
    SOC_SAND_OUT SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_KEY *oem1_key
  );

void
  SOC_PPC_OAM_CLASSIFIER_OEM2_ENTRY_KEY_clear(
    SOC_SAND_OUT SOC_PPC_OAM_CLASSIFIER_OEM2_ENTRY_KEY *oem2_key
  );

void
  SOC_PPC_OAM_CLASSIFIER_OAM_ENTRY_PAYLOAD_clear(
    SOC_SAND_OUT SOC_PPC_OAM_CLASSIFIER_OAM_ENTRY_PAYLOAD *oam1_payload
  );

void
  SOC_PPC_OAM_CLASSIFIER_OAM1_ENTRY_KEY_clear(
    SOC_SAND_OUT SOC_PPC_OAM_CLASSIFIER_OAM1_ENTRY_KEY *oam1_key
  );

void
  SOC_PPC_OAM_CLASSIFIER_OAM2_ENTRY_KEY_clear(
    SOC_SAND_OUT SOC_PPC_OAM_CLASSIFIER_OAM2_ENTRY_KEY *oam2_key
  );

void
  SOC_PPC_OAM_TCAM_ENTRY_KEY_clear(
    SOC_SAND_OUT SOC_PPC_OAM_TCAM_ENTRY_KEY *oam_tcam_key
  );

void
  SOC_PPC_OAM_TCAM_ENTRY_ACTION_clear(
    SOC_SAND_OUT SOC_PPC_OAM_TCAM_ENTRY_ACTION *oam_tcam_action
  );


void
  SOC_PPC_BFD_INIT_TRAP_INFO_clear(
    SOC_SAND_OUT SOC_PPC_BFD_INIT_TRAP_INFO *init_trap_info
  );

void
  SOC_PPC_BFD_PDU_STATIC_REGISTER_clear(
    SOC_SAND_OUT SOC_PPC_BFD_PDU_STATIC_REGISTER *bfd_pdu
  );

void
  SOC_PPC_BFD_CC_PACKET_STATIC_REGISTER_clear(
    SOC_SAND_OUT SOC_PPC_BFD_CC_PACKET_STATIC_REGISTER *bfd_cc_packet
  );

void
  SOC_PPC_MPLS_PWE_PROFILE_DATA_clear(
    SOC_SAND_OUT SOC_PPC_MPLS_PWE_PROFILE_DATA *mpls_pwe_profile
  );

void
  SOC_PPC_BFD_IP_MULTI_HOP_TOS_TTL_DATA_clear(
    SOC_SAND_OUT SOC_PPC_BFD_IP_MULTI_HOP_TOS_TTL_DATA *tos_ttl_data
  );

void
  SOC_PPC_OAMP_TX_ITMH_ATTRIBUTES_clear(
    SOC_SAND_OUT SOC_PPC_OAMP_TX_ITMH_ATTRIBUTES *itmh_attr
  );

void
  SOC_PPC_OAM_OAMP_PUNT_PROFILE_DATA_clear(
    SOC_SAND_OUT SOC_PPC_OAM_OAMP_PUNT_PROFILE_DATA *info
  );

void
  SOC_PPC_OAM_EVENT_DATA_clear(
    SOC_SAND_OUT SOC_PPC_OAM_EVENT_DATA *info
  );

void
  SOC_PPC_OAM_RX_REPORT_EVENT_DATA_clear(
    SOC_SAND_OUT SOC_PPC_OAM_RX_REPORT_EVENT_DATA *info
  );



void
  SOC_PPC_OAM_OAMP_RMEP_DB_ENTRY_print(
    SOC_SAND_IN SOC_PPC_OAM_OAMP_RMEP_DB_ENTRY *rmep_db_entry
  );

void
  SOC_PPC_OAM_OAMP_MEP_DB_ENTRY_print(
    SOC_SAND_IN SOC_PPC_OAM_OAMP_MEP_DB_ENTRY *mep_db_entry
  );

void
  SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_print(
    SOC_SAND_IN SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY *classifier_entry
  );

void
  SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD_print(
    SOC_SAND_IN SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD *oem1_payload
  );

void
  SOC_PPC_OAM_CLASSIFIER_OEM2_ENTRY_PAYLOAD_print(
    SOC_SAND_IN SOC_PPC_OAM_CLASSIFIER_OEM2_ENTRY_PAYLOAD *oem2_payload
  );

void
  SOC_PPC_OAM_CLASSIFIER_OAM_ENTRY_PAYLOAD_print(
    SOC_SAND_IN SOC_PPC_OAM_CLASSIFIER_OAM_ENTRY_PAYLOAD *oam1_payload
  );

void
  SOC_PPC_OAM_OAMP_RMEP_DB_EXT_ENTRY_clear(
    SOC_SAND_OUT SOC_PPC_OAM_OAMP_RMEP_DB_EXT_ENTRY *info
  );

void
  SOC_PPC_OAM_OAMP_RMEP_DB_EXT_ENTRY_print(
    SOC_SAND_IN  SOC_PPC_OAM_OAMP_RMEP_DB_EXT_ENTRY *rmep_ext_entry
  );

void
  SOC_PPC_OAM_OAMP_SD_SF_DB_ENTRY_clear(
    SOC_SAND_OUT SOC_PPC_OAM_OAMP_SD_SF_DB_ENTRY *info
  );

void
  SOC_PPC_OAM_OAMP_SD_SF_DB_ENTRY_print(
    SOC_SAND_IN  SOC_PPC_OAM_OAMP_SD_SF_DB_ENTRY *info
  );

void
  SOC_PPC_OAM_OAMP_SD_SF_Y_1711_DB_ENTRY_clear(
    SOC_SAND_OUT SOC_PPC_OAM_OAMP_SD_SF_Y_1711_DB_ENTRY *info
  );

void
  SOC_PPC_OAM_OAMP_SD_SF_Y_1711_DB_ENTRY_print(
    SOC_SAND_IN  SOC_PPC_OAM_OAMP_SD_SF_Y_1711_DB_ENTRY *info
  );

void
  SOC_PPC_OAM_OAMP_SD_SF_PROFILE_DB_ENTRY_clear(
    SOC_SAND_OUT SOC_PPC_OAM_OAMP_SD_SF_PROFILE_DB *info
  );

void
  SOC_PPC_OAM_OAMP_SD_SF_PROFILE_DB_ENTRY_print(
    SOC_SAND_IN  SOC_PPC_OAM_OAMP_SD_SF_PROFILE_DB *info
  );

#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif
