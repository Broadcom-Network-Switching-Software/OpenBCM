/*----------------------------------------------------------------------
 * 
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
#ifndef tefmod16_sc_lkup_table_H_
#define tefmod16_sc_lkup_table_H_ 

#ifndef _SDK_TEFMOD16_
#ifndef _DV_TB_
 #define _SDK_TEFMOD16_ 1
#endif
#endif

#ifdef _SDK_TEFMOD16_
#include "tefmod16_enum_defines.h"
#include "tefmod16.h"
#include <phymod/phymod.h>
#endif
#include "tf16PCSRegEnums.h"
#include "tefmod16_sc_defines.h"

typedef struct sc_pcs_entry_t
{
  int num_lanes;
  int os_mode;
  int t_fifo_mode;
  int t_enc_mode;
  int t_HG2_ENABLE;
  int t_pma_btmx_mode;

  int scr_mode;
  int descr_mode;
  int dec_tl_mode;
  int deskew_mode;
  int dec_fsm_mode;
  int r_HG2_ENABLE;

  int bs_sm_sync_mode;
  int bs_sync_en;
  int bs_dist_mode;
  int bs_btmx_mode;
  int cl72_en;

/*
  Notes:
    1. When the loop_cnt1 == 0, the clk_cnt1 is a don't care
    2. the pcs crd/cnt are valid only when byte replication is valid, say for 10M, 100M cases
*/
  /* credit settings*/

  int clkcnt0;
  int clkcnt1;
  int lpcnt0;
  int lpcnt1;
  int cgc;

/*  New SC override fields for GEN2 */

  int t_cl91_cw_scr;
  int t_5bit_xor_en;
  int t_pma_cl91_mux_sel;
  int t_pma_watermark;
  int t_pma_btmx_delay;
  int am_spacing_mul;
  int cl91_blksync_mode;
  int t_merge_mode;
  int r_cl91_cw_scr;
  int r_tc_in_mode;
  int r_tc_mode;
  int r_5bit_xor_en;
  int r_tc_out_mode;
  int r_HG2_en;
  int r_merge_mode; 
  int USE_100G_AM0;  
  int USE_100G_AM123; 
  int ber_window_sel;   
  int ber_count_sel;    
  int cl74_shcorrupt;   
  int corrupt_6th_group;
  int corrupt_2nd_group;


  int clkcnt0_by48;
  int clkcnt1_by48;
  int lpcnt0_by48;
  int lpcnt1_by48;
  int cgc_by48;

} sc_pcs_entry;

