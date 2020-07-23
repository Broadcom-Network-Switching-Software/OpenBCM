/*----------------------------------------------------------------------
 * $Id: tbhmod_gen2.h,
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

#ifndef _tbhmod_gen2_H_
#define _tbhmod_gen2_H_

#include <phymod/phymod.h>
#include <phymod/phymod_debug.h>
#include <tscbh/tier1/tbhmod_enum_defines.h>

#define TSCBH_GEN2_NRZ_MAX_PER_LANE_SPEED 28125
#define TSCBH_GEN2_AUTONEG_SPEED_ID_COUNT 44
#define TSCBH_GEN2_PLL_NUM_PER_CORE       2

/* UI macros definition. */
#define TBHMOD_GEN2_UI_20G_NRZ_OSX1 0x0C698000
#define TBHMOD_GEN2_UI_20G_NRZ_OSX2 0x18D30000
#define TBHMOD_GEN2_UI_25G_PAM4 0x04F70000
#define TBHMOD_GEN2_UI_25G_NRZ_OSX1 0x09EE0000
#define TBHMOD_GEN2_UI_25G_NRZ_OSX2P5 0x18D30000
#define TBHMOD_GEN2_UI_26G_PAM4 0x04D19E00
#define TBHMOD_GEN2_UI_26G_NRZ 0x09A33C00

/* FCLK period macros definition. */
#define TBHMOD_GEN2_FCLK_PERIOD_20G_DIV8 0x634c
#define TBHMOD_GEN2_FCLK_PERIOD_20G_DIV6 0x4a79
#define TBHMOD_GEN2_FCLK_PERIOD_25G 0x4f70
#define TBHMOD_GEN2_FCLK_PERIOD_26G 0x4d19

/* PMD datapath latency. From Blackhawk7 user spec datapath latency table. */
#define TBHMOD_GEN2_PMD_TX_DP_LATENCY_NRZ 167
#define TBHMOD_GEN2_PMD_TX_DP_LATENCY_PAM4 147
#define TBHMOD_GEN2_PMD_RX_DP_LATENCY_NRZ 125
#define TBHMOD_GEN2_PMD_RX_DP_LATENCY_PAM4 85

extern int tbhmod_gen2_pcs_ts_en(PHYMOD_ST* pc, int en, int sfd, int rpm);
extern int tbhmod_gen2_pcs_rx_ts_en(PHYMOD_ST* pc, uint32_t en);
extern int tbhmod_gen2_set_pmd_timer_offset(PHYMOD_ST* pc, int ts_clk_period);
extern int tbhmod_gen2_pcs_set_1588_ui (PHYMOD_ST* pc, uint32_t vco, uint32_t vco1, int os_mode, int clk4sync_div, int pam4);
extern int tbhmod_gen2_1588_pmd_latency(PHYMOD_ST* pc, int an_en, uint32_t vco, int os_mode, int pam4);
extern int tbhmod_gen2_update_tx_pmd_latency (PHYMOD_ST* pc, uint32_t latency_adj, int normalize_to_latest);
extern int tbhmod_gen2_pcs_set_tx_lane_skew_capture (PHYMOD_ST* pc, int tx_skew_en);
extern int tbhmod_gen2_pcs_measure_tx_lane_skew (PHYMOD_ST* pc, int an_en, uint32_t vco, int os_mode, int pam4, int normalize_to_latest, int *tx_max_skew);
extern int tbhmod_gen2_pcs_mod_rx_1588_tbl_val(PHYMOD_ST* pc, int an_en, int bit_mux_mode, uint32_t vco, int os_mode, int pam4, int tbl_ln, int normalize_to_latest, int rx_skew_adjust_enb, int *table , int *rx_max_skew); 
extern int tbhmod_gen2_pcs_ts_deskew_valid(PHYMOD_ST* pc, int bit_mux_mode, int *rx_ts_deskew_valid);
extern int tbhmod_gen2_tx_ts_info_unpack_tx_ts_tbl_entry(uint32_t *tx_ts_tbl_entry, phymod_ts_fifo_status_t *tx_ts_info);
extern int tbhmod_gen2_pcs_reset_sw_war(const PHYMOD_ST* pc);
extern int tbhmod_gen2_pcs_clk_blk_en(const PHYMOD_ST* pc, uint32_t en);
extern int tbhmod_gen2_fec_three_cw_bad_state_get(PHYMOD_ST* pc, uint32_t *state);
#endif  /*  _tbhmod_gen2_H_ */
