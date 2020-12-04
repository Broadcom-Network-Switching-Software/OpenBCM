/*----------------------------------------------------------------------
 *
 * Broadcom Corporation
 * Proprietary and Confidential information
 * All rights reserved
 * This source file is the property of Broadcom Corporation, and
 * may not be copied or distributed in any isomorphic form without the
 * prior written consent of Broadcom Corporation.
 *---------------------------------------------------------------------
 * File       : madura_cfg_seq.h
 * Description: c functions implementing Tier1
 *---------------------------------------------------------------------*/
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * 
*/


#ifndef MADURA_CFG_SEQ_H 
#define MADURA_CFG_SEQ_H
#include <phymod/phymod_acc.h>

#define MADURA_DEV_PMA_PMD                  1
#define MADURA_DEV_AN                       7
#define MADURA_M0_EEPROM_PAGE_SIZE          64
#define MADURA_FW_DLOAD_RETRY_CNT           40
#define MADURA_AN_SET_RETRY_CNT             20
#define MADURA_FORCE_TRAIN_RETRY_CNT        20
#define MADURA_ULL_SET_RETRY_CNT          200
#define MADURA_RPTR_SET_RETRY_CNT         200
#define MADURA_ENABLE                       1
#define MADURA_DISABLE                      0
#define MADURA_SPD_100G                     100000
#define MADURA_SPD_106G                     106000
#define MADURA_SPD_10G                      10000
#define MADURA_SPD_11G                      11000
#define MADURA_SPD_40G                      40000
#define MADURA_SPD_20G                      20000
#define MADURA_SPD_42G                      42000
#define MADURA_SPD_25G                      25000
#define MADURA_SPD_50G                      50000
#define MADURA_SPD_1G                       1000
#define MADURA_MAX_FALCON_LANE              4
#define MADURA_MAX_2X_FALCON_LANE           8
#define MADURA_MST_INTR_GRP                 0 
#define MADURA_PLL_INTR_GRP                 1
#define MADURA_CL73_INTR_GRP                2 
#define MADURA_MISC0_INTR_GRP               3 
#define MADURA_MISC1_INTR_GRP               4 
#define MADURA_MCTRL_INTR_GRP               5 
#define MADURA_FW_ALREADY_DOWNLOADED        0xFAD
#define MADURA_PMD_ID0                      0xAE02
#define MADURA_PMD_ID1                      0x52b0
#define MADURA_CHIP_ID_82864                0x82864
#define DEV1_SLICE_SLICE_ADR 0x8000
#define DEV7_SLICE_SLICE_ADR 0x8000
#define MADURA_MODULE_CNTRL_RAM_NVR0_ADR    0x8800
#define MADURA_MAX_INTRS_SUPPORT 8

typedef enum {
	MSGOUT_DONTCARE = 0x0000,
	MSGOUT_GET_CNT	= 0x8888,
	MSGOUT_GET_LSB	= 0xABCD,
	MSGOUT_GET_MSB	= 0x4321,
	MSGOUT_GET_2B	= 0xEEEE,
	MSGOUT_GET_B	= 0xF00D,
	MSGOUT_ERR		= 0x0BAD,
	MSGOUT_NEXT		= 0x2222, 
	MSGOUT_FLASH    = 0xF1AC,
        MSGOUT_NOT_DWNLD     = 0x0101, 
	MSGOUT_DWNLD_ALREADY = 0x0202,
	MSGOUT_DWNLD_DONE    = 0x0303,
	MSGOUT_PRGRM_DONE    = 0x0404/*changed from 1414!*/
} MADURA_MSGOUT_E;

typedef enum {
    MADURA_2X_FALCON_CORE = 0,
    MADURA_FALCON_CORE = 1
}MADURA_CORE_TYPE;

/**  Madura Device Aux mode structure
 *   This structure is used to set different modes
 *
 *   @param pass_thru 				Set this to 1 for pass thru mode and 0 for Demux mode
 *   @param alternate 		        	Set this to 1 for pass thru alternate mode
 *   @param pass_thru_dual_lane 		Set this to 1 for 40G pass thru dual mode   
 *   @param passthru_sys_side_core      	used internally by the drivers
 */

typedef struct {
    uint16_t pass_thru;
    uint16_t alternate;
    uint16_t pass_thru_dual_lane;
    MADURA_CORE_TYPE passthru_sys_side_core;
}MADURA_DEVICE_AUX_MODE_T;


typedef struct {
    uint32_t an_mst_lane_p0;
    uint32_t an_mst_lane_p1;
}MADURA_DEVICE_STATIC_AUX_MODE_T;


typedef enum {
    MADURA_SLICE_UNICAST = 0,
    MADURA_SLICE_MULTICAST = 1,
    MADURA_SLICE_BROADCAST = 2,
    MADURA_CAST_INVALID = 3
}MADURA_SLICE_CAST_TYPE;

typedef enum {
    MADURA_IF_LINE = 0,
    MADURA_IF_SYS = 1
}MADURA_IF_SIDE;

typedef enum {
    MADURA_AN_NONE = 0,
    MADURA_AN_CL73 = 1,
    MADURA_AN_PROP = 2
}MADURA_AN_MODE;

