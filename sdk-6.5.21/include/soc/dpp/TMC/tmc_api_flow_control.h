/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __SOC_TMC_API_FLOW_CONTROL_INCLUDED__

#define __SOC_TMC_API_FLOW_CONTROL_INCLUDED__





#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>

#include <soc/dpp/TMC/tmc_api_general.h>
#include <soc/dpp/TMC/tmc_api_ingress_traffic_mgmt.h>






typedef uint32 SOC_TMC_FC_CAL_IF_ID;

#define SOC_TMC_FC_NOF_PP_INTF     SOC_DPP_DEFS_MAX(NOF_CORES)

#define SOC_TMC_FC_OOB_CAL_LEN_MAX 512

#define SOC_TMC_FC_PFC_GENERIC_BITMAP_SIZE 256

#define SOC_TMC_FC_HCFC_BITMAP_SIZE 512

#define SOC_TMC_FC_ILKN_MUB_GEN_CAL_LEN 8

#define  SOC_TMC_FC_OOB_CAL_REP_MIN 1
#define  SOC_TMC_FC_OOB_CAL_REP_MAX 15

#define  SOC_TMC_FC_HCFC_CHANNEL_NUM 5


#define  SOC_TMC_FC_CAL_PAUSE_RESOLUTION_CLK_MAX    0x2000

typedef enum  
{
  SOC_TMC_FC_OOB_TX_SPEED_CORE_2 = 0,
    
  SOC_TMC_FC_OOB_TX_SPEED_CORE_4 = 1,
  
  SOC_TMC_FC_OOB_TX_SPEED_CORE_8 = 2,
  
  SOC_TMC_FC_NOF_OOB_TX_SPEED = 3
}SOC_TMC_FC_OOB_TX_SPEED;

typedef enum
{
    SOC_TMC_FC_EN = 0,

    SOC_TMC_FC_EGQ_TO_NIF_CNM_LLFC_EN = 1, 

    SOC_TMC_FC_EGQ_TO_NIF_CNM_PFC_EN = 2, 

    SOC_TMC_FC_EGQ_TO_SCH_DEVICE_EN = 3,

    SOC_TMC_FC_EGQ_TO_SCH_ERP_EN = 4, 
    
    SOC_TMC_FC_EGQ_TO_SCH_ERP_TC_EN = 5, 

    SOC_TMC_FC_EGQ_TO_SCH_IF_EN = 6,
    
    SOC_TMC_FC_EGQ_TO_SCH_PFC_EN = 7,
    
    SOC_TMC_FC_GLB_RSC_TO_EGQ_RCL_PFC_EN = 8, 

    SOC_TMC_FC_GLB_RSC_TO_HCFC_HP_CFG = 9, 

    SOC_TMC_FC_GLB_RSC_TO_HCFC_LP_CFG = 10, 

    SOC_TMC_FC_GLB_RSC_TO_NIF_LLFC_EN = 11, 
    
    SOC_TMC_FC_GLB_RSC_TO_NIF_PFC_EN = 12, 

    SOC_TMC_FC_GLB_RSC_TO_RCL_PFC_HP_CFG = 13, 

    SOC_TMC_FC_GLB_RSC_TO_RCL_PFC_LP_CFG = 14, 

    SOC_TMC_FC_ILKN_RX_TO_EGQ_PFC_EN = 15, 

    SOC_TMC_FC_ILKN_RX_TO_EGQ_PORT_EN = 16, 
    
    SOC_TMC_FC_ILKN_RX_TO_GEN_PFC_EN = 17, 

    SOC_TMC_FC_ILKN_RX_TO_NIF_FAST_LLFC_EN = 18, 

    SOC_TMC_FC_ILKN_RX_TO_RET_REQ_EN = 19, 

    SOC_TMC_FC_LLFC_VSQ_TO_NIF_EN = 20, 
    
    SOC_TMC_FC_NIF_TO_GEN_PFC_EN = 21, 

    SOC_TMC_FC_PFC_VSQ_TO_NIF_EN = 22, 

    SOC_TMC_FC_SCH_OOB_RX_EN = 23, 

    SOC_TMC_FC_SPI_OOB_RX_TO_EGQ_PFC_EN = 24, 

    SOC_TMC_FC_SPI_OOB_RX_TO_EGQ_PORT_EN = 25, 

    SOC_TMC_FC_SPI_OOB_RX_TO_GEN_PFC_EN = 26, 

    SOC_TMC_FC_SPI_OOB_RX_TO_NIF_FAST_LLFC_EN = 27, 

    SOC_TMC_FC_SPI_OOB_RX_TO_RET_REQ_EN = 28, 

    SOC_TMC_FC_STAT_VSQ_TO_HCFC_EN = 29,

    SOC_TMC_FC_EGQ_TO_SCH_LAG_EN = 30,
    SOC_TMC_FC_NOF_ENABLEs = 31
}SOC_TMC_FC_ENABLE;

