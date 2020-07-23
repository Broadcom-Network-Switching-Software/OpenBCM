/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */ 

#ifndef _DV_TB_
 #define _SDK_QMOD_ 1 
#endif

#ifdef _DV_TB_
#include "qmod_phyreg.h"
#include <stdint.h>
#include "qmod_main.h"
#include "qmod_enum_defines.h"
#include "qmod.h"
#include "qmod_defines.h"
#include "qmod_phyreg.h"
#include "qePCSRegEnums.h"
#include "phy_tsc_iblk.h"
#endif

#ifdef _SDK_QMOD_
#include "phy_tsc_iblk.h"
#include <phymod/phymod.h>
#include <phymod/phymod_system.h>
#include <phymod/phymod_util.h>
#include <phymod/phymod_debug.h>
#include <phymod/chip/bcmi_qtce_xgxs_defs.h>
#include "qmod_enum_defines.h"
#include "qmod.h"
#include "qmod_device.h"
#include "qmod_sc_lkup_table.h"
#include "qePCSRegEnums.h"
#endif

#define _TRGO_(a,l) PHYMOD_DEBUG_ERROR(("<< PHY_TRG: Adr:%06d Ln:%02d\n",a,l));
#define _TRGC_(a,l) PHYMOD_DEBUG_ERROR((">> PHY TRG: Adr:%06d Ln:%02d\n",a,l));
#ifndef PHYMOD_ST
#ifdef _SDK_QMOD_
  #define PHYMOD_ST phymod_access_t
  #define _TRG_OPN(a) _TRGO_(a->addr, a->lane_mask)
  #define _TRG_CLS(a) _TRGC_(a->addr, a->lane_mask)
#endif /*_SDK_QMOD_ */
#endif /* PHYMOD_ST */

#ifdef _DV_TB_
  #define PHYMOD_ST  qmod_st
  #define _TRG_OPN(a) _TRGO_(a->prt_ad, a->this_lane)
  #define _TRG_CLS(a) _TRGC_(a->prt_ad, a->this_lane)
#endif

int _get_no_of_pcs_lane(PHYMOD_ST* ws);
int qmod_diag_link(PHYMOD_ST *ws);
int qmod_diag_st(PHYMOD_ST *ws, int resolved_speed);

/**
@brief   Field OVR diagnostic function
@param   pc handle to current TSCE context (#PHYMOD_ST)
@returns PHYMOD_E_NONE if no errors. PHYMOD_E_FAIL else.
@details Field OVR Details
   Check if the field OVR is set, and if OVR enabled, print the OVR value
*/

int qmod_diag_field_or(PHYMOD_ST *ws)
{
   return PHYMOD_E_NONE;
}

/**
@brief   pmd pcs lane cfg diagnostic function
@param   pc handle to current TSCE context (#PHYMOD_ST)
@returns PHYMOD_E_NONE if no errors. PHYMOD_E_FAIL else.
@details Get the uc controls for pmd lane cfg
  Read the pcs_lane_cfg information from uc RAM. To check if the pmd is to be
  programmed from pcs lane cfg.
 */
int qmod_diag_pmd_pcs_lane_cfg(PHYMOD_ST *ws, int lane)
{
  return PHYMOD_E_NONE ;
} /* qmod_diag_pmd_pcs_lane_cfg */

/**
@brief   Speed diagnostic function
@param   pc handle to current TSCE context (#PHYMOD_ST)
@returns PHYMOD_E_NONE if no errors. PHYMOD_E_FAIL else.
@details Speed Details
 1. Get the Speed set
 2. Get the Speed valis and Speed Done Status
 3. Get the final stats of the speed
 */
int qmod_diag_speed(PHYMOD_ST *ws)
{
  return PHYMOD_E_NONE;
}

/**
@brief   Autoneg diagnostic function
@param   pc handle to current TSCE context (#PHYMOD_ST)
@returns PHYMOD_E_NONE if no errors. PHYMOD_E_FAIL else.
@details Autoneg diags
 1.  Get the AN complete status for AN37/73
 2.  Get the resolved HCD
 3.  Get the link up status after AN
*/
int qmod_diag_autoneg(PHYMOD_ST *ws)
{
  return PHYMOD_E_NONE;
}

/**
@brief   Topology diagnostic function.
@param   pc handle to current TSCE context (#PHYMOD_ST)
@returns PHYMOD_E_NONE if no errors. PHYMOD_E_FAIL else.
@details Get the topology details
1. LoopBack
   a. Remote Loopback - RX loopback
      i.  PCS loopback
      ii. PMD loopback  //Not tested in TSCE
   b. Local loopback/gloop - TX loopback
      i.  PCS loopback
      ii. PMD loopback
2. LaneSwap
   a. PCS LaneSwap(RX)
   b. PMD LaneSwap(TX)
3. Polarity Inversion
   a. tx
   b. rx
4. Master PortNum, port mode and single port mode
*/

