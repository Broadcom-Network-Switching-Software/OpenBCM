
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __ARAD_API_FLOW_CONTROL_INCLUDED__

#define __ARAD_API_FLOW_CONTROL_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Utils/sand_pp_mac.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/arad_api_general.h>
#include <soc/dpp/TMC/tmc_api_flow_control.h>
#include <soc/dpp/ARAD/arad_api_nif.h>















typedef enum
{
  
  ARAD_FC_OFP_PRIORITY_NONE = 0x0,
  
  ARAD_FC_OFP_PRIORITY_LP = 0x1,
  
  ARAD_FC_OFP_PRIORITY_HP = 0x3,
  
  ARAD_FC_NOF_OFP_PRIORITYS = 3
}ARAD_FC_OFP_PRIORITY;

typedef enum
{
  
  ARAD_FC_ILKN_ID_A = 0,

  
  ARAD_FC_ILKN_ID_B = 1,

  
  ARAD_FC_ILKN_IDS = 2
}ARAD_FC_ILKN_ID;

typedef enum
{
  
  ARAD_FC_CAL_TYPE_RX = 0,

  
  ARAD_FC_CAL_TYPE_TX = 1,

  
  ARAD_FC_CAL_TYPES = 2
}ARAD_FC_CAL_TYPE;

typedef SOC_TMC_FC_ILKN_CAL_LLFC ARAD_FC_ILKN_CAL_LLFC;

typedef SOC_TMC_FC_ILKN_LLFC_INFO ARAD_FC_ILKN_LLFC_INFO;


typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR

  
  uint32 enable;

  
  uint32 rx_polarity;
  
  uint32 tx_polarity;

  
  uint32 error_indication;

}ARAD_FC_ILKN_RETRANSMIT_INFO;

#if defined(BCM_88650_B0)
typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR

  
  uint32 enable;

  
  uint32 length;

}ARAD_FC_ILKN_RETRANSMIT_CAL_CFG;
#endif

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR

  
  uint32 enable;

  
  uint32 period;

  
  uint32 error_indication;

  
  uint32 crc_error;

}ARAD_FC_SCH_OOB_WD_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR

  
  uint32 enable;

  
  uint32 sch_range;

  
  ARAD_FC_SCH_OOB_WD_INFO wd_info;

}ARAD_FC_SCH_OOB_INFO;

typedef struct  
{
  SOC_SAND_MAGIC_NUM_VAR

  
  uint32 queue_pair;

  
  uint8 pfc_bm;

}ARAD_FC_PFC_BM_INFO;

typedef enum  
{
  ARAD_FC_OOB_TX_SPEED_CORE_2 = SOC_TMC_FC_OOB_TX_SPEED_CORE_2,
  ARAD_FC_OOB_TX_SPEED_CORE_4 = SOC_TMC_FC_OOB_TX_SPEED_CORE_4,
  ARAD_FC_OOB_TX_SPEED_CORE_8 = SOC_TMC_FC_OOB_TX_SPEED_CORE_8
}ARAD_FC_OOB_TX_SPEED;










void 
  ARAD_FC_ILKN_RETRANSMIT_INFO_clear(
    SOC_SAND_OUT ARAD_FC_ILKN_RETRANSMIT_INFO *info
  );

#if defined(BCM_88650_B0)
void 
  ARAD_FC_ILKN_RETRANSMIT_CAL_CFG_clear(
    SOC_SAND_OUT ARAD_FC_ILKN_RETRANSMIT_CAL_CFG *info
  );
#endif


int arad_fc_shr_mapping(int unit, int fcv_bit, int cl_index, int select, int valid);
int arad_fc_init_shr_mapping(int unit);



uint32
  arad_fc_gen_inbnd_glb_hp_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_TMC_FC_INGR_GEN_GLB_HP_MODE     fc_mode
  );



uint32
  arad_fc_gen_inbnd_glb_hp_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_OUT SOC_TMC_FC_INGR_GEN_GLB_HP_MODE     *fc_mode
  );



uint32
  arad_fc_nif_pause_frame_src_addr_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      mal_ndx,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS            *mac_addr
  );


#if defined(BCM_88650_B0)



uint32
  arad_fc_ilkn_retransmit_cal_set(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  ARAD_NIF_ILKN_ID                   ilkn_ndx,
    SOC_SAND_IN  ARAD_FC_DIRECTION                  direction_ndx,
    SOC_SAND_IN  ARAD_FC_ILKN_RETRANSMIT_CAL_CFG    *cal_cfg
  );

uint32
  arad_fc_ilkn_retransmit_cal_get(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  ARAD_NIF_ILKN_ID                   ilkn_ndx,
    SOC_SAND_IN  ARAD_FC_DIRECTION                  direction_ndx,
    SOC_SAND_OUT ARAD_FC_ILKN_RETRANSMIT_CAL_CFG    *cal_cfg
  );

#endif






#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