typedef enum
{
  
  SOC_TMC_FC_OOB_ID_A = 0,
  
  SOC_TMC_FC_OOB_ID_B = 1,
  
  SOC_TMC_FC_NOF_OOB_IDS = 2
}SOC_TMC_FC_OOB_ID;

typedef enum
{
  
  SOC_TMC_FC_CAL_MODE_SPI_OOB = 0,
  
  SOC_TMC_FC_CAL_MODE_ILKN_INBND = 1,
  
  SOC_TMC_FC_CAL_MODE_ILKN_OOB = 2,
  
  SOC_TMC_FC_CAL_MODE_IHB_OOB = 3,
  
  SOC_TMC_FC_CAL_MODE_IHB_INBND = 4,
  
  SOC_TMC_FC_NOF_CAL_MODES = 5
}SOC_TMC_FC_CAL_MODE;

typedef enum
{

  SOC_TMC_FC_CAL_TYPE_NONE = 0,
  SOC_TMC_FC_CAL_TYPE_SPI  = 1,
  SOC_TMC_FC_CAL_TYPE_ILKN = 2,
  SOC_TMC_FC_CAL_TYPE_HCFC = 3,
  SOC_TMC_FC_CAL_TYPE_COE  = 4,
  SOC_TMC_FC_CAL_TYPE_E2E  = 5,
  SOC_TMC_FC_NOF_TYPES
}SOC_TMC_FC_OOB_TYPE;

typedef enum
{
  SOC_TMC_FC_CAL_INB_TYPE_NONE = 0,
  SOC_TMC_FC_CAL_INB_TYPE_ILKN = 1,
  SOC_TMC_FC_CAL_INB_TYPE_COE  = 2,
  SOC_TMC_FC_CAL_INB_TYPE_E2E  = 3,
  SOC_TMC_FC_CAL_INB_NOF_TYPES
}SOC_TMC_FC_CAL_INB_TYPE;

typedef enum
{
  SOC_TMC_FC_COE_MODE_PAUSE  = 0,
  SOC_TMC_FC_COE_MODE_PFC = 1,
  SOC_TMC_FC_NOF_COE_MODES = 2
}SOC_TMC_FC_COE_MODE;

typedef enum
{
  SOC_TMC_FC_E2E_STATUS_SIZE_8B  = 0,
  SOC_TMC_FC_E2E_STATUS_SIZE_16B = 1,
  SOC_TMC_FC_E2E_STATUS_SIZE_24B = 2,
  SOC_TMC_FC_E2E_STATUS_SIZE_32B = 3,
  SOC_TMC_FC_E2E_STATUS_SIZE_48B = 4,
  SOC_TMC_FC_E2E_STATUS_SIZE_64B = 5,
  SOC_TMC_FC_NOF_E2E_STATUS_SIZE
}SOC_TMC_FC_E2E_STATUS_TYPE;

typedef enum
{
  
  SOC_TMC_FC_INGR_GEN_GLB_HP_MODE_NONE = 0,
  
  SOC_TMC_FC_INGR_GEN_GLB_HP_MODE_LL = 1,
  
  SOC_TMC_FC_INGR_GEN_GLB_HP_MODE_CB = 2,

  SOC_TMC_FC_INGR_GEN_GLB_HP_MODE_PFC = 3,

  
  SOC_TMC_FC_INGR_GEN_GLB_HP_MODE_ALL = 4,
  
  SOC_TMC_FC_NOF_INGR_GEN_GLB_HP_MODES
}SOC_TMC_FC_INGR_GEN_GLB_HP_MODE;

