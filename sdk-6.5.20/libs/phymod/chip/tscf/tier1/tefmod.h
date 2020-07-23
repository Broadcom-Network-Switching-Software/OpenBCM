/*----------------------------------------------------------------------
 * $Id: tefmod.h,
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * $Copyright: (c) 2014 Broadcom Corporation All Rights Reserved.$
 *  Broadcom Corporation
 *  Proprietary and Confidential information
 *  All rights reserved
 *  This source file is the property of Broadcom Corporation, and
 *  may not be copied or distributed in any isomorphic form without the
 *  prior written consent of Broadcom Corporation.
 *----------------------------------------------------------------------
 *  Description: define enumerators  
 *----------------------------------------------------------------------*/
/*
 *  
 * $Copyright:
 * All Rights Reserved.$
 */

#ifndef _tefmod_H_
#define _tefmod_H_

#ifndef _DV_TB_
 #define _SDK_TEFMOD_ 1 
#endif

#ifdef _DV_TB_
#ifdef LINUX
#include <stdint.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "errno.h"
#endif

#ifdef _SDK_TEFMOD_
#include <phymod/phymod.h>
#include <phymod/phymod_debug.h>
#include "tefmod_enum_defines.h"
#endif

#ifdef _SDK_TEFMOD_
#define PHYMOD_ST const phymod_access_t
#else
#define PHYMOD_ST tefmod_st
#endif

#define TEFMOD_FEC_NOT_SUPRTD         0
#define TEFMOD_FEC_SUPRTD_NOT_REQSTD  1
#define TEFMOD_FEC_CL74_SUPRTD_REQSTD 4
#define TEFMOD_FEC_CL91_SUPRTD_REQSTD 8

/* matching SOC_PHY_CONTROL_FEC_OFF 
 *          SOC_PHY_CONTROL_FEC_ON
 *          SOC_PHY_CONTROL_FEC_AUTO 
 */
#define TEFMOD_PHY_CONTROL_FEC_OFF           0
#define TEFMOD_PHY_CONTROL_FEC_ON            1
#define TEFMOD_PHY_CONTROL_FEC_AUTO          2 


/* So far 4 bits debug mask are used by TEFMOD */
#define TEFMOD_DBG_MEM        (1L << 4) /* allocation/object */
#define TEFMOD_DBG_REGACC     (1L << 3) /* Print all register accesses */ 
#define TEFMOD_DBG_FUNCVALOUT (1L << 2) /* All values returned by Tier1*/
#define TEFMOD_DBG_FUNCVALIN  (1L << 1) /* All values pumped into Tier1*/
#define TEFMOD_DBG_FUNC       (1L << 0) /* Every time we enter a  Tier1*/

typedef enum {
    TEFMOD_AN_MODE_CL73 = 0,
    TEFMOD_AN_MODE_CL73BAM,
    TEFMOD_AN_MODE_HPAM,       
    TEFMOD_AN_MODE_NONE,
    TEFMOD_AN_MODE_CL73_MSA,
    TEFMOD_AN_MODE_MSA,
    TEFMOD_AN_MODE_TYPE_COUNT
}tefmod_an_mode_type_t;



typedef enum {
    TEFMOD_AN_SET_RF_DISABLE = 0,
    TEFMOD_AN_SET_SGMII_SPEED,
    TEFMOD_AN_SET_SGMII_MASTER,
    TEFMOD_AN_SET_HG_MODE,
    TEFMOD_AN_SET_FEC_MODE,
    TEFMOD_AN_SET_CL72_MODE,
    TEFMOD_AN_SET_CL37_ATTR,
    TEFMOD_AN_SET_CL48_SYNC,
    TEFMOD_AN_SET_LK_FAIL_INHIBIT_TIMER_NO_CL72,
    TEFMOD_AN_SET_CL73_FEC_OFF,
    TEFMOD_AN_SET_CL73_FEC_ON,
    TEFMOD_AN_SET_SPEED_PAUSE,
    TEFMOD_AN_SET_TYPE_COUNT
}tefmod_an_set_type_t;

typedef union {
    int tefmod_an_set_rf_disable;
    int tefmod_an_set_sgmii_speed;
    int tefmod_an_set_sgmii_master;
    tefmod_an_set_type_t tefmod_an_set_cl37_attr;
}tefmod_an_set_value_t;

typedef enum {
    TEFMOD_CL73_10GBASE_KR1 = 0,  
    TEFMOD_CL73_40GBASE_KR4,  
    TEFMOD_CL73_40GBASE_CR4,  
    TEFMOD_CL73_100GBASE_KR4,
    TEFMOD_CL73_100GBASE_CR4,
    TEFMOD_CL73_1GBASE_KX,
    TEFMOD_CL73_25GBASE_KR1,
    TEFMOD_CL73_25GBASE_CR1,
    TEFMOD_CL73_25GBASE_KRS1,
    TEFMOD_CL73_25GBASE_CRS1,
    TEFMOD_CL73_SPEED_COUNT
}tefmod_cl73_speed_t;

