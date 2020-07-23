/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __SOC_TMC_API_CNM_INCLUDED__

#define __SOC_TMC_API_CNM_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_pp_mac.h>

#include <soc/dpp/TMC/tmc_api_general.h>






#define  SOC_TMC_CNM_NOF_SAMPLING_BASES (8)


#define  SOC_TMC_CNM_NOF_UINT32S_IN_CP_ID (2)


#define  SOC_TMC_CNM_NOF_PROFILES (8)


#define  SOC_TM_CNM_MAX_CP_QUEUES      (8*1024)

#define  SOC_TCM_CNM_DEST_TM_PORT_AS_INCOMING  (0xffffffff)









typedef enum
{
  
  SOC_TMC_CNM_GEN_MODE_EXT_PP = 0,
  
  SOC_TMC_CNM_GEN_MODE_PETRA_B_PP = 1,
  
  SOC_TMC_CNM_GEN_MODE_SAMPLING = 2,

  
  SOC_TMC_CNM_GEN_MODE_DUNE_PP = 3,

  
  SOC_TMC_CNM_GEN_MODE_HIGIG = 4,

  
  SOC_TMC_CNM_NOF_GEN_MODES
}SOC_TMC_CNM_GEN_MODE;

typedef enum
{
  
  SOC_TMC_CNM_Q_SET_8_CPS = 0,
  
  SOC_TMC_CNM_Q_SET_4_CPS = 1,
  
  SOC_TMC_CNM_NOF_Q_SETS = 2
}SOC_TMC_CNM_Q_SET;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 q_base;
  
  SOC_TMC_CNM_Q_SET q_set;
  
  uint32 nof_queues;

} SOC_TMC_CNM_Q_MAPPING_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 is_mc_also;
  
  uint8 is_ingr_rep_also;
  
  uint8 is_snoop_also;
  
  uint8 is_mirr_also;

} SOC_TMC_CNM_CONGESTION_TEST_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 q_eq;
  
  int32 cpw_power;
  
  uint32 sampling_base[SOC_TMC_CNM_NOF_SAMPLING_BASES];
  
  uint32 max_neg_fb_value;
  
  uint32 quant_div;
  
  uint8 is_sampling_th_random;

} SOC_TMC_CNM_CP_PROFILE_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 is_cp_enabled;
  
  uint32 profile;

} SOC_TMC_CNM_CPQ_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 ing_vlan_edit_cmd;

} SOC_TMC_CNM_PPH;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 ether_type;
  
  uint32 version;
  
  uint32 res_v;
  
  uint32 cp_id_6_msb[SOC_TMC_CNM_NOF_UINT32S_IN_CP_ID];

} SOC_TMC_CNM_PDU;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_SAND_PP_MAC_ADDRESS mac_sa;
  
  SOC_TMC_CNM_PPH pph;
  
  SOC_TMC_CNM_PDU pdu;
  
  uint32 ether_type;

} SOC_TMC_CNM_PETRA_B_PP;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 dest_tm_port;
  
  uint32 tc;
  
  uint32 cp_id_4_msb;

  
  uint32 dp;

  
  
  uint32 qsig;

} SOC_TMC_CNM_PACKET;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 is_cp_enabled;
  
  SOC_TMC_CNM_GEN_MODE pkt_gen_mode;
  
  SOC_TMC_CNM_PACKET pckt;
  
  SOC_TMC_CNM_PETRA_B_PP pp;

} SOC_TMC_CNM_CP_INFO;









void
  SOC_TMC_CNM_Q_MAPPING_INFO_clear(
    SOC_SAND_OUT SOC_TMC_CNM_Q_MAPPING_INFO *info
  );

void
  SOC_TMC_CNM_CONGESTION_TEST_INFO_clear(
    SOC_SAND_OUT SOC_TMC_CNM_CONGESTION_TEST_INFO *info
  );

void
  SOC_TMC_CNM_CP_PROFILE_INFO_clear(
    SOC_SAND_OUT SOC_TMC_CNM_CP_PROFILE_INFO *info
  );

void
  SOC_TMC_CNM_CPQ_INFO_clear(
    SOC_SAND_OUT SOC_TMC_CNM_CPQ_INFO *info
  );

void
  SOC_TMC_CNM_PPH_clear(
    SOC_SAND_OUT SOC_TMC_CNM_PPH *info
  );

void
  SOC_TMC_CNM_PDU_clear(
    SOC_SAND_OUT SOC_TMC_CNM_PDU *info
  );

void
  SOC_TMC_CNM_PETRA_B_PP_clear(
    SOC_SAND_OUT SOC_TMC_CNM_PETRA_B_PP *info
  );

void
  SOC_TMC_CNM_PACKET_clear(
    SOC_SAND_OUT SOC_TMC_CNM_PACKET *info
  );

void
  SOC_TMC_CNM_CP_INFO_clear(
    SOC_SAND_OUT SOC_TMC_CNM_CP_INFO *info
  );

#if SOC_TMC_DEBUG_IS_LVL1

const char*
  SOC_TMC_CNM_GEN_MODE_to_string(
    SOC_SAND_IN  SOC_TMC_CNM_GEN_MODE enum_val
  );

const char*
  SOC_TMC_CNM_Q_SET_to_string(
    SOC_SAND_IN  SOC_TMC_CNM_Q_SET enum_val
  );

void
  SOC_TMC_CNM_Q_MAPPING_INFO_print(
    SOC_SAND_IN  SOC_TMC_CNM_Q_MAPPING_INFO *info
  );

void
  SOC_TMC_CNM_CONGESTION_TEST_INFO_print(
    SOC_SAND_IN  SOC_TMC_CNM_CONGESTION_TEST_INFO *info
  );

void
  SOC_TMC_CNM_CP_PROFILE_INFO_print(
    SOC_SAND_IN  SOC_TMC_CNM_CP_PROFILE_INFO *info
  );

void
  SOC_TMC_CNM_CPQ_INFO_print(
    SOC_SAND_IN  SOC_TMC_CNM_CPQ_INFO *info
  );

void
  SOC_TMC_CNM_PPH_print(
    SOC_SAND_IN  SOC_TMC_CNM_PPH *info
  );

void
  SOC_TMC_CNM_PDU_print(
    SOC_SAND_IN  SOC_TMC_CNM_PDU *info
  );

void
  SOC_TMC_CNM_PETRA_B_PP_print(
    SOC_SAND_IN  SOC_TMC_CNM_PETRA_B_PP *info
  );

void
  SOC_TMC_CNM_PACKET_print(
    SOC_SAND_IN  SOC_TMC_CNM_PACKET *info
  );

void
  SOC_TMC_CNM_CP_INFO_print(
    SOC_SAND_IN  SOC_TMC_CNM_CP_INFO *info
  );

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif
