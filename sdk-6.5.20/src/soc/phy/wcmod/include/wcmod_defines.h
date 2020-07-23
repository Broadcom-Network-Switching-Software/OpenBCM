/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <sal/types.h>
/*----------------------------------------------------------------------
 *  Broadcom Corporation
 *  Proprietary and Confidential information
 *  All rights reserved
 *  This source file is the property of Broadcom Corporation, and
 *  may not be copied or distributed in any isomorphic form without the
 *  prior written consent of Broadcom Corporation.
 *----------------------------------------------------------------------
 *  Description: define enumerators
 *----------------------------------------------------------------------
 *  CVS INFORMATION:
 * 
 *      $Revision: 1.17 $
 *      $Source: /projects/ntsw-cvs2/swcvs4/minerva/sdk/src/soc/phy/wcmod/include/wcmod_defines.h,v $
 *      $Date: 2012/06/04 18:23:53 $
 *      $Header: /projects/ntsw-cvs2/swcvs4/minerva/sdk/src/soc/phy/wcmod/include/wcmod_defines.h,v 1.17 2012/06/04 18:23:53 wniu Exp $
 *      $Log: wcmod_defines.h,v $
 *      Revision 1.17  2012/06/04 18:23:53  wniu
 *      PHY-615 added a new define for PLL enable without lock wait
 *
 *      Revision 1.16  2012/05/29 23:58:13  wniu
 *      PHY-613 added a member of fiber_pref for wcmod structure
 *
 *      Revision 1.15  2012/05/21 18:47:30  wniu
 *      PHY-594 added a member of firmware_mode for wcmod structure
 *
 *      Revision 1.14  2012/05/01 17:50:59  wniu
 *      added 4 variables into wcmod_st to accommodate deadlink vga/dfe value
 *
 *      Revision 1.13  2012/01/18 06:22:06  mjang
 *      	Adding dual_port.  dxgxs is for dual port BCST for register access.
 *      	dual_port is for indication of being a dual port.
 *      	For some dual port setting, only regsisters of one lane should be set.
 *
 *      Revision 1.12  2012/01/13 08:26:13  mjang
 *      	Add one variable for diagnostics functions.
 *
 *      Revision 1.11  2011/12/20 02:12:26  wniu
 *      added a reg defines for 0x820e
 *
 *      Revision 1.10  2011/12/07 23:05:35  wniu
 *      added two more mask defines for AUTONEG_ENABLE and AUTONEG_DONE
 *
 *      Revision 1.9  2011/12/01 22:31:15  wniu
 *      added three mebers vco_freq, pll_divider and oversample_mode into ws structure
 *
 *      Revision 1.8  2011/11/23 19:44:23  wniu
 *      added another defines for getting vertical offset for eyescan
 *
 *      Revision 1.7  2011/11/09 22:45:30  wniu
 *      added many diag eyescan defines for soc layer
 *
 *      Revision 1.6  2011/11/04 20:29:24  wniu
 *      added a new define for low freq PF value override
 *
 *      Revision 1.5  2011/10/11 01:37:30  wniu
 *      added a void pointer in wcmod structure to accommodate the eye-scan
 *
 *      Revision 1.4  2011/10/05 02:03:42  wniu
 *      added two more variables for wcmod structure
 *
 *      Revision 1.3  2011/10/01 03:15:31  wniu
 *      added two more member for tx/rx polarity
 *
 *      Revision 1.2  2011/09/14 02:17:34  miyarn
 *      Keep the build env happy. shut of complaints about missing id, copyright etc
 *
 *      Revision 1.1  2011/08/15 15:47:57  mlarsen
 *      Moved from soc/phy directory.
 *
 *      Revision 1.7  2011/07/28 22:17:25  wniu
 *      added two new variable for HW read and write of the wcmod_st
 *
 *      Revision 1.6  2011/06/29 23:50:29  wniu
 *      PHY-479 added eye margin stuff into wcmod structure
 *
 *      Revision 1.5  2011/06/09 23:22:52  wniu
 *      sync up with the Irvine database
 *
 *      Revision 1.59  2011/06/09 17:17:04  ravick
 *      incorporated RedStone and TR3 changes
 *
 *      Revision 1.58  2011/03/22 19:46:25  ravick
 *      consistant handles for TAPS + add refclk+plldiv in wcmod_st
 *
 *      Revision 1.57  2011/03/18 17:59:20  ravick
 *      new wcmod (derived from WC-C0)
 *
 *      Revision 1.56  2011/03/14 23:00:42  prasunp
 *      power control defines
 *
 *      Revision 1.55  2011/03/11 18:54:00  ravick
 *      c89 compatibility issues
 *
 *      Revision 1.54  2011/03/02 21:08:53  ravick
 *      *** empty log message ***
 *
 *      Revision 1.53  2011/02/28 23:42:16  ravick
 *      *** empty log message ***
 *
 *      Revision 1.52  2011/02/15 19:39:49  ravick
 *      extended control variables
 *      extended control variables
 *
 *      Revision 1.51  2011/02/14 23:34:47  ravick
 *      *** empty log message ***
 *
 *      Revision 1.50  2011/02/04 22:09:31  ravick
 *      removed slash-slash comments
 *
 *      Revision 1.49  2011/01/28 17:41:00  ravick
 *      *** empty log message ***
 *
 *      Revision 1.48  2011/01/17 23:52:51  ravick
 *      redstone changes. Also pcs_bypass_control
 *
 *      Revision 1.47  2011/01/17 23:09:52  ravick
 *      correct typo RX_M1_THESH_ZERO_ON. Documentation cleanup
 *
 *      Revision 1.46  2010/12/18 00:06:24  aerik
 *      wrote scrambler control function + documentation
 *
 *      Revision 1.45  2010/12/17 19:35:19  aerik
 *      added DFE, VGA to tier1 selector + documentation for lane power control
 *
 *      Revision 1.44  2010/12/17 03:17:39  aerik
 *      added peaking filter tier1
 *
 *      Revision 1.43  2010/12/17 01:37:51  aerik
 *      added dfe functions
 *
 *      Revision 1.42  2010/12/14 22:42:59  ravick
 *      merge prasuns changes
 *
 *      Revision 1.41  2010/12/11 00:25:04  ravick
 *      removed slashslash comments
 *
 *      Revision 1.40  2010/11/19 23:13:14  ravick
 *      use autogeneration of enums on verilog side. chandle, fix diag enums
 *
 *      Revision 1.39  2010/11/19 16:39:55  ravick
 *      *** empty log message ***
 *
 *      Revision 1.38  2010/11/18 18:14:33  ravick
 *      changed lane_num to lane_select
 *
 *      Revision 1.37  2010/11/08 19:06:18  ravick
 *      changes to consolidate address calc.
 *
 *      Revision 1.36  2010/11/01 20:44:53  aerik
 *      patch for 1g gmii in xenia -- changed pll divider ratio
 *
 *      Revision 1.35  2010/10/26 23:53:55  aerik
 *      patch for broadcast address bug + 1G xgmii/sgmii in xenia
 *
 *      Revision 1.34  2010/10/21 17:20:39  aerik
 *      added a div70 macro fro WC B0
 *
 *      Revision 1.33  2010/10/20 19:45:19  aerik
 *      model number updates
 *
 *      Revision 1.32  2010/10/14 22:18:36  ravick
 *      autoneg related changes. Too numerous to type
 *
 *      Revision 1.31  2010/10/13 22:51:56  ravick
 *      changed lane_num to lane_select. Next step is to use enums (currently using
 *      numbers)
 *      added MULTI_REG type. (must enumize this)
 *
 *      Revision 1.30  2010/09/30 22:14:47  ravick
 *      diags enum
 *
 *      Revision 1.29  2010/09/24 22:19:14  aerik
 *      added functionality to enable TX with gloop on
 *
 *      Revision 1.28  2010/09/17 22:11:21  ravick
 *      1. script to autogenerate enum files.
 *      2. Makefile changes to support above script
 *      3. Test vectors changed for new speed modes
 *      4. Regression debugged
 *      5. WC verif. checkers debugged.
 *      6. Documentation enhancements
 *      7. Remove redundant functions.
 *
 *      Revision 1.27  2010/09/16 19:48:56  aerik
 *      fixed autoneg function to work with lanes other than 0
 *
 *      Revision 1.26  2010/09/09 23:04:40  aerik
 *      changed Tier2 param from platform_info_type* to wcmod_st*
 *
 *      Revision 1.25  2010/08/11 23:30:52  aerik
 *      added additional Tier2 functions + a return value from the Tier2 configuration selector
 *
 *      Revision 1.24  2010/08/04 21:20:36  aerik
 *      added extra fields to struct wcmod_st to support Triumph3 and Tier2+
 *      modifications to support Triumph3 and Tier2+
 *
 *      Revision 1.23  2010/07/31 01:06:14  aerik
 *      adding a dv target
 *      updated files for integration with full chip DV environments
 *
 *      Revision 1.22  2010/07/28 22:20:57  aerik
 *      added multi-lane support
 *      added multi-lane support
 *
 *      Revision 1.21  2010/07/15 19:11:46  ravick
 *      documentation changes
 *
 *      Revision 1.20  2010/07/13 19:22:30  aerik
 *      generated the wcmod_spd_intf_set enum values from an x-macro definition file
 *
 *      Revision 1.19  2010/07/09 23:39:09  aerik
 *      introduced some constants for use with wcmod_autoneg_control function for improved readability and usage
 *
 *      Revision 1.18  2010/07/08 22:45:53  aerik
 *      added USE_CJPAT and USE_CRPAT #defines for use with the CJPAT/CRPAT function
 *
 *      Revision 1.17  2010/07/02 20:18:58  ravick
 *      modifed lane_select enum to only do any one lane or all lanes.
 *
 *      Revision 1.16  2010/07/01 21:13:23  ravick
 *      add doxygen target to makefile. Change printfs to display phy_ad
 *
 *      Revision 1.15  2010/06/28 18:31:20  ravick
 *      added reg_read and reg_write functions. Lots of doxygen hooks
 *
 *      Revision 1.14  2010/06/24 18:46:18  ravick
 *      this version of code supports doxygen. runwc fixed to  understand logext
 *
 *      Revision 1.13  2010/06/16 18:08:39  ravick
 *      bcst offset
 *
 *      Revision 1.12  2010/06/15 21:30:56  ravick
 *      AER support for lane access in indep mode
 *
 *      Revision 1.11  2010/06/07 17:21:25  ravick
 *      pass port/lane etc to final drivers
 *
 *      Revision 1.10  2010/05/26 22:38:38  aerik
 *      removed prbs_data and prbs_mask fields
 *
 *      Revision 1.9  2010/05/25 22:47:51  ravick
 *      added wc_touched and other speed modes
 *
 *      Revision 1.8  2010/05/25 19:13:22  ravick
 *      new wcmod interface, removed most of SDK deps. + indep. lane support
 *
 *      Revision 1.7  2010/05/19 17:43:20  ravick
 *      combined checkin with fixes,merges,whathaveyous. PRBS yet to work
 *
 *      Revision 1.6  2010/05/05 22:41:46  aerik
 *      added a few fields to a struct to support DPI calls for prbs
 *      added a few fields to support DPI calls for prbs
 *
 *      Revision 1.5  2010/05/04 15:53:27  ravick
 *      added another enum for port_type and changed spd_set to spd_intfc_set
 *
 *      Revision 1.4  2010/05/03 18:24:36  ravick
 *      added port_type, and modified port_mode func....
 *
 *      Revision 1.3  2010/04/29 16:09:13  ravick
 *      remove stop/start type handles. Add en_dis variable. tick/hashdef ENABLE_DISABLE
 *
 *      Revision 1.2  2010/04/29 10:37:33  ravick
 *      changed PDET_CONTRL to PDET_CONTROL
 *      File: wcmod_gloop_combo_1g.vec	Status: Locally Modified
 *      added per_lane_info
 *      File: wcmod_defines.h  	Status: Locally Modified
 *      added several Tier1 funcs
 *      File: wcmod_cfg_seq.c  	Status: Locally Modified
 *      added selection for several Tier1 funcs
 *      File: wcmod_main.c     	Status: Locally Modified
 *
 *      Revision 1.1  2010/04/02 23:17:56  jlouie
 *      Initial Version
 *
 *----------------------------------------------------------------------*/