typedef enum
{
  
  SOC_TMC_FC_INBND_PFC_INHERIT_DISABLED = 0,
  
  SOC_TMC_FC_INBND_PFC_INHERIT_UP = 1,
  
  SOC_TMC_FC_INBND_PFC_INHERIT_DOWN = 2,
  
  SOC_TMC_FC_NOF_INBND_PFC_INHERITS = 3
}SOC_TMC_FC_INBND_PFC_INHERIT;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_FC_INBND_PFC_INHERIT inherit;
  
  uint8 sch_hr_bitmap;

} SOC_TMC_FC_REC_INBND_PFC;

typedef enum
{
  
  SOC_TMC_FC_REC_OFP_RP_SCH = 0x1,
  
  SOC_TMC_FC_REC_OFP_RP_EGQ = 0x2,
  
  SOC_TMC_FC_NOF_REC_OFP_RPS = 3
}SOC_TMC_FC_REC_OFP_RP;

typedef enum
{
  
  SOC_TMC_FC_OFP_PRIORITY_NONE = 0x0,
  
  SOC_TMC_FC_OFP_PRIORITY_LP = 0x1,
  
  SOC_TMC_FC_OFP_PRIORITY_HP = 0x3,
  
  SOC_TMC_FC_NOF_OFP_PRIORITYS = 3
}SOC_TMC_FC_OFP_PRIORITY;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_FC_REC_OFP_RP react_point;
  
  uint32 ofp_ndx;
  
  SOC_TMC_FC_OFP_PRIORITY priority;

} SOC_TMC_FC_REC_OFP_MAP_INFO;

typedef enum
{
  
  SOC_TMC_FC_INBND_MODE_DISABLED = 0,
  
  SOC_TMC_FC_INBND_MODE_LL = 1,
  
  SOC_TMC_FC_INBND_MODE_CB = 2,
  
  SOC_TMC_FC_NOF_INBND_MODES_PB = 3,
   
   SOC_TMC_FC_INBND_MODE_PFC = SOC_TMC_FC_NOF_INBND_MODES_PB,
   
   SOC_TMC_FC_INBND_MODE_SAFC = 4,

   SOC_TMC_FC_INBND_MODE_DEVICE_DISABLED = 5,

   SOC_TMC_FC_NOF_INBND_MODES = 6
}SOC_TMC_FC_INBND_MODE;

typedef enum
{
  
  SOC_TMC_FC_GEN_SRC_NONE = 0,
  
  SOC_TMC_FC_GEN_SRC_VSQ_LLFC = 1,
  
  SOC_TMC_FC_GEN_SRC_VSQ_PFC = 2,
  
  SOC_TMC_FC_GEN_SRC_GLB_HIGH = 3,
  
  SOC_TMC_FC_GEN_SRC_GLB_LOW = 4,
  
  SOC_TMC_FC_GEN_SRC_VSQ_CAT2TC = 5,
  
  SOC_TMC_FC_GEN_SRC_NIF = 6,
  
  SOC_TMC_FC_GEN_SRC_CNM = 7,

  SOC_TMC_FC_NOF_GEN_SRC = 18
}SOC_TMC_FC_GEN_SRC;


typedef enum
{
  
  SOC_TMC_FC_GLB_RES_TYPE_BDB = 0,
  
  SOC_TMC_FC_GLB_RES_TYPE_MINI_MC_DB = 1,
  
  SOC_TMC_FC_GLB_RES_TYPE_MC_DB = 2,
  
  SOC_TMC_FC_GLB_RES_TYPE_OCB_DB = 3,
  
  SOC_TMC_FC_GLB_RES_TYPE_MIX_P0 = 4,
  
  SOC_TMC_FC_GLB_RES_TYPE_MIX_P1 = 5,
  
  SOC_TMC_FC_GLB_RES_TYPE_OCB_P0 = 6,
  
  SOC_TMC_FC_GLB_RES_TYPE_OCB_P1 = 7,  

  
  SOC_TMC_FC_GLB_RES_TYPE_OCB = 8,

  SOC_TMC_FC_GLB_RES_TYPE_DRAM = 9,

  SOC_TMC_FC_GLB_RES_TYPE_POOL0 = 10,

  SOC_TMC_FC_GLB_RES_TYPE_POOL1 = 11,

  SOC_TMC_FC_GLB_RES_TYPE_OCB_HEADROOM = 12,

  SOC_TMC_FC_NOF_GLB_RES_TYPE = 13
}SOC_TMC_FC_GLB_RES_TYPE;