typedef enum {
    TEFMOD_CL73_BAM_20GBASE_KR2 = 0,  
    TEFMOD_CL73_BAM_20GBASE_CR2,  
    TEFMOD_CL73_BAM_40GBASE_KR2,  
    TEFMOD_CL73_BAM_40GBASE_CR2,
    TEFMOD_CL73_BAM_RESERVED_1,
    TEFMOD_CL73_BAM_RESERVED_2,
    TEFMOD_CL73_BAM_50GBASE_KR2,  
    TEFMOD_CL73_BAM_50GBASE_CR2,
    TEFMOD_CL73_BAM_50GBASE_KR4,
    TEFMOD_CL73_BAM_50GBASE_CR4,
    TEFMOD_CL73_BAM_SPEED_COUNT
}tefmod_cl73_bam_speed_t;

typedef enum {
    TEFMOD_CL73_BAM_20GBASE_KR1 = 1,  
    TEFMOD_CL73_BAM_20GBASE_CR1,  
    TEFMOD_CL73_BAM_25GBASE_KR1,  
    TEFMOD_CL73_BAM_25GBASE_CR1,
    TEFMOD_CL73_BAM_SPEED1_COUNT
}tefmod_cl73_bam_speed1_t;

typedef enum {
    TEFMOD_NO_PAUSE = 0,  
    TEFMOD_SYMM_PAUSE,
    TEFMOD_ASYM_PAUSE,
    TEFMOD_ASYM_SYMM_PAUSE,  
    TEFMOD_AN_PAUSE_COUNT
}tefmod_an_pause_t;

typedef enum {
  OVERRIDE_CLEAR                  =  0x0000,
  OVERRIDE_NUM_LANES              =  0x0001,
  OVERRIDE_OS_MODE                =  0x0002,
  OVERRIDE_T_FIFO_MODE            =  0x0004,
  OVERRIDE_T_ENC_MODE             =  0x0008,
  OVERRIDE_T_HG2_ENABLE           =  0x0010,
  OVERRIDE_T_PMA_BTMX_MODE_OEN    =  0x0020,
  OVERRIDE_SCR_MODE               =  0x0040,
  OVERRIDE_DESCR_MODE_OEN         =  0x0100,
  OVERRIDE_DEC_TL_MODE            =  0x0200,
  OVERRIDE_DESKEW_MODE            =  0x0400,
  OVERRIDE_DEC_FSM_MODE           =  0x0800,
  OVERRIDE_R_HG2_ENABLE           =  0x8001,
  OVERRIDE_BS_SM_SYNC_MODE_OEN    =  0x8002,
  OVERRIDE_BS_SYNC_EN_OEN         =  0x8004,
  OVERRIDE_BS_DIST_MODE_OEN       =  0x8008,
  OVERRIDE_BS_BTMX_MODE_OEN       =  0x8010,
  OVERRIDE_CL72_EN                =  0x8020,
  OVERRIDE_CLOCKCNT0              =  0xf040,
  OVERRIDE_CLOCKCNT1              =  0x8080,
  OVERRIDE_LOOPCNT0               =  0x8100,
  OVERRIDE_LOOPCNT1               =  0x8200,
  OVERRIDE_MAC_CREDITGENCNT       =  0x8400,
  OVERRIDE_NUM_LANES_DIS          =  0xff01,
  OVERRIDE_OS_MODE_DIS            =  0xff02,
  OVERRIDE_T_FIFO_MODE_DIS        =  0xff03,
  OVERRIDE_T_ENC_MODE_DIS         =  0xff04,
  OVERRIDE_T_HG2_ENABLE_DIS       =  0xff05,
  OVERRIDE_T_PMA_BTMX_MODE_OEN_DIS=  0xff06,
  OVERRIDE_SCR_MODE_DIS           =  0xff07,
  OVERRIDE_DESCR_MODE_OEN_DIS     =  0xff08,
  OVERRIDE_DEC_TL_MODE_DIS        =  0xff09,
  OVERRIDE_DESKEW_MODE_DIS        =  0xff0a,
  OVERRIDE_DEC_FSM_MODE_DIS       =  0xff0b,
  OVERRIDE_R_HG2_ENABLE_DIS       =  0xff0c,
  OVERRIDE_BS_SM_SYNC_MODE_OEN_DIS=  0xff0d,
  OVERRIDE_BS_SYNC_EN_OEN_DIS     =  0xff0e,
  OVERRIDE_BS_DIST_MODE_OEN_DIS   =  0xff0f,
  OVERRIDE_BS_BTMX_MODE_OEN_DIS   =  0xff10,
  OVERRIDE_CL72_EN_DIS            =  0xff11,
  OVERRIDE_CLOCKCNT0_DIS          =  0xff12,
  OVERRIDE_CLOCKCNT1_DIS          =  0xff13,
  OVERRIDE_LOOPCNT0_DIS           =  0xff14,
  OVERRIDE_LOOPCNT1_DIS           =  0xff15,
  OVERRIDE_MAC_CREDITGENCNT_DIS   =  0xff16
} override_type_t;

