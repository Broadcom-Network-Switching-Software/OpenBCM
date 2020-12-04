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
 *  Description: define enumerators and tscmod_st
 *----------------------------------------------------------------------*/

#ifndef _TSCMOD_DEFINES_H_
#define _TSCMOD_DEFINES_H_

#define  TSCMOD_AN_ENABLE_MASK   0x8000
#define  TSCMOD_AN_DONE_MASK     0x1

#include "tscmod_enum_defines.h"

/*! The number of serdes blocks (not lanes) in the system (may be multiple
 * chips) -- this should be changed by the user as it determines the number of
 * mutexes */ 
#define NUMBER_OF_LOCKS (2*3*18)

/*! \struct tscmod_int_st

This is an internal structure containing things useful only to TSCMod. It
is not exposed to customer code.

*/

typedef struct {
  /*! this bit is set if we are in asymmetric mode. (for Xenia only) */
  int asymmetric_mode;
} tscmod_int_st;

/*! \struct tscmod_st

This is the central TSC context storing information of a single TSC.
It has multiple sections, each with a set of elementtsc

\li Address Context: Elements to uniquely access a TSC 

*/

typedef struct {
  /*! chip id. This identifies which mutex to use. The exact
   * mapping of "id" to chip/tsc number/etc. will differ for verification, the
   * SW stack, or the testboard development environment */
  int id;
  /* ===== BEGIN ===== TSC port address section */
  /*! chip number of the chip where the TSC resides, defined by MDK/SDK
   * platforms. It is not user in verilog and customer software platforms. */
  int                 unit; 
  /*! Port no. defined in MDK/SDK platforms. It is not used in verilog and 
   * customer software platforms. */
  int                 port;
  /*! physical addr. of the TSC as defined by MDK/SDK platforms. This
   * is not used in verilog and customer software platforms. */
  int                 phy_ad; 
  /*! lanes to access. Can simultaneously select multiple lanes. */
  tscmod_lane_select   lane_select; 
  /*! Port address strap value. Used only in TSC verif. env. to identify
   * the TSC to the station manager */
  int                 prt_ad; 
  /*! Current lane being processed can be 0, 1, 2 or 3. */
  int this_lane;
  /*! May not need this. Legacy. Ignore lane number implications*/
  int lane_num_ignore;
  /*! Tier1 Features are controlled by adding values to this var. Currently we
   * manage with 32 bits (int type). If more bits are needed to specify a
   * feature,more feature control variables will be added or this will be
   * turned into an array. In general we try to use at most 8 bits per lane. */
  int                 per_lane_control; 
  /*! see doc. on tscmod_spd_intfc_type */
  tscmod_spd_intfc_type spd_intf; 
  /*! Serial Bus protocol or PCB */
  tscmod_regacc_type     regacc_type; 
  /*! combo or independent */
  tscmod_port_type     port_type; 
  /*! Oversampling rate. This is fixed in combo, but not in indep. */
  tscmod_os_type       os_type; 
  /*! Single copy registers cannot be modified repeatedly. Tier1 init function
   * access these registers has to do this only once between resets in
   * independent mode when the same Tier1 is called for multiple lanes. This
   * sticky bit, remembers if the single copy regs. were already modified. */
  int                 tsc_touched; 
  /*! Lane swap contl variable. The positional values of the 8 nibbles indicate
   * location of the logical lanes, like so. rx3_rx2_rx1_rx0_tx3_tx2_tx1_tx0.
   * Default value 0x32103210 indicates no lanes swapped. */
  int                 lane_swap; 
  /*! Not set by user, but set by TSCMod based on speed and interface. */
  tscmod_duplex_type   duplex; 
  /*! Address of reg. to access */
  int                 accAddr; 
  /*! Data context. Contains data to write or data that is just read. Also
   * contains revid after read_revid. */
  int                 accData; 
  /*! Various status of the link. Note check status of one link at a time. */
  int     diag_type;
  /*! Flavors of TSC */
  tscmod_model_type    model_type;
  /*! Autoneg types. */
  tscmod_an_type       an_type;
  int                  an_hg_vco ;
  /*! Autoneg parameter RF */
  int                 an_rf;
  /*! Autoneg parameter pause */
  int                 an_pause;
  /*! Autoneg parameter various speeds (bit encoded) */
  int                 an_tech_ability;
  int                 an_bam37_ability;
  /*! Autoneg parameter enable FEC */
  int                 an_fec;
  int                 an_higig2;
  int                 an_cl72;
  /*! Control information the driver prints. If set to zero, only warnings and
   * errors will be printed. If set to 1, msgs (developers discretion will be
   * printed) but not register transactions. If set to 2 transactions will be
   * printed (plus everything else).
   */
  int verbosity;
   /* last 4 bits are for TSC state information */
  int ctrl_type;
  int err_code ;
  int msg_code ;
  /*! for when user wants to provide custom refclk. Will also work for standard
   * values. The 10ths value of the frequency need not be entered. So 156.1
   * becomes 156
   */
  int refclk;
  /*! pll div value is auto computed. But override is provided. */
  int plldiv;
  /*! dxgxs is to indicate that the mode should make dual port bcst 
   *  dxgxs=1 for lane0-1 and dxgxs=2 for lane2-3 
   */
  int dxgxs ;

#ifdef _SDK_TSCMOD_
  void *arg ;  /* check arch byte length */
#endif
  int live_link;
  int ber;
  int tol;
  int eye_lane ;
  int time_upper_bound ;

  int vga_frzval;
  int vga_frcfrz;
  int dfe_frzval;
  int dfe_frcfrz;
  int linkup_bert_cnt[4] ; 
  int firmware_mode ; 
  int an_ctl  ;
  int iintf ; 
  int reg_sync ;
  int blk_adr ;
  int rxlos_restart_time;
  int rxlos_linkup_time;
} tscmod_st;

#endif  /*  _TSCMOD_DEFINES_H_ */