typedef enum
{
  
  SOC_TMC_FC_INBND_CB_INHERIT_DISABLED = 0,
  
  SOC_TMC_FC_INBND_CB_INHERIT_UP = 1,
  
  SOC_TMC_FC_INBND_CB_INHERIT_DOWN = 2,
  
  SOC_TMC_FC_NOF_INBND_CB_INHERITS = 3
}SOC_TMC_FC_INBND_CB_INHERIT;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR

  SOC_TMC_FC_GEN_SRC gen_src;
  
  
  SOC_TMC_FC_INBND_PFC_INHERIT inherit;

  
  uint8 vsq_pfc_enable;

  
  uint32 is_ocb_only;


  
  uint32 glbl_rcs_pool;
  
  
  uint32 glbl_rcs_low;

  
  uint32 glbl_rcs_high;

  
  
  uint8 cnm_intercept_enable;

  uint32 cnm_pfc_channel;
  
  uint8 nif_cls_bitmap;

  uint8 cat2_tc;

  uint32 cat2_tc_bitmap;

  
 
  
  uint32 is_headroom;

  
  uint32 is_mask_pool;
} SOC_TMC_FC_GEN_INBND_PFC;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 enable;
  
  uint32 cal_len;
  
  uint32 cal_reps;

} SOC_TMC_FC_CAL_IF_INFO;

typedef enum
{
  SOC_TMC_FC_GEN_CAL_SRC_STE = 0,
  
  SOC_TMC_FC_GEN_CAL_SRC_NIF = 1,
  
  SOC_TMC_FC_GEN_CAL_SRC_GLB_HP = 2,
  
  SOC_TMC_FC_GEN_CAL_SRC_GLB_LP = 3,
  
  SOC_TMC_FC_GEN_CAL_SRC_NONE = 4,
  
  SOC_TMC_FC_NOF_GEN_CAL_SRCS_PB = 5,

  
  SOC_TMC_FC_GEN_CAL_SRC_LLFC_VSQ = SOC_TMC_FC_NOF_GEN_CAL_SRCS_PB,

  
  SOC_TMC_FC_GEN_CAL_SRC_PFC_VSQ,

  
  SOC_TMC_FC_GEN_CAL_SRC_GLB_RCS,

  
  SOC_TMC_FC_GEN_CAL_SRC_HCFC,
  
  
  SOC_TMC_FC_GEN_CAL_SRC_LLFC,

  
  SOC_TMC_FC_GEN_CAL_SRC_RETRANSMIT,

  
  SOC_TMC_FC_GEN_CAL_SRC_CONST,

  
  SOC_TMC_FC_GEN_CAL_SRC_STTSTCS_TAG,

  
  SOC_TMC_FC_NOF_GEN_CAL_SRCS
}SOC_TMC_FC_GEN_CAL_SRC;

typedef enum
{
  
  SOC_TMC_FC_CAL_GLB_RCS_ID_BDB = 0,
  
  SOC_TMC_FC_CAL_GLB_RCS_ID_MIX = SOC_TMC_FC_CAL_GLB_RCS_ID_BDB,
  
  SOC_TMC_FC_CAL_GLB_RCS_ID_UNI = 1,
  
  SOC_TMC_FC_CAL_GLB_RCS_ID_OCB = SOC_TMC_FC_CAL_GLB_RCS_ID_UNI,
  
  SOC_TMC_FC_CAL_GLB_RCS_ID_MUL = 2,
  
  SOC_TMC_FC_CAL_GLB_RCS_ID_P0 = SOC_TMC_FC_CAL_GLB_RCS_ID_MUL,
  
  SOC_TMC_FC_NOF_CAL_GLB_RCS_IDS_ARAD = 3,
  
  SOC_TMC_FC_CAL_GLB_RCS_ID_OCB_DB = SOC_TMC_FC_NOF_CAL_GLB_RCS_IDS_ARAD,
  
  SOC_TMC_FC_CAL_GLB_RCS_ID_P1 = SOC_TMC_FC_NOF_CAL_GLB_RCS_IDS_ARAD,
  
  SOC_TMC_FC_CAL_GLB_RCS_ID_MIX_P0 = 4,
  
  SOC_TMC_FC_CAL_GLB_RCS_ID_HEADROOM = SOC_TMC_FC_CAL_GLB_RCS_ID_MIX_P0,
  
  SOC_TMC_FC_CAL_GLB_RCS_ID_MIX_P1 = 5,
  
  SOC_TMC_FC_NOF_CAL_GLB_RCS_IDS_QAX = SOC_TMC_FC_CAL_GLB_RCS_ID_MIX_P1,
  
  SOC_TMC_FC_CAL_GLB_RCS_ID_OCB_P0 = 6,
  
  SOC_TMC_FC_CAL_GLB_RCS_ID_OCB_P1 = 7,
  
  SOC_TMC_FC_NOF_CAL_GLB_RCS_IDS = 8
}SOC_TMC_FC_CAL_GLB_RCS_ID;