typedef enum {
    TEFMOD_HG2_CODEC_OFF = 0,  
    TEFMOD_HG2_CODEC_ON_8BYTE_IPG,
    TEFMOD_HG2_CODEC_ON_9BYTE_IPG,
    TEFMOD_HG2_CODEC_COUNT
}tefmod_hg2_codec_t;

typedef enum {
    TEFMOD_NO_CL91_FEC = 0,
    TEFMOD_CL91_SINGLE_LANE_FC,
    TEFMOD_CL91_SINGLE_LANE_BRCM_PROP,
    TEFMOD_CL91_DUAL_LANE_BRCM_PROP,
    TEFMOD_CL91_QUAD_LANE,
    TEFMOD_IEEE_25G_CL91_SINGLE_LANE,
    TEFMOD_CL91_COUNT
}tefmod_cl91_fec_mode_t;

/**
\struct tefmod_an_adv_ability_s
TBD
*/

typedef struct tefmod_an_adv_ability_s{
  tefmod_cl73_speed_t an_base_speed; 
  tefmod_cl73_bam_speed_t an_bam_speed; 
  tefmod_cl73_bam_speed1_t an_bam_speed1; 
  uint16_t an_nxt_page; 
  tefmod_an_pause_t an_pause; 
  uint16_t an_fec; 
  uint16_t an_cl72;
  uint16_t an_hg2; 
}tefmod_an_adv_ability_t;

typedef struct tefmod_an_ability_s{
  tefmod_an_adv_ability_t cl73_adv; /*includes cl73 and cl73-bam related */
}tefmod_an_ability_t;

/**
\struct tefmod_an_oui_s
  oui                : New oui
  override_bam73_adv : Adv new OUI for BAM73
  override_bam73_det : Detect new OUI for BAM73
  override_hpam_adv  : Adv new OUI for HPAM
  override_hpam_det  : Detect new OUI for HPAM
 */
typedef struct tefmod_an_oui_s {
  uint32_t oui;
  uint16_t oui_override_bam73_adv;
  uint16_t oui_override_bam73_det;
  uint16_t oui_override_hpam_adv;
  uint16_t oui_override_hpam_det;
} tefmod_an_oui_t;

typedef struct tefmod_an_control_s {
  tefmod_an_type_t an_type; 
  uint16_t num_lane_adv; 
  uint16_t enable;
  uint16_t pd_kx_en;
  an_property_enable  an_property_type;
  uint16_t cl72_config_allowed; 
} tefmod_an_control_t;

/**
\struct tefmod_an_init_s

This embodies all parameters required to program autonegotiation features in the
PHY for both CL37 and CL73 type autonegotiation and the BAM variants.
For details of individual fields, please refer to microarchitectural doc.
*/
typedef struct tefmod_an_init_s{
  uint16_t  an_fail_cnt;
  uint16_t  linkfailtimer_dis; 
  uint16_t  linkfailtimerqua_en; 
  uint16_t  an_good_check_trap; 
  uint16_t  an_good_trap; 
  uint16_t  bam73_adv_oui;  
  uint16_t  bam73_det_oui;  
  uint16_t  hpam_adv_oui;  
  uint16_t  hpam_det_oui;  
  uint16_t  disable_rf_report; 
  uint16_t  cl73_remote_fault; 
  uint16_t  cl73_nonce_match_over;
  uint16_t  cl73_nonce_match_val;
  uint16_t  cl73_transmit_nonce; 
  uint16_t  base_selector;
} tefmod_an_init_t;


/**
\struct tefmod_an_timers_s
TBD
*/
typedef struct tefmod_an_timers_s{
  uint16_t  value;
} tefmod_an_timers_t;

/* definitions for SW AN PAGE */
#define BRCM_OUI 0xAF7
#define BRCM_BAM_CODE 0x3
#define HP_OUI 0x643150
#define HP_USR_DEF_FIELD 0xABE20
#define MSA_OUI 0x6A737D /* OUI defined for consortium 25G */
#define MSA_OUI_13to23 0x353
#define MSA_OUT_2to12  0x4DF
#define MSA_OUI_0to1   0x1 
#define EXT_TECH_ABILITY_CODE 0x3
#define MSA_MSG_CODE 0x5


/* Below macros are defined based on IEEE and MSA 25G spec
 * for detailed information Please refer to the 
 * IEEE 802.3by section 73.6 Link codeword encoding
 * MSA Spec section 3.2.5 Auto-negotiation Figure 10 
 */

#define AN_BASE0_PAGE_IEEE_SEL_MASK   0x1F
#define AN_BASE0_PAGE_IEEE_SEL_OFFSET 0x0

#define AN_BASE0_PAGE_CL73_NOUNCE_MASK   0x1F
#define AN_BASE0_PAGE_CL73_NOUNCE_OFFSET 0x5

/* pause capability not related to PHY */
#define AN_BASE0_PAGE_CAP_MASK   0x3
#define AN_BASE0_PAGE_CAP_OFFSET 0xA

#define AN_BASE0_PAGE_REMOTE_FAULT_MASK   0x1
#define AN_BASE0_PAGE_REMOTE_FAULT_OFFSET 0xD