typedef enum {
  MADURA_PLL_MODE_64  = 0x0, /*4'b0000*/ 
  MADURA_PLL_MODE_66  = 0x1, /*4'b0001*/ 
  MADURA_PLL_MODE_80  = 0x2, /*4'b0010*/ 
  MADURA_PLL_MODE_128 = 0x3, /*4'b0011*/ 
  MADURA_PLL_MODE_132 = 0x4, /*4'b0100*/ 
  MADURA_PLL_MODE_140 = 0x5, /*4'b0101*/ 
  MADURA_PLL_MODE_160 = 0x6, /*4'b0110*/ 
  MADURA_PLL_MODE_165 = 0x7, /*4'b0111*/ 
  MADURA_PLL_MODE_168 = 0x8, /*4'b1000*/ 
  MADURA_PLL_MODE_170 = 0x9, /*4'b1001*/ 
  MADURA_PLL_MODE_175 = 0xa, /*4'b1010*/ 
  MADURA_PLL_MODE_180 = 0xb, /*4'b1011*/ 
  MADURA_PLL_MODE_184 = 0xc, /*4'b1100*/ 
  MADURA_PLL_MODE_200 = 0xd, /*4'b1101*/ 
  MADURA_PLL_MODE_224 = 0xe, /*4'b1110*/ 
  MADURA_PLL_MODE_264 = 0xf, /*4'b1111*/ 
  MADURA_PLL_MODE_INVALID = -1
} MADURA_PLL_MODE_E;

typedef enum PHY82864_INTERRUPT {
    INT_TIMESYNC_FRAMESYNC,
    INT_TIMESYNC_TIMESTAMP,
    INT_M0_SLV_MISC,
    INT_M0_MST_MISC,
    INT_M0_SLV_MSGOUT,
    INT_M0_MST_MSGOUT,
    INT_MUX_PMDLOCKLOST,
    INT_MUX_PMDLOCKFOUND,
    INT_MUX_SIGDETLOST,
    INT_MUX_SIGDETFOUND,
    INT_MUX_PCSMONLOCKLOST,
    INT_MUX_PCSMONLOCKFOUND,
    INT_MUX_FIFOERR,
    INT_MUX_CL73_RESTARTED,
    INT_MUX_CL73_COMPLETE,
    INT_MUX_CL73_RESTARTEDP2,
    INT_MUX_CL73_RESTARTEDP1,
    INT_MUX_CL73_COMPLETEP2,
    INT_MUX_CL73_COMPLETEP1,
    INT_CL73_AN_RESTARTED_L0,
    INT_CL73_AN_RESTARTED_L1,
    INT_CL73_AN_RESTARTED_L2,
    INT_CL73_AN_RESTARTED_L3,
    INT_CL73_AN_COMPLETE_L0,
    INT_CL73_AN_COMPLETE_L1,
    INT_CL73_AN_COMPLETE_L2,
    INT_CL73_AN_COMPLETE_L3,
    INT_DEMUX_PMDLOCKLOST,
    INT_DEMUX_PMDLOCKFOUND,
    INT_DEMUX_SIGDETLOST,
    INT_DEMUX_SIGDETFOUND,
    INT_DEMUX_PCSMONLOCKLOST,
    INT_DEMUX_PCSMONLOCKFOUND,
    INT_DEMUX_FIFOERR,
    INT_DEMUX_CL73_RESTARTED,
    INT_DEMUX_CL73_COMPLETE,
    INT_PLL_10G_LOCKLOST,
    INT_PLL_10G_LOCKFOUND,
    INT_PLL_25G_LOCKLOST,
    INT_PLL_25G_LOCKFOUND,
    INT_SYS_PLL_LOCKLOST,
    INT_SYS_PLL_LOCKFOUND,
    INT_LINE_PLL_LOCKLOST,
    INT_LINE_PLL_LOCKFOUND,
    PHY82864_MAX_INT
}PHY82864_INTERRUPT_T;