#ifndef _WCMOD_DEFINES_H_
#define _WCMOD_DEFINES_H_  

#if defined (_SDK_WCMOD_)
#include <sal/types.h>
#endif
#include "wcmod_enum_defines.h"

/*! The number of serdes blocks (not lanes) in the system (may be multiple
 * chips) -- this should be changed by the user as it determines the number of
 * mutexes */ 
#define NUMBER_OF_LOCKS (2*3*18)

/*! Use CJPPAT. This constant is used with the function #wcmod_cjpat_crpat_control() */
#define USE_CJPAT 0x1
/*! Use CRPPAT. This constant is used with the function #wcmod_cjpat_crpat_control() */
#define USE_CRPAT 0x10

/*! Do not use auto-negotiation.  This constant is used with the function #wcmod_autoneg_control() */
#define DO_NOT_USE_AN             0x0
/*! Use clause-37 auto-negotiation.  This constant is used with the function #wcmod_autoneg_control() */
#define USE_CLAUSE_37_AN          0x1
#define USE_CLAUSE_37_AN_LANE0    (USE_CLAUSE_37_AN << 0)
#define USE_CLAUSE_37_AN_LANE1    (USE_CLAUSE_37_AN << 4)
#define USE_CLAUSE_37_AN_LANE2    (USE_CLAUSE_37_AN << 8)
#define USE_CLAUSE_37_AN_LANE3    (USE_CLAUSE_37_AN << 12)
/*! Use clause-73 auto-negotiation.  This constant is used with the function #wcmod_autoneg_control() */
#define USE_CLAUSE_73_AN          0x2
#define USE_CLAUSE_73_AN_LANE0    (USE_CLAUSE_73_AN << 0)
#define USE_CLAUSE_73_AN_LANE1    (USE_CLAUSE_73_AN << 4)
#define USE_CLAUSE_73_AN_LANE2    (USE_CLAUSE_73_AN << 8)
#define USE_CLAUSE_73_AN_LANE3    (USE_CLAUSE_73_AN << 12)
/*! Use clause-37 auto-negotiation with BAM.  This constant is used with the function #wcmod_autoneg_control() */
#define USE_CLAUSE_37_AN_WITH_BAM       0x4
#define USE_CLAUSE_37_AN_WITH_BAM_LANE0 (USE_CLAUSE_37_AN_WITH_BAM << 0)
#define USE_CLAUSE_37_AN_WITH_BAM_LANE1 (USE_CLAUSE_37_AN_WITH_BAM << 4)
#define USE_CLAUSE_37_AN_WITH_BAM_LANE2 (USE_CLAUSE_37_AN_WITH_BAM << 8)
#define USE_CLAUSE_37_AN_WITH_BAM_LANE3 (USE_CLAUSE_37_AN_WITH_BAM << 12)
/*! Use clause-73 auto-negotiation with BAM.  This constant is used with the function #wcmod_autoneg_control() */
#define USE_CLAUSE_73_AN_WITH_BAM        0x8
#define USE_CLAUSE_73_AN_WITH_BAM_LANE0  (USE_CLAUSE_73_AN_WITH_BAM << 0)
#define USE_CLAUSE_73_AN_WITH_BAM_LANE1  (USE_CLAUSE_73_AN_WITH_BAM << 4)
#define USE_CLAUSE_73_AN_WITH_BAM_LANE2  (USE_CLAUSE_73_AN_WITH_BAM << 8)
#define USE_CLAUSE_73_AN_WITH_BAM_LANE3  (USE_CLAUSE_73_AN_WITH_BAM << 12)
/*! \def Max number of legal speed and interface combinations. */
#define MAX_SPD_INTF_MODES 100