#ifdef _DV_TB_
static sc_pcs_entry sc_ht_pcs[] = {

  /*`SPEED_10G_CR1: 0*/
  {
    NUM_LANES_01,
    OS_MODE_2,
    T_FIFO_MODE_BYPASS,
    T_ENC_MODE_CL49,
    0,
    T_PMA_BITMUX_MODE_1to1,
    T_SCR_MODE_CL49,
    R_DESCR_MODE_CL49,
    R_DEC_TL_MODE_CL49,
    R_DESKEW_MODE_CL49,
    R_DEC_FSM_MODE_CL49,
    0,
    BS_CL49_SYNC_MODE,
    1,
    BS_DIST_MODE_5_LANE_TDM,
    BS_BTMX_MODE_1to1,
    1,
    9, 5, 1, 18, 4,
    33, 0, 1, 0, 8
  },
  /*`SPEED_10G_KR1: 1*/
  {
    NUM_LANES_01,
    OS_MODE_2,
    T_FIFO_MODE_BYPASS,
    T_ENC_MODE_CL49,
    0,
    T_PMA_BITMUX_MODE_1to1,
    T_SCR_MODE_CL49,
    R_DESCR_MODE_CL49,
    R_DEC_TL_MODE_CL49,
    R_DESKEW_MODE_CL49,
    R_DEC_FSM_MODE_CL49,
    0,
    BS_CL49_SYNC_MODE,
    1,
    BS_DIST_MODE_5_LANE_TDM,
    BS_BTMX_MODE_1to1,
    1,
    9, 5, 1, 18, 4,
    33, 0, 1, 0, 8
  },
  /*`SPEED_10G_X1: 2*/
  {
    NUM_LANES_01,
    OS_MODE_2,
    T_FIFO_MODE_BYPASS,
    T_ENC_MODE_CL49,
    0,
    T_PMA_BITMUX_MODE_1to1,
    T_SCR_MODE_CL49,
    R_DESCR_MODE_CL49,
    R_DEC_TL_MODE_CL49,
    R_DESKEW_MODE_CL49,
    R_DEC_FSM_MODE_CL49,
    0,
    BS_CL49_SYNC_MODE,
    1,
    BS_DIST_MODE_5_LANE_TDM,
    BS_BTMX_MODE_1to1,
    1,
    9, 5, 1, 18, 4,
    33, 0, 1, 0, 8
  },
  /*`NULL ENTRY: 3*/
  {0},
  /*`SPEED_10G_HG2_CR1: 4*/
  {
    NUM_LANES_01,
    OS_MODE_2,
    T_FIFO_MODE_BYPASS,
    T_ENC_MODE_CL49,
    1,
    T_PMA_BITMUX_MODE_1to1,
    T_SCR_MODE_CL49,
    R_DESCR_MODE_CL49,
    R_DEC_TL_MODE_CL49,
    R_DESKEW_MODE_CL49,
    R_DEC_FSM_MODE_CL49,
    1,
    BS_CL49_SYNC_MODE,
    1,
    BS_DIST_MODE_5_LANE_TDM,
    BS_BTMX_MODE_1to1,
    1,
    9, 5, 1, 18, 4,
    33, 0, 1, 0, 8
  },
  /*`SPEED_10G_HG2_KR1: 5*/
  {
    NUM_LANES_01,
    OS_MODE_2,
    T_FIFO_MODE_BYPASS,
    T_ENC_MODE_CL49,
    1,
    T_PMA_BITMUX_MODE_1to1,
    T_SCR_MODE_CL49,
    R_DESCR_MODE_CL49,
    R_DEC_TL_MODE_CL49,
    R_DESKEW_MODE_CL49,
    R_DEC_FSM_MODE_CL49,
    1,
    BS_CL49_SYNC_MODE,
    1,
    BS_DIST_MODE_5_LANE_TDM,
    BS_BTMX_MODE_1to1,
    1,
    9, 5, 1, 18, 4,
    33, 0, 1, 0, 8
  },
  /*`SPEED_10G_HG2_X1: 6*/
  {
    NUM_LANES_01,
    OS_MODE_2,
    T_FIFO_MODE_BYPASS,
    T_ENC_MODE_CL49,
    1,
    T_PMA_BITMUX_MODE_1to1,
    T_SCR_MODE_CL49,
    R_DESCR_MODE_CL49,
    R_DEC_TL_MODE_CL49,
    R_DESKEW_MODE_CL49,
    R_DEC_FSM_MODE_CL49,
    1,
    BS_CL49_SYNC_MODE,
    1,
    BS_DIST_MODE_5_LANE_TDM,
    BS_BTMX_MODE_1to1,
    1,
    9, 5, 1, 18, 4,
    33, 0, 1, 0, 8
  },
  /*`NULL ENTRY: 7*/ {0},
  /*`SPEED_20G_CR1: 8*/
  {
    NUM_LANES_01,
    OS_MODE_1,
    T_FIFO_MODE_BYPASS,
    T_ENC_MODE_CL49,
    0,
    T_PMA_BITMUX_MODE_1to1,
    T_SCR_MODE_CL49,
    R_DESCR_MODE_CL49,
    R_DEC_TL_MODE_CL49,
    R_DESKEW_MODE_CL49,
    R_DEC_FSM_MODE_CL49,
    0,
    BS_CL49_SYNC_MODE,
    1,
    BS_DIST_MODE_5_LANE_TDM,
    BS_BTMX_MODE_1to1,
    1,
    9, 5, 1,18, 4,
    33, 0, 1, 0, 4 
  },
  /*`SPEED_20G_KR1: 9 */
  {
    NUM_LANES_01,
    OS_MODE_1,
    T_FIFO_MODE_BYPASS,
    T_ENC_MODE_CL49,
    0,
    T_PMA_BITMUX_MODE_1to1,
    T_SCR_MODE_CL49,
    R_DESCR_MODE_CL49,
    R_DEC_TL_MODE_CL49,
    R_DESKEW_MODE_CL49,
    R_DEC_FSM_MODE_CL49,
    0,
    BS_CL49_SYNC_MODE,
    1,
    BS_DIST_MODE_5_LANE_TDM,
    BS_BTMX_MODE_1to1,
    1,
    9, 5, 1,18 , 4,
    33, 0, 1, 0, 4
  },
  /*`SPEED_20G_X1: 10*/
  {
    NUM_LANES_01,
    OS_MODE_1,
    T_FIFO_MODE_BYPASS,
    T_ENC_MODE_CL49,
    0,
    T_PMA_BITMUX_MODE_1to1,
    T_SCR_MODE_CL49,
    R_DESCR_MODE_CL49,
    R_DEC_TL_MODE_CL49,
    R_DESKEW_MODE_CL49,
    R_DEC_FSM_MODE_CL49,
    0,
    BS_CL49_SYNC_MODE,
    1,
    BS_DIST_MODE_5_LANE_TDM,
    BS_BTMX_MODE_1to1,
    1,
    9, 5, 1,18 , 4,
    33, 0, 1, 0, 4
  },
  /*`NULL ENTRY: 11*/ {0},
  /*`SPEED_20G_HG2_CR1: 12*/
  {
    NUM_LANES_01,
    OS_MODE_1,
    T_FIFO_MODE_BYPASS,
    T_ENC_MODE_CL49,
    1,
    T_PMA_BITMUX_MODE_1to1,
    T_SCR_MODE_CL49,
    R_DESCR_MODE_CL49,
    R_DEC_TL_MODE_CL49,
    R_DESKEW_MODE_CL49,
    R_DEC_FSM_MODE_CL49,
    1,
    BS_CL49_SYNC_MODE,
    1,
    BS_DIST_MODE_5_LANE_TDM,
    BS_BTMX_MODE_1to1,
    1,
    9, 5, 1,18 , 4,
    33, 0, 1, 0, 4
  },
  /*`SPEED_20G_HG2_KR1: 13*/
  {
    NUM_LANES_01,
    OS_MODE_1,
    T_FIFO_MODE_BYPASS,
    T_ENC_MODE_CL49,
    1,
    T_PMA_BITMUX_MODE_1to1,
    T_SCR_MODE_CL49,
    R_DESCR_MODE_CL49,
    R_DEC_TL_MODE_CL49,
    R_DESKEW_MODE_CL49,
    R_DEC_FSM_MODE_CL49,
    1,
    BS_CL49_SYNC_MODE,
    1,
    BS_DIST_MODE_5_LANE_TDM,
    BS_BTMX_MODE_1to1,
    1,
    9, 5, 1,18 , 4,
    33, 0, 1, 0, 4
  },
  /*`SPEED_20G_HG2_X1: 14*/
  {
    NUM_LANES_01,
    OS_MODE_1,
    T_FIFO_MODE_BYPASS,
    T_ENC_MODE_CL49,
    1,
    T_PMA_BITMUX_MODE_1to1,
    T_SCR_MODE_CL49,
    R_DESCR_MODE_CL49,
    R_DEC_TL_MODE_CL49,
    R_DESKEW_MODE_CL49,
    R_DEC_FSM_MODE_CL49,
    1,
    BS_CL49_SYNC_MODE,
    1,
    BS_DIST_MODE_5_LANE_TDM,
    BS_BTMX_MODE_1to1,
    1,
    9, 5, 1,18 , 4,
    33, 0, 1, 0, 4
  },
  /*`NULL ENTRY: 15*/ {0},
  /*`SPEED_25G_CR1: 16*/
  {
    NUM_LANES_01,
    OS_MODE_1,
    T_FIFO_MODE_BYPASS,
    T_ENC_MODE_CL49,
    0,
    T_PMA_BITMUX_MODE_1to1,
    T_SCR_MODE_CL49,
    R_DESCR_MODE_CL49,
    R_DEC_TL_MODE_CL49,
    R_DESKEW_MODE_CL49,
    R_DEC_FSM_MODE_CL49,
    0,
    BS_CL49_SYNC_MODE,
    1,
    BS_DIST_MODE_5_LANE_TDM,
    BS_BTMX_MODE_1to1,
    1,
    9, 5, 1,18 , 4,
    33, 0, 1, 0, 4
  },
  /*`SPEED_25G_KR1:  17*/
  {
    NUM_LANES_01,
    OS_MODE_1,
    T_FIFO_MODE_BYPASS,
    T_ENC_MODE_CL49,
    0,
    T_PMA_BITMUX_MODE_1to1,
    T_SCR_MODE_CL49,
    R_DESCR_MODE_CL49,
    R_DEC_TL_MODE_CL49,
    R_DESKEW_MODE_CL49,
    R_DEC_FSM_MODE_CL49,
    0,
    BS_CL49_SYNC_MODE,
    1,
    BS_DIST_MODE_5_LANE_TDM,
    BS_BTMX_MODE_1to1,
    1,
    9, 5, 1,18 , 4,
    33, 0, 1, 0, 4
  },
  /*`SPEED_25G_X1: 18*/
  {
    NUM_LANES_01,
    OS_MODE_1,
    T_FIFO_MODE_BYPASS,
    T_ENC_MODE_CL49,
    0,
    T_PMA_BITMUX_MODE_1to1,
    T_SCR_MODE_CL49,
    R_DESCR_MODE_CL49,
    R_DEC_TL_MODE_CL49,
    R_DESKEW_MODE_CL49,
    R_DEC_FSM_MODE_CL49,
    0,
    BS_CL49_SYNC_MODE,
    1,
    BS_DIST_MODE_5_LANE_TDM,
    BS_BTMX_MODE_1to1,
    1,
    9, 5, 1,18 , 4,
    33, 0, 1, 0, 4
  },
  /*`NULL ENTRY: 19*/ {0},
  /*`SPEED_25G_HG2_CR1: 20*/
  {
    NUM_LANES_01,
    OS_MODE_1,
    T_FIFO_MODE_BYPASS,
    T_ENC_MODE_CL49,
    1,
    T_PMA_BITMUX_MODE_1to1,
    T_SCR_MODE_CL49,
    R_DESCR_MODE_CL49,
    R_DEC_TL_MODE_CL49,
    R_DESKEW_MODE_CL49,
    R_DEC_FSM_MODE_CL49,
    1,
    BS_CL49_SYNC_MODE,
    1,
    BS_DIST_MODE_5_LANE_TDM,
    BS_BTMX_MODE_1to1,
    1,
    9, 5, 1,18 , 4,
    33, 0, 1, 0, 4
  },
  /*`SPEED_25G_HG2_KR1: 21*/
  {
    NUM_LANES_01,
    OS_MODE_1,
    T_FIFO_MODE_BYPASS,
    T_ENC_MODE_CL49,
    1,
    T_PMA_BITMUX_MODE_1to1,
    T_SCR_MODE_CL49,
    R_DESCR_MODE_CL49,
    R_DEC_TL_MODE_CL49,
    R_DESKEW_MODE_CL49,
    R_DEC_FSM_MODE_CL49,
    1,
    BS_CL49_SYNC_MODE,
    1,
    BS_DIST_MODE_5_LANE_TDM,
    BS_BTMX_MODE_1to1,
    1,
    9, 5, 1,18 , 4,
    33, 0, 1, 0, 4
  },
  /*`SPEED_25G_HG2_X1: 22*/
  {
    NUM_LANES_01,
    OS_MODE_1,
    T_FIFO_MODE_BYPASS,
    T_ENC_MODE_CL49,
    1,
    T_PMA_BITMUX_MODE_1to1,
    T_SCR_MODE_CL49,
    R_DESCR_MODE_CL49,
    R_DEC_TL_MODE_CL49,
    R_DESKEW_MODE_CL49,
    R_DEC_FSM_MODE_CL49,
    1,
    BS_CL49_SYNC_MODE,
    1,
    BS_DIST_MODE_5_LANE_TDM,
    BS_BTMX_MODE_1to1,
    1,
    9, 5, 1,18 , 4,
    33, 0, 1, 0, 4
  },
  /*`NULL ENTRY: 23*/ {0},
  /*`SPEED_20G_CR2:  24*/
  {
    NUM_LANES_02,
    OS_MODE_2,
    T_FIFO_MODE_20G,
    T_ENC_MODE_CL82,
    0,
    T_PMA_BITMUX_MODE_1to1,
    T_SCR_MODE_20G,
    R_DESCR_MODE_CL82,
    R_DEC_TL_MODE_CL82,
    R_DESKEW_MODE_20G,
    R_DEC_FSM_MODE_CL82,
    0,
    BS_CL82_SYNC_MODE,
    1,
    BS_DIST_MODE_2_LANE_TDM_1_VLANE,
    BS_BTMX_MODE_1to1,
    1,
    9, 5, 1,18 , 2,
    33, 0, 1, 0, 4
  },
  /*`SPEED_20G_KR2: 25*/
  {
    NUM_LANES_02,
    OS_MODE_2,
    T_FIFO_MODE_20G,
    T_ENC_MODE_CL82,
    0,
    T_PMA_BITMUX_MODE_1to1,
    T_SCR_MODE_20G,
    R_DESCR_MODE_CL82,
    R_DEC_TL_MODE_CL82,
    R_DESKEW_MODE_20G,
    R_DEC_FSM_MODE_CL82,
    0,
    BS_CL82_SYNC_MODE,
    1,
    BS_DIST_MODE_2_LANE_TDM_1_VLANE,
    BS_BTMX_MODE_1to1,
    1,
    9, 5, 1,18 , 2,
    33, 0, 1, 0, 4
  },
  /*`SPEED_20G_X2: 26*/
  {
    NUM_LANES_02,
    OS_MODE_2,
    T_FIFO_MODE_20G,
    T_ENC_MODE_CL82,
    0,
    T_PMA_BITMUX_MODE_1to1,
    T_SCR_MODE_20G,
    R_DESCR_MODE_CL82,
    R_DEC_TL_MODE_CL82,
    R_DESKEW_MODE_20G,
    R_DEC_FSM_MODE_CL82,
    0,
    BS_CL82_SYNC_MODE,
    1,
    BS_DIST_MODE_2_LANE_TDM_1_VLANE,
    BS_BTMX_MODE_1to1,
    1,
    9, 5, 1,18 , 2,
    33, 0, 1, 0, 4
  },
  /*`NULL ENTRY: 27*/ {0},
  /*`SPEED_20G_HG2_CR2: 28*/
  {
    NUM_LANES_02,
    OS_MODE_2,
    T_FIFO_MODE_20G,
    T_ENC_MODE_CL82,
    1,
    T_PMA_BITMUX_MODE_1to1,
    T_SCR_MODE_20G,
    R_DESCR_MODE_CL82,
    R_DEC_TL_MODE_CL82,
    R_DESKEW_MODE_20G,
    R_DEC_FSM_MODE_CL82,
    1,
    BS_CL82_SYNC_MODE,
    1,
    BS_DIST_MODE_2_LANE_TDM_1_VLANE,
    BS_BTMX_MODE_1to1,
    1,
    9, 5, 1,18 , 2,
    33, 0, 1, 0, 4
  },
  /*`SPEED_20G_HG2_KR2: 29*/
  {
    NUM_LANES_02,
    OS_MODE_2,
    T_FIFO_MODE_20G,
    T_ENC_MODE_CL82,
    1,
    T_PMA_BITMUX_MODE_1to1,
    T_SCR_MODE_20G,
    R_DESCR_MODE_CL82,
    R_DEC_TL_MODE_CL82,
    R_DESKEW_MODE_20G,
    R_DEC_FSM_MODE_CL82,
    1,
    BS_CL82_SYNC_MODE,
    1,
    BS_DIST_MODE_2_LANE_TDM_1_VLANE,
    BS_BTMX_MODE_1to1,
    1,
    9, 5, 1,18 , 2,
    33, 0, 1, 0, 4
  },
  /*`SPEED_20G_HG2_X2: 30*/
  {
    NUM_LANES_02,
    OS_MODE_2,
    T_FIFO_MODE_20G,
    T_ENC_MODE_CL82,
    1,
    T_PMA_BITMUX_MODE_1to1,
    T_SCR_MODE_20G,
    R_DESCR_MODE_CL82,
    R_DEC_TL_MODE_CL82,
    R_DESKEW_MODE_20G,
    R_DEC_FSM_MODE_CL82,
    1,
    BS_CL82_SYNC_MODE,
    1,
    BS_DIST_MODE_2_LANE_TDM_1_VLANE,
    BS_BTMX_MODE_1to1,
    1,
    9, 5, 1,18 , 2,
    33, 0, 1, 0, 4
  },
  /*`NULL ENTRY: 31*/ {0},
  /*`SPEED_40G_CR2: 32*/
  {
    NUM_LANES_02,
    OS_MODE_1,
    T_FIFO_MODE_40G,
    T_ENC_MODE_CL82,
    0,
    T_PMA_BITMUX_MODE_2to1,
    T_SCR_MODE_40G_2_LANE,
    R_DESCR_MODE_CL82,
    R_DEC_TL_MODE_CL82,
    R_DESKEW_MODE_40G_2_LANE,
    R_DEC_FSM_MODE_CL82,
    0,
    BS_CL82_SYNC_MODE,
    1,
    BS_DIST_MODE_2_LANE_TDM_2_VLANE,
    BS_BTMX_MODE_2to1,
    1,
    9, 5, 1,18 , 2,
    33, 0, 1, 0, 2
  },
  /*`SPEED_40G_KR2: 33*/
  {
    NUM_LANES_02,
    OS_MODE_1,
    T_FIFO_MODE_40G,
    T_ENC_MODE_CL82,
    0,
    T_PMA_BITMUX_MODE_2to1,
    T_SCR_MODE_40G_2_LANE,
    R_DESCR_MODE_CL82,
    R_DEC_TL_MODE_CL82,
    R_DESKEW_MODE_40G_2_LANE,
    R_DEC_FSM_MODE_CL82,
    0,
    BS_CL82_SYNC_MODE,
    1,
    BS_DIST_MODE_2_LANE_TDM_2_VLANE,
    BS_BTMX_MODE_2to1,
    1,
    9, 5, 1,18,2,
    33, 0, 1, 0, 2 
  },
  /*`SPEED_40G_X2: 34*/
  {
    NUM_LANES_02,
    OS_MODE_1,
    T_FIFO_MODE_40G,
    T_ENC_MODE_CL82,
    0,
    T_PMA_BITMUX_MODE_2to1,
    T_SCR_MODE_40G_2_LANE,
    R_DESCR_MODE_CL82,
    R_DEC_TL_MODE_CL82,
    R_DESKEW_MODE_40G_2_LANE,
    R_DEC_FSM_MODE_CL82,
    0,
    BS_CL82_SYNC_MODE,
    1,
    BS_DIST_MODE_2_LANE_TDM_2_VLANE,
    BS_BTMX_MODE_2to1,
    1,
    9, 5, 1,18,2,
    33, 0, 1, 0, 2 
  },
  /*`NULL ENTRY: 35*/ {0},
  /*`SPEED_40G_HG2_CR2: 36*/
  {
    NUM_LANES_02,
    OS_MODE_1,
    T_FIFO_MODE_40G,
    T_ENC_MODE_CL82,
    1,
    T_PMA_BITMUX_MODE_2to1,
    T_SCR_MODE_40G_2_LANE,
    R_DESCR_MODE_CL82,
    R_DEC_TL_MODE_CL82,
    R_DESKEW_MODE_40G_2_LANE,
    R_DEC_FSM_MODE_CL82,
    1,
    BS_CL82_SYNC_MODE,
    1,
    BS_DIST_MODE_2_LANE_TDM_2_VLANE,
    BS_BTMX_MODE_2to1,
    1,
    9, 5, 1,18,2,
    33, 0, 1, 0, 2 
  },
  /*`SPEED_40G_HG2_KR2: 37*/
  {
    NUM_LANES_02,
    OS_MODE_1,
    T_FIFO_MODE_40G,
    T_ENC_MODE_CL82,
    1,
    T_PMA_BITMUX_MODE_2to1,
    T_SCR_MODE_40G_2_LANE,
    R_DESCR_MODE_CL82,
    R_DEC_TL_MODE_CL82,
    R_DESKEW_MODE_40G_2_LANE,
    R_DEC_FSM_MODE_CL82,
    1,
    BS_CL82_SYNC_MODE,
    1,
    BS_DIST_MODE_2_LANE_TDM_2_VLANE,
    BS_BTMX_MODE_2to1,
    1,
    9, 5, 1,18,2,
    33, 0, 1, 0, 2 
  },
  /*`SPEED_40G_HG2_X2: 38*/
  {
    NUM_LANES_02,
    OS_MODE_1,
    T_FIFO_MODE_40G,
    T_ENC_MODE_CL82,
    1,
    T_PMA_BITMUX_MODE_2to1,
    T_SCR_MODE_40G_2_LANE,
    R_DESCR_MODE_CL82,
    R_DEC_TL_MODE_CL82,
    R_DESKEW_MODE_40G_2_LANE,
    R_DEC_FSM_MODE_CL82,
    1,
    BS_CL82_SYNC_MODE,
    1,
    BS_DIST_MODE_2_LANE_TDM_2_VLANE,
    BS_BTMX_MODE_2to1,
    1,
    9, 5, 1,18,2,
    33, 0, 1, 0, 2 
  },
  /*`NULL ENTRY: 39*/ {0},
  /*`SPEED_40G_CR4:  40*/
  {
    NUM_LANES_04,
    OS_MODE_2,
    T_FIFO_MODE_40G,
    T_ENC_MODE_CL82,
    0,
    T_PMA_BITMUX_MODE_1to1,
    T_SCR_MODE_40G_4_LANE,
    R_DESCR_MODE_CL82,
    R_DEC_TL_MODE_CL82,
    R_DESKEW_MODE_40G_4_LANE,
    R_DEC_FSM_MODE_CL82,
    0,
    BS_CL82_SYNC_MODE,
    1,
    BS_DIST_MODE_2_LANE_TDM_1_VLANE,
    BS_BTMX_MODE_1to1,
    1,
    9, 5, 1,18,1,
    33, 0, 1, 0, 2 
  },
  /*`SPEED_40G_KR4:  41*/
  {
    NUM_LANES_04,
    OS_MODE_2,
    T_FIFO_MODE_40G,
    T_ENC_MODE_CL82,
    0,
    T_PMA_BITMUX_MODE_1to1,
    T_SCR_MODE_40G_4_LANE,
    R_DESCR_MODE_CL82,
    R_DEC_TL_MODE_CL82,
    R_DESKEW_MODE_40G_4_LANE,
    R_DEC_FSM_MODE_CL82,
    0,
    BS_CL82_SYNC_MODE,
    1,
    BS_DIST_MODE_2_LANE_TDM_1_VLANE,
    BS_BTMX_MODE_1to1,
    1,
    9, 5, 1,18,1,
    33, 0, 1, 0, 2 
  },
  /*`SPEED_40G_X4: 42*/
  {
    NUM_LANES_04,
    OS_MODE_2,
    T_FIFO_MODE_40G,
    T_ENC_MODE_CL82,
    0,
    T_PMA_BITMUX_MODE_1to1,
    T_SCR_MODE_40G_4_LANE,
    R_DESCR_MODE_CL82,
    R_DEC_TL_MODE_CL82,
    R_DESKEW_MODE_40G_4_LANE,
    R_DEC_FSM_MODE_CL82,
    0,
    BS_CL82_SYNC_MODE,
    1,
    BS_DIST_MODE_2_LANE_TDM_1_VLANE,
    BS_BTMX_MODE_1to1,
    1,
    9, 5, 1,18,1,
    33, 0, 1, 0, 2 
  },
  /*`NULL ENTRY: 43*/ {0},
  /*`SPEED_40G_HG2_CR4: 44*/
  {
    NUM_LANES_04,
    OS_MODE_2,
    T_FIFO_MODE_40G,
    T_ENC_MODE_CL82,
    1,
    T_PMA_BITMUX_MODE_1to1,
    T_SCR_MODE_40G_4_LANE,
    R_DESCR_MODE_CL82,
    R_DEC_TL_MODE_CL82,
    R_DESKEW_MODE_40G_4_LANE,
    R_DEC_FSM_MODE_CL82,
    1,
    BS_CL82_SYNC_MODE,
    1,
    BS_DIST_MODE_2_LANE_TDM_1_VLANE,
    BS_BTMX_MODE_1to1,
    1,
    9, 5, 1,18,1,
    33, 0, 1, 0, 2 
  },
  /*`SPEED_40G_HG2_KR4: 45*/
  {
    NUM_LANES_04,
    OS_MODE_2,
    T_FIFO_MODE_40G,
    T_ENC_MODE_CL82,
    1,
    T_PMA_BITMUX_MODE_1to1,
    T_SCR_MODE_40G_4_LANE,
    R_DESCR_MODE_CL82,
    R_DEC_TL_MODE_CL82,
    R_DESKEW_MODE_40G_4_LANE,
    R_DEC_FSM_MODE_CL82,
    1,
    BS_CL82_SYNC_MODE,
    1,
    BS_DIST_MODE_2_LANE_TDM_1_VLANE,
    BS_BTMX_MODE_1to1,
    1,
    9, 5, 1,18,1,
    33, 0, 1, 0, 2 
  },
  /*`SPEED_40G_HG2_X4: 46*/
  {
    NUM_LANES_04,
    OS_MODE_2, /* NICK Jira 898 */       
    T_FIFO_MODE_40G,
    T_ENC_MODE_CL82,
    1,
    T_PMA_BITMUX_MODE_1to1,
    T_SCR_MODE_40G_4_LANE,
    R_DESCR_MODE_CL82,
    R_DEC_TL_MODE_CL82,
    R_DESKEW_MODE_40G_4_LANE,
    R_DEC_FSM_MODE_CL82,
    1,
    BS_CL82_SYNC_MODE,
    1,
    BS_DIST_MODE_2_LANE_TDM_1_VLANE,
    BS_BTMX_MODE_1to1,
    1,
    9, 5, 1,18,1,
    33, 0, 1, 0, 2 
  },
  /*`NULL ENTRY: 47*/ {0},
  /*`SPEED_50G_CR2: 48*/
  {
    NUM_LANES_02,
    OS_MODE_1,
    T_FIFO_MODE_40G,
    T_ENC_MODE_CL82,
    0,
    T_PMA_BITMUX_MODE_2to1,
    T_SCR_MODE_40G_2_LANE,
    R_DESCR_MODE_CL82,
    R_DEC_TL_MODE_CL82,
    R_DESKEW_MODE_40G_2_LANE,
    R_DEC_FSM_MODE_CL82,
    0,
    BS_CL82_SYNC_MODE,
    1,
    BS_DIST_MODE_2_LANE_TDM_2_VLANE,
    BS_BTMX_MODE_2to1,
    1,
    9, 5, 1,18,2,
    33, 0, 1, 0, 2 
  },
  /*`SPEED_50G_KR2: 49*/
  {
    NUM_LANES_02,
    OS_MODE_1,
    T_FIFO_MODE_40G,
    T_ENC_MODE_CL82,
    0,
    T_PMA_BITMUX_MODE_2to1,
    T_SCR_MODE_40G_2_LANE,
    R_DESCR_MODE_CL82,
    R_DEC_TL_MODE_CL82,
    R_DESKEW_MODE_40G_2_LANE,
    R_DEC_FSM_MODE_CL82,
    0,
    BS_CL82_SYNC_MODE,
    1,
    BS_DIST_MODE_2_LANE_TDM_2_VLANE,
    BS_BTMX_MODE_2to1,
    1,
    9, 5, 1,18,2,
    33, 0, 1, 0, 2 
  },
  /*`SPEED_50G_X2: 50*/
  {
    NUM_LANES_02,
    OS_MODE_1,
    T_FIFO_MODE_40G,
    T_ENC_MODE_CL82,
    0,
    T_PMA_BITMUX_MODE_2to1,
    T_SCR_MODE_40G_2_LANE,
    R_DESCR_MODE_CL82,
    R_DEC_TL_MODE_CL82,
    R_DESKEW_MODE_40G_2_LANE,
    R_DEC_FSM_MODE_CL82,
    0,
    BS_CL82_SYNC_MODE,
    1,
    BS_DIST_MODE_2_LANE_TDM_2_VLANE,
    BS_BTMX_MODE_2to1,
    1,
    9, 5, 1,18,2,
    33, 0, 1, 0, 2 
  },
  /*`NULL ENTRY: 51*/ {0},
  /*`SPEED_50G_HG2_CR2: 52*/
  {
    NUM_LANES_02,
    OS_MODE_1,
    T_FIFO_MODE_40G,
    T_ENC_MODE_CL82,
    1,
    T_PMA_BITMUX_MODE_2to1,
    T_SCR_MODE_40G_2_LANE,
    R_DESCR_MODE_CL82,
    R_DEC_TL_MODE_CL82,
    R_DESKEW_MODE_40G_2_LANE,
    R_DEC_FSM_MODE_CL82,
    1,
    BS_CL82_SYNC_MODE,
    1,
    BS_DIST_MODE_2_LANE_TDM_2_VLANE,
    BS_BTMX_MODE_2to1,
    1,
    9, 5, 1,18,2,
    33, 0, 1, 0, 2 
  },
  /*`SPEED_50G_HG2_KR2: 53*/
  {
    NUM_LANES_02,
    OS_MODE_1,
    T_FIFO_MODE_40G,
    T_ENC_MODE_CL82,
    1,
    T_PMA_BITMUX_MODE_2to1,
    T_SCR_MODE_40G_2_LANE,
    R_DESCR_MODE_CL82,
    R_DEC_TL_MODE_CL82,
    R_DESKEW_MODE_40G_2_LANE,
    R_DEC_FSM_MODE_CL82,
    1,
    BS_CL82_SYNC_MODE,
    1,
    BS_DIST_MODE_2_LANE_TDM_2_VLANE,
    BS_BTMX_MODE_2to1,
    1,
    9, 5, 1,18,2,
    33, 0, 1, 0, 2 
  },
  /*`SPEED_50G_HG2_X2: 54*/
  {
    NUM_LANES_02,
    OS_MODE_1,
    T_FIFO_MODE_40G,
    T_ENC_MODE_CL82,
    1,
    T_PMA_BITMUX_MODE_2to1,
    T_SCR_MODE_40G_2_LANE,
    R_DESCR_MODE_CL82,
    R_DEC_TL_MODE_CL82,
    R_DESKEW_MODE_40G_2_LANE,
    R_DEC_FSM_MODE_CL82,
    1,
    BS_CL82_SYNC_MODE,
    1,
    BS_DIST_MODE_2_LANE_TDM_2_VLANE,
    BS_BTMX_MODE_2to1,
    1,
    9, 5, 1,18,2,
    33, 0, 1, 0, 2 
  },
  /*`NULL ENTRY: 55*/ {0},
  /*`SPEED_50G_CR4: 56*/
  {
    NUM_LANES_04,
    OS_MODE_2,
    T_FIFO_MODE_40G,
    T_ENC_MODE_CL82,
    0,
    T_PMA_BITMUX_MODE_1to1,
    T_SCR_MODE_40G_4_LANE,
    R_DESCR_MODE_CL82,
    R_DEC_TL_MODE_CL82,
    R_DESKEW_MODE_40G_4_LANE,
    R_DEC_FSM_MODE_CL82,
    0,
    BS_CL82_SYNC_MODE,
    1,
    BS_DIST_MODE_2_LANE_TDM_1_VLANE,
    BS_BTMX_MODE_1to1,
    1,
    9, 5, 1,18,1,
    33, 0, 1, 0, 2 
  },
  /*`SPEED_50G_KR4:  57*/
  {
    NUM_LANES_04,
    OS_MODE_2,
    T_FIFO_MODE_40G,
    T_ENC_MODE_CL82,
    0,
    T_PMA_BITMUX_MODE_1to1,
    T_SCR_MODE_40G_4_LANE,
    R_DESCR_MODE_CL82,
    R_DEC_TL_MODE_CL82,
    R_DESKEW_MODE_40G_4_LANE,
    R_DEC_FSM_MODE_CL82,
    0,
    BS_CL82_SYNC_MODE,
    1,
    BS_DIST_MODE_2_LANE_TDM_1_VLANE,
    BS_BTMX_MODE_1to1,
    1,
    9, 5, 1,18,1,
    33, 0, 1, 0, 2 
  },
  /*`SPEED_50G_X4: 58*/
  {
    NUM_LANES_04,
    OS_MODE_2,
    T_FIFO_MODE_40G,
    T_ENC_MODE_CL82,
    0,
    T_PMA_BITMUX_MODE_1to1,
    T_SCR_MODE_40G_4_LANE,
    R_DESCR_MODE_CL82,
    R_DEC_TL_MODE_CL82,
    R_DESKEW_MODE_40G_4_LANE,
    R_DEC_FSM_MODE_CL82,
    0,
    BS_CL82_SYNC_MODE,
    1,
    BS_DIST_MODE_2_LANE_TDM_1_VLANE,
    BS_BTMX_MODE_1to1,
    1,
    9, 5, 1,18,1,
    33, 0, 1, 0, 2 
  },
  /*`NULL ENTRY: 59*/ {0},
  /*`SPEED_50G_HG2_CR4: 60*/
  {
    NUM_LANES_04,
    OS_MODE_2,
    T_FIFO_MODE_40G,
    T_ENC_MODE_CL82,
    1,
    T_PMA_BITMUX_MODE_1to1,
    T_SCR_MODE_40G_4_LANE,
    R_DESCR_MODE_CL82,
    R_DEC_TL_MODE_CL82,
    R_DESKEW_MODE_40G_4_LANE,
    R_DEC_FSM_MODE_CL82,
    1,
    BS_CL82_SYNC_MODE,
    1,
    BS_DIST_MODE_2_LANE_TDM_1_VLANE,
    BS_BTMX_MODE_1to1,
    1,
    9, 5, 1,18,1,
    33, 0, 1, 0, 2 
  },
  /*`SPEED_50G_HG2_KR4: 61*/
  {
    NUM_LANES_04,
    OS_MODE_2,
    T_FIFO_MODE_40G,
    T_ENC_MODE_CL82,
    1,
    T_PMA_BITMUX_MODE_1to1,
    T_SCR_MODE_40G_4_LANE,
    R_DESCR_MODE_CL82,
    R_DEC_TL_MODE_CL82,
    R_DESKEW_MODE_40G_4_LANE,
    R_DEC_FSM_MODE_CL82,
    1,
    BS_CL82_SYNC_MODE,
    1,
    BS_DIST_MODE_2_LANE_TDM_1_VLANE,
    BS_BTMX_MODE_1to1,
    1,
    9, 5, 1,18,1,
    33, 0, 1, 0, 2 
  },
  /*`SPEED_50G_HG2_X4: 62*/
  {
    NUM_LANES_04,
    OS_MODE_2, /* NICK Jira 898 */
    T_FIFO_MODE_40G,
    T_ENC_MODE_CL82,
    1,
    T_PMA_BITMUX_MODE_1to1,
    T_SCR_MODE_40G_4_LANE,
    R_DESCR_MODE_CL82,
    R_DEC_TL_MODE_CL82,
    R_DESKEW_MODE_40G_4_LANE,
    R_DEC_FSM_MODE_CL82,
    1,
    BS_CL82_SYNC_MODE,
    1,
    BS_DIST_MODE_2_LANE_TDM_1_VLANE,
    BS_BTMX_MODE_1to1,
    1,
    9, 5, 1,18,1,
    33, 0, 1, 0, 2 
  },
  /*`NULL ENTRY: 63*/ {0},
  /*`SPEED_100G_CR4: 64*/
  {
    NUM_LANES_04,
    OS_MODE_1,
    T_FIFO_MODE_100G,
    T_ENC_MODE_CL82,
    0,
    T_PMA_BITMUX_MODE_5to1,
    T_SCR_MODE_100G,
    R_DESCR_MODE_CL82,
    R_DEC_TL_MODE_CL82,
    R_DESKEW_MODE_100G,
    R_DEC_FSM_MODE_CL82,
    0,
    BS_CL82_SYNC_MODE,
    1,
    BS_DIST_MODE_NO_TDM,
    BS_BTMX_MODE_5to1,
    1,
    9, 5, 1,18,1,
    33, 0, 1, 0, 1 
  },
  /*`SPEED_100G_KR4: 65*/
  {
    NUM_LANES_04,
    OS_MODE_1,
    T_FIFO_MODE_100G,
    T_ENC_MODE_CL82,
    0,
    T_PMA_BITMUX_MODE_5to1,
    T_SCR_MODE_100G,
    R_DESCR_MODE_CL82,
    R_DEC_TL_MODE_CL82,
    R_DESKEW_MODE_100G,
    R_DEC_FSM_MODE_CL82,
    0,
    BS_CL82_SYNC_MODE,
    1,
    BS_DIST_MODE_NO_TDM,
    BS_BTMX_MODE_5to1,
    1,
    9, 5, 1,18,1,
    33, 0, 1, 0, 1 
  },
  /*`SPEED_100G_X4: 66*/
  {
    NUM_LANES_04,
    OS_MODE_1,
    T_FIFO_MODE_100G,
    T_ENC_MODE_CL82,
    0,
    T_PMA_BITMUX_MODE_5to1,
    T_SCR_MODE_100G,
    R_DESCR_MODE_CL82,
    R_DEC_TL_MODE_CL82,
    R_DESKEW_MODE_100G,
    R_DEC_FSM_MODE_CL82,
    0,
    BS_CL82_SYNC_MODE,
    1,
    BS_DIST_MODE_NO_TDM,
    BS_BTMX_MODE_5to1,
    1,
    9, 5, 1,18,1,
    33, 0, 1, 0, 1 
  },
  /*`NULL ENTRY: 67*/ {0},
  /*`SPEED_100G_HG2_CR4: 68*/
  {
    NUM_LANES_04,
    OS_MODE_1,
    T_FIFO_MODE_100G,
    T_ENC_MODE_CL82,
    1,
    T_PMA_BITMUX_MODE_5to1,
    T_SCR_MODE_100G,
    R_DESCR_MODE_CL82,
    R_DEC_TL_MODE_CL82,
    R_DESKEW_MODE_100G,
    R_DEC_FSM_MODE_CL82,
    1,
    BS_CL82_SYNC_MODE,
    1,
    BS_DIST_MODE_NO_TDM,
    BS_BTMX_MODE_5to1,
    1,
    9, 5, 1,18,1,
    33, 0, 1, 0, 1 
  },
  /*`SPEED_100G_HG2_KR4: 69*/
  {
    NUM_LANES_04,
    OS_MODE_1,
    T_FIFO_MODE_100G,
    T_ENC_MODE_CL82,
    1,
    T_PMA_BITMUX_MODE_5to1,
    T_SCR_MODE_100G,
    R_DESCR_MODE_CL82,
    R_DEC_TL_MODE_CL82,
    R_DESKEW_MODE_100G,
    R_DEC_FSM_MODE_CL82,
    1,
    BS_CL82_SYNC_MODE,
    1,
    BS_DIST_MODE_NO_TDM,
    BS_BTMX_MODE_5to1,
    1,
    9, 5, 1,18,1,
    33, 0, 1, 0, 1 
  },
  /*`SPEED_100G_HG2_X4: 70*/
  {
    NUM_LANES_04,
    OS_MODE_1,
    T_FIFO_MODE_100G,
    T_ENC_MODE_CL82,
    1,
    T_PMA_BITMUX_MODE_5to1,
    T_SCR_MODE_100G,
    R_DESCR_MODE_CL82,
    R_DEC_TL_MODE_CL82,
    R_DESKEW_MODE_100G,
    R_DEC_FSM_MODE_CL82,
    1,
    BS_CL82_SYNC_MODE,
    1,
    BS_DIST_MODE_NO_TDM,
    BS_BTMX_MODE_5to1,
    1,
    9, 5, 1,18,1,
    33, 0, 1, 0, 1 
  },
  /*`NULL ENTRY: 71*/ {0},
  /*`SPEED_CL73_20GVCO: 72*/ 
  {
    NUM_LANES_01,
    OS_MODE_16p5,
    T_FIFO_MODE_BYPASS,
    0,
    0,
    T_PMA_BITMUX_MODE_1to1,
    0,
    0,
    0,
    0,
    0,
    0,
    BS_CL49_SYNC_MODE,
    0,
    BS_DIST_MODE_5_LANE_TDM,
    BS_BTMX_MODE_1to1,
    0,
    9, 5, 1,18, 4,
    33, 0, 1, 0, 8 
  },
  /*`NULL ENTRY: 73*/ {0},
  /*`NULL ENTRY: 74*/ {0},
  /*`NULL ENTRY: 75*/ {0},
  /*`NULL ENTRY: 76*/ {0},
  /*`NULL ENTRY: 77*/ {0},
  /*`NULL ENTRY: 78*/ {0},
  /*`NULL ENTRY: 79*/ {0},
  /*`SPEED_CL73_25GVCO: 80*/ 
  {
    NUM_LANES_01,
    OS_MODE_20p625,
    T_FIFO_MODE_BYPASS,
    0,
    0,
    T_PMA_BITMUX_MODE_1to1,
    0,
    0,
    0,
    0,
    0,
    0,
    BS_CL49_SYNC_MODE,
    0,
    BS_DIST_MODE_5_LANE_TDM,
    BS_BTMX_MODE_1to1,
    0,
    9, 5, 1,18 , 4,
    10, 11, 11, 5, 6
  },
  /*`NULL ENTRY: 81*/ {0},
  /*`NULL ENTRY: 82*/ {0},
  /*`NULL ENTRY: 83*/ {0},
  /*`NULL ENTRY: 84*/ {0},
  /*`NULL ENTRY: 84*/ {0},
  /*`NULL ENTRY: 86*/ {0},
  /*`NULL ENTRY: 87*/ {0},
  /*`SPEED_1G_20GVCO: 88*/ 
  {
    NUM_LANES_01,
    OS_MODE_16p5,
    T_FIFO_MODE_BYPASS,
    0,
    0,
    T_PMA_BITMUX_MODE_1to1,
    0,
    0,
    0,
    0,
    0,
    0,
    BS_CL49_SYNC_MODE,
    0,
    BS_DIST_MODE_5_LANE_TDM,
    BS_BTMX_MODE_1to1,
    0,
    99, 0, 1,0 , 50,
    10, 11, 11, 5, 6
  },
  /*`NULL ENTRY: 89*/ {0},
  /*`NULL ENTRY: 90*/ {0},
  /*`NULL ENTRY: 91*/ {0},
  /*`NULL ENTRY: 92*/ {0},
  /*`NULL ENTRY: 93*/ {0},
  /*`NULL ENTRY: 94*/ {0},
  /*`NULL ENTRY: 95*/ {0},
  /*`SPEED_1G_25GVCO: 96*/ 
  {
    NUM_LANES_01,
    OS_MODE_20p625,
    T_FIFO_MODE_BYPASS,
    0,
    0,
    T_PMA_BITMUX_MODE_1to1,
    0,
    0,
    0,
    0,
    0,
    0,
    BS_CL49_SYNC_MODE,
    0,
    BS_DIST_MODE_5_LANE_TDM,
    BS_BTMX_MODE_1to1,
    0,
    124, 123, 3, 1, 63,
    104, 103, 1, 7, 53
  },
  /*`NULL ENTRY: 97*/ {0},
  /*`NULL ENTRY: 98*/ {0},
  /*`NULL ENTRY: 99*/ {0},
  /*`NULL ENTRY: 100*/ {0},
  /*`NULL ENTRY: 101*/ {0},
  /*`NULL ENTRY: 102*/ {0},
  /*`NULL ENTRY: 103*/ {0},
  /*`NULL ENTRY: 104*/ {0},
  /*`NULL ENTRY: 105*/ {0},
  /*`NULL ENTRY: 106*/ {0},
  /*`NULL ENTRY: 107*/ {0},
  /*`NULL ENTRY: 108*/ {0},
  /*`NULL ENTRY: 109*/ {0},
  /*`NULL ENTRY: 110*/ {0},
  /*`NULL ENTRY: 111*/ {0},
  /*`SPEED_25G_CR1_IEEE: 'h70 ; 'd112*/
  {
    NUM_LANES_01,
    OS_MODE_1,
    T_FIFO_MODE_BYPASS,
    T_ENC_MODE_CL49,
    0,
    T_PMA_BITMUX_MODE_1to1,
    T_SCR_MODE_CL49,
    R_DESCR_MODE_CL49,
    R_DEC_TL_MODE_CL49,
    R_DESKEW_MODE_CL49,
    R_DEC_FSM_MODE_CL49,
    0,
    BS_CL49_SYNC_MODE,
    1,
    BS_DIST_MODE_5_LANE_TDM,
    BS_BTMX_MODE_1to1,
    1,
    9, 5, 1,18 , 4,
    33, 0, 1, 0, 4
  },
  /*`SPEED_25G_CRS: 'h71 ; 'd113*/
  {
    NUM_LANES_01,
    OS_MODE_1,
    T_FIFO_MODE_BYPASS,
    T_ENC_MODE_CL49,
    0,
    T_PMA_BITMUX_MODE_1to1,
    T_SCR_MODE_CL49,
    R_DESCR_MODE_CL49,
    R_DEC_TL_MODE_CL49,
    R_DESKEW_MODE_CL49,
    R_DEC_FSM_MODE_CL49,
    0,
    BS_CL49_SYNC_MODE,
    1,
    BS_DIST_MODE_5_LANE_TDM,
    BS_BTMX_MODE_1to1,
    1,
    9, 5, 1,18 , 4,
    33, 0, 1, 0, 4
  },
  /*`SPEED_25G_KR1_IEEE: 'h72 ; 'd114*/
  {
    NUM_LANES_01,
    OS_MODE_1,
    T_FIFO_MODE_BYPASS,
    T_ENC_MODE_CL49,
    0,
    T_PMA_BITMUX_MODE_1to1,
    T_SCR_MODE_CL49,
    R_DESCR_MODE_CL49,
    R_DEC_TL_MODE_CL49,
    R_DESKEW_MODE_CL49,
    R_DEC_FSM_MODE_CL49,
    0,
    BS_CL49_SYNC_MODE,
    1,
    BS_DIST_MODE_5_LANE_TDM,
    BS_BTMX_MODE_1to1,
    1,
    9, 5, 1,18 , 4,
    33, 0, 1, 0, 4
  },
  /*`SPEED_25G_KRS: 'h73 ; 'd115*/
  {
    NUM_LANES_01,
    OS_MODE_1,
    T_FIFO_MODE_BYPASS,
    T_ENC_MODE_CL49,
    0,
    T_PMA_BITMUX_MODE_1to1,
    T_SCR_MODE_CL49,
    R_DESCR_MODE_CL49,
    R_DEC_TL_MODE_CL49,
    R_DESKEW_MODE_CL49,
    R_DEC_FSM_MODE_CL49,
    0,
    BS_CL49_SYNC_MODE,
    1,
    BS_DIST_MODE_5_LANE_TDM,
    BS_BTMX_MODE_1to1,
    1,
    9, 5, 1,18 , 4,
    33, 0, 1, 0, 4
  }

};
#endif

/**
\struct sc_pmd16_entry_t 

This embodies all parameters passed from PCS to PMD. For more details
please refer to the micro-arch document.
*/
typedef struct sc_pmd16_entry_t
{
  int t_pma_os_mode;
  int pll_mode;
} sc_pmd16_entry_st;

extern const sc_pmd16_entry_st sc_pmd16_entry[], sc_pmd16_entry_125M_ref[];

extern int cl91_valid_speed(PHYMOD_ST* pc); 

#endif