typedef enum  {
    MADURA_INT_MODULE1_INTRB_HIGH,
    MADURA_INT_MODULE1_INTRB_LOW,
    MADURA_INT_MODULE2_INTRB_HIGH,
    MADURA_INT_MODULE2_INTRB_LOW,
    MADURA_INT_M0_MST_SEC,
    MADURA_INT_M0_MST_DED,
    MADURA_INT_M0_MST_MSGOUT_INTR,
    MADURA_INT_M0_MST_MISC_INTR,
    MADURA_INT_M0_MST_LOCKUP,
    MADURA_INT_M0_MST_SYSRESETREQ,
    MADURA_INT_SYS_FAL_0_PLL0_LOCK_FOUND,
    MADURA_INT_SYS_FAL_0_PLL0_LOCK_LOST,
    MADURA_INT_SYS_FAL_0_PLL1_LOCK_FOUND,
    MADURA_INT_SYS_FAL_0_PLL1_LOCK_LOST,
    MADURA_INT_SYS_FAL_1_PLL0_LOCK_FOUND,
    MADURA_INT_SYS_FAL_1_PLL0_LOCK_LOST,
    MADURA_INT_SYS_FAL_1_PLL1_LOCK_FOUND,
    MADURA_INT_SYS_FAL_1_PLL1_LOCK_LOST,
    MADURA_INT_LIN_FAL_0_PLL0_LOCK_FOUND,
    MADURA_INT_LIN_FAL_0_PLL0_LOCK_LOST,
    MADURA_INT_LIN_FAL_0_PLL1_LOCK_FOUND,
    MADURA_INT_LIN_FAL_0_PLL1_LOCK_LOST,
    MADURA_INT_LIN_FAL_1_PLL0_LOCK_FOUND,
    MADURA_INT_LIN_FAL_1_PLL0_LOCK_LOST,
    MADURA_INT_LIN_FAL_1_PLL1_LOCK_FOUND,
    MADURA_INT_LIN_FAL_1_PLL1_LOCK_LOST,
    MADURA_INT_SYS_CL73_INCOMPATIBLE_LINK_0,
    MADURA_INT_SYS_CL73_INCOMPATIBLE_LINK_1,
    MADURA_INT_SYS_CL73_LINK_FAIL_INHIBIT_TIMER_DONE_0,
    MADURA_INT_SYS_CL73_LINK_FAIL_INHIBIT_TIMER_DONE_1,
    MADURA_INT_SYS_CL73_AN_COMPLETE_P1,
    MADURA_INT_SYS_CL73_AN_RESTARTED_P1,
    MADURA_INT_SYS_CL73_AN_COMPLETE_P2,
    MADURA_INT_SYS_CL73_AN_RESTARTED_P2,
    MADURA_INT_LIN_CL73_INCOMPATIBLE_LINK_0,
    MADURA_INT_LIN_CL73_INCOMPATIBLE_LINK_1,
    MADURA_INT_LIN_CL73_LINK_FAIL_INHIBIT_TIMER_DONE_0,
    MADURA_INT_LIN_CL73_LINK_FAIL_INHIBIT_TIMER_DONE_1,
    MADURA_INT_LIN_CL73_AN_COMPLETE_P1,
    MADURA_INT_LIN_CL73_AN_RESTARTED_P1,
    MADURA_INT_LIN_CL73_AN_COMPLETE_P2,
    MADURA_INT_LIN_CL73_AN_RESTARTED_P2,
    MADURA_INT_SYS_SIGDET_FOUND,
    MADURA_INT_SYS_SIGDET_LOST,
    MADURA_INT_SYS_PMD_LOCK_FOUND,
    MADURA_INT_SYS_PMD_LOCK_LOST,
    MADURA_INT_SYS_FAL_0_PMI_ARB_WDOG_EXP,
    MADURA_INT_SYS_FAL_1_PMI_ARB_WDOG_EXP,
    MADURA_INT_LIN_SIGDET_FOUND,
    MADURA_INT_LIN_SIGDET_LOST,
    MADURA_INT_LIN_PMD_LOCK_FOUND,
    MADURA_INT_LIN_PMD_LOCK_LOST,
    MADURA_INT_LIN_FAL_0_PMI_ARB_WDOG_EXP,
    MADURA_INT_LIN_FAL_1_PMI_ARB_WDOG_EXP,
    MADURA_INT_ONEG_INBAND_MSG_LOCK_FOUND,
    MADURA_INT_ONEG_INBAND_MSG_LOCK_LOST,
    MADURA_INT_SYS_PCS_MON_LOCK_FOUND,
    MADURA_INT_SYS_PCS_MON_LOCK_LOST,
    MADURA_INT_LIN_PCS_MON_LOCK_FOUND,
    MADURA_INT_LIN_PCS_MON_LOCK_LOST,
    MADURA_INT_CL91_0_RX_FEC_ALIGN_FOUND,
    MADURA_INT_CL91_0_RX_FEC_ALIGN_LOST,
    MADURA_INT_CL91_1_RX_FEC_ALIGN_FOUND,
    MADURA_INT_CL91_1_RX_FEC_ALIGN_LOST,
    MADURA_INT_CL91_0_TX_PCS_ALIGN_FOUND,
    MADURA_INT_CL91_0_TX_PCS_ALIGN_LOST,
    MADURA_INT_CL91_1_TX_PCS_ALIGN_FOUND,
    MADURA_INT_CL91_1_TX_PCS_ALIGN_LOST,
    MADURA_INT_SYS_FIFOERR,
    MADURA_INT_LIN_FIFOERR
} MADURA_INTR_TYPE_E;

#define MADURA_IF_ERR_RETURN_FREE(B, A) \
    do {   \
        int loc_err ; \
        if ((loc_err = (A)) != PHYMOD_E_NONE) \
        {  if (B) { PHYMOD_FREE(B) ; } ;  return loc_err ; } \
    } while (0)

#define READ_MADURA_PMA_PMD_REG(acc, addr, value)                                 \
    do {                                                                         \
        uint32_t read_tmp;                                                       \
        PHYMOD_IF_ERR_RETURN                                                     \
          (PHYMOD_BUS_READ(acc, ((MADURA_DEV_PMA_PMD << 16) | addr), &read_tmp)); \
        value = (read_tmp & 0xffff);                                             \
      } while (0);

#define WRITE_MADURA_PMA_PMD_REG(acc, addr, value)                                \
        PHYMOD_IF_ERR_RETURN                                                     \
	        (PHYMOD_BUS_WRITE(acc, (MADURA_DEV_PMA_PMD << 16) | addr, value));

#define READ_MADURA_AN_REG(acc, addr, value)                                      \
    do {                                                                         \
        uint32_t an_read_tmp;                                                    \
        PHYMOD_IF_ERR_RETURN                                                     \
          (PHYMOD_BUS_READ(acc, ((MADURA_DEV_AN << 16) | addr), &an_read_tmp));   \
        value = (an_read_tmp & 0xffff);                                          \
      } while (0);

#define WRITE_MADURA_AN_REG(acc, addr, value)                                     \
        PHYMOD_IF_ERR_RETURN                                                     \
	        (PHYMOD_BUS_WRITE(acc, (MADURA_DEV_AN << 16) | addr, value));

#define MADURA_GET_CORE(_C, _LT, _ST)                                                                               \
    {                                                                                                              \
        MADURA_DEVICE_AUX_MODE_T  *aux_mode_m;                                                                        \
        aux_mode_m = (MADURA_DEVICE_AUX_MODE_T*)_C.device_aux_modes;                                                  \
        if (aux_mode_m->pass_thru)    {                                                                              \
            _LT = (aux_mode_m->passthru_sys_side_core == MADURA_FALCON_CORE) ? MADURA_2X_FALCON_CORE : MADURA_FALCON_CORE; \
        } else {                                                                                                   \
            _LT = MADURA_2X_FALCON_CORE;                                                                               \
        }                                                                                                          \
        _ST = (_LT == MADURA_2X_FALCON_CORE) ? MADURA_FALCON_CORE : MADURA_2X_FALCON_CORE;                                  \
    }