typedef enum
{
  
  SOC_TMC_FC_REC_CAL_DEST_HR = 0,
  
  SOC_TMC_FC_REC_CAL_DEST_OFP_EGQ_LP = 1,
  
  SOC_TMC_FC_REC_CAL_DEST_OFP_EGQ_HP = 2,
  
  SOC_TMC_FC_REC_CAL_DEST_OFP_SCH_HR_LP = 3,
  
  SOC_TMC_FC_REC_CAL_DEST_OFP_SCH_HR_HP = 4,
  
  SOC_TMC_FC_REC_CAL_DEST_NIF = 5,
  
  SOC_TMC_FC_REC_CAL_DEST_NONE = 6,
  
  SOC_TMC_FC_NOF_REC_CAL_DESTS_PB = 7,

  
  SOC_TMC_FC_REC_CAL_DEST_PFC = SOC_TMC_FC_NOF_REC_CAL_DESTS_PB,

  
  SOC_TMC_FC_REC_CAL_DEST_NIF_LL,

  
  SOC_TMC_FC_REC_CAL_DEST_PORT_2_PRIORITY,

  
  SOC_TMC_FC_REC_CAL_DEST_PORT_8_PRIORITY,

  
  SOC_TMC_FC_REC_CAL_DEST_GENERIC_PFC,

  
  SOC_TMC_FC_REC_CAL_DEST_RETRANSMIT,

  SOC_TMC_FC_NOF_REC_CAL_DESTS
}SOC_TMC_FC_REC_CAL_DEST;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_FC_GEN_CAL_SRC source;

  uint32 id;

} SOC_TMC_FC_GEN_CALENDAR;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_FC_REC_CAL_DEST destination;

  uint32 id;
  
} SOC_TMC_FC_REC_CALENDAR;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR

  SOC_TMC_FC_GEN_SRC gen_src;

  
  uint32 is_ocb_only;


  
  uint32 glbl_rcs_pool;
  
  
  uint8 cnm_enable;

  
  uint8 glbl_rcs_enable;

  
  uint8 vsq_llfc_enable;

  
  uint8 nif_enable;

  

  
  uint32 is_headroom;

  
  uint32 is_mask_pool;
} SOC_TMC_FC_GEN_INBND_LL;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_FC_INBND_CB_INHERIT inherit;
  
  uint8 sch_hr_bitmap;

} SOC_TMC_FC_REC_INBND_CB;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_FC_INBND_CB_INHERIT inherit;
  
  uint32 glbl_rcs_low;
  
  uint8 cnm_intercept_enable;
  
  uint8 nif_cls_bitmap;

} SOC_TMC_FC_GEN_INBND_CB;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_FC_INBND_MODE mode;
  
  SOC_TMC_FC_GEN_INBND_CB cb;
  
  SOC_TMC_FC_GEN_INBND_LL ll;

  
  SOC_TMC_FC_GEN_INBND_PFC pfc;

  
  int core;

} SOC_TMC_FC_GEN_INBND_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_FC_INBND_MODE mode;
  
  SOC_TMC_FC_REC_INBND_CB cb;

  SOC_TMC_FC_REC_INBND_PFC pfc;

} SOC_TMC_FC_REC_INBND_INFO;

typedef struct 
{
  
  int core;
  
  
  uint32 bitmap[SOC_TMC_FC_PFC_GENERIC_BITMAP_SIZE / 32];
}SOC_TMC_FC_PFC_GENERIC_BITMAP;