/* Indicate whether to power down TX on lane n */
#define POWER_DOWN_TX0 0x1
#define POWER_DOWN_TX1 0x100
#define POWER_DOWN_TX2 0x10000
#define POWER_DOWN_TX3 0x1000000

/* Indicate whether to power down RX on lane n */
#define POWER_DOWN_RX0 0x2
#define POWER_DOWN_RX1 0x200
#define POWER_DOWN_RX2 0x20000
#define POWER_DOWN_RX3 0x2000000

/* Indicate whether to power down TX on lane n */
#define CHANGE_TX0_POWERED_STATE 0x80
#define CHANGE_TX1_POWERED_STATE 0x8000
#define CHANGE_TX2_POWERED_STATE 0x800000
#define CHANGE_TX3_POWERED_STATE 0x80000000

/* Indicate whether to power down RX on lane n */
#define CHANGE_RX0_POWERED_STATE 0x40
#define CHANGE_RX1_POWERED_STATE 0x4000
#define CHANGE_RX2_POWERED_STATE 0x400000
#define CHANGE_RX3_POWERED_STATE 0x40000000


#define BYPASS_SCRAMBLER_64B_66B      0x1
#define SCRAMBLE_ENTIRE_64B_66B       0x2
#define SCRAMBLE_PAYLOAD_ONLY_64B_66B 0x4