/* acknowledgement bit postion */
#define AN_BASE0_PAGE_ACK_MASK   0x1
#define AN_BASE0_PAGE_ACK_OFFSET 0xE

/* Next Page bit postion */
#define AN_BASE0_PAGE_NP_MASK   0x1
#define AN_BASE0_PAGE_NP_OFFSET 0xF


#define AN_BASE1_PAGE_TX_NOUNCE_MASK   0x1F
#define AN_BASE1_PAGE_TX_NOUNCE_OFFSET 0x0

#define AN_BASE1_PAGE_ABILITY_MASK   0x7FF
#define AN_BASE1_PAGE_ABILITY_OFFSET 0x5

#define AN_BASE_TECH_ABILITY_1000KX_MASK   0x1
#define AN_BASE_TECH_ABILITY_1000KX_OFFSET 0x5

#define AN_BASE_TECH_ABILITY_10GKX4_MASK   0x1
#define AN_BASE_TECH_ABILITY_10GKX4_OFFSET 0x6

#define AN_BASE_TECH_ABILITY_10GKR_MASK   0x1
#define AN_BASE_TECH_ABILITY_10GKR_OFFSET 0x7

#define AN_BASE_TECH_ABILITY_40GKR4_MASK   0x1
#define AN_BASE_TECH_ABILITY_40GKR4_OFFSET 0x8

#define AN_BASE_TECH_ABILITY_40GCR4_MASK   0x1
#define AN_BASE_TECH_ABILITY_40GCR4_OFFSET 0x9

#define AN_BASE_TECH_ABILITY_100GCR10_MASK   0x1
#define AN_BASE_TECH_ABILITY_100GCR10_OFFSET 0xA

#define AN_BASE_TECH_ABILITY_100GKP4_MASK   0x1
#define AN_BASE_TECH_ABILITY_100GKP4_OFFSET 0xB

#define AN_BASE_TECH_ABILITY_100GKR4_MASK   0x1
#define AN_BASE_TECH_ABILITY_100GKR4_OFFSET 0xC

#define AN_BASE_TECH_ABILITY_100GCR4_MASK   0x1
#define AN_BASE_TECH_ABILITY_100GCR4_OFFSET 0xD

#define AN_BASE_TECH_ABILITY_25GKRS_MASK   0x1
#define AN_BASE_TECH_ABILITY_25GKRS_OFFSET 0xE

#define AN_BASE_TECH_ABILITY_25GKR_MASK   0x1
#define AN_BASE_TECH_ABILITY_25GKR_OFFSET 0xF

#define AN_BASE2_PAGE_ABILITY_RSVD_MASK   0xFF7
#define AN_BASE2_PAGE_ABILITY_RSVD_OFFSET 0x0

#define AN_BASE2_PAGE_FEC_ABILITY_MASK   0xF
#define AN_BASE2_PAGE_FEC_ABILITY_OFFSET 0xC

#define AN_BASE_CL74_ABILITY_SUP_MASK   0x1
#define AN_BASE_CL74_ABILITY_SUP_OFFSET 0xE

#define AN_BASE_CL74_ABILITY_REQ_MASK   0x1
#define AN_BASE_CL74_ABILITY_REQ_OFFSET 0xF

#define AN_BASE_CL91_ABILITY_REQ_MASK   0x1
#define AN_BASE_CL91_ABILITY_REQ_OFFSET 0xC

#define AN_BASE_CL74_25GKRS_REQ_MASK   0x1
#define AN_BASE_CL74_25GKRS_REQ_OFFSET 0xD

/* Message Page definitions */

#define AN_MSG_PAGE0_MSG_CODE_MASK   0x3FF
#define AN_MSG_PAGE0_MSG_CODE_OFFSET 0x0

#define AN_MSG_PAGE0_TOGGLE_MASK   0x1
#define AN_MSG_PAGE0_TOGGLE_OFFSET 0xB

#define AN_MSG_PAGE0_ACK2_MASK   0x1
#define AN_MSG_PAGE0_ACK2_OFFSET 0xC

#define AN_MSG_PAGE0_MSG_PAGE_MASK   0x1
#define AN_MSG_PAGE0_MSG_PAGE_OFFSET 0xD

#define AN_MSG_PAGE0_ACK_MASK   0x1
#define AN_MSG_PAGE0_ACK_OFFSET 0xE

#define AN_MSG_PAGE0_NP_MASK   0x1
#define AN_MSG_PAGE0_NP_OFFSET 0xF

#define AN_MSG_PAGE1_OUI_13to23_MASK 0x7FF
#define AN_MSG_PAGE1_OUI_13to23_OFFSET 0x0

#define AN_MSG_PAGE2_OUI_2to12_MASK   0x7FF
#define AN_MSG_PAGE2_OUI_2to12_OFFSET 0x0

/* Unformatted Page for MSA */

#define AN_UF_PAGE0_EXT_TECH_ABILITY_MASK   0xFF
#define AN_UF_PAGE0_EXT_TECH_ABILITY_OFFSET 0x0

