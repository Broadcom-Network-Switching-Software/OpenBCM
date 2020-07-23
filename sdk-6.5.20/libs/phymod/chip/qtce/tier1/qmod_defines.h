/*----------------------------------------------------------------------
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 *  Broadcom Corporation
 *  Proprietary and Confidential information
 *  All rights reserved
 *  This source file is the property of Broadcom Corporation, and
 *  may not be copied or distributed in any isomorphic form without the
 *  prior written consent of Broadcom Corporation.
 *----------------------------------------------------------------------
 *  Description: define enumerators and qmod_st
 *----------------------------------------------------------------------*/

#ifndef _qmod_DEFINES_H_
#define _qmod_DEFINES_H_

#include "qmod_enum_defines.h"

/*! \struct qmod_st

This is the central TSCE context storing information of a single TE.
It has multiple sections, each with a set of elementtsc

\li Address Context: Elements to uniquely access a TSCE 

*/

typedef struct {
#if defined (_DV_TB_)
  /* ===== BEGIN ===== TSCE port address section */
  /*! chip number of the chip where the TSCE resides, defined by MDK/SDK
   * platforms. It is not user in verilog and customer software platforms. */
  int unit; 
  /*! Port no. defined in MDK/SDK platforms. It is not used in verilog and 
   * customer software platforms. */
  int port;
   /*! Port address strap value. Used only in TSCE verif. env. to identify
    * the TSCE to the station manager */
  int prt_ad;
#endif
  /*! lanes to access. Can simultaneously select multiple lanes. */
  qmod_lane_select   lane_select; 
  /*! Current lane being processed can be 0, 1, 2 or 3. */
  int this_lane;
  int sub_port;
  /*! May not need this. Legacy. Ignore lane number implications*/
  int adjust_port_mode;
  /*! Tier1 Features are controlled by adding values to this var. Currently we
   * manage with 32 bits (int type). If more bits are needed to specify a
   * feature,more feature control variables will be added or this will be
   * turned into an array. In general we try to use at most 8 bits per lane. */
  int                 per_lane_control; 

  int  pcs_gloop;
  int  pmd_gloop;
  int  pcs_rloop;

  int  pmd_reset_control;

  int  pkt_number;
  int  pkt_size;
  int  ipg_size;

  /*! see doc. on qmod_spd_intfc_type */
  qmod_spd_intfc_type spd_intf; 
  /*! Serial Bus protocol or PCB */
  qmod_regacc_type     regacc_type; 
  /*! combo or independent */
  qmod_port_type     port_type; 
  /*! Single copy registers cannot be modified repeatedly. Tier1 init function
   * access these registers has to do this only once between resets in
   * independent mode when the same Tier1 is called for multiple lanes. This
   * sticky bit, remembers if the single copy regs. were already modified. */
  int                 tsc_touched; 
  int                 tsc_port_touched; 
  int                 pcs_lane_swap_touched; 
  int                 pmd_touched; 
  int                 uc_touched; 

  int                 master_portnum; 
  /*! Lane swap contl variable. The positional values of the 8 nibbles indicate
   * location of the logical lanes, like so. rx3_rx2_rx1_rx0_tx3_tx2_tx1_tx0.
   * Default value 0x32103210 indicates no lanes swapped. */
  int                 lane_swap; 
  /*! Address of reg. to access */
  int                 accAddr; 
  /*! Data context. Contains data to write or data that is just read. Also
   * contains revid after read_revid. */
  int                 accData; 
  /*! Various status of the link. Note check status of one link at a time. */
  int     diag_type;
  /*! Autoneg types. */
  qmod_an_type       an_type;
  /*! Autoneg parameter RF */
  int                 an_rf;
  /*! Autoneg parameter pause */
  int                 an_pause;
  /*! Autoneg parameter various speeds (bit encoded) */
  int                 an_tech_ability;
  int                 an_bam37_ability;
  /*! Control information the driver prints. If set to zero, only warnings and
   * errors will be printed. If set to 1, msgs (developers discretion will be
   * printed) but not register transactions. If set to 2 transactions will be
   * printed (plus everything else).
   */
  int verbosity;
   /* last 4 bits are for TSCE state information */
  int ctrl_type;
  /*! for when user wants to provide custom refclk. Will also work for
  * standard values. The 10ths value of the frequency need not be entered. So 156.1
  * becomes 156
  */
  int refclk;
  /*! pll div value is auto computed. But override is provided. */
  int plldiv;

  int vga_frzval;
  int vga_frcfrz;
  int dfe_frzval;
  int dfe_frcfrz;
  int linkup_bert_cnt[4] ; 
  int firmware_mode ; 
  int an_ctl  ;
  int pattern_enable  ;
  int pattern_type  ;

  int prbs_inv;
  int prbs_enable;
  int prbs_check_mode;
  int real_check;

  /* SC Table Entry  */
  int sc_mode;                      /* choose the override modes to pick the
                                        speed fields */
  int qsgmii_mode;                  /* Qsgmii_mode                      */
  int speed;                        /* Speed                            */
  int subp_speed;                   /* Set subport speed, needed for qmode*/ 
  int pcs_lane_map_sel;
  int num_lanes;                    /* Number of lanes                  */

  int pll_mode;                     /* PLL mode                         */

  int t_pma_os_mode;                /* T_PMA over-sampling mode         */
  int t_encode_mode;                /* TX Encode mode                   */

  int r_dec1_mode;                  /* R_DEC1 Decode mode               */
  int r_deskew_mode;                /* R_DESKEW mode                    */
  int r_desc2_mode;                 /* R_DESC2 mode                     */
  int r_desc2_byte_deletion;        /* R_DESC2 byte-deletion mode       */
  int r_dec1_brcm64b66_descr;       /* R_DEC1 Descramble mode           */

  int sigdet_filter;                /* Signal detect mode               */
  int blk_sync_mode;                /* Block Sync Mode                  */
  int r_reorder_mode;               /* R_REORDER mode                   */
  int cl36_en;                      /* CL36_EN mode                   */

  int sgmii;
  int qsgmii_en;
  int crd_en;
  int clkcnt0;
  int clkcnt1;
  int lpcnt0;
  int lpcnt1;
  int mac_cgc;
  int pcs_repcnt;
  int pcs_crdten;
  int pcs_clkcnt;
  int pcs_cgc;
  int uc_active;
  /*CL37 capabilities*/
  int cl37_an_pause; 
  int cl37_an_hd; 
  int cl37_an_fd; 
  int cl37_an_np; 
  int an_tick_override; 
  int an_tick_denominator; 
  int an_tick_numerator; 
  int an_fail_cnt; 
  int an_oui_ctrl; 
  int cl37_sgmii_speed; 
  int cl37_sgmii_duplex; 
  int cl37_sgmii_master_mode; 
  int cl37_bam_code; 
  int cl37_bam_ovr1g_en; 
  int cl37_bam_ovr1g_pgcnt; 
  int cl37_bam_speed; 
  int cl37_bam_to_sgmii_auto_enable; 
  int sgmii_to_cl37_auto_enable; 
  int cl37_bam_en; 
  int cl37_sgmii_en; 
  int cl37_an_en; 
  int cl37_an_restart; 
  int cl37_sw_restart_reset_disable; 
  int cl37_an_disable_reset_disable; 
  int transmit_nonce;
  int base_selector;
  int oui_upper_data;
  int oui_lower_data;
  int cl37_restart_timer_period;
  int cl37_ack_timer_period;
  int cl37_error_timer_period;
  int cl37_sgmii_timer_period;
  int link_fail_inhibit_timer_not_cl72_period;
  int pd_sd_timer_period;
  int an_ignore_link_timer;
  int bam_spd_pri_5_0;
  int bam_spd_pri_11_6;
  int bam_spd_pri_17_12;
  int bam_spd_pri_23_18;
  int bam_spd_pri_29_24;
  int bam_spd_pri_35_30;
  int no_of_lanes;
  int pd_kx_en;
  int pd_kx4_en;
  int an_pd_to_cl37_enable;
  int cl37_high_vco;
  int single_port_mode;
  int pd_to_cl37_retry_cnt;
  int pd_to_cl37_wait_timer;
  int disable_rf_report;
  /* NICK */
  int linkfailtimer_dis;
  int linkfailtimerqua_en;
  int an_good_check_trap;
  int an_good_trap;
  int tx_reset_count;
  int cl37_sync_status_filter;
} qmod_st;

/* function table */
typedef int (*tier1FuncTable)(qmod_st* c);

#endif  /*  _qmod_DEFINES_H_ */