#define BYPASS_DESCRAMBLER_64B_66B      0x8
#define DESCRAMBLE_ENTIRE_64B_66B       0x10
#define DESCRAMBLE_PAYLOAD_ONLY_64B_66B 0x20

#define SCRAMBLER_ON_8B_10B  0x40
#define SCRAMBLER_OFF_8B_10B 0x80

/* Values for the RX PF tier1 */
#define RX_PF_VAL_MASK        0xF
#define CHANGE_RX_PF_VALUES   0x10
#define RX_THRESH_125_MV      0x20
#define RX_THRESH_150_MV      0x40 
#define RX_THRESH_175_MV      0x80
#define RX_THRESH_200_MV      0x100
#define RX_M1_THRESH_ZERO_ON  0x200
#define RX_M1_THRESH_ZERO_OFF 0x400
#define RX_PF_HIZ_ON          0x800
#define RX_PF_HIZ_OFF         0x1000
#define CHANGE_RX_LOW_FREQ_PF_VALUES   0x2000

/* enable/disable uc control of rx params. */
#define RX_UC_CONTROL      0x80000000

/*autoneg enable and done bit mask */
#define AUTONEG_ENABLE_MASK   0x1
#define AUTONEG_DONE_MASK     0x8000

/* the cmd value for eyescan API */
#define WC_UTIL_2D_LIVELINK_EYESCAN               0
#define WC_UTIL_2D_PRBS_EYESCAN                   1
#define WC_UTIL_ENABLE_LIVELINK                   2
#define WC_UTIL_DISABLE_LIVELINK                  3
#define WC_UTIL_ENABLE_DEADLINK                   4
#define WC_UTIL_DISABLE_DEADLINK                  5
#define WC_UTIL_SET_VOFFSET                       6
#define WC_UTIL_SET_HOFFSET                       7
#define WC_UTIL_GET_MAX_VOFFSET                   8
#define WC_UTIL_GET_MIN_VOFFSET                   9
#define WC_UTIL_GET_MAX_LEFT_HOFFSET              10
#define WC_UTIL_GET_MAX_RIGHT_HOFFSET             11
#define WC_UTIL_START_LIVELINK                    12
#define WC_UTIL_START_DEADLINK                    13
#define WC_UTIL_STOP_LIVELINK                     14
#define WC_UTIL_CLEAR_LIVELINK                    15
#define WC_UTIL_READ_LIVELINK                     16
#define WC_UTIL_READ_DEADLINK                     17
#define WC_UTIL_GET_INIT_VOFFSET                  18