#define AN_UF_PAGE0_OUI_MASK   0x3
#define AN_UF_PAGE0_OUI_OFFSET 0x9

#define AN_UF_PAGE0_TOGGLE_MASK   0x1
#define AN_UF_PAGE0_TOGGLE_OFFSET 0xB

#define AN_UF_PAGE0_ACK_MASK   0x1
#define AN_UF_PAGE0_ACK_OFFSET 0xE

#define AN_UF_PAGE0_NP_MASK   0x1
#define AN_UF_PAGE0_NP_OFFSET 0xF

#define AN_UF_PAGE1_25G_KR1_ABILITY_MASK   0x1
#define AN_UF_PAGE1_25G_KR1_ABILITY_OFFSET 0x4

#define AN_UF_PAGE1_25G_CR1_ABILITY_MASK   0x1
#define AN_UF_PAGE1_25G_CR1_ABILITY_OFFSET 0x5

#define AN_UF_PAGE1_50G_KR2_ABILITY_MASK   0x1
#define AN_UF_PAGE1_50G_KR2_ABILITY_OFFSET 0x8

#define AN_UF_PAGE1_50G_CR2_ABILITY_MASK   0x1
#define AN_UF_PAGE1_50G_CR2_ABILITY_OFFSET 0x9

#define AN_UF_PAGE2_CL91_SUPPORT_MASK   0x1
#define AN_UF_PAGE2_CL91_SUPPORT_OFFSET 0x8

#define AN_UF_PAGE2_CL74_SUPPORT_MASK   0x1
#define AN_UF_PAGE2_CL74_SUPPORT_OFFSET 0x9

#define AN_UF_PAGE2_CL91_REQ_MASK   0x1
#define AN_UF_PAGE2_CL91_REQ_OFFSET 0xA

#define AN_UF_PAGE2_CL74_REQ_MASK   0x1
#define AN_UF_PAGE2_CL74_REQ_OFFSET 0xB

#define TEFMOD_25G_KR_SPD_IF 0x11
#define TEFMOD_25G_CR_SPD_IF 0x10

#define TEFMOD_50G_KR2_SPD_IF 0x31
#define TEFMOD_50G_CR2_SPD_IF 0x30

#define TEFMOD_100G_KR4_SPD_IF 0x41
#define TEFMOD_100G_CR4_SPD_IF 0x40

#define HCD_SPEED_ID_UNDEFINED 0xFF 

typedef enum ieee_sel_e {
    IEEE_SEL_RSVD = 0,
    IEEE_SEL_802P3,
    IEEE_SEL_802P9,
    IEEE_SEL_802P5,
    IEEE_SEL_1394
} ieee_sel_t;

typedef enum an_page_id_e {
  TEFMOD_AN_PAGE_ID_NONE = 0,
  TEFMOD_AN_PAGE_ID_BASE_PAGE,
  TEFMOD_AN_PAGE_ID_MSG_PAGE,
  TEFMOD_AN_PAGE_ID_UP_PAGE,
  TEFMOD_AN_PAGE_ID_NULL_PAGE,
  TEFMOD_AN_PAGE_ID_MAX
} an_page_id_t; 

/* Bit error mask to generate FEC error. */
typedef struct tefmod_fec_error_mask_s {
    uint32_t error_mask_bit_31_0;     /* Error mask bit 31-0. */
    uint32_t error_mask_bit_63_32;    /* Error mask bit 63-32. */
    uint16_t error_mask_bit_79_64;    /* Error mask bit 79-64. */
} tefmod_fec_error_mask_t;

extern int phymod_debug_check(uint32_t flags, PHYMOD_ST *pc);

extern int configure_st_entry(int st_entry_num, int st_entry_speed, PHYMOD_ST* pc);