int qmod_diag_topology(PHYMOD_ST *ws)
{
  MAIN_LPBK_CTLr_t pcs_loopback_ctrl;
  TLB_RX_DIG_LPBK_CFGr_t pmd_loopback_dig;
  TLB_TX_RMT_LPBK_CFGr_t pmd_loopback_remote;
  MAIN_LN_SWPr_t pcs_lane_swap;
  DIG_TX_LN_MAP_3_N_LN_ADDR_0_1r_t pmd_lane_addr_swap_1;
  DIG_LN_ADDR_2_3r_t pmd_lane_addr_swap_2;
  DIG_TX_LN_MAP_0_1_2r_t pmd_lane_addr_swap_3;
  TLB_TX_TLB_TX_MISC_CFGr_t tx_pol_inv;
  TLB_RX_TLB_RX_MISC_CFGr_t rx_pol_inv;
  MAIN_SETUPr_t reg_main0_setup;
 
  /* Main0 Setup Register */
  /*1. Loopback  */
  READ_MAIN_LPBK_CTLr(ws, &pcs_loopback_ctrl);
 
  READ_TLB_RX_DIG_LPBK_CFGr(ws, &pmd_loopback_dig);
 
  READ_TLB_TX_RMT_LPBK_CFGr(ws, &pmd_loopback_remote);
 
   /*2. LaneSwap  */
  READ_MAIN_LN_SWPr(ws, &pcs_lane_swap);
 
  READ_DIG_TX_LN_MAP_3_N_LN_ADDR_0_1r(ws, &pmd_lane_addr_swap_1);
  READ_DIG_LN_ADDR_2_3r(ws, &pmd_lane_addr_swap_2);
  READ_DIG_TX_LN_MAP_0_1_2r(ws, &pmd_lane_addr_swap_3);
 
  /*3.Polarity Inversion */
  READ_TLB_TX_TLB_TX_MISC_CFGr(ws, &tx_pol_inv);
  READ_TLB_RX_TLB_RX_MISC_CFGr(ws, &rx_pol_inv);
 
   /* 4. Master PortNum */
  READ_MAIN_SETUPr(ws, &reg_main0_setup);
 
  return PHYMOD_E_NONE;
}

/**
@brief   Get the link status
@param   pc handle to current TSCE context (#PHYMOD_ST)
@returns PHYMOD_E_NONE if no errors. PHYMOD_E_FAIL else.
@details Call all the Diags
*/
int qmod_diag_general(PHYMOD_ST *ws)
{
  qmod_diag_topology(ws);
  qmod_diag_link(ws);
  qmod_diag_autoneg(ws);
  qmod_diag_speed(ws);

  return (PHYMOD_E_NONE);
}

/**
@brief   Get the link status
@param   pc handle to current TSCE context (#PHYMOD_ST)
@returns PHYMOD_E_NONE if no errors. PHYMOD_E_FAIL else.
@details Print the link status.
1. pmd_live_status
   a. pll_lock_sts : PMD PLL lock indication
   //b. tx_clk_vld_sts : The txclk is valid Don't see a need for this now, so will not check this bit
2a. pmd_lane_live_status
   a. signal_detect_sts : The signal detected on the lane
   b. rx_lock_sts : The recovery clock locked for the rx lane
   //c. rx_clk_vld_sts : The rxclk is valid for the lane. Don't see a need for this now, so will not check this bit
2b. pmd_lane_latched_status
3a. pcs_live_status
   a. sync status : per port: Live Sync Status indicator for cl36, cl48, brcm mode. Comma detect/code sync
                    per lane: Live Block Lock Status indicator for cl49, cl82.Sync bits locked, on the 2 sync bits of the 66 bits(of 64/66 encoding)
   b. link_status : Link status for that port
3b. pcs_latched_status
*/
int qmod_diag_link(PHYMOD_ST *ws)
{

   return PHYMOD_E_NONE;
}

/**
@brief   Internal Tfc info (PRTP. vs. PktGen vs. MAC dataetc.).
@param   pc handle to current TSCE context (#PHYMOD_ST)
@returns PHYMOD_E_NONE if no errors. PHYMOD_E_ERROR else.
@details Print the Internal Tfc types.
*/
int qmod_diag_internal_tfc(PHYMOD_ST *ws)
{
  return PHYMOD_E_NONE;
}