typedef struct 
{
  
  uint32 bitmap[SOC_TMC_FC_HCFC_BITMAP_SIZE / 32];
}SOC_TMC_FC_HCFC_BITMAP;

typedef struct 
{
  
  SHR_BITDCL bmp[_SHR_BITDCLSIZE(SOC_TMC_FC_NOF_ENABLEs)]; 
}SOC_TMC_FC_ENABLE_BITMAP;

typedef enum
{
  
  SOC_TMC_FC_PFC_MAP_EGQ = 0,
  
  SOC_TMC_FC_PFC_MAP_GENERIC_BITMAP = 1,
  
  SOC_TMC_FC_NOF_PFC_MAP_MODE = 2
}SOC_TMC_FC_PFC_MAP_MODE;

typedef struct 
{
  
  uint32 index;

  SOC_TMC_FC_PFC_MAP_MODE mode;

  uint32 valid;
  
}SOC_TMC_FC_PFC_MAP;

typedef enum
{
  
  SOC_TMC_FC_ILKN_CAL_LLFC_NONE = 0,
  
  SOC_TMC_FC_ILKN_CAL_LLFC_CH_0 = 1,
  
  SOC_TMC_FC_ILKN_CAL_LLFC_CH_16N = 2,
  
  SOC_TMC_FC_NOF_ILKN_CAL_LLFCS = 3
}SOC_TMC_FC_ILKN_CAL_LLFC;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 multi_use_mask;
  
  SOC_TMC_FC_ILKN_CAL_LLFC cal_channel;

} SOC_TMC_FC_ILKN_LLFC_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR

  SOC_TMC_FC_GEN_CALENDAR entries[SOC_TMC_FC_ILKN_MUB_GEN_CAL_LEN];
} SOC_TMC_FC_ILKN_MUB_GEN_CAL;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_THRESH_WITH_HYST_INFO llfc;
  
  SOC_TMC_THRESH_WITH_HYST_INFO pfc;

} SOC_TMC_FC_PORT_FIFO_TH;


typedef struct 
{
  
  uint32 enable;

  
  uint32 period;

  
  uint32 error_status;

}SOC_TMC_FC_HCFC_WATCHDOG;

typedef enum
{
  SOC_TMC_FC_PFC_GEN_BMP_SRC_NIF =      0,
  SOC_TMC_FC_PFC_GEN_BMP_SRC_OOB =      1,
  SOC_TMC_FC_PFC_GEN_BMP_SRC_ILKN_INB = 2,
  SOC_TMC_FC_PFC_GEN_BMP_SRC_MUB =      3,
  SOC_TMC_FC_PFC_GEN_BMP_SRC_TYPES
}SOC_TMC_FC_PFC_GEN_BMP_SRC_TYPE;

typedef struct {
  uint32 nif_pfc_gen_bmp_used[8];
  uint32 cal_pfc_gen_bmp_used[8];
  uint32 ilkn_inb_pfc_gen_bmp_used[8];
  uint32 mub_pfc_gen_bmp_used[8];
} SOC_TMC_FC_PFC_GEN_BMP_INFO;

#define SOC_TMC_FC_INTF_TYPE_NIF      0
#define SOC_TMC_FC_INTF_TYPE_ILKN     1
#define SOC_TMC_FC_INTF_TYPE_MUB      2
#define SOC_TMC_FC_INTF_TYPE_SPI      3
#define SOC_TMC_FC_INTF_TYPE_HCFC     4

#define SOC_TMC_FC_NIF_TYPE_LLFC      0
#define SOC_TMC_FC_NIF_TYPE_PFC       1
#define SOC_TMC_FC_NIF_TYPE_SAFC      2

#define SOC_TMC_FC_CALENDAR_TYPE_LLFC 0
#define SOC_TMC_FC_CALENDAR_TYPE_CHFC 1

#define SOC_TMC_FC_SRC_TYPE_GLB       0
#define SOC_TMC_FC_SRC_TYPE_VSQ       1
#define SOC_TMC_FC_SRC_TYPE_NIF       2
#define SOC_TMC_FC_SRC_TYPE_HCFC      3