#define MADURA_IS_MULTI_BROAD_CAST_SET(_C)   (_C == MADURA_SLICE_MULTICAST || _C == MADURA_SLICE_BROADCAST)
#define MADURA_GET_INTF_SIDE(pa, if_side)     (if_side = (pa->flags >> 31) ? MADURA_IF_SYS : MADURA_IF_LINE)

#define MADURA_GET_CORE_SLICE_INFO(_ACC, _IP, _CON )           \
    do {                                                                 \
        MADURA_CORE_TYPE line_ip_type, sys_ip_type;                       \
        uint16_t intf_side = 0;                                          \
        MADURA_GET_INTF_SIDE(_ACC, intf_side);                            \
        MADURA_GET_CORE(_CON, line_ip_type, sys_ip_type);                 \
        _IP = (intf_side == MADURA_IF_LINE) ? line_ip_type : sys_ip_type; \
    }while(0);

#define MADURA_GET_IP(_ACC, _CON, _IP)                                    \
    do {                                                                 \
        MADURA_CORE_TYPE line_ip_type, sys_ip_type;                       \
        uint16_t intf_side = 0;                                          \
        MADURA_GET_INTF_SIDE(_ACC, intf_side);                            \
        MADURA_GET_CORE(_CON, line_ip_type, sys_ip_type);                 \
        _IP = (intf_side == MADURA_IF_LINE) ? line_ip_type : sys_ip_type; \
    }while(0);

#define MADURA_RESET_SLICE(_P, _DT)                                         \
    if (_DT == MADURA_DEV_PMA_PMD) {                                        \
        WRITE_MADURA_PMA_PMD_REG(_P, DEV1_SLICE_SLICE_ADR, 0x0);            \
    } else {                                                               \
        WRITE_MADURA_AN_REG(_P, DEV7_SLICE_SLICE_ADR, 0x0);                 \
    }

#define MADURA_GET_PORT_FROM_MODE(_C, _PRT, aux_mode)                          \
    do {                                                                      \
        if (_C->data_rate == MADURA_SPD_100G ||                                \
            _C->data_rate == MADURA_SPD_106G) {                                \
            _PRT = 1;                                                         \
        } else if (_C->data_rate == MADURA_SPD_40G && aux_mode->pass_thru_dual_lane) {   \
            _PRT = 2;                                                         \
        } else if (_C->data_rate == MADURA_SPD_50G && aux_mode->pass_thru_dual_lane) {   \
            _PRT = 2;                                                         \
        } else if (_C->data_rate == MADURA_SPD_40G && aux_mode->pass_thru) {   \
            _PRT = 1;                                                         \
        } else if (_C->data_rate == MADURA_SPD_40G && (!aux_mode->pass_thru)){ \
            _PRT = 2;                                                         \
        } else if (_C->data_rate == MADURA_SPD_42G && aux_mode->pass_thru_dual_lane) {   \
            _PRT = 2;                                                         \
        } else if (_C->data_rate == MADURA_SPD_42G && aux_mode->pass_thru) {   \
            _PRT = 1;                                                         \
        } else if (_C->data_rate == MADURA_SPD_42G && (!aux_mode->pass_thru)){ \
            _PRT = 2;                                                         \
        } else if (_C->data_rate == MADURA_SPD_10G  ||                         \
                   _C->data_rate == MADURA_SPD_11G  ||                         \
                   _C->data_rate == MADURA_SPD_1G  ||                         \
                   _C->data_rate == MADURA_SPD_25G) {                          \
            _PRT = 4;                                                         \
        }                                                                     \
    }while(0);

typedef enum {
    MADURA_CL73_NO_TECH = 0,
    MADURA_CL73_1000GBASE_KX = 0x1,
    MADURA_CL73_10GBASE_KR   = 0x4,
    MADURA_CL73_40GBASE_KR4  = 0x8,
    MADURA_CL73_40GBASE_CR4  = 0x10,
    MADURA_CL73_100GBASE_KR4 = 0x80,
    MADURA_CL73_100GBASE_CR4 = 0x100
} MADURA_CL73_SPEED_T;

typedef enum {
    MADURA_NO_PAUSE = 0,
    MADURA_ASYM_PAUSE,
    MADURA_SYMM_PAUSE,
    MADURA_ASYM_SYMM_PAUSE,
    MADURA_AN_PAUSE_COUNT
} MADURA_AN_PAUSE_T;

/* Module controller I2C master commands */
typedef enum {
  MADURA_FLUSH   = 0,
  MADURA_RANDOM_ADDRESS_READ,
  MADURA_CURRENT_ADDRESS_READ,
  MADURA_WRITE
} MADURA_I2CM_CMD_E;

typedef struct MADURA_AN_ADV_ABILITY_S{
  MADURA_CL73_SPEED_T an_base_speed;
  MADURA_AN_PAUSE_T an_pause;
  uint16_t an_fec;
} madura_an_adv_ability_t;

typedef struct madura_an_ability_s {
  madura_an_adv_ability_t cl73_adv; /*includes cl73 related */
  uint32_t an_master_lane; /* Master lane belongs to port. For 10 lane port[0-9], for 4 lane port [0-3], for 2 lane port [0-1]*/
} madura_an_ability_t;