extern int tefmod_pmd_osmode_set(PHYMOD_ST* pc, tefmod_spd_intfc_type_t spd_intf, phymod_ref_clk_t refclk, int os_mode);
extern int tefmod_pmd_x4_reset(PHYMOD_ST* pc);
extern int tefmod_tx_loopback_get(PHYMOD_ST* pc, uint32_t *enable);
extern int tefmod_firmware_set(PHYMOD_ST* pc);
extern int tefmod_tx_lane_disable (PHYMOD_ST* pc, int tx);
extern int tefmod_init_pcs_ilkn(PHYMOD_ST* pc);
extern int tefmod_pcs_ilkn_chk(PHYMOD_ST* pc, int *ilkn_set);
extern int tefmod_pmd_reset_bypass (PHYMOD_ST* pc, int pmd_reset_control);
extern int tefmod_afe_speed_up_dsc_vga(PHYMOD_ST* pc);
extern int tefmod_get_plldiv(PHYMOD_ST* pc, uint32_t *plldiv_r_val);
extern int tefmod_update_port_mode_select(PHYMOD_ST* pc, tefmod_port_type_t port_type, int master_port, int tsc_clk_freq_pll_by_48, int pll_reset_en);
extern int tefmod_set_port_mode(PHYMOD_ST* pc, int refclk, int plldiv, tefmod_port_type_t port_type, int master_port, int tsc_clk_freq_pll_by_48, int pll_reset_en);
extern int tefmod_tx_loopback_control(PHYMOD_ST* pc, int pcs_gloop_en, int starting_lane, int num_lanes);
extern int tefmod_tx_pmd_loopback_control(PHYMOD_ST* pc, int pmd_gloop_en);
extern int tefmod_rx_loopback_control(PHYMOD_ST* pc, int pcs_rloop_en);
extern int tefmod_rx_pmd_loopback_control(PHYMOD_ST* pc, int pcs_rloop_en, int pmd_rloop_en, int lane);
extern int tefmod_credit_set(PHYMOD_ST* pc);
extern int tefmod_encode_set(PHYMOD_ST* pc, int per_lane_control, tefmod_spd_intfc_type_t spd_intf, int tx_am_timer_init_val);
extern int tefmod_lf_rf_control(PHYMOD_ST* pc);
extern int tefmod_decode_set(PHYMOD_ST* pc);
extern int tefmod_trigger_speed_change(PHYMOD_ST* pc);
extern int tefmod_set_override_0(PHYMOD_ST* pc, int per_field_override_en);
extern int tefmod_set_override_1(PHYMOD_ST* pc, int per_lane_control, uint32_t per_field_override_en);
extern int tefmod_credit_control(PHYMOD_ST* pc, int per_lane_control);
extern int tefmod_tx_lane_control_set(PHYMOD_ST* pc, tefmod_tx_disable_enum_t dis);
extern int tefmod_tx_lane_control_get(PHYMOD_ST* pc,  int *reset, int *enable);
extern int tefmod_rx_lane_control(PHYMOD_ST* pc, int accData, int per_lane_control);
extern int tefmod_bypass_sc(PHYMOD_ST* pc);
extern int tefmod_revid_read(PHYMOD_ST* pc, uint32_t *revIdV);
extern int tefmod_clause72_control(PHYMOD_ST* pc, int per_lane_control);
extern int tefmod_prbs_check(PHYMOD_ST* pc, int accData);
extern int tefmod_prbs_mode(PHYMOD_ST* pc, int per_lane_control);
extern int tefmod_prbs_control(PHYMOD_ST* pc, int per_lane_control);
extern int tefmod_cjpat_crpat_control(PHYMOD_ST* pc);
extern int tefmod_cjpat_crpat_check(PHYMOD_ST* pc);
extern int tefmod_tx_bert_control(PHYMOD_ST* pc);
extern int tefmod_FEC_control(PHYMOD_ST* pc, tefmod_fec_type_t fec_type, int enable, int cl74or91);
extern int tefmod_FEC_get(PHYMOD_ST* pc,  tefmod_fec_type_t fec_type, int *fec_en);
extern int tefmod_power_control(PHYMOD_ST* pc, int tx, int rx);
extern int tefmod_duplex_control(PHYMOD_ST* pc);
extern int tefmod_disable_pcs_falcon(PHYMOD_ST* pc);
extern int tefmod_pmd_addr_lane_swap(PHYMOD_ST *pc, int per_lane_control);
extern int tefmod_pmd_lane_swap_tx(PHYMOD_ST *pc, int per_lane_control);
extern int tefmod_pmd_reset_remove(PHYMOD_ST *pc, int pmd_touched);
extern int tefmod_pcs_lane_swap(PHYMOD_ST *pc, int per_lane_control);
extern int tefmod_init_pcs_falcon(PHYMOD_ST* pc);
extern int tefmod_init_pmd_falcon(PHYMOD_ST* pc);
extern int tefmod_set_sc_speed(PHYMOD_ST* pc);
extern int tefmod_poll_for_sc_done(PHYMOD_ST* pc, int mapped_speed);
extern int tefmod_check_status(PHYMOD_ST* pc);
extern int tefmod_set_port_mode_sel(PHYMOD_ST* pc, int tsc_touched, tefmod_port_type_t port_type);
extern int tefmod_init_pcs_fs(PHYMOD_ST* pc);
extern int tefmod_init_pcs_an(PHYMOD_ST* pc);
extern int tefmod_set_an_override(PHYMOD_ST* pc);
extern int tefmod_tx_squelch_get(PHYMOD_ST *pc, int *val);
extern int tefmod_refclk_get(PHYMOD_ST* pc, phymod_ref_clk_t* ref_clk);
extern int tefmod_refclk_set(PHYMOD_ST* pc, phymod_ref_clk_t ref_clock);

extern int tefmod_get_mapped_speed(tefmod_spd_intfc_type_t spd_intf, int *speed);
extern int tefmod_toggle_sw_speed_change(PHYMOD_ST* pc);