typedef struct  {
    uint32                          port;

    uint32                          intf_type;            
    uint32                          is_oob;
    SOC_TMC_FC_DIRECTION            direction;

    uint32                          nif_fc_type;          

    uint32                          calendar_fc_type;     
    uint32                          calendar_id;

    uint32                          src_type;             
    SOC_TMC_ITM_VSQ_GROUP           vsq_type;             
    uint32                          vsq_id;
} SOC_TMC_FC_STATUS_KEY;

typedef struct  {
    
    
    uint32 iqm_glb_mnmc_db_hi_fc_state[2];
    uint32 iqm_glb_mnmc_db_lo_fc_state[2];
    uint32 iqm_glb_flmc_db_hi_fc_state[2];
    uint32 iqm_glb_flmc_db_lo_fc_state[2];
    uint32 iqm_glb_bdb_hi_fc_state[2];
    uint32 iqm_glb_bdb_lo_fc_state[2];
    uint32 iqm_glb_ocb_hi_fc_state[2];
    uint32 iqm_glb_ocb_lo_fc_state[2];
    uint32 iqm_glb_pdb_hi_fc_state[2];         
    uint32 iqm_glb_pdb_lo_fc_state[2];         
    uint32 iqm_glb_headroom_pd_hi_fc_state[2]; 
    uint32 iqm_glb_headroom_pd_lo_fc_state[2]; 
    uint32 iqm_glb_headroom_bd_hi_fc_state[2]; 
    uint32 iqm_glb_headroom_bd_lo_fc_state[2]; 

    
    uint32 cfc_iqm_glb_mnmc_db_hi_fc_state[2];
    uint32 cfc_iqm_glb_mnmc_db_lo_fc_state[2];
    uint32 cfc_iqm_glb_flmc_db_hi_fc_state[2];
    uint32 cfc_iqm_glb_flmc_db_lo_fc_state[2];
    uint32 cfc_iqm_glb_bdb_hi_fc_state[2];
    uint32 cfc_iqm_glb_bdb_lo_fc_state[2];
    uint32 cfc_iqm_glb_ocb_hi_fc_state[2];
    uint32 cfc_iqm_glb_ocb_lo_fc_state[2];
    uint32 cfc_cgm_pool_0_hi_fc_state[2];
    uint32 cfc_cgm_pool_0_lo_fc_state[2];
    uint32 cfc_cgm_pool_1_hi_fc_state[2];
    uint32 cfc_cgm_pool_1_lo_fc_state[2];
    uint32 cfc_iqm_glb_pdb_hi_fc_state[2];  
    uint32 cfc_iqm_glb_pdb_lo_fc_state[2];  

    uint32 cfc_iqm_vsq_fc_state[2];
    uint32 cfc_iqm_vsq_fc_state_2[2];

    uint32 cfc_nif_af_fc_status;

    
    uint32 nbi_llfc_status_from_mlf;
    uint32 nbi_pfc_status_from_mlf;
    uint32 nbi_tx_llfc_from_cfc;
    uint32 nbi_tx_llfc_to_mac;
    uint32 nbi_tx_pfc_from_cfc;
    uint32 nbi_tx_pfc_to_mac;
    uint32 nbi_eth_tx_fc_cnt;
    uint32 nbi_ilkn_rx_chfc_from_cfc_raw;
    uint32 nbi_ilkn_tx_chfc_roc;
    uint32 nbi_mub_tx_from_cfc;
    uint32 nib_mub_tx_value;

    
    uint64 mib_tx_pause_cnt;
    uint64 mib_tx_pfc_cnt;
    uint64 mib_tx_pfc_x_cnt[8];
    uint64 mib_tx_safc_log_cnt;
    

     
    
    uint64 mib_rx_pause_cnt;
    uint64 mib_rx_pfc_cnt;
    uint64 mib_rx_pfc_x_cnt[8];
    uint64 mib_rx_safc_log_cnt;
    uint64 mib_rx_safc_phy_cnt;
    uint64 mib_rx_safc_crc_cnt;
    

    
    uint32 nbi_rx_pfc_from_mac;
    uint32 nbi_rx_pfc_to_cfc;
    uint32 nbi_ilkn_rx_chfc_from_port_raw;
    uint32 nbi_ilkn_rx_chfc_from_port_roc;
    uint64 nbi_ilkn_rx_llfc_cnt;
    uint32 nbi_mub_rx_value;
    uint32 nbi_mub_rx_to_cfc;
    uint64 nbi_ilkn_llfc_stop_tx_cnt;
    uint32 nbi_mub_llfc_stop_tx_from_mub;
    uint32 nbi_llfc_stop_tx_from_cfc;
    uint32 nbi_llfc_stop_tx_to_mac;

    
    uint32 cfc_nif_pfc_status;
    uint32 cfc_ilkn_fc_status;
    uint32 cfc_mub_fc_status;
    uint32 cfc_spi_rx_llfc_status;
    uint32 cfc_spi_rx_pfc_status;
    uint32 cfc_spi_rx_gen_pfc_status;
    uint32 cfc_egq_pfc_status[2];                       
    soc_reg_above_64_val_t cfc_egq_pfc_status_full[2];  
    uint32 cfc_egq_inf_fc;
    uint32 cfc_egq_dev_fc;
    uint32 cfc_ilkn_rx_crc_err_status;
    uint32 cfc_spi_rx_crc_err_status;
    uint32 cfc_hcfc_rx_crc_err_status;
    uint32 cfc_ilkn_rx_crc_err_cnt;
    uint32 cfc_spi_rx_frame_err_cnt;
    uint32 cfc_spi_rx_dip_2_err_cnt;
    uint32 cfc_hcfc_rx_crc_err_cnt[5];
    uint32 cfc_oob_rx_lanes_status;
    uint32 cfc_hcfc_rx_wd_err_status;

    
    uint32 egq_nif_fc_status[2];       
    uint32 egq_cfc_fc_status[2];                       
    soc_reg_above_64_val_t egq_cfc_fc_status_full[2];  

    
    uint32 sch_fc_port_cnt[8];
    uint32 sch_fc_inf_cnt;
    uint32 sch_fc_dev_cnt;

    
    uint32 cal_tx_src_type;            
    uint32 cal_tx_src_vsq_type;        
    uint32 cal_tx_src_id;
    
    uint32 cal_rx_dst_type;            
    uint32 cal_rx_dst_id;

    
    uint32 nif_tx_src_id;

    SOC_TMC_FC_PFC_MAP_MODE nif_rx_dst_type[8];            
    uint32 nif_rx_dst_id[8];

    
    uint32 core_id;
} SOC_TMC_FC_STATUS_INFO;
