typedef enum {
    MADURA_MIDX_10GPT_0               = 0x80 ,
    MADURA_MIDX_10GPT_0A              = 0xC0 ,
    MADURA_MIDX_10HGPT_0              = 0x88 ,
    MADURA_MIDX_10HGPT_0A             = 0xC8 ,
    MADURA_MIDX_FC4PT_0               = 0x90 ,
    MADURA_MIDX_FC4PT_0A              = 0xD0 ,
    MADURA_MIDX_FC8PT_0               = 0x98 ,
    MADURA_MIDX_FC8PT_0A              = 0xD8 ,
    MADURA_MIDX_FC16PT_0              = 0xA0 ,
    MADURA_MIDX_FC16PT_0A             = 0xE0 ,
    MADURA_MIDX_FC32PT_0              = 0xA8 ,
    MADURA_MIDX_FC32PT_0A             = 0xE8 ,
    MADURA_MIDX_OTU2_1e_2e_PT_0       = 0xB0 ,
    MADURA_MIDX_OTU2_1e_2e_PT_0A      = 0xF0 ,
    MADURA_MIDX_10GPT_1               = 0x81 ,
    MADURA_MIDX_10GPT_1A              = 0xC1 ,
    MADURA_MIDX_10HGPT_1              = 0x89 ,
    MADURA_MIDX_10HGPT_1A             = 0xC9 ,
    MADURA_MIDX_FC4PT_1               = 0x91 ,
    MADURA_MIDX_FC4PT_1A              = 0xD1 ,
    MADURA_MIDX_FC8PT_1               = 0x99 ,
    MADURA_MIDX_FC8PT_1A              = 0xD9 ,
    MADURA_MIDX_FC16PT_1              = 0xA1 ,
    MADURA_MIDX_FC16PT_1A             = 0xE1 ,
    MADURA_MIDX_FC32PT_1              = 0xA9 ,
    MADURA_MIDX_FC32PT_1A             = 0xE9 ,
    MADURA_MIDX_OTU2_1e_2e_PT_1       = 0xB1 ,
    MADURA_MIDX_OTU2_1e_2e_PT_1A      = 0xF1 ,
    MADURA_MIDX_10GPT_2               = 0x82 ,
    MADURA_MIDX_10GPT_2A              = 0xC2 ,
    MADURA_MIDX_10HGPT_2              = 0x8A ,
    MADURA_MIDX_10HGPT_2A             = 0xCA ,
    MADURA_MIDX_FC4PT_2               = 0x92 ,
    MADURA_MIDX_FC4PT_2A              = 0xD2 ,
    MADURA_MIDX_FC8PT_2               = 0x9A ,
    MADURA_MIDX_FC8PT_2A              = 0xDA ,
    MADURA_MIDX_FC16PT_2              = 0xA2 ,
    MADURA_MIDX_FC16PT_2A             = 0xE2 ,
    MADURA_MIDX_FC32PT_2              = 0xAA ,
    MADURA_MIDX_FC32PT_2A             = 0xEA ,
    MADURA_MIDX_OTU2_1e_2e_PT_2       = 0xB2 ,
    MADURA_MIDX_OTU2_1e_2e_PT_2A      = 0xF2 ,
    MADURA_MIDX_10GPT_3               = 0x83 ,
    MADURA_MIDX_10GPT_3A              = 0xC3 ,
    MADURA_MIDX_10HGPT_3              = 0x8B ,
    MADURA_MIDX_10HGPT_3A             = 0xCB ,
    MADURA_MIDX_FC4PT_3               = 0x93 ,
    MADURA_MIDX_FC4PT_3A              = 0xD3 ,
    MADURA_MIDX_FC8PT_3               = 0x9B ,
    MADURA_MIDX_FC8PT_3A              = 0xDB ,
    MADURA_MIDX_FC16PT_3              = 0xA3 ,
    MADURA_MIDX_FC16PT_3A             = 0xE3 ,
    MADURA_MIDX_FC32PT_3              = 0xAB ,
    MADURA_MIDX_FC32PT_3A             = 0xEB ,
    MADURA_MIDX_OTU2_1e_2e_PT_3       = 0xB3 ,
    MADURA_MIDX_OTU2_1e_2e_PT_3A      = 0xF3 ,
    MADURA_MIDX_10GPT_4               = 0x84 ,
    MADURA_MIDX_10GPT_4A              = 0xC4 ,
    MADURA_MIDX_10HGPT_4              = 0x8C ,
    MADURA_MIDX_10HGPT_4A             = 0xCC ,
    MADURA_MIDX_FC4PT_4               = 0x94 ,
    MADURA_MIDX_FC4PT_4A              = 0xD4 ,
    MADURA_MIDX_FC8PT_4               = 0x9C ,
    MADURA_MIDX_FC8PT_4A              = 0xDC ,
    MADURA_MIDX_FC16PT_4              = 0xA4 ,
    MADURA_MIDX_FC16PT_4A             = 0xE4 ,
    MADURA_MIDX_FC32PT_4              = 0xAC ,
    MADURA_MIDX_FC32PT_4A             = 0xEC ,
    MADURA_MIDX_OTU2_1e_2e_PT_4       = 0xB4 ,
    MADURA_MIDX_OTU2_1e_2e_PT_4A      = 0xF4 ,
    MADURA_MIDX_10GPT_5               = 0x85 ,
    MADURA_MIDX_10GPT_5A              = 0xC5 ,
    MADURA_MIDX_10HGPT_5              = 0x8D ,
    MADURA_MIDX_10HGPT_5A             = 0xCD ,
    MADURA_MIDX_FC4PT_5               = 0x95 ,
    MADURA_MIDX_FC4PT_5A              = 0xD5 ,
    MADURA_MIDX_FC8PT_5               = 0x9D ,
    MADURA_MIDX_FC8PT_5A              = 0xDD ,
    MADURA_MIDX_FC16PT_5              = 0xA5 ,
    MADURA_MIDX_FC16PT_5A             = 0xE5 ,
    MADURA_MIDX_FC32PT_5              = 0xAD ,
    MADURA_MIDX_FC32PT_5A             = 0xED ,
    MADURA_MIDX_OTU2_1e_2e_PT_5       = 0xB5 ,
    MADURA_MIDX_OTU2_1e_2e_PT_5A      = 0xF5 ,
    MADURA_MIDX_10GPT_6               = 0x86 ,
    MADURA_MIDX_10GPT_6A              = 0xC6 ,
    MADURA_MIDX_10HGPT_6              = 0x8E ,
    MADURA_MIDX_10HGPT_6A             = 0xCE ,
    MADURA_MIDX_FC4PT_6               = 0x96 ,
    MADURA_MIDX_FC4PT_6A              = 0xD6 ,
    MADURA_MIDX_FC8PT_6               = 0x9E ,
    MADURA_MIDX_FC8PT_6A              = 0xDE ,
    MADURA_MIDX_FC16PT_6              = 0xA6 ,
    MADURA_MIDX_FC16PT_6A             = 0xE6 ,
    MADURA_MIDX_FC32PT_6              = 0xAE ,
    MADURA_MIDX_FC32PT_6A             = 0xEE ,
    MADURA_MIDX_OTU2_1e_2e_PT_6       = 0xB6 ,
    MADURA_MIDX_OTU2_1e_2e_PT_6A      = 0xF6 ,
    MADURA_MIDX_10GPT_7               = 0x87 ,
    MADURA_MIDX_10GPT_7A              = 0xC7 ,
    MADURA_MIDX_10HGPT_7              = 0x8F ,
    MADURA_MIDX_10HGPT_7A             = 0xCF ,
    MADURA_MIDX_FC4PT_7               = 0x97 ,
    MADURA_MIDX_FC4PT_7A              = 0xD7 ,
    MADURA_MIDX_FC8PT_7               = 0x9F ,
    MADURA_MIDX_FC8PT_7A              = 0xDF ,
    MADURA_MIDX_FC16PT_7              = 0xA7 ,
    MADURA_MIDX_FC16PT_7A             = 0xE7 ,
    MADURA_MIDX_FC32PT_7              = 0xAF ,
    MADURA_MIDX_FC32PT_7A             = 0xEF ,
    MADURA_MIDX_OTU2_1e_2e_PT_7       = 0xB7 ,
    MADURA_MIDX_OTU2_1e_2e_PT_7A      = 0xF7 ,
    MADURA_MIDX_40GPT20_0             = 0x00 ,
    MADURA_MIDX_40GPT20_0A            = 0x40 ,
    MADURA_MIDX_40GPT20_1             = 0x01 ,
    MADURA_MIDX_40GPT20_1A            = 0x41 ,
    MADURA_MIDX_40GPT20_2             = 0x02 ,
    MADURA_MIDX_40GPT20_2A            = 0x42 ,
    MADURA_MIDX_40GPT20_3             = 0x03 ,
    MADURA_MIDX_40GPT20_3A            = 0x43 ,
    MADURA_MIDX_40HGPT20_0            = 0x04 ,
    MADURA_MIDX_40HGPT20_0A           = 0x44 ,
    MADURA_MIDX_40HGPT20_1            = 0x05 ,
    MADURA_MIDX_40HGPT20_1A           = 0x45 ,
    MADURA_MIDX_40HGPT20_2            = 0x06 ,
    MADURA_MIDX_40HGPT20_2A           = 0x46 ,
    MADURA_MIDX_40HGPT20_3            = 0x07 ,
    MADURA_MIDX_40HGPT20_3A           = 0x47 ,
    MADURA_MIDX_OTU3PT_0              = 0x10 ,
    MADURA_MIDX_OTU3PT_0A             = 0x50 ,
    MADURA_MIDX_OTU3PT_1              = 0x11 ,
    MADURA_MIDX_OTU3PT_1A             = 0x51 ,
    MADURA_MIDX_OTU4PT_0              = 0x12 ,
    MADURA_MIDX_OTU4PT_0A             = 0x52 ,
    MADURA_MIDX_OTU4PT_1              = 0x13 ,
    MADURA_MIDX_OTU4PT_1A             = 0x53 ,
    MADURA_MIDX_40GPT10_0             = 0x18 ,
    MADURA_MIDX_40GPT10_0A            = 0x58 ,
    MADURA_MIDX_40GPT10_1             = 0x19 ,
    MADURA_MIDX_40GPT10_1A            = 0x59 ,
    MADURA_MIDX_100GPT_0              = 0x1A ,
    MADURA_MIDX_100GPT_0A             = 0x5A ,
    MADURA_MIDX_100GPT_1              = 0x1B ,
    MADURA_MIDX_100GPT_1A             = 0x5B ,
    MADURA_MIDX_40HGPT10_0            = 0x1C ,
    MADURA_MIDX_40HGPT10_0A           = 0x5C ,
    MADURA_MIDX_40HGPT10_1            = 0x1D ,
    MADURA_MIDX_40HGPT10_1A           = 0x5D ,
    MADURA_MIDX_100HGPT_0             = 0x1E ,
    MADURA_MIDX_100HGPT_0A            = 0x5E ,
    MADURA_MIDX_100HGPT_1             = 0x1F ,
    MADURA_MIDX_100HGPT_1A            = 0x5F ,
    MADURA_MIDX_40GMX_0               = 0x20 ,
    MADURA_MIDX_40GMX_1               = 0x21 ,
    MADURA_MIDX_40HGMX_0              = 0x24 ,
    MADURA_MIDX_40HGMX_1              = 0x25 ,
    MADURA_MIDX_40GDM_0               = 0x30 ,
    MADURA_MIDX_40GDM_1               = 0x31 ,
    MADURA_MIDX_40HGDM_0              = 0x34 ,
    MADURA_MIDX_40HGDM_1              = 0x35 ,
    MADURA_MIDX_40GDM_0B              = 0x38 ,
    MADURA_MIDX_40GDM_1B              = 0x39 ,
    MADURA_MIDX_40HGDM_0B             = 0x3C ,
    MADURA_MIDX_40HGDM_1B             = 0x3D ,
    MADURA_MIDX_MODE_INVALID      = -1
} MADURA_MIDX_MODE_E;