extern int tefmod_autoneg_get(PHYMOD_ST* pc);
#ifdef _DV_TB_
extern int tefmod_autoneg_set(PHYMOD_ST* pc);
extern int tefmod_autoneg_control(PHYMOD_ST* pc);
extern int tefmod_set_spd_intf(PHYMOD_ST* pc);
extern int tefmod_set_an_port_mode(PHYMOD_ST* pc);
#endif /* _DV_TB_ */
#ifdef _SDK_TEFMOD_
int tefmod_set_an_port_mode(PHYMOD_ST* pc, int num_of_lanes, int starting_lane);
extern int tefmod_set_an_single_port_mode(PHYMOD_ST* pc, int an_enable); 
extern int tefmod_set_spd_intf(PHYMOD_ST *pc, tefmod_spd_intfc_type_t spd_intf);
extern int tefmod_autoneg_set_init(PHYMOD_ST* pc, tefmod_an_init_t *an_init_st);
extern int tefmod_autoneg_timer_init(PHYMOD_ST* pc);
extern int tefmod_autoneg_control(PHYMOD_ST* pc, tefmod_an_control_t *an_control);
extern int tefmod_autoneg_ability_set(PHYMOD_ST* pc, tefmod_an_adv_ability_t *an_ability_st);
extern int tefmod_diag(PHYMOD_ST *ws, tefmod_diag_type_t diag_type);
extern int tefmod_diag_disp(PHYMOD_ST *ws, const char* arg);
extern int tefmod_autoneg_control_get(PHYMOD_ST* pc, tefmod_an_control_t *an_control, int *an_complete);
extern int tefmod_autoneg_ability_get(PHYMOD_ST* pc, tefmod_an_adv_ability_t *an_ability_st);
extern int tefmod_autoneg_remote_ability_get(PHYMOD_ST* pc, tefmod_an_adv_ability_t *an_ability_st);
extern int tefmod_st_control_field_set (PHYMOD_ST* pc,uint16_t st_entry_no, override_type_t  st_control_field, uint16_t st_field_value);
extern int tefmod_port_enable_set(PHYMOD_ST *pc, int enable);
extern int tefmod_tx_squelch_set(PHYMOD_ST *pc, int enable);
extern int tefmod_rx_squelch_set(PHYMOD_ST *pc, int enable);
extern int tefmod_rx_squelch_get(PHYMOD_ST *pc, int *enable);
extern int tefmod_default_init(PHYMOD_ST* pc);
extern int tefmod_pmd_x4_reset_get(PHYMOD_ST *pc, int *is_in_reset);
#endif /* _SDK_TEFMOD_ */

extern int tefmod_port_enable_get(PHYMOD_ST *pc, int *tx_enable, int *rx_enable);
extern int tefmod_pll_lock_get(PHYMOD_ST* pc, int* lockStatus);
extern int tefmod_pmd_lock_get(PHYMOD_ST* pc, uint32_t* lockStatus);
extern int tefmod_set_pll_mode(PHYMOD_ST* pc, int pmd_touched, tefmod_spd_intfc_type_t spd_intf, int pll_mode);           /* SET_PLL_MODE */
extern int tefmod_speed_id_get(PHYMOD_ST* pc, int *speed_id);
extern int tefmod_pmd_reset_seq(PHYMOD_ST* pc, int pmd_touched); /* PMD_RESET_SEQ */
extern int tefmod_master_port_num_set( PHYMOD_ST *pc,  int port_num);
extern int tefmod_update_port_mode( PHYMOD_ST *pa, int *pll_restart);
extern int tefmod_pll_reset_enable_set (PHYMOD_ST *pa , int enable);
extern int tefmod_get_pcs_link_status(PHYMOD_ST* pc, uint32_t *link);
extern int tefmod_get_pcs_latched_link_status(PHYMOD_ST* pc, uint32_t *link);
extern int tefmod_disable_get(PHYMOD_ST* pc, uint32_t* enable);
extern int tefmod_disable_set(PHYMOD_ST* pc);
extern int tefmod_enable_set(PHYMOD_ST* pc);
extern int tefmod_plldiv_lkup_get(PHYMOD_ST* pc, tefmod_spd_intfc_type_t spd_intf, phymod_ref_clk_t refclk, uint32_t *plldiv);
extern int tefmod_osmode_lkup_get(PHYMOD_ST* pc, tefmod_spd_intfc_type_t spd_intf, uint32_t *osmode);
extern int tefmod_pcs_lane_swap_get ( PHYMOD_ST *pc,  uint32_t *tx_rx_swap);
extern int tefmod_pmd_lane_swap_tx_get ( PHYMOD_ST *pc, uint32_t *tx_lane_map);
extern int tefmod_rx_lane_control_set(PHYMOD_ST* pc, int enable);         /* RX_LANE_CONTROL */
extern int tefmod_rx_lane_control_get(PHYMOD_ST* pc, int *enable);
extern int tefmod_tx_rx_polarity_get ( PHYMOD_ST *pc, uint32_t* tx_polarity, uint32_t* rx_polarity);
extern int tefmod_tx_rx_polarity_set ( PHYMOD_ST *pc, uint32_t tx_polarity, uint32_t rx_polarity);

int tefmod_eee_control_set(PHYMOD_ST* pc, uint32_t enable);
int tefmod_eee_control_get(PHYMOD_ST* pc, uint32_t* enable);
int tefmod_cl74_chng_default (PHYMOD_ST* pc);
int tefmod_cl91_ecc_clear(PHYMOD_ST* pc, uint32_t model);