void SOC_TMC_FC_PFC_GENERIC_BITMAP_clear(SOC_SAND_OUT SOC_TMC_FC_PFC_GENERIC_BITMAP *generic_bm);

void SOC_TMC_FC_CAL_IF_INFO_clear(SOC_SAND_OUT SOC_TMC_FC_CAL_IF_INFO *cal_info);

void SOC_TMC_FC_GEN_CALENDAR_clear(SOC_SAND_OUT SOC_TMC_FC_GEN_CALENDAR *cal_info);

void SOC_TMC_FC_GEN_INBND_INFO_clear(SOC_SAND_OUT SOC_TMC_FC_GEN_INBND_INFO *info);

void SOC_TMC_FC_GEN_INBND_CB_clear(SOC_SAND_OUT SOC_TMC_FC_GEN_INBND_CB *info);

void SOC_TMC_FC_GEN_INBND_LL_clear(SOC_SAND_OUT SOC_TMC_FC_GEN_INBND_LL *info);

void SOC_TMC_FC_REC_INBND_CB_clear(SOC_SAND_OUT SOC_TMC_FC_REC_INBND_CB *info);

void SOC_TMC_FC_GEN_INBND_PFC_clear(SOC_SAND_OUT SOC_TMC_FC_GEN_INBND_PFC *info);

void SOC_TMC_FC_REC_CALENDAR_clear(SOC_SAND_OUT SOC_TMC_FC_REC_CALENDAR *info);

void
  SOC_TMC_FC_REC_INBND_INFO_clear(
    SOC_SAND_OUT SOC_TMC_FC_REC_INBND_INFO *info
  );

void
  SOC_TMC_FC_ILKN_LLFC_INFO_clear(
    SOC_SAND_OUT SOC_TMC_FC_ILKN_LLFC_INFO *info
  );

#include <soc/dpp/SAND/Utils/sand_footer.h>

#endif