/**
@brief   Soft Table diagnostic function
@param   pc handle to current TSCE context (#PHYMOD_ST)
@returns PHYMOD_E_NONE if no errors. PHYMOD_E_FAIL else.
@details ST diags
 Print the soft table enteries
 if the resolved_speed is not set, then all the table enteries are displayed
*/
int qmod_diag_st(PHYMOD_ST *ws, int resolved_speed)
{
  return PHYMOD_E_NONE;
} /*qmod_diag_st */

/**
@brief   AN Timers diagnostic function
@param   pc handle to current TSCE context (#PHYMOD_ST)
@returns PHYMOD_E_NONE if no errors. PHYMOD_E_FAIL else.
@details Prints the AN timers
*/
int qmod_diag_an_timers(PHYMOD_ST* ws)
{
  return PHYMOD_E_NONE;
}

/**
@brief   Prints out the debug state register
@param   pc handle to current TSCE context (#PHYMOD_ST)
@returns PHYMOD_E_NONE if no errors. PHYMOD_E_FAIL else.
@details Prints the debug state regs.
*/
int qmod_diag_state(PHYMOD_ST* ws)
{
  return PHYMOD_E_NONE;
}

/**
@brief  TBD
@param   pc handle to current TSCE context (#PHYMOD_ST)
@returns PHYMOD_E_NONE if no errors. PHYMOD_E_FAIL else.
@details  Misc debug
*/
int qmod_diag_debug(PHYMOD_ST* ws)
{
  return PHYMOD_E_NONE;
}

int qmod_diag_g_rd_ram(PHYMOD_ST *ws, int mode, int lane)
{
  return PHYMOD_E_NONE ;
}

int qmod_diag_g_rd_uC(PHYMOD_ST *ws, int lane)
{
  return PHYMOD_E_NONE ;
}

int qmod_diag_g_info(PHYMOD_ST *ws)
{
  return PHYMOD_E_NONE ;
}

int qmod_diag_ieee(PHYMOD_ST *ws)
{
  return PHYMOD_E_NONE;
}

/* get the no of pcs lanes */
int _get_no_of_pcs_lane(PHYMOD_ST* ws)
{
  return 0;
}

/*!

*/
int qmod_diag(PHYMOD_ST *pc, qmod_diag_type diag_type)
{
  int rv;
  rv = PHYMOD_E_NONE;

  PHYMOD_DEBUG_ERROR(("+------------------------------------------------------------------------------+\n"));
  if(diag_type & QMOD_DIAG_GENERAL)    rv |= qmod_diag_general(pc);
  if(diag_type & QMOD_DIAG_TOPOLOGY)   rv |= qmod_diag_topology(pc);
  if(diag_type & QMOD_DIAG_LINK)       rv |= qmod_diag_link(pc);
  if(diag_type & QMOD_DIAG_SPEED)      rv |= qmod_diag_speed(pc);
  if(diag_type & QMOD_DIAG_ANEG)       rv |= qmod_diag_autoneg(pc);
  if(diag_type & QMOD_DIAG_TFC)        rv |= qmod_diag_internal_tfc(pc);
  if(diag_type & QMOD_DIAG_AN_TIMERS)  rv |= qmod_diag_an_timers(pc);
  if(diag_type & QMOD_DIAG_STATE)      rv |= qmod_diag_state(pc);
  if(diag_type & QMOD_DIAG_DEBUG)      rv |= qmod_diag_debug(pc);
  return rv;
}

/*!
*/
int qmod_diag_disp(PHYMOD_ST *pc, const char* cmd_str)
{
    qmod_diag_type type;

    if (!cmd_str)
        type = QMOD_DIAG_GENERAL;
    else if (!PHYMOD_STRCMP(cmd_str, "topo"))
        type = QMOD_DIAG_TOPOLOGY;
    else if (!PHYMOD_STRCMP(cmd_str, "link"))
        type = QMOD_DIAG_LINK;
    else if (!PHYMOD_STRCMP(cmd_str, "speed"))
        type = QMOD_DIAG_SPEED;
    else if (!PHYMOD_STRCMP(cmd_str, "aneg"))
        type = QMOD_DIAG_ANEG;
    else if (!PHYMOD_STRCMP(cmd_str, "tfc"))
        type = QMOD_DIAG_TFC;
    else if (!PHYMOD_STRCMP(cmd_str, "antimers"))
        type = QMOD_DIAG_AN_TIMERS;
    else if (!PHYMOD_STRCMP(cmd_str, "state"))
        type = QMOD_DIAG_STATE;
    else if (!PHYMOD_STRCMP(cmd_str, "debug"))
        type = QMOD_DIAG_DEBUG;
    else
        type = QMOD_DIAG_GENERAL;

    return (qmod_diag(pc, type));
}