/* Model values from the SerdesId0 register */
#define MODEL_SERDES_CL73_COMBO_CORE 0
#define MODEL_XGXS_16G               1 /* This is Xenia */
#define MODEL_HYPERCORE              2
#define MODEL_HYPERLITE              3
#define MODEL_PCIE_G2_PIPE           4
#define MODEL_1p25GBd_SERDES         5
#define MODEL_SATA2                  6
#define MODEL_QSGMII                 7
#define MODEL_XGXS10G                8
#define MODEL_WARPCORE               9
#define MODEL_WARPLITE               0xc
#define MODEL_ESM_SERDES             0xe
#define MODEL_QUADSGMII              0xf
#define MODEL_WARPCORE_C             0x10 /* strange but true. */
#define MODEL_XGXS_CL73              0x1d
#define MODEL_SERDES_CL73            0x1e
 
/* Settings for Xenia */
#define XGXSBLK0_XGXSCONTROL_MODE_10G_IndLane_XENIA 6
#define SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div16_XENIA 0
#define SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div40_XENIA 8

/*! \def Enable a feature. Keep the bit location flexible. */
#define WCMOD_ENABLE  0x1
/*! \def Disable a feature. Keep the bit location flexible. */
#define WCMOD_DISABLE 0x0
/*! \def enable the PLL seqeuncer without lock wait */
#define WCMOD_PLL_ENABLE_NO_WAIT 0x2