#define MADURA_MIDX_IS_4X25G_PLUS(_midx)\
(                                                     \
    MADURA_MIDX_100GPT_0              == (_midx) ||   \
    MADURA_MIDX_100GPT_0A             == (_midx) ||   \
    MADURA_MIDX_100GPT_1              == (_midx) ||   \
    MADURA_MIDX_100GPT_1A             == (_midx) ||   \
    MADURA_MIDX_100HGPT_0             == (_midx) ||   \
    MADURA_MIDX_100HGPT_0A            == (_midx) ||   \
    MADURA_MIDX_100HGPT_1             == (_midx) ||   \
    MADURA_MIDX_100HGPT_1A            == (_midx) )

int madura_get_chipid (const phymod_access_t *pa, uint32_t *chipid, uint32_t *rev);

int _madura_wait_mst_msgout(const phymod_access_t *pa,
                            MADURA_MSGOUT_E exp_message,
                            int poll_time);

int _madura_core_init(const phymod_core_access_t* core,
                     const phymod_core_init_config_t* init_config);

int _madura_set_slice_reg (
    const phymod_access_t* pm_acc,
    uint16_t               ip,
    uint16_t               dev_type,
    uint16_t               lane,
    uint16_t               pll_sel);
 
void _madura_lane_cast_get(const phymod_access_t *pa, MADURA_CORE_TYPE ip, 
                          uint16_t *cast_type, uint16_t *mcast_val); 