extern int tefmod_an_msa_mode_set(PHYMOD_ST* pc, uint32_t val);
extern int tefmod_an_msa_mode_get(PHYMOD_ST* pc, uint32_t* val);
extern int tefmod_an_oui_set(PHYMOD_ST* pc, tefmod_an_oui_t oui);
extern int tefmod_an_oui_get(PHYMOD_ST* pc, tefmod_an_oui_t *oui);
extern int tefmod_autoneg_status_get(PHYMOD_ST* pc, int *an_en, int *an_done);
extern int tefmod_autoneg_lp_status_get(PHYMOD_ST* pc, uint16_t *baseP0, uint16_t *baseP1, uint16_t *baseP2, uint16_t *nextP3, uint16_t *nextP4);
extern int tefmod_hg2_codec_set(PHYMOD_ST* pc, tefmod_hg2_codec_t hg2_codec);
extern int tefmod_hg2_codec_get(PHYMOD_ST* pc, tefmod_hg2_codec_t* hg2_codec);

extern int tefmod_fec_correctable_counter_get(PHYMOD_ST* pc, uint32_t* count);
extern int tefmod_fec_uncorrectable_counter_get(PHYMOD_ST* pc, uint32_t* count);
extern int tefmod_fec_cl91_correctable_counter_get(PHYMOD_ST* pc, uint32_t* count);
extern int tefmod_fec_cl91_uncorrectable_counter_get(PHYMOD_ST* pc, uint32_t* count);
extern int tefmod_pcs_ilkn_mode_set(PHYMOD_ST* pc);              /* INIT_PCS_ILKN */

extern int tefmod_sw_an_control(PHYMOD_ST* pc, int enable);
extern int tefmod_sw_an_ld_page_load(PHYMOD_ST* pc, phymod_an_page_t *pages, int is_base_page);
extern int tefmod_sw_an_lp_page_read(PHYMOD_ST* pc, phymod_an_page_t *pages);
extern int tefmod_sw_an_ctl_sts_get(PHYMOD_ST* pc, phymod_sw_an_ctrl_status_t *an_ctrl_status);
extern int tefmod_sw_an_set_ack(PHYMOD_ST* pc);
extern int tefmod_set_speed_id(PHYMOD_ST *pc, int speed_id);
extern int tefmod_setup_for_sw_an(PHYMOD_ST* pc, int num_lanes);
extern int tefmod_pxgng_status_an_ack_complete_get(PHYMOD_ST *pc, int *ack_complte);
extern int tefmod_pxgng_status_an_good_chk_get(PHYMOD_ST *pc, uint8_t *an_good_chk);
extern int tefmod_sw_an_control(PHYMOD_ST *pc, int enable);
extern int tefmod_sw_an_advert_set(PHYMOD_ST* pc, tefmod_an_adv_ability_t *cl73_adv, phymod_sw_an_ctxt_t *an_ctxt);
extern int tefmod_sw_an_fec_override(PHYMOD_ST *pc, tefmod_fec_type_t fec_type, int enable);
extern int tefmod_serdes_model_get(PHYMOD_ST *pc, uint32_t *model);
extern int tefmod_25g_rsfec_am_init(PHYMOD_ST *pc);

extern int tefmod_encode_mode_get(PHYMOD_ST* pc, tefmod_encode_mode *mode);
extern int tefmod_bip_error_counter_get(PHYMOD_ST* pc, uint32_t* count);
extern int tefmod_cl49_ber_counter_get(PHYMOD_ST* pc, uint32_t* count);
extern int tefmod_cl82_ber_counter_get(PHYMOD_ST* pc, uint32_t* count);
extern int tefmod_fec_override_set(PHYMOD_ST *pc, uint32_t enable);
extern int tefmod_fec_override_get(PHYMOD_ST *pc, uint32_t *enable);

extern int tefmod_synce_stg0_mode_set(PHYMOD_ST *pc, int mode);
extern int tefmod_synce_stg0_mode_get(PHYMOD_ST *pc, int *mode);
extern int tefmod_synce_stg1_mode_set(PHYMOD_ST *pc, int mode);
extern int tefmod_synce_stg1_mode_get(PHYMOD_ST *pc, int *mode);
extern int tefmod_synce_clk_ctrl_set(PHYMOD_ST *pc, uint32_t val);
extern int tefmod_synce_clk_ctrl_get(PHYMOD_ST *pc, uint32_t *val);
extern int tefmod_rsfec_symbol_error_counter_get(PHYMOD_ST* pc, int max_count,
                                                 int* actual_count, uint32_t* symb_err_cnt);
extern int tefmod_fec_error_inject_set(PHYMOD_ST *pc,
                                       uint16_t error_control_map,
                                       tefmod_fec_error_mask_t bit_error_mask);
extern int tefmod_fec_error_inject_get(PHYMOD_ST *pc,
                                       uint16_t* error_control_map,
                                       tefmod_fec_error_mask_t* bit_error_mask);
#endif  /*  _tefmod_H_ */