#define DSC1B0_UC_CTRLr 0x820e


/*! \struct wcmod_int_st

This is an internal structure containing things useful only to WCMod. It
is not exposed to customer code.

*/

typedef struct {
  /*! this bit is set if we are in asymmetric mode. (for Xenia only) */
  int asymmetric_mode;
} wcmod_int_st;

/*! \struct wcmod_st

This is the central Warpcore context storing information of a single warpcore.
It has multiple sections, each with a set of elements.

\li Address Context: Elements to uniquely access a warpcore 

*/

typedef struct {
  /*! The id of the chip -- this identifies which mutex to use -- the exact
   * mapping of "id" to chip/warpcore #/etc. will differ for verification the
   * SW stack or the testboard development environment */
  int id;
  /* ===== BEGIN ===== WC port address section */
  /*! chip number of the chip where the warpcore resides, defined by MDK/SDK
   * platforms. This information is not useful to verilog platforms and other
   * customer specific software platforms. */
  int                 unit; 
  /*! port no. defined in MDK/SDK platforms. This information is not useful to
   * verilog platforms and other customer specific software platforms. */
  int                 port; 
  /*! physical addr. of the warpcore as defined by MDK/SDK platforms. This
   * information is not useful to verilog platforms and other customer specific
   * software platforms. */
  int                 phy_ad; 
  /*! lanes to be accessed. Can simultaneously select multiple lanes. */
  wcmod_lane_select   lane_select; 
  /*! port address strap value. This is used only in WC verif. env. to identify
   * the warpcore to the station manager */
  int                 prt_ad; 
  /*! Current lane being processed can be 0, 1, 2 or 3. */
  int this_lane;
  int num_of_lane;
  /*! Temp. variable to manage reg. access when there are'nt multiple copies of
   * registers. */
  int                 lane_num_ignore; 
  /*! Tier1 Features are controlled by adding values to this var. Currently we
   * manage with 32 bits (int type). If more bits are needed to specify a
   * feature,more feature control variables will be added or this will be
   * turned into an array. In general we try to use at most 8 bits per lane. */
  int                 per_lane_control; 
  int                 per_lane_control_x; 
  int                 per_lane_control_1; 
  int                 per_lane_control_2; 
  int                 per_lane_control_3; 
  int                 per_lane_control_4; 
  int                 per_lane_control_5; 
  int                 per_lane_control_6; 
  int                 per_lane_control_7; 
  int                 per_lane_control_8; 
  /*! enable disable feature. Deprecated. No longer used. */
  int                 en_dis; 
  /*! not needed in verification platforms. Not needed in radian either.  */
  int                 aer_ofs_strap; 
  /*! enables a second way of accessing lanes. Not supported. */
  int                 multiport_addr_en; 
  /*! Broadcast addr. ext. Currently hardcoded to 0x1ff. */
  int                 aer_bcst_ofs_strap; 
  /*! see doc. on wcmod_spd_intfc_set */
  wcmod_spd_intfc_set spd_intf; 
  /*! cl22 or cl45 or PCB */
  wcmod_mdio_type     mdio_type; 
  /*! combo or independent */
  wcmod_port_type     port_type; 
  /*! Oversampling rate. This is fixed in combo, but not in indep. */
  wcmod_os_type       os_type; 
  /*! Lanes in dual XGXS? Info. is bit encoded. bit 1:lane 0/1 bit 2:lane 2/3 */
  int                 dual_type;
  int                 dxgxs;
  /*! Single copy registers cannot be modified repeatedly. Tier1 init function
   * access these registers has to be aware to do this only once between
   * resets, especially in independent mode when the same Tier1 is called for
   * multiple lanes. This sticky bit, remembers if the single copy regs. were
   * already modified. */
  int                 wc_touched; 
  /*! Lane swap contl variable. The positional values of the 8 nibbles indicate
   * location of the logical lanes, like so. rx3_rx2_rx1_rx0_tx3_tx2_tx1_tx0.
   * Default value 0x32103210 indicates no lanes swapped. */
  int                 lane_swap; 
  /*! Not set by user, but set by WCMod based on speed and interface. */
  wcmod_duplex_type   duplex; 
  /*! Address of reg. to access */
  int                 accAddr; 
  /*! Data context. Contains data to write or data that is just read. Also
   * contains revid after read_revid. */
  int                 accData; 
  /*! Various status of the link. Note check status of one link at a time. */
  int     diag_type;
  int     info_type;
  /*! Flavors of Warpcore (includes Xenia, quad_sgmii, B0/C0, revisions etc.) */
  wcmod_model_type    model_type;
  /*!Autoneg types. */
  wcmod_an_type       an_type;
  /*!autoneg parameter RF */
  int                 an_rf;
  /*!autoneg parameter pause */
  int                 an_pause;
  /*!autoneg parameter various speeds (bit encoded) */
  int                 an_tech_ability;
  /*!autoneg parameter enable FEC */
  int                 an_fec;
  int                 an_higig2;
  int                 an_cl72;
  int                 tx_pol;
  int                 rx_pol;
  int                 vco_freq;
  int                 pll_divider;
  int                 oversample_mode;
  int                 asymmetric_mode;
  int                 s100g_plus;
  /*! Platform specific Information about the Warpcore. It usually contains
   * platform driver specific information. Note that platform must create
   * (allocate memory), initialize this variable and connect it to wcmod_st*/
  void *platform_info;
  /*! Used to control how much information the driver prints. If set to zero,
   * only warnings and errors will be printed. If set to 1, msgs (developers
   * discretion will be printed) but not register transactions.  If set to 2
   * transactions will be printed (plus everything else). This should be an
   * enum. FIXME.
   */
  int verbosity;
  int wcmod_sdk_ctrl_type;
  /*! for when user wants to provide custom refclk. Will also work for standard
   * values. The 10ths value of the frequency need not be entered. So 156.1
   * becomes 156
   */
  int refclk;
  /*! pll div value is auto computed. But override is provided. */
  int live_link;
  int ber;
  int tol;
  uint16 vga_frzval;
  uint16 vga_frcfrz;
  uint16 dfe_frzval;
  uint16 dfe_frcfrz;
  uint16 firmware_mode;
  uint16 fiber_pref;
  int eye_cmd;
  int plldiv;
  int (*read)(int, uint32, uint32, uint16*); /* HW read */
  int (*write)(int, uint32, uint32, uint16); /* HW write */
  void *arg;

} wcmod_st;

#endif  /*  _WCMOD_DEFINES_H_ */