int _madura_phy_interface_config_get(const phymod_access_t *pa, 
                                   uint32_t flags, phymod_phy_inf_config_t *config);

int _madura_rx_pmd_lock_get(const phymod_access_t* pa, uint32_t* rx_pmd_locked);

int _madura_config_dut_mode_reg(const phymod_phy_access_t *pa, 
                               const phymod_phy_inf_config_t* config);

int _madura_udms_config(const phymod_phy_access_t *pa, 
                       const phymod_phy_inf_config_t* config,
                       MADURA_AN_MODE mode);

int _madura_phy_interface_config_set(const phymod_phy_access_t* pa, 
                                uint32_t flags, const phymod_phy_inf_config_t* config);

int madura_is_lane_mask_set(const phymod_phy_access_t *pa, uint16_t prt, 
                           const phymod_phy_inf_config_t *config);

int _madura_config_dut_mode_reg(const phymod_phy_access_t *pa, 
                               const phymod_phy_inf_config_t* config);

int _madura_udms_config(const phymod_phy_access_t *pa, const phymod_phy_inf_config_t* config,
                        MADURA_AN_MODE mode);

int _madura_interface_set(const phymod_access_t *pa, uint16_t ip,  const phymod_phy_inf_config_t* config);

int _madura_autoneg_get_val(const phymod_access_t *pa, uint16_t ip,  const phymod_phy_inf_config_t* config, uint16_t *AN_get_val);

int _madura_autoneg_set_val(const phymod_access_t *pa, uint16_t ip,  const phymod_phy_inf_config_t* config, uint16_t AN_set_val);

int _madura_force_training_get(const phymod_access_t *pa, uint16_t ip,  const phymod_phy_inf_config_t* config, uint16_t *training_get);

int _madura_force_training_set(const phymod_access_t *pa, uint16_t ip,  const phymod_phy_inf_config_t* config, uint16_t training_set);

int _madura_phy_interface_config_set(const phymod_phy_access_t* pa, uint32_t flags, 
                                    const phymod_phy_inf_config_t* config);

int _madura_phy_interface_config_get(const phymod_access_t *pa, uint32_t flags, 
                                    phymod_phy_inf_config_t *config);

int _madura_phy_pcs_link_get(const phymod_access_t *pa, 
                            uint32_t *pcs_link);

int _madura_configure_ref_clock(const phymod_access_t *pa, 
                               phymod_ref_clk_t ref_clk);

int _madura_get_pll_modes(phymod_ref_clk_t ref_clk, const phymod_phy_inf_config_t *config, 
                         uint16_t *fal_pll_mode, uint16_t *fal_2x_pll_mode);

int _madura_config_pll_div(const phymod_access_t *pa, const phymod_phy_inf_config_t config);

int _madura_core_dp_rstb(const phymod_access_t *pa, const phymod_phy_inf_config_t cfg);

int _madura_core_reset_set(const phymod_access_t *pa, phymod_reset_mode_t reset_mode, phymod_reset_direction_t direction);

int _madura_get_pcs_link_status(const phymod_access_t *pa, uint32_t *link_sts);

int _madura_pcs_link_monitor_enable_set(const phymod_access_t *pa, uint16_t en_dis);

int _madura_pcs_link_monitor_enable_get(const phymod_access_t *pa, uint32_t *get_pcs);

int _madura_fw_enable(const phymod_access_t *pa, uint16_t en_dis) ;

int _madura_loopback_set(const phymod_access_t *pa, phymod_loopback_mode_t loopback, uint32_t enable);

int _madura_loopback_get(const phymod_access_t *pa, phymod_loopback_mode_t loopback, uint32_t *enable);

int _madura_tx_rx_polarity_set(const phymod_access_t *pa, uint32_t tx_polarity, uint32_t rx_polarity);

int _madura_tx_rx_polarity_get(const phymod_access_t *pa, uint32_t *tx_polarity, uint32_t *rx_polarity);

int _madura_tx_set(const phymod_access_t *pa, const phymod_tx_t* tx);

int _madura_tx_get(const phymod_access_t *pa, phymod_tx_t* tx);

int _madura_rx_set(const phymod_access_t* pa, const phymod_rx_t* rx);

int _madura_rx_adaptation_resume(const phymod_access_t* pa);

int _madura_rx_get(const phymod_access_t *pa, phymod_rx_t* rx);

int _madura_tx_power_set(const phymod_access_t *pa, uint32_t power_tx);

int _madura_rx_power_set(const phymod_access_t *pa, uint32_t power_rx);

int _madura_tx_rx_power_get(const phymod_access_t *pa, phymod_phy_power_t* power);

int _madura_force_tx_training_set(const phymod_access_t *pa, uint32_t enable);

int _madura_force_tx_training_get(const phymod_access_t *pa, uint32_t *enable);

int _madura_force_tx_training_status_get(const phymod_access_t *pa, phymod_cl72_status_t* status);

int _madura_phy_reset_set(const phymod_access_t *pa, const phymod_phy_reset_t* reset);

int _madura_phy_reset_get(const phymod_access_t *pa, phymod_phy_reset_t* reset);

int _madura_phy_status_dump(const phymod_access_t *pa);

int _madura_tx_lane_control_set(const phymod_access_t *pa,  phymod_phy_tx_lane_control_t tx_control);

int _madura_rx_lane_control_set(const phymod_access_t *pa,  phymod_phy_rx_lane_control_t rx_control);

int _madura_tx_lane_control_get(const phymod_access_t *pa,  phymod_phy_tx_lane_control_t *tx_control);

int _madura_rx_lane_control_get(const phymod_access_t *pa,  phymod_phy_rx_lane_control_t *rx_control);

int _madura_ext_intr_status_get(const phymod_phy_access_t *phy, uint32_t intr_type, uint32_t* intr_status);

int _madura_ext_intr_enable_set(const phymod_phy_access_t *phy, uint32_t intr_type, uint32_t enable); 

int _madura_ext_intr_enable_get(const phymod_phy_access_t *phy, uint32_t intr_type, uint32_t* enable); 

int _madura_ext_intr_status_clear(const phymod_phy_access_t *phy, uint32_t intr_type);

int _madura_pll_sequencer_restart(const  phymod_core_access_t *core, phymod_sequencer_operation_t operation);

int _madura_fec_enable_set(const phymod_access_t *pa, uint32_t enable);

int _madura_fec_enable_get(const phymod_access_t *pa, uint32_t* enable);

int _madura_config_hcd_link_sts (const phymod_access_t* pa, phymod_phy_inf_config_t config, uint16_t ip, uint16_t port); 

int _madura_autoneg_ability_set(const phymod_access_t* pa, madura_an_ability_t *an_ability);

int _madura_autoneg_set(const phymod_phy_access_t* pa, const phymod_autoneg_control_t* an);

int _madura_autoneg_get(const phymod_access_t* pa, phymod_autoneg_control_t* an, uint32_t *an_done);

int _madura_autoneg_ability_get (const phymod_access_t* pa, madura_an_ability_t *an_ablity);

int _madura_autoneg_remote_ability_get(const phymod_access_t *pa, phymod_autoneg_ability_t* an_ability_get);

int _madura_get_pcs_link_status(const phymod_access_t *pa, uint32_t *link_sts);

int _madura_pcs_link_monitor_enable_get(const phymod_access_t *pa, uint32_t *get_pcs);

int _madura_pcs_link_monitor_enable_set(const phymod_access_t *pa, uint16_t en_dis);

int _madura_port_from_lane_map_get(const phymod_access_t *pa, const phymod_phy_inf_config_t* cfg, uint16_t *port, uint16_t *physical_port);

int _madura_core_cfg_polarity_set(const phymod_access_t *pa, uint32_t tx_polarity, uint32_t rx_polarity);

int _madura_gpio_config_set(const phymod_access_t *pa, int pin_no, phymod_gpio_mode_t gpio_mode);

int _madura_gpio_config_get(const phymod_access_t *pa, int pin_no, phymod_gpio_mode_t* gpio_mode);

int _madura_gpio_pin_value_set(const phymod_access_t *pa, int pin_no, int value);

int _madura_gpio_pin_value_get(const phymod_access_t *pa, int pin_no, int* value);

int _madura_module_write(const phymod_access_t *pa, uint32_t slv_addr, uint32_t start_addr, uint32_t no_of_bytes, const uint8_t *write_data);

int _madura_module_read(const phymod_access_t *pa, uint32_t slv_addr, uint32_t start_addr, uint32_t no_of_bytes, uint8_t *read_data);

int _madura_set_module_command(const phymod_access_t *pa, uint16_t xfer_addr, uint32_t slv_addr, unsigned char xfer_cnt, MADURA_I2CM_CMD_E cmd);

int _madura_cfg_fw_ull_dp(const phymod_access_t* pa, phymod_datapath_t op_datapath);

int _madura_core_rptr_rtmr_mode_set(const phymod_access_t* pa, uint16_t op_mode);

int _madura_firmware_lane_config_set(const phymod_access_t* phy, phymod_firmware_lane_config_t fw_config);
    
int _madura_firmware_lane_config_get(const phymod_access_t* phy, phymod_firmware_lane_config_t* fw_config);

int _madura_get_ieee_intf(const phymod_access_t *pa, const phymod_phy_inf_config_t *config, uint16_t *intf);

int _madura_set_ieee_intf(const phymod_access_t *pa, const phymod_phy_inf_config_t *config, uint16_t intf);

#endif /* MADURA_CFG_SEQ_H */
