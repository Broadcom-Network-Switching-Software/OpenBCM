/*----------------------------------------------------------------------
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Broadcom Corporation
 * Proprietary and Confidential information
 * Copyright: (c) 2012 Broadcom Corp.
 * All rights reserved
 * This source file is the property of Broadcom Corporation, and
 * may not be copied or distributed in any isomorphic form without the
 * prior written consent of Broadcom Corporation.
 *---------------------------------------------------------------------
 * File       : tscmod_diagnostics.c
 * Description: C sequences which utilize tier1s to build configurations
 *              which target only the diagnostic features of the Serdes
 *-----------------------------------------------------------------------*/
#include <shared/bsl.h>
#include "tscmod_main.h"

#ifdef _SDK_TSCMOD_
#include "phydefs.h"
#endif

#ifdef INCLUDE_XGXS_TSCMOD

#include "tscmod_main.h"
#include "tscmod.h"
#include "tscmod_defines.h"
#ifndef __KERNEL__
#include <math.h>
#endif
#ifdef _SDK_TSCMOD_
#include <sal/types.h>
#include <sal/core/spl.h>
#include <sal/core/thread.h>
#include "tscmod_phyreg.h"

#endif

#define MAX_TAP_SUM_CONSTRAINT 63
#define MIN_TAP_EYE_CONSTRAINT 17

int _tscmod_enable_livelink_diagnostics(tscmod_st* ws);
int _tscmod_disable_livelink_diagnostics(tscmod_st* ws);
int _tscmod_enable_deadlink_diagnostics(tscmod_st* ws);
int _tscmod_disable_deadlink_diagnostics(tscmod_st* ws);
int _tscmod_set_voff(tscmod_st* ws, int voff);
int _tscmod_set_hoff(tscmod_st* ws, int hoff);
int _tscmod_get_max_voffset(tscmod_st* ws);
int _tscmod_get_min_voffset(tscmod_st* ws);
int _tscmod_get_init_voffset(tscmod_st* ws);
int _tscmod_get_max_hoffset_left(tscmod_st* ws);
int _tscmod_get_max_hoffset_right(tscmod_st* ws);
int _tscmod_start_livelink_diagnostics(tscmod_st* ws);
int _tscmod_start_deadlink_diagnostics(tscmod_st* ws);
int _tscmod_stop_livelink_diagnostics(tscmod_st* ws);
int _tscmod_clear_livelink_diagnostics(tscmod_st* ws);
int _tscmod_read_livelink_diagnostics(tscmod_st* ws);
int _tscmod_read_deadlink_diagnostics(tscmod_st* ws);

extern int tscmod_prbs_check(tscmod_st* ws);

#ifndef TSCMOD_MEM_SET
#define TSCMOD_MEM_SET(_dst,_val,_len) sal_memset(_dst,_val,_len)
#endif


/*
static char *TSCMOD_SPEED_NAME[] = {
"10M",
"100M",
"1G",
"2p5G",
"5G_X4",
"6G_X4",
"10G_HiG",
"10G_CX4",
"12G_HiG",
"12p5G_X4",
"13G_X4",
"15G_X4",
"16G_X4",
"1G_KX",
"10G_KX4",
"10G_KR",
"5G",
"6p4G",
"20G_X4",
"21G_X4",
"25G_X4",
"10G_HiG_DXGXS",
"10G_DXGXS",
"10p5G_HiG_DXGXS",
"10p5G_DXGXS",
"12p773G_HiG_DXGXS",
"12p773G_DXGXS",
"10G_XFI",
"40G",
"20G_HiG_DXGXS",
"20G_DXGXS",
"10G_SFI",
"31p5G",
"32p7G",
"20G_SCR",
"10G_HiG_DXGXS_SCR",
"10G_DXGXS_SCR",
"12G_R2",
"10G_X2",
"40G_KR4",
"40G_CR4",
"100G_CR10"
};

char *getSpeedString(int speed)
{
  if (speed < 0)
    return "ILLEGAL SPEEDNUM";
  if (speed >= TSCMOD_SPD_ILLEGAL)
    return "ILLEGAL SPEEDNUM";
  return SPEED_NAME[speed];
}

char* an_adv0_print(uint16 data)
{
  return ("");
}

char* an_adv1_print(uint16 data)
{
  return ("");
}

char* an_adv2_print(uint16 data)
{
  return ("");
}
*/

/* accData[3:0] = TX->RX LOOPBACK_EN[ln3:0] */
int tscmod_diag_tx_loopback(tscmod_st *ws)
{
   uint16 data ;

   SOC_IF_ERROR_RETURN
      (READ_WC40_MAIN0_LOOPBACK_CONTROLr(ws->unit, ws, &data));

   ws->accData=(data & MAIN0_LOOPBACK_CONTROL_LOCAL_PCS_LOOPBACK_ENABLE_MASK)
                >> MAIN0_LOOPBACK_CONTROL_LOCAL_PCS_LOOPBACK_ENABLE_SHIFT ;
   return SOC_E_NONE;
}

/* accData[3:0] = RX->TX PMD LOOPBACK_EN[ln3:0] */
/* accData[7:4] = RX->TX PCS LOOPBACK_EN[ln3:0] */
int tscmod_diag_rx_loopback(tscmod_st *ws)
{
   uint16 data ;  int val ;
   SOC_IF_ERROR_RETURN
      (READ_WC40_MAIN0_LOOPBACK_CONTROLr(ws->unit, ws, &data));

   val =  (data &MAIN0_LOOPBACK_CONTROL_REMOTE_PCS_LOOPBACK_ENABLE_MASK)
      >>MAIN0_LOOPBACK_CONTROL_REMOTE_PCS_LOOPBACK_ENABLE_SHIFT ;
   val = val << 4 ;
   ws->accData=((data &MAIN0_LOOPBACK_CONTROL_REMOTE_PMD_LOOPBACK_ENABLE_MASK)
                 >>MAIN0_LOOPBACK_CONTROL_REMOTE_PMD_LOOPBACK_ENABLE_SHIFT) ;
   ws->accData |= val ;
   return SOC_E_NONE;
}

int tscmod_diag_speed(tscmod_st *ws)
{
   uint16 vec=0, data16 = 0; int side ; int pd_complete, pd_mode ;

   if(ws->an_type) {  /* 0xc1ab */
      /* 0xc1ac */
      SOC_IF_ERROR_RETURN
      (READ_WC40_AN_X4_SW_MANAGEMENT_AN_MISC_STATUSr(ws->unit, ws, &data16));
      pd_complete = (data16 & AN_X4_SW_MANAGEMENT_AN_MISC_STATUS_PD_COMPLETED_MASK ) ? 1: 0 ;
      pd_mode     = (data16 & AN_X4_SW_MANAGEMENT_AN_MISC_STATUS_PD_HCD_KX4_OR_KX_MASK ) ? 1: 0 ;
      /* pd_mode =0 -> 10G KX4, pd_mode=1->1G KX */
      if(pd_complete==1) {
         if(pd_mode) vec = e2n_tscmod_aspd_type[TSCMOD_ASPD_1G_KX1] ;
         else        vec = e2n_tscmod_aspd_type[TSCMOD_ASPD_10G_KX4] ;
      } else {

         SOC_IF_ERROR_RETURN
            (READ_WC40_AN_X4_SW_MANAGEMENT_AN_ABILITY_RESOLUTION_STATUSr
             (ws->unit, ws, &data16));
         vec = ( data16
                 & AN_X4_SW_MANAGEMENT_AN_ABILITY_RESOLUTION_STATUS_AN_HCD_SPEED_MASK)
            >> AN_X4_SW_MANAGEMENT_AN_ABILITY_RESOLUTION_STATUS_AN_HCD_SPEED_SHIFT;
      }
      side =0 ;
   } else {
      SOC_IF_ERROR_RETURN
         (READ_WC40_CL72_MISC2_CONTROLr(ws->unit, ws, &data16));
      vec = (data16 & CL72_MISC2_CONTROL_SW_ACTUAL_SPEED_MASK)>>
                      CL72_MISC2_CONTROL_SW_ACTUAL_SPEED_SHIFT ;
      side =1 ;
   }
   ws->accData = vec  ;
   if(ws->verbosity & TSCMOD_DBG_SPD)
      printf("%-22s: p=%0d read back %s asp=%x side=%0d\n",
             FUNCTION_NAME(), ws->port, tscmod_aspd_vec2s(vec), vec, side) ;
   return SOC_E_NONE;
}

/*!
  An autoneg diagnostic function.
  Set per_lane_control[TSCMOD_DIAG_AN_DONE] for AN completion.
  Set per_lane_control[HCD] for AN HCD speed result.

 */
int tscmod_diag_autoneg(tscmod_st *ws)
{
      uint16         data16 ;  int an_cl73, an_cl37, speed_vec ;
   data16 = 0 ;
   an_cl73 = 0 ; an_cl37 = 0 ;  speed_vec = 0 ;
   ws->accData = FALSE ;
   if((ws->per_lane_control >>TSCMOD_DIAG_AN_DONE) & 1) {
      /* 0xc180 */
      SOC_IF_ERROR_RETURN
         (READ_WC40_AN_X4_ABILITIES_ENABLESr
          (ws->unit, ws, &data16));
      if(data16 & AN_X4_ABILITIES_ENABLES_CL37_BAM_ENABLE_MASK ) an_cl37 = 1 ;
      if(data16 & AN_X4_ABILITIES_ENABLES_CL73_BAM_ENABLE_MASK ) an_cl73 = 1 ;
      if(data16 & AN_X4_ABILITIES_ENABLES_CL73_HPAM_ENABLE_MASK) an_cl73 = 1 ;
      if(data16 & AN_X4_ABILITIES_ENABLES_CL73_ENABLE_MASK)      an_cl73 = 1 ;
      if(data16 & AN_X4_ABILITIES_ENABLES_CL37_SGMII_ENABLE_MASK)an_cl37 = 1 ;
      if(data16 & AN_X4_ABILITIES_ENABLES_CL37_ENABLE_MASK)      an_cl37 = 1 ;

      if( an_cl73 && an_cl37)
         printf("%s FATAL: u=%0d p=%0d CL73 and CL37 are enabled at the same\
 time d=%0x\n", FUNCTION_NAME(), ws->unit, ws->port, data16) ;

      /* 0xc1ac */
      SOC_IF_ERROR_RETURN
         (READ_WC40_AN_X4_SW_MANAGEMENT_AN_MISC_STATUSr
          (ws->unit, ws, &data16));

      if(an_cl73) {
         ws->accData |= TSCMOD_AN_ENABLE_MASK ;
         if(data16 &
            AN_X4_SW_MANAGEMENT_AN_MISC_STATUS_AN_COMPLETE_MASK){
            ws->accData |= TSCMOD_AN_DONE_MASK ;
         }
      } else if (an_cl37) {
         ws->accData |= TSCMOD_AN_ENABLE_MASK ;
         if(data16 &
            AN_X4_SW_MANAGEMENT_AN_MISC_STATUS_AN_COMPLETE_MASK){
            ws->accData |= TSCMOD_AN_DONE_MASK ;
         }
      }
   } else if((ws->per_lane_control >>TSCMOD_DIAG_AN_HCD) & 1) {
      SOC_IF_ERROR_RETURN   /* 0xc1ab */
         (READ_WC40_AN_X4_SW_MANAGEMENT_AN_ABILITY_RESOLUTION_STATUSr
          (ws->unit, ws, &data16));
      speed_vec = ((data16
         & AN_X4_SW_MANAGEMENT_AN_ABILITY_RESOLUTION_STATUS_AN_HCD_SPEED_MASK)
        >> AN_X4_SW_MANAGEMENT_AN_ABILITY_RESOLUTION_STATUS_AN_HCD_SPEED_SHIFT);
      ws->accData = speed_vec ;
      if(ws->verbosity & TSCMOD_DBG_SPD)
      printf("%s AN HCD read back %s asp=%x\n",
             FUNCTION_NAME(), tscmod_aspd_vec2s(speed_vec), speed_vec) ;
   } else if((ws->per_lane_control>>TSCMOD_DIAG_AN_MODE) & 1){
      SOC_IF_ERROR_RETURN
         (READ_WC40_AN_X4_ABILITIES_ENABLESr(ws->unit, ws, &data16));
      ws->accData = 0 ;
      if(data16&AN_X4_ABILITIES_ENABLES_CL73_ENABLE_MASK) ws->accData |= TSCMOD_DIAG_AN_MODE_CL73 ;
      if(data16&AN_X4_ABILITIES_ENABLES_CL37_ENABLE_MASK) ws->accData |= TSCMOD_DIAG_AN_MODE_CL37 ;
      if(data16&AN_X4_ABILITIES_ENABLES_CL73_BAM_ENABLE_MASK)
                                                          ws->accData |= TSCMOD_DIAG_AN_MODE_CL73BAM ;
      if(data16&AN_X4_ABILITIES_ENABLES_CL37_BAM_ENABLE_MASK)
                                                          ws->accData |= TSCMOD_DIAG_AN_MODE_CL37BAM ;
      if(data16&AN_X4_ABILITIES_ENABLES_CL73_HPAM_ENABLE_MASK)
                                                          ws->accData |= TSCMOD_DIAG_AN_MODE_HPAM ;
      if(data16&AN_X4_ABILITIES_ENABLES_CL37_SGMII_ENABLE_MASK)
                                                          ws->accData |= TSCMOD_DIAG_AN_MODE_SGMII ;
   }

   return SOC_E_NONE;
}

int tscmod_diag_topology(tscmod_st *ws)
{
  return SOC_E_NONE;
}

STATIC int tscmod_diag_anapll_status(tscmod_st *ws) {
   uint16 data  ;
   int seq_d, freq_d, vco_d, seq_p, freq_p, vco_p, pll_mode_vec ;
   /* 0x9010 */
   SOC_IF_ERROR_RETURN (READ_WC40_ANAPLL_STATUSr(ws->unit, ws, &data));
   seq_d  =(data&ANAPLL_STATUS_PLLSEQDONE_MASK)>>ANAPLL_STATUS_PLLSEQDONE_SHIFT;
   freq_d =(data&ANAPLL_STATUS_FREQDONE_MASK)>>ANAPLL_STATUS_FREQDONE_SHIFT;
   vco_d  =(data&ANAPLL_STATUS_VCODONE_MASK)>>ANAPLL_STATUS_VCODONE_SHIFT;
   seq_p  =(data&ANAPLL_STATUS_PLLSEQPASS_MASK)>>ANAPLL_STATUS_PLLSEQPASS_SHIFT;
   freq_p =(data&ANAPLL_STATUS_FREQPASS_MASK)>>ANAPLL_STATUS_FREQPASS_SHIFT;
   vco_p  =(data&ANAPLL_STATUS_VCOPASS_MASK)>>ANAPLL_STATUS_VCOPASS_SHIFT;
   pll_mode_vec= (data&ANAPLL_STATUS_PLL_MODE_AFE_MASK)>>
      ANAPLL_STATUS_PLL_MODE_AFE_SHIFT;
   printf("  ANAPLL_STATUSr(%0x) d=%0x seq_done=%0x freq_done=%0x vco_done=%0x\n",
          ws->accAddr, data, seq_d, freq_d, vco_d);
   printf("      seq_pas=%0x freq_pas=%0x vco_pas=%0x pll_vec=%0x\n",
          seq_p, freq_p, vco_p, pll_mode_vec) ;

   return SOC_E_NONE ;
}

STATIC int tscmod_diag_rx_x4_status1_pma_pmd_live_status(tscmod_st *ws) {
   uint16 data ;
   int det, afe_det, pmd_lck, sig_ok;
   int pmd_lk_lh, pmd_lk_ll, ext_sig_det_lh, ext_sig_det_ll, afe_det_lh, afe_det_ll, ok_lh, ok_ll ;

   SOC_IF_ERROR_RETURN                   /* c150 */
      (READ_WC40_RX_X4_STATUS1_PMA_PMD_LIVE_STATUSr(ws->unit, ws, &data));
   det = (data&RX_X4_STATUS1_PMA_PMD_LIVE_STATUS_EXTERNAL_SIGNAL_DETECT_MASK)
            >> RX_X4_STATUS1_PMA_PMD_LIVE_STATUS_EXTERNAL_SIGNAL_DETECT_SHIFT;
   afe_det =(data & RX_X4_STATUS1_PMA_PMD_LIVE_STATUS_AFE_SIGNAL_DETECT_MASK)
                 >> RX_X4_STATUS1_PMA_PMD_LIVE_STATUS_AFE_SIGNAL_DETECT_SHIFT;
   sig_ok  =(data & RX_X4_STATUS1_PMA_PMD_LIVE_STATUS_SIGNAL_OK_MASK)
                 >> RX_X4_STATUS1_PMA_PMD_LIVE_STATUS_SIGNAL_OK_SHIFT ;
   pmd_lck =(data & RX_X4_STATUS1_PMA_PMD_LIVE_STATUS_PMD_LOCK_MASK)
      >>RX_X4_STATUS1_PMA_PMD_LIVE_STATUS_PMD_LOCK_SHIFT ;
   printf("  PMA_PMD_LIVE_STATUS (%0x) d=%0x ext_det=%0d afe_det=%0d\
 pmd_lck=%0d sig_ok=%0d\n", ws->accAddr, data, det, afe_det, pmd_lck, sig_ok) ;
   SOC_IF_ERROR_RETURN                  /* c151 */
      (READ_WC40_RX_X4_STATUS1_PMA_PMD_LATCHED_STATUSr(ws->unit, ws, &data));
   pmd_lk_lh = (data&RX_X4_STATUS1_PMA_PMD_LATCHED_STATUS_PMD_LOCK_LH_MASK)
      >> RX_X4_STATUS1_PMA_PMD_LATCHED_STATUS_PMD_LOCK_LH_SHIFT ;
   pmd_lk_ll = (data&RX_X4_STATUS1_PMA_PMD_LATCHED_STATUS_PMD_LOCK_LL_MASK)
      >> RX_X4_STATUS1_PMA_PMD_LATCHED_STATUS_PMD_LOCK_LL_SHIFT ;
   ext_sig_det_lh =
      (data&RX_X4_STATUS1_PMA_PMD_LATCHED_STATUS_EXTERNAL_SIGNAL_DETECT_LH_MASK)
      >>RX_X4_STATUS1_PMA_PMD_LATCHED_STATUS_EXTERNAL_SIGNAL_DETECT_LH_SHIFT ;
   ext_sig_det_ll =
      (data&RX_X4_STATUS1_PMA_PMD_LATCHED_STATUS_EXTERNAL_SIGNAL_DETECT_LL_MASK)
      >>RX_X4_STATUS1_PMA_PMD_LATCHED_STATUS_EXTERNAL_SIGNAL_DETECT_LL_SHIFT ;
   afe_det_lh     =
      (data&RX_X4_STATUS1_PMA_PMD_LATCHED_STATUS_AFE_SIGNAL_DETECT_LH_MASK)
      >>RX_X4_STATUS1_PMA_PMD_LATCHED_STATUS_AFE_SIGNAL_DETECT_LH_SHIFT ;
   afe_det_ll     =
       (data&RX_X4_STATUS1_PMA_PMD_LATCHED_STATUS_AFE_SIGNAL_DETECT_LL_MASK)
      >>RX_X4_STATUS1_PMA_PMD_LATCHED_STATUS_AFE_SIGNAL_DETECT_LL_SHIFT ;
   ok_lh          =
      (data&RX_X4_STATUS1_PMA_PMD_LATCHED_STATUS_SIGNAL_OK_LH_MASK)
      >>RX_X4_STATUS1_PMA_PMD_LATCHED_STATUS_SIGNAL_OK_LH_SHIFT ;
   ok_ll          =
      (data&RX_X4_STATUS1_PMA_PMD_LATCHED_STATUS_SIGNAL_OK_LL_MASK)
      >>RX_X4_STATUS1_PMA_PMD_LATCHED_STATUS_SIGNAL_OK_LL_SHIFT ;
   printf("  PMA_PMD_LATCHED_STATUS (%0x) d=%0x pmd_lk lh=%0d ll=%0d \
ext_sig_det lh=%0d ll=%0d afe_det lh=%0d ll=%0d ok lh=%0d ll=%0d\n",
          ws->accAddr, data,pmd_lk_lh, pmd_lk_ll,ext_sig_det_lh, ext_sig_det_ll,
          afe_det_lh, afe_det_ll, ok_lh, ok_ll) ;

   return SOC_E_NONE ;
}

STATIC int tscmod_diag_rx_x4_status1_pcs_live_status(tscmod_st *ws)
{
   uint16 data ;
   int lf, rf, li, lpi, am_lock, deskew, hi_ber, link_s, sync_s;
   SOC_IF_ERROR_RETURN                  /* c154 */
      (READ_WC40_RX_X4_STATUS1_PCS_LIVE_STATUSr(ws->unit, ws, &data));
   lf = (data & RX_X4_STATUS1_PCS_LIVE_STATUS_LOCAL_FAULT_MASK )
      >> RX_X4_STATUS1_PCS_LIVE_STATUS_LOCAL_FAULT_SHIFT ;
   rf = (data & RX_X4_STATUS1_PCS_LIVE_STATUS_REMOTE_FAULT_MASK )
      >> RX_X4_STATUS1_PCS_LIVE_STATUS_REMOTE_FAULT_SHIFT ;
   li = (data & RX_X4_STATUS1_PCS_LIVE_STATUS_LINK_INTERRUPT_MASK )
             >> RX_X4_STATUS1_PCS_LIVE_STATUS_LINK_INTERRUPT_SHIFT ;
   lpi =(data & RX_X4_STATUS1_PCS_LIVE_STATUS_LPI_RECEIVED_MASK)
             >> RX_X4_STATUS1_PCS_LIVE_STATUS_LPI_RECEIVED_SHIFT ;
   deskew =(data & RX_X4_STATUS1_PCS_LIVE_STATUS_DESKEW_STATUS_MASK)
                 >>RX_X4_STATUS1_PCS_LIVE_STATUS_DESKEW_STATUS_SHIFT ;
   hi_ber =(data & RX_X4_STATUS1_PCS_LIVE_STATUS_HI_BER_MASK)
                 >>RX_X4_STATUS1_PCS_LIVE_STATUS_HI_BER_SHIFT ;
   am_lock =(data & RX_X4_STATUS1_PCS_LIVE_STATUS_AM_LOCK_MASK)
      >> RX_X4_STATUS1_PCS_LIVE_STATUS_AM_LOCK_SHIFT ;
   link_s = (data & RX_X4_STATUS1_PCS_LIVE_STATUS_LINK_STATUS_MASK )
      >> RX_X4_STATUS1_PCS_LIVE_STATUS_LINK_STATUS_SHIFT ;
   sync_s = (data & RX_X4_STATUS1_PCS_LIVE_STATUS_SYNC_STATUS_MASK )
      >> RX_X4_STATUS1_PCS_LIVE_STATUS_SYNC_STATUS_SHIFT ;
   printf("  PCS_LIVE_STATUS (%0x) d=%0x LF=%0d RF=%0d LI=%0d LPI=%0d AM_LCK=%0d"
          " DESKEW=%0d HI_BER=%0d LINK=%0d SYNC=%0d\n",
          ws->accAddr, data, lf, rf, li, lpi, am_lock, deskew, hi_ber, link_s, sync_s);

   return SOC_E_NONE ;
}

int tscmod_diag_g_port(tscmod_st *ws, int lane) {
   uint16 data ;
   int plldiv, plldiv_vec, afe_sel, refclk_sel, s_seq, port_mode, single_port;

   int powerdn_tx, powerdn_rx ;

   int sync_ll, sync_lh, link_lh, link_ll, hi_ber_lh, hi_ber_ll, deskew_lh, deskew_ll, am_lock_lh, am_lock_ll ;

   printf("%-22s u=%0d p=%0d\n", FUNCTION_NAME(), ws->unit, ws->port) ;

   /* 0x9000 */
   SOC_IF_ERROR_RETURN (READ_WC40_MAIN0_SETUPr(ws->unit, ws, &data));

   afe_sel= (data & MAIN0_SETUP_FORCE_PLL_MODE_AFE_SEL_MASK) ? 1 : 0 ;
   plldiv_vec = ( data & MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_MASK ) >>
                MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_SHIFT ;
   switch(plldiv_vec) {
   case MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div32:   plldiv = 32 ;  break;
   case MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div36:   plldiv = 36 ;  break;
   case MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div40:   plldiv = 40 ;  break;
   case MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div42:   plldiv = 42 ;  break;
   case MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div48:   plldiv = 48 ;  break;
   case MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div50:   plldiv = 50 ;  break;
   case MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div52:   plldiv = 52 ;  break;
   case MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div54:   plldiv = 54 ;  break;
   case MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div60:   plldiv = 40 ;  break;
   case MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div64:   plldiv = 64 ;  break;
   case MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div66:   plldiv = 66 ;  break;
   case MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div68:   plldiv = 68 ;  break;
   case MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div70:   plldiv = 70 ;  break;
   case MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div80:   plldiv = 80 ;  break;
   case MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div92:   plldiv = 92 ;  break;
   case MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div100:  plldiv =100 ; break;
   default : plldiv = 40;
   }
   printf("  MAIN0_SETUPr(%0x) d=%0x afe_sel=%0d plldiv=%0d(vec=%0d)\n",
          ws->accAddr, data, afe_sel, plldiv, plldiv_vec) ;

   s_seq       = (data & MAIN0_SETUP_START_SEQUENCER_MASK) >>
      MAIN0_SETUP_START_SEQUENCER_SHIFT;
   port_mode   = (data & MAIN0_SETUP_PORT_MODE_SEL_MASK) >>
      MAIN0_SETUP_PORT_MODE_SEL_SHIFT ;
   single_port = (data & MAIN0_SETUP_SINGLE_PORT_MODE_MASK )>>
      MAIN0_SETUP_SINGLE_PORT_MODE_SHIFT ;
   refclk_sel  = (data & MAIN0_SETUP_REFCLK_SEL_MASK) >>
      MAIN0_SETUP_REFCLK_SEL_SHIFT ;
   printf("   s_seq=%0x port_mode=%0x single_port=%0x ref_clk=%0x\n",
          s_seq, port_mode, single_port, refclk_sel) ;
   /* 9001 */
   SOC_IF_ERROR_RETURN (READ_WC40_MAIN0_RESETr(ws->unit, ws, &data)) ;
   printf("  MAIN0_RESET(%0x) d=%0x rx_rst_ana=%0x tx_rst_ana=%0x\n", ws->accAddr, data,
          (data &MAIN0_RESET_RX_RESET_ANALOG_MASK)>> MAIN0_RESET_RX_RESET_ANALOG_SHIFT,
          (data &MAIN0_RESET_TX_RESET_ANALOG_MASK)>> MAIN0_RESET_TX_RESET_ANALOG_SHIFT) ;
   /* 0x9002 main0_power */
   SOC_IF_ERROR_RETURN (READ_WC40_MAIN0_POWERr(ws->unit, ws, &data)) ;
   powerdn_tx =(data&MAIN0_POWER_TX_PWRDWN_ANALOG_MASK) >>
                     MAIN0_POWER_TX_PWRDWN_ANALOG_SHIFT ;
   powerdn_rx =(data&MAIN0_POWER_RX_PWRDWN_ANALOG_MASK) >>
                     MAIN0_POWER_RX_PWRDWN_ANALOG_SHIFT ;
   printf("  MAIN0_POWER(%0x) d=%0x powerdn_tx=%0x powerdn_rx=%0x\n",
          ws->accAddr, data, powerdn_tx, powerdn_rx) ;

   /* 0x9010 */
   tscmod_diag_anapll_status(ws) ;

   /* link info */

   /* 0xc150 */
   tscmod_diag_rx_x4_status1_pma_pmd_live_status(ws) ;


   SOC_IF_ERROR_RETURN                   /* c152 */
      (READ_WC40_RX_X4_STATUS1_PCS_LATCHED_STATUS_1r(ws->unit, ws, &data));
   printf("  PCS_LATCHED_STATUS1 (%0x) d=%0x\n", ws->accAddr, data) ;
   SOC_IF_ERROR_RETURN                  /* c153 */
      (READ_WC40_RX_X4_STATUS1_PCS_LATCHED_STATUS_0r(ws->unit, ws, &data));
   am_lock_lh = (data&RX_X4_STATUS1_PCS_LATCHED_STATUS_0_AM_LOCK_LH_MASK)
      >>RX_X4_STATUS1_PCS_LATCHED_STATUS_0_AM_LOCK_LH_SHIFT ;
   am_lock_ll = (data&RX_X4_STATUS1_PCS_LATCHED_STATUS_0_AM_LOCK_LL_MASK)
      >>RX_X4_STATUS1_PCS_LATCHED_STATUS_0_AM_LOCK_LL_SHIFT ;
   deskew_lh = (data&RX_X4_STATUS1_PCS_LATCHED_STATUS_0_DESKEW_STATUS_LH_MASK)
                  >> RX_X4_STATUS1_PCS_LATCHED_STATUS_0_DESKEW_STATUS_LH_SHIFT;
   deskew_ll = (data&RX_X4_STATUS1_PCS_LATCHED_STATUS_0_DESKEW_STATUS_LL_MASK)
      >> RX_X4_STATUS1_PCS_LATCHED_STATUS_0_DESKEW_STATUS_LL_SHIFT;
   hi_ber_lh = (data&RX_X4_STATUS1_PCS_LATCHED_STATUS_0_HI_BER_LH_MASK)
      >> RX_X4_STATUS1_PCS_LATCHED_STATUS_0_HI_BER_LH_SHIFT;
   hi_ber_ll = (data&RX_X4_STATUS1_PCS_LATCHED_STATUS_0_HI_BER_LL_MASK)
      >> RX_X4_STATUS1_PCS_LATCHED_STATUS_0_HI_BER_LL_SHIFT;
   link_lh   = (data&RX_X4_STATUS1_PCS_LATCHED_STATUS_0_LINK_STATUS_LH_MASK)
      >> RX_X4_STATUS1_PCS_LATCHED_STATUS_0_LINK_STATUS_LH_SHIFT;
   link_ll   = (data&RX_X4_STATUS1_PCS_LATCHED_STATUS_0_LINK_STATUS_LL_MASK)
      >> RX_X4_STATUS1_PCS_LATCHED_STATUS_0_LINK_STATUS_LL_SHIFT;
   sync_lh   = (data&RX_X4_STATUS1_PCS_LATCHED_STATUS_0_SYNC_STATUS_LH_MASK)
      >> RX_X4_STATUS1_PCS_LATCHED_STATUS_0_SYNC_STATUS_LH_SHIFT;
   sync_ll   = (data&RX_X4_STATUS1_PCS_LATCHED_STATUS_0_SYNC_STATUS_LL_MASK)
      >> RX_X4_STATUS1_PCS_LATCHED_STATUS_0_SYNC_STATUS_LL_SHIFT;
   printf("  PCS_LATCHED_STATUS0 (%0x) d=%0x am_lock lh=%0d ll=%0d desk lh=%0d\
 ll=%0d hi_ber lh=%0d ll=%0d link lh=%0d ll=%0d sync lh=%0d ll=%0d\n",
       ws->accAddr, data, am_lock_lh,am_lock_ll,deskew_lh,deskew_ll,hi_ber_lh,
          hi_ber_ll, link_lh, link_ll, sync_lh, sync_ll) ;

   /* 0xc154 */
   tscmod_diag_rx_x4_status1_pcs_live_status(ws) ;

   return SOC_E_NONE ;
}

int tscmod_diag_g_speed(tscmod_st *ws, int lane)
{
   uint16 data ; int actual_spd ; int rx_lane_en, txp_reset, txp_enable;

   printf("%s u=%0d p=%0d",
          FUNCTION_NAME(), ws->unit, ws->port);
   printf("  spd_intf=%0d(%s) an=%0d(%s)\n", ws->spd_intf,
          e2s_tscmod_spd_intfc_type[ws->spd_intf],
          ws->an_type, e2s_tscmod_an_type[ws->an_type]);

   ws->this_lane = lane ;
   SOC_IF_ERROR_RETURN (READ_WC40_CL72_MISC2_CONTROLr(ws->unit, ws, &data));
   actual_spd = (data & CL72_MISC2_CONTROL_SW_ACTUAL_SPEED_MASK)
                      >>CL72_MISC2_CONTROL_SW_ACTUAL_SPEED_SHIFT ;
   printf("  CL72_MISC2_CONTROL(%0x) d=%0x actual_spd=%x %s\n",ws->accAddr,data,
          actual_spd, tscmod_aspd_vec2s(actual_spd));

   SOC_IF_ERROR_RETURN
      (READ_WC40_AN_X4_SW_MANAGEMENT_AN_ABILITY_RESOLUTION_STATUSr
       (ws->unit, ws, &data));
   actual_spd = ( data
       & AN_X4_SW_MANAGEMENT_AN_ABILITY_RESOLUTION_STATUS_AN_HCD_SPEED_MASK)
      >> AN_X4_SW_MANAGEMENT_AN_ABILITY_RESOLUTION_STATUS_AN_HCD_SPEED_SHIFT;
   printf("   AN_X4_SW_MANAGEMENT_AN_ABILITY_RESOLUTION_STATUSr(%0x) d=%0x "
          "spd=%x %s\n",
          ws->accAddr, data, actual_spd, tscmod_aspd_vec2s(actual_spd)) ;

   /* 0xc100 */
   SOC_IF_ERROR_RETURN (READ_WC40_TX_X4_CREDIT0_CREDIT0r(ws->unit, ws, &data));
   printf("  TX_X4_CREDIT0_CREDIT0r(%0x) d=%0x\n", ws->accAddr, data) ;
   /* 0xc101 */
   SOC_IF_ERROR_RETURN (READ_WC40_TX_X4_CREDIT0_CREDIT1r(ws->unit, ws, &data));
   printf("  TX_X4_CREDIT0_CREDIT1r(%0x) d=%0x\n", ws->accAddr, data) ;
   /* 0xc102 */
   SOC_IF_ERROR_RETURN(READ_WC40_TX_X4_CREDIT0_LOOPCNTr(ws->unit, ws, &data));
   printf("  TX_X4_CREDIT0_LOOPCNTr(%0x) d=%0x\n", ws->accAddr, data) ;
   /* 0xc103 */
   SOC_IF_ERROR_RETURN
      (READ_WC40_TX_X4_CREDIT0_MAC_CREDITGENCNTr(ws->unit, ws, &data));
	printf("  TX_X4_CREDIT0_MAC_CREDITGENCNTr(%0x) d=%0x\n", ws->accAddr, data) ;
   /* 0xc104 */
   SOC_IF_ERROR_RETURN
      (READ_WC40_TX_X4_CREDIT0_PCS_CLOCKCNT0r(ws->unit, ws, &data));
   printf("  TX_X4_CREDIT0_PCS_CLOCKCNT0r(%0x) d=%0x\n", ws->accAddr, data) ;
   /* 0xc105 */
   SOC_IF_ERROR_RETURN
      (READ_WC40_TX_X4_CREDIT0_PCS_CREDITGENCNTr(ws->unit, ws, &data));
   printf("  TX_X4_CREDIT0_PCS_CREDITGENCNTr(%0x) d=%0x\n", ws->accAddr, data) ;

   /* set encoder  0xc111 */
   SOC_IF_ERROR_RETURN(READ_WC40_TX_X4_CONTROL0_ENCODE_0r(ws->unit, ws, &data));
   printf("  TX_X4_CONTROL0_ENCODE_0r(%0x) d=%0x\n", ws->accAddr, data) ;
   /* 0xc113 */
   SOC_IF_ERROR_RETURN(READ_WC40_TX_X4_CONTROL0_MISCr(ws->unit, ws, &data));
   txp_reset  = (data&TX_X4_CONTROL0_MISC_RSTB_TX_LANE_MASK)?0:1 ;
   txp_enable = (data&TX_X4_CONTROL0_MISC_ENABLE_TX_LANE_MASK)?1:0 ;
   printf("  TX_X4_CONTROL0_MISCr(%0x) d=%0x  txp_reset=%0d txp_en=%0d\n", ws->accAddr, data, txp_reset, txp_enable) ;
   /* 0xc114 */
   SOC_IF_ERROR_RETURN(READ_WC40_TX_X4_CONTROL0_CL36_TX_0r(ws->unit, ws, &data));
   printf("  TX_X4_CONTROL0_CL36_TX_0r(%0x) d=%0x\n", ws->accAddr, data) ;
   /* 0xa000 */
   SOC_IF_ERROR_RETURN
      (READ_WC40_TX_X2_CONTROL0_MLD_SWAP_COUNTr(ws->unit, ws, &data));
   printf("  TX_X2_CONTROL0_MLD_SWAP_COUNTr(%0x) d=%0x\n", ws->accAddr, data) ;

   /* 0xc130 */
   SOC_IF_ERROR_RETURN
      (READ_WC40_RX_X4_CONTROL0_PCS_CONTROL_0r(ws->unit, ws, &data));
   printf("  RX_X4_CONTROL0_PCS_CONTROL_0r(%0x) d=%0x\n", ws->accAddr, data) ;
   /* 0xc131 */
   SOC_IF_ERROR_RETURN(READ_WC40_RX_X4_CONTROL0_FEC_0r(ws->unit, ws, &data));
   printf("  RX_X4_CONTROL0_FEC_0r(%0x) d=%0x\n", ws->accAddr, data) ;
   /* 0xc134 */
   SOC_IF_ERROR_RETURN
      (READ_WC40_RX_X4_CONTROL0_DECODE_CONTROL_0r(ws->unit, ws, &data));
   printf("  RX_X4_CONTROL0_DECODE_CONTROL_0r(%0x) d=%0x\n", ws->accAddr, data);
   /* 0xc136 */
   SOC_IF_ERROR_RETURN(READ_WC40_RX_X4_CONTROL0_CL36_RX_0r(ws->unit, ws,&data));
   printf("  RX_X4_CONTROL0_CL36_RX_0r(%0x) d=%0x\n", ws->accAddr, data) ;
   /* 0xc137 */
   SOC_IF_ERROR_RETURN
      (READ_WC40_RX_X4_CONTROL0_PMA_CONTROL_0r(ws->unit, ws, &data));
   rx_lane_en = (data&RX_X4_CONTROL0_PMA_CONTROL_0_RSTB_LANE_MASK)?1:0 ;
   printf("  RX_X4_CONTROL0_PMA_CONTROL_0r(%0x) d=%0x rx_lane_en=%0d\n", ws->accAddr, data, rx_lane_en) ;
   /* 0x9220 */
   SOC_IF_ERROR_RETURN
      (READ_WC40_RX_X1_CONTROL0_DECODE_CONTROL_0r(ws->unit, ws, &data));
   printf("  RX_X1_CONTROL0_DECODE_CONTROL_0r(%0x) d=%0x\n", ws->accAddr, data);
   /* 0x9222 */
   SOC_IF_ERROR_RETURN
      (READ_WC40_RX_X1_CONTROL0_DESKEW_WINDOWSr(ws->unit, ws, &data));
   printf("  RX_X1_CONTROL0_DESKEW_WINDOWSr(%0x) d=%0x\n", ws->accAddr, data) ;
   /* 0xa023 */
   SOC_IF_ERROR_RETURN(READ_WC40_RX_X2_CONTROL0_MISC_0r(ws->unit, ws, &data));
   printf("  RX_X2_CONTROL0_MISC_0r(%0x) d=%0x\n", ws->accAddr, data) ;
   /* 0x9123 */
   SOC_IF_ERROR_RETURN
      (READ_WC40_CL82_SHARED_CL82_RX_AM_TIMERr(ws->unit, ws, &data));
   printf("  CL82_SHARED_CL82_RX_AM_TIMERr(%0x) d=%0x\n", ws->accAddr, data) ;
   /* 0x9130 */
   SOC_IF_ERROR_RETURN
      (READ_WC40_CL82_AM_REGS_LANE_0_AM_BYTE10r(ws->unit, ws, &data));
   printf("  CL82_AM_REGS_LANE_0_AM_BYTE10r(%0x) d=%0x\n", ws->accAddr, data) ;
   /* 0x9131 */
   SOC_IF_ERROR_RETURN
      (READ_WC40_CL82_AM_REGS_LANE_1_AM_BYTE10r(ws->unit, ws, &data));
   printf("  CL82_AM_REGS_LANE_1_AM_BYTE10r(%0x) d=%0x\n", ws->accAddr, data) ;
   /* 0x9132 */
   SOC_IF_ERROR_RETURN
      (READ_WC40_CL82_AM_REGS_LANES_1_0_AM_BYTE2r(ws->unit, ws, &data));
   printf("  CL82_AM_REGS_LANES_1_0_AM_BYTE2r(%0x) d=%0x\n", ws->accAddr, data) ;

   /* 0xc017 */
   SOC_IF_ERROR_RETURN
      (READ_WC40_ANATX_TX_DRIVERr(ws->unit, ws, &data)) ;
   printf("  ANATX_TX_DRIVERr(%0x) d=%0x e_idle=%0x\n", ws->accAddr, data, ((data&ANATX_TX_DRIVER_ELEC_IDLE_MASK)?1:0));

   /* 0xc01a */
   SOC_IF_ERROR_RETURN
      (READ_WC40_ANATX_ACONTROL6r(ws->unit, ws, &data)) ;
   printf("  ANATX_ACONTROL6r(%0x) d=%0x dis_tx_r=%0x\n", ws->accAddr, data, ((data&ANATX_ACONTROL6_DISABLE_TX_R_MASK)?1:0));

   /* 0xc138 */
   SOC_IF_ERROR_RETURN
      (READ_WC40_RX_X4_CONTROL0_RX_SIGDETr(ws->unit, ws, &data));
   printf("  RX_X4_CONTROL0_RX_SIGDETr(%0x) d=%0x over_wr_sig_ok=%0d over_wr_val=%0d\n", ws->accAddr, data,
          ((data&RX_X4_CONTROL0_RX_SIGDET_OVERRIDE_SIGNAL_OK_MASK)?1:0),
          ((data&RX_X4_CONTROL0_RX_SIGDET_OVERRIDE_VALUE_SIGNAL_OK_MASK)?1:0));

   return SOC_E_NONE ;
}


int tscmod_diag_g_uc_ctrl_status(tscmod_st *ws, int lane) 
{
   uint16 data ;
   uint16 gen_status_data ;
   int ram_count ;
   int ram_addr ;
   int init_cmd, md_en_override, md_en_override_val, ram_cs_force_val, ram_cs_force;
   int autowakeup, byte_mode, reset_n, write, read, stop, run ;
   int wrdata ;
   int rddata ;
   int init_done, FSM, ERR1, ERR0 ;
   int SFR_FSM ;
   int gen_status ;
   int ram_access, ram_r_w, data_ram, register_ram, eitu_int5, eitu_int4, eitu_int3, eitu_int2, eitu_int1, eitu_int0 ;
   int uc_mailbox ;
   int mdio_mailbox ;
   int u8051_reset, pcb_reset, firmware_reset_dis, zero_rom_dataout, timer_mux_sel, wdog_disable;
   int refdiv4_tmr_en, refdiv2_tmr_en, wdog_en, tmr_en ;
   int wdog_evnt_cnt ;
   int gen_status_sel, disable_ecc, out_staging_flop_bypass, tm, pm_override_value;
   int enable_pm_override, in_staging_flop_bypass, ext_clock_enable ;
   int timer_lower16bits ;
   int timer_upper16bits ;


   printf("%-22s u=%0d p=%0d\n", FUNCTION_NAME(), ws->unit, ws->port) ;


   /* 0xFFC0 */
   SOC_IF_ERROR_RETURN (READ_WC40_UC_RAMWORDr(ws->unit, ws, &data));

   ram_count = data &  UC_RAMWORD_RAM_COUNT_MASK ;

   printf(" UC_RAMWORDr(%0x) d=%0x ram_count=%0d\n",
          ws->accAddr, data, ram_count) ;

   /* 0xFFC1 */
   SOC_IF_ERROR_RETURN (READ_WC40_UC_ADDRESSr(ws->unit, ws, &data));

   ram_addr = data &  UC_ADDRESS_RAM_ADDRESS_MASK ;

   printf(" UC_ADDRESSr(%0x) d=%0x ram_addr=%0d\n",
          ws->accAddr, data, ram_addr) ;


   /* 0xFFC2 */
   SOC_IF_ERROR_RETURN (READ_WC40_UC_COMMANDr (ws->unit, ws, &data));

   init_cmd= (data & UC_COMMAND_INIT_CMD_MASK) >>
             UC_COMMAND_INIT_CMD_SHIFT ;
   md_en_override= (data & UC_COMMAND_UC_MD_EN_OVERRIDE_MASK) >>
                   UC_COMMAND_UC_MD_EN_OVERRIDE_SHIFT ;
   md_en_override_val= (data & UC_COMMAND_UC_MD_EN_OVERRIDE_VALUE_MASK) >>
                       UC_COMMAND_UC_MD_EN_OVERRIDE_VALUE_SHIFT ;
   ram_cs_force_val= (data & UC_COMMAND_MDIO_RAM_CS_FORCE_VAL_MASK) >>
                     UC_COMMAND_MDIO_RAM_CS_FORCE_VAL_SHIFT;
   ram_cs_force= (data & UC_COMMAND_MDIO_RAM_CS_FORCE_MASK) >>
                 UC_COMMAND_MDIO_RAM_CS_FORCE_SHIFT ;
   autowakeup= (data & UC_COMMAND_MDIO_AUTOWAKEUP_MASK) >>
               UC_COMMAND_MDIO_AUTOWAKEUP_SHIFT ;
   byte_mode= (data & UC_COMMAND_BYTE_MODE_MASK) >>
              UC_COMMAND_BYTE_MODE_SHIFT ;
   reset_n= (data & UC_COMMAND_MDIO_UC_RESET_N_MASK) >>
               UC_COMMAND_MDIO_UC_RESET_N_SHIFT ;
   write= (data & UC_COMMAND_WRITE_MASK) >>
          UC_COMMAND_WRITE_SHIFT ;
   read= (data & UC_COMMAND_READ_MASK) >>
         UC_COMMAND_READ_SHIFT ;
   stop= (data & UC_COMMAND_STOP_MASK) >>
         UC_COMMAND_STOP_SHIFT ;
   run= (data & UC_COMMAND_RUN_MASK) >>
        UC_COMMAND_RUN_SHIFT ;
   printf(" MAIN0_SETUPr(%0x) d=%0x init_cmd=%0d md_en_override=%0d md_en_override_val=%0d  \n",
          ws->accAddr, data, init_cmd, md_en_override, md_en_override_val ) ;
   printf("  ram_cs_force=%0d ram_cs_f_val=%0d autowakeup=%0d  byte_mode=%0d reset_n=%0d \n",
          ram_cs_force, ram_cs_force_val, autowakeup, byte_mode, reset_n ) ;
   printf(" write=%0d read=%0d  stop=%0d run=%0d \n",
          write, read, stop, run ) ;



   /* 0xFFC3 */
   SOC_IF_ERROR_RETURN (READ_WC40_UC_WRDATAr(ws->unit, ws, &data));

   wrdata= data & UC_WRDATA_WRDATA_MASK ;

   printf(" UC_WRDATAr(%0x) d=%0x wrdata=%0d\n",
          ws->accAddr, data, wrdata ) ;



   /* 0xFFC4 */
   SOC_IF_ERROR_RETURN (READ_WC40_UC_RDDATAr(ws->unit, ws, &data));

   rddata= data & UC_RDDATA_RDDATA_MASK ;

   printf(" UC_RDDATAr(%0x) d=%0x rddata=%0d\n",
          ws->accAddr, data, rddata ) ;



   /* 0xFFC5 */
   SOC_IF_ERROR_RETURN (READ_WC40_UC_DOWNLOAD_STATUSr(ws->unit, ws, &data));

   init_done= (data & UC_DOWNLOAD_STATUS_INIT_DONE_MASK)  >>
              UC_DOWNLOAD_STATUS_INIT_DONE_SHIFT ;
   FSM= (data & UC_DOWNLOAD_STATUS_FSM_MASK) >>
        UC_DOWNLOAD_STATUS_FSM_SHIFT ;
   ERR1= (data & UC_DOWNLOAD_STATUS_ERR1_MASK) >>
         UC_DOWNLOAD_STATUS_ERR1_SHIFT ;
   ERR0= (data & UC_DOWNLOAD_STATUS_ERR0_MASK) >>
         UC_DOWNLOAD_STATUS_ERR0_SHIFT ;

   printf(" UC_DOWNLOAD_STATUSr(%0x) d=%0x init_done=%0d FSM=%0d ERR1=%0d ERR0=%0d\n",
          ws->accAddr, data, init_done, FSM, ERR1 , ERR0 ) ;



   /* 0xFFC6 */

   /*  Read 0xffcc register  to get gen_status_sel */
   SOC_IF_ERROR_RETURN ( READ_WC40_UC_COMMAND3r(ws->unit,ws,&gen_status_data));
   gen_status= (gen_status_data & UC_COMMAND3_GEN_STATUS_SEL_MASK) >>
               UC_COMMAND3_GEN_STATUS_SEL_SHIFT ;

   SOC_IF_ERROR_RETURN (READ_WC40_UC_SFR_STATUSr(ws->unit, ws, &data));
   switch(gen_status) {
   case  0: SFR_FSM = (data & UC_SFR_STATUS_ECC_ERROR_COUNTERS_ECC_ERROR_COUNTERS_MASK )   ; break;
   case  1: SFR_FSM = (data & UC_SFR_STATUS_DW8051_TO_PCB_FSM_L_DW8051_TO_PCB_FSM_L_MASK ) ; break;
   case  2: SFR_FSM = (data & UC_SFR_STATUS_DW8051_TO_PCB_FSM_U_DW8051_TO_PCB_FSM_U_MASK ) ; break;
   default: SFR_FSM = 0 ;
   }
   printf(" UC_SFR_STATUSr(%0x) d=%0x gen_status_sel=%0d SFR_FSM=%0d\n",
          ws->accAddr, data, gen_status, SFR_FSM ) ;


   /* 0xFFC7 */


   SOC_IF_ERROR_RETURN ( READ_WC40_UC_COMMAND4r(ws->unit,ws,&data));

   ram_access= (data & UC_COMMAND4_DATA_RAM_ACCESS_MASK) >>
               UC_COMMAND4_DATA_RAM_ACCESS_SHIFT ;
   ram_r_w= (data & UC_COMMAND4_DATA_RAM_R_W_MASK) >>
            UC_COMMAND4_DATA_RAM_R_W_SHIFT ;
   data_ram= (data & UC_COMMAND4_TM_DATA_RAM_MASK) >>
             UC_COMMAND4_TM_DATA_RAM_SHIFT ;
   register_ram= (data & UC_COMMAND4_TM_REGISTER_RAM_MASK) >>
                 UC_COMMAND4_TM_REGISTER_RAM_SHIFT ;
   eitu_int5= (data & UC_COMMAND4_MDIO_EITU_INT5_MASK) >>
              UC_COMMAND4_MDIO_EITU_INT5_SHIFT ;
   eitu_int4= (data & UC_COMMAND4_MDIO_EITU_INT4_MASK) >>
              UC_COMMAND4_MDIO_EITU_INT4_SHIFT ;
   eitu_int3= (data & UC_COMMAND4_MDIO_EITU_INT3_MASK) >>
              UC_COMMAND4_MDIO_EITU_INT3_SHIFT ;
   eitu_int2= (data & UC_COMMAND4_MDIO_EITU_INT2_MASK) >>
              UC_COMMAND4_MDIO_EITU_INT2_SHIFT ;
   eitu_int1= (data & UC_COMMAND4_MDIO_EITU_INT1_MASK) >>
              UC_COMMAND4_MDIO_EITU_INT1_SHIFT ;
   eitu_int0= (data & UC_COMMAND4_MDIO_EITU_INT0_MASK) >>
              UC_COMMAND4_MDIO_EITU_INT0_SHIFT ;

   printf(" UC_COMMAND4r(%0x) d=%0x ram_access=%0d ram_r_w=%0d data_ram=%0d\n",
          ws->accAddr, data, ram_access, ram_r_w, data_ram ) ;
   printf(" register_ram=%0d eitu_int5=%0d eitu_int4=%0d eitu_int3=%0d eitu_int2=%0d\n",
          register_ram, eitu_int5, eitu_int4, eitu_int3, eitu_int2 ) ;
   printf(" eitu_int1=%0d eitu_int0=%0d\n",
          eitu_int1, eitu_int0 ) ;



   /* 0xFFC8 */

   SOC_IF_ERROR_RETURN ( READ_WC40_UC_MDIO_UC_MAILBOXr(ws->unit,ws,&data));

   uc_mailbox= data & UC_MDIO_UC_MAILBOX_MDIO_UC_MAILBOX_SHIFT ;

   printf(" UC_MDIO_UC_MAILBOXr(%0x) d=%0x uc_mailbox=%0d\n",
          ws->accAddr, data, uc_mailbox ) ;



   /* 0xFFC9 */

   SOC_IF_ERROR_RETURN ( READ_WC40_UC_UC_MDIO_MAILBOXr(ws->unit,ws,&data));

   mdio_mailbox= data & UC_UC_MDIO_MAILBOX_UC_MDIO_MAILBOX_MASK ;

   printf(" UC_UC_MDIO_MAILBOXr(%0x) d=%0x mdio_mailbox=%0d\n",
          ws->accAddr, data, mdio_mailbox ) ;



   /* 0xFFCA */

   SOC_IF_ERROR_RETURN ( READ_WC40_UC_COMMAND2r(ws->unit,ws,&data));

   u8051_reset= (data & UC_COMMAND2_MDIO_TO_8051_RESET_MASK) >>
               UC_COMMAND2_MDIO_TO_8051_RESET_SHIFT ;
   pcb_reset= (data & UC_COMMAND2_M8051_TO_PCB_RESET_MASK) >>
              UC_COMMAND2_M8051_TO_PCB_RESET_SHIFT ;
   firmware_reset_dis= (data & UC_COMMAND2_FIRMWARE_RESET_DISABLE_MASK) >>
                       UC_COMMAND2_FIRMWARE_RESET_DISABLE_SHIFT ;
   zero_rom_dataout= (data & UC_COMMAND2_ZERO_ROM_DATAOUT_MASK) >>
                     UC_COMMAND2_ZERO_ROM_DATAOUT_SHIFT ;
   timer_mux_sel= (data & UC_COMMAND2_TIMER_MUX_SEL_MASK) >>
                  UC_COMMAND2_TIMER_MUX_SEL_SHIFT ;
   wdog_disable= (data & UC_COMMAND2_WDOG_DISABLE_MASK) >>
                 UC_COMMAND2_WDOG_DISABLE_SHIFT ;
   refdiv4_tmr_en= (data & UC_COMMAND2_REFDIV4_TMR_EN_MASK) >>
                   UC_COMMAND2_REFDIV4_TMR_EN_SHIFT ;
   refdiv2_tmr_en= (data & UC_COMMAND2_REFDIV2_TMR_EN_MASK) >>
                   UC_COMMAND2_REFDIV2_TMR_EN_SHIFT ;
   wdog_en= (data & UC_COMMAND2_WDOG_EN_MASK) >>
            UC_COMMAND2_WDOG_EN_SHIFT ;
   tmr_en= (data & UC_COMMAND2_TMR_EN_MASK) >>
           UC_COMMAND2_TMR_EN_SHIFT ;

   printf(" UC_COMMAND2r(%0x) d=%0x u8051_reset=%0d pcb_reset=%0d firmware_reset_dis=%0d\n",
          ws->accAddr, data, u8051_reset, pcb_reset, firmware_reset_dis ) ;
   printf(" zero_rom_dataout=%0d timer_mux_sel=%0d wdog_disable=%0d refdiv4_tmr_en=%0d\n",
          zero_rom_dataout, timer_mux_sel, wdog_disable, refdiv4_tmr_en ) ;
   printf(" refdiv2_tmr_en=%0d wdog_en=%0d tmr_en=%0d\n",
          refdiv2_tmr_en, wdog_en, tmr_en ) ;



   /* 0xFFCB */

   SOC_IF_ERROR_RETURN ( READ_WC40_UC_WDOG_EVNT_CNTr(ws->unit,ws,&data));

   wdog_evnt_cnt= data & UC_WDOG_EVNT_CNT_WDOG_EVNT_CNT_MASK ;

   printf(" UC_WDOG_EVNT_CNTr(%0x) d=%0x wdog_evnt_cnt=%0d\n",
          ws->accAddr, data, wdog_evnt_cnt ) ;


   /* 0xFFCC */
   SOC_IF_ERROR_RETURN ( READ_WC40_UC_COMMAND3r(ws->unit,ws,&data));
   gen_status_sel= (data & UC_COMMAND3_GEN_STATUS_SEL_MASK) >>
               UC_COMMAND3_GEN_STATUS_SEL_SHIFT ;
   disable_ecc= (data & UC_COMMAND3_DISABLE_ECC_MASK) >>
                UC_COMMAND3_DISABLE_ECC_SHIFT ;
   out_staging_flop_bypass= (data & UC_COMMAND3_OUT_STAGING_FLOP_BYPASS_MASK) >>
                            UC_COMMAND3_OUT_STAGING_FLOP_BYPASS_SHIFT ;
   tm= (data & UC_COMMAND3_TM_MASK) >>
       UC_COMMAND3_TM_SHIFT ;
   pm_override_value= (data & UC_COMMAND3_PM_OVERRIDE_VALUE_MASK) >>
                      UC_COMMAND3_PM_OVERRIDE_VALUE_SHIFT ;
   enable_pm_override= (data & UC_COMMAND3_ENABLE_PM_OVERRIDE_MASK) >>
                       UC_COMMAND3_ENABLE_PM_OVERRIDE_SHIFT ;
   in_staging_flop_bypass= (data & UC_COMMAND3_IN_STAGING_FLOP_BYPASS_MASK) >>
                           UC_COMMAND3_IN_STAGING_FLOP_BYPASS_SHIFT ;
   ext_clock_enable= (data & UC_COMMAND3_EXT_CLK_ENABLE_MASK) >>
                     UC_COMMAND3_EXT_CLK_ENABLE_SHIFT ;

   printf(" UC_COMMAND3r(%0x) d=%0x gen_status_sel=%0d disable_ecc=%0d\n",
          ws->accAddr, data, gen_status_sel, disable_ecc ) ;
   printf(" out_staging_flop_bypass=%0d tm=%0d pm_override_value=%0d enable_pm_override=%0d\n",
          out_staging_flop_bypass, tm, pm_override_value, enable_pm_override ) ;
   printf(" in_staging_flop_bypass=%0d ext_clock_enable=%0d\n",
          in_staging_flop_bypass, ext_clock_enable) ;


   /* 0xFFCD */
   SOC_IF_ERROR_RETURN ( READ_WC40_UC_TIMER_LOWER16r(ws->unit,ws,&data));

   timer_lower16bits= data & UC_TIMER_LOWER16_TIMER_LOWER16BITS_MASK ;

   printf(" UC_TIMER_LOWER16r(%0x) d=%0x timer_lower16bits=%0d\n",
          ws->accAddr, data, timer_lower16bits ) ;

   /* 0xFFCE */
   SOC_IF_ERROR_RETURN ( READ_WC40_UC_TIMER_UPPER16r(ws->unit,ws,&data));

   timer_upper16bits= data & UC_TIMER_UPPER16_TIMER_UPPER16BITS_MASK ;

   printf(" UC_TIMER_UPPER16r(%0x) d=%0x timer_upper16bits=%0d\n",
          ws->accAddr, data, timer_upper16bits ) ;

   return SOC_E_NONE ;
}


int tscmod_diag_g_uc_info(tscmod_st *ws, int lane) {

   uint16 data ;
   int info_b1_ver ;
   int target ;
   int bp_steady_state_tuning, bp_cdr_lock_monitor, bp_dcd_tuning, bp_slicer_offset_tuning, bp_slicer_target_tuning;
   int bp_vga_bias_tuning, bp_pf_tuning, bp_clk90_tuning, bp_os_vga_tuning ;
   int force_temp, temp_idx, temp ;
   int rep_temp_dec, rep_temp_frac ;
   int crc ;
   int timer_mux_sel, timer_en ;

   printf("%-22s u=%0d p=%0d\n", FUNCTION_NAME(), ws->unit, ws->port) ;

   /* 0xF010 */
   SOC_IF_ERROR_RETURN ( READ_WC40_UC_INFO_B1_VERSIONr(ws->unit,ws,&data));
   info_b1_ver= data & UC_INFO_B1_VERSION_VERSION_MASK ;
   printf(" UC_INFO_B1_VERSIONr(%0x) d=%0x info_b1_ver=%0d\n",
          ws->accAddr, data, info_b1_ver ) ;

   /* 0xF011 */
   SOC_IF_ERROR_RETURN ( READ_WC40_UC_INFO_B1_TARGETr(ws->unit,ws,&data));
   target= data & UC_INFO_B1_TARGET_TARGET_MASK ;
   printf(" UC_INFO_B1_TARGETr(%0x) d=%0x target=%0d\n",
          ws->accAddr, data, target ) ;

   /* 0xF013 */
   SOC_IF_ERROR_RETURN ( READ_WC40_UC_INFO_B1_TUNING_STATE_BYPASSr(ws->unit,ws,&data));

   bp_steady_state_tuning= (data & UC_INFO_B1_TUNING_STATE_BYPASS_BYPASS_STEADY_STATE_TUNING_MASK) >>
                           UC_INFO_B1_TUNING_STATE_BYPASS_BYPASS_STEADY_STATE_TUNING_SHIFT ;
   bp_cdr_lock_monitor= (data & UC_INFO_B1_TUNING_STATE_BYPASS_BYPASS_CDR_LOCK_MONITOR_MASK) >>
                         UC_INFO_B1_TUNING_STATE_BYPASS_BYPASS_CDR_LOCK_MONITOR_SHIFT ;
   bp_dcd_tuning= (data & UC_INFO_B1_TUNING_STATE_BYPASS_BYPASS_DCD_TUNING_MASK) >>
                  UC_INFO_B1_TUNING_STATE_BYPASS_BYPASS_DCD_TUNING_SHIFT ;
   bp_slicer_offset_tuning= (data & UC_INFO_B1_TUNING_STATE_BYPASS_BYPASS_SLICER_OFFSET_TUNING_MASK) >>
                            UC_INFO_B1_TUNING_STATE_BYPASS_BYPASS_SLICER_OFFSET_TUNING_SHIFT ;
   bp_slicer_target_tuning= (data & UC_INFO_B1_TUNING_STATE_BYPASS_BYPASS_SLICER_TARGET_TUNING_MASK) >>
                            UC_INFO_B1_TUNING_STATE_BYPASS_BYPASS_SLICER_TARGET_TUNING_SHIFT ;
   bp_vga_bias_tuning= (data & UC_INFO_B1_TUNING_STATE_BYPASS_BYPASS_VGA_BIAS_TUNING_MASK) >>
                       UC_INFO_B1_TUNING_STATE_BYPASS_BYPASS_VGA_BIAS_TUNING_SHIFT ;
   bp_pf_tuning= (data & UC_INFO_B1_TUNING_STATE_BYPASS_BYPASS_PF_TUNING_MASK) >>
                 UC_INFO_B1_TUNING_STATE_BYPASS_BYPASS_PF_TUNING_SHIFT ;
   bp_clk90_tuning= (data & UC_INFO_B1_TUNING_STATE_BYPASS_BYPASS_CLK90_TUNING_MASK) >>
                    UC_INFO_B1_TUNING_STATE_BYPASS_BYPASS_CLK90_TUNING_SHIFT ;
   bp_os_vga_tuning= (data & UC_INFO_B1_TUNING_STATE_BYPASS_BYPASS_OS_VGA_TUNING_MASK) >>
                     UC_INFO_B1_TUNING_STATE_BYPASS_BYPASS_OS_VGA_TUNING_SHIFT ;

   printf(" UC_INFO_B1_TUNING_STATE_BYPASSr(%0x) d=%0x bp_steady_state_tuning=%0d bp_cdr_lock_monitor=%d\n",
          ws->accAddr, data, bp_steady_state_tuning, bp_cdr_lock_monitor ) ;
   printf(" bp_dcd_tuning=%0d bp_slicer_offset_tuning=%0d bp_slicer_target_tuning=%0d bp_vga_bias_tuning=%0d\n",
          bp_dcd_tuning, bp_slicer_offset_tuning, bp_slicer_target_tuning, bp_vga_bias_tuning ) ;
   printf(" bp_pf_tuning=%0d bp_clk90_tuning=%0d bp_os_vga_tuning=%0d\n",
          bp_pf_tuning, bp_clk90_tuning, bp_os_vga_tuning ) ;



   /* 0xF014 */

   SOC_IF_ERROR_RETURN ( READ_WC40_UC_INFO_B1_TEMPERATUREr(ws->unit,ws,&data));

   force_temp= (data & UC_INFO_B1_TEMPERATURE_FORCE_TEMPERATURE_MASK) >>
                           UC_INFO_B1_TEMPERATURE_FORCE_TEMPERATURE_SHIFT ;

   temp_idx = (data & UC_INFO_B1_TEMPERATURE_TEMP_IDX_MASK ) >>
              UC_INFO_B1_TEMPERATURE_TEMP_IDX_SHIFT ;
   switch(temp_idx) {
   case UC_INFO_B1_TEMPERATURE_TEMP_IDX_LTE__22p9C:   {rep_temp_dec = -22 ; rep_temp_frac=9 ; }  break;
   case UC_INFO_B1_TEMPERATURE_TEMP_IDX_LTE__12p6C:   {rep_temp_dec = -12 ; rep_temp_frac=6 ; }  break;
   case UC_INFO_B1_TEMPERATURE_TEMP_IDX_LTE__3p0C :   {rep_temp_dec =  -3 ; rep_temp_frac=0 ; }  break;
   case UC_INFO_B1_TEMPERATURE_TEMP_IDX_LTE_6p7C  :   {rep_temp_dec =   6 ; rep_temp_frac=7 ; }  break;
   case UC_INFO_B1_TEMPERATURE_TEMP_IDX_LTE_16p4C :   {rep_temp_dec =  16 ; rep_temp_frac=4 ; }  break;
   case UC_INFO_B1_TEMPERATURE_TEMP_IDX_LTE_26p6C :   {rep_temp_dec =  26 ; rep_temp_frac=6 ; }  break;
   case UC_INFO_B1_TEMPERATURE_TEMP_IDX_LTE_36p3C :   {rep_temp_dec =  36 ; rep_temp_frac=3 ; }  break;
   case UC_INFO_B1_TEMPERATURE_TEMP_IDX_LTE_46p0C :   {rep_temp_dec =  46 ; rep_temp_frac=0 ; }  break;
   case UC_INFO_B1_TEMPERATURE_TEMP_IDX_LTE_56p2C :   {rep_temp_dec =  56 ; rep_temp_frac=2 ; }  break;
   case UC_INFO_B1_TEMPERATURE_TEMP_IDX_LTE_65p9C :   {rep_temp_dec =  65 ; rep_temp_frac=9 ; }  break;
   case UC_INFO_B1_TEMPERATURE_TEMP_IDX_LTE_75p6C :   {rep_temp_dec =  75 ; rep_temp_frac=6 ; }  break;
   case UC_INFO_B1_TEMPERATURE_TEMP_IDX_LTE_85p3C :   {rep_temp_dec =  85 ; rep_temp_frac=3 ; }  break;
   case UC_INFO_B1_TEMPERATURE_TEMP_IDX_LTE_95p5C :   {rep_temp_dec =  95 ; rep_temp_frac=5 ; }  break;
   case UC_INFO_B1_TEMPERATURE_TEMP_IDX_LTE_105p2C:   {rep_temp_dec = 105 ; rep_temp_frac=2 ; }  break;
   case UC_INFO_B1_TEMPERATURE_TEMP_IDX_LTE_114p9C:   {rep_temp_dec = 114 ; rep_temp_frac=9 ; }  break;
   case UC_INFO_B1_TEMPERATURE_TEMP_IDX_LTE_125p1C:   {rep_temp_dec = 125 ; rep_temp_frac=1 ; }  break;
   default : {rep_temp_dec = 125 ; rep_temp_frac=1 ; }
   }
   temp = data & UC_INFO_B1_TEMPERATURE_TEMPERATURE_MASK ;

   printf(" UC_INFO_B1_TEMPERATUREr(%0x) d=%0x force_temp=%0d temp_idx=%d\n",
          ws->accAddr, data, force_temp, temp_idx ) ;
   printf(" rep_temp=%0d.%0d current_die_temp=%0d\n",
          rep_temp_dec, rep_temp_frac, temp ) ;

   /* 0xF015 */
   SOC_IF_ERROR_RETURN ( READ_WC40_UC_INFO_B1_CRCr(ws->unit,ws,&data));

   crc= data & UC_INFO_B1_CRC_CRC_MASK ;

   printf(" UC_INFO_B1_CRCr((%0x) d=%0x crc=%d\n",
          ws->accAddr, data, crc ) ;

   /* 0xF016 */
   SOC_IF_ERROR_RETURN ( READ_WC40_UC_INFO_B1_TSC_12_TIMER_CONTROLr(ws->unit,ws,&data));

   timer_mux_sel= (data & UC_INFO_B1_TSC_12_TIMER_CONTROL_TIMER_MUX_SEL_MASK ) >>
                  UC_INFO_B1_TSC_12_TIMER_CONTROL_TIMER_MUX_SEL_SHIFT ;
   timer_en= data & UC_INFO_B1_TSC_12_TIMER_CONTROL_TIMER_ENABLE_MASK;
   printf(" UC_INFO_B1_TSC_12_TIMER_CONTROLr(((%0x) d=%0x timer_mux_sel=%d timer_en=%d\n",
          ws->accAddr, data, timer_mux_sel, timer_en  ) ;

   return SOC_E_NONE ;
}

int tscmod_diag_g_ucode(tscmod_st *ws, int lane)
{
   tscmod_diag_g_uc_ctrl_status(ws, lane) ;
   tscmod_diag_g_uc_info(ws, lane) ;

   return SOC_E_NONE ;
}

int tscmod_diag_rd_dsc_misc_ctrl0r(tscmod_st *ws) {
   uint16 data ;
   int rxSeqStart, forceRxSeqDone, forceRxSeqDone_val, rxSeqStart_AN_disable ;
   int test_bus_sel_bit4, cdrbr_sel_force, cdrbr_sel_force_val, osr_mode_force ;
   int osr_mode_force_val, test_bus_sel ;

   /* 0xC21E */

   SOC_IF_ERROR_RETURN ( READ_WC40_DSC2B0_DSC_MISC_CTRL0r(ws->unit,ws,&data));

   rxSeqStart= (data & DSC2B0_DSC_MISC_CTRL0_RXSEQSTART_MASK) >>
               DSC2B0_DSC_MISC_CTRL0_RXSEQSTART_SHIFT ;
   forceRxSeqDone= (data & DSC2B0_DSC_MISC_CTRL0_FORCERXSEQDONE_MASK) >>
                   DSC2B0_DSC_MISC_CTRL0_FORCERXSEQDONE_SHIFT ;
   forceRxSeqDone_val= (data & DSC2B0_DSC_MISC_CTRL0_FORCERXSEQDONE_VAL_MASK) >>
                       DSC2B0_DSC_MISC_CTRL0_FORCERXSEQDONE_VAL_SHIFT ;
   rxSeqStart_AN_disable= (data & DSC2B0_DSC_MISC_CTRL0_RXSEQSTART_AN_DISABLE_MASK) >>
                          DSC2B0_DSC_MISC_CTRL0_RXSEQSTART_AN_DISABLE_SHIFT ;
   test_bus_sel_bit4= (data & DSC2B0_DSC_MISC_CTRL0_TEST_BUS_SEL_BIT4_MASK) >>
                      DSC2B0_DSC_MISC_CTRL0_TEST_BUS_SEL_BIT4_SHIFT ;
   cdrbr_sel_force= (data & DSC2B0_DSC_MISC_CTRL0_CDRBR_SEL_FORCE_MASK) >>
                    DSC2B0_DSC_MISC_CTRL0_CDRBR_SEL_FORCE_SHIFT ;
   cdrbr_sel_force_val= (data & DSC2B0_DSC_MISC_CTRL0_CDRBR_SEL_FORCE_VAL_MASK) >>
                        DSC2B0_DSC_MISC_CTRL0_CDRBR_SEL_FORCE_VAL_SHIFT ;
   osr_mode_force= (data & DSC2B0_DSC_MISC_CTRL0_OSR_MODE_FORCE_MASK) >>
                   DSC2B0_DSC_MISC_CTRL0_OSR_MODE_FORCE_SHIFT ;
   osr_mode_force_val= (data & DSC2B0_DSC_MISC_CTRL0_OSR_MODE_FORCE_VAL_MASK) >>
                       DSC2B0_DSC_MISC_CTRL0_OSR_MODE_FORCE_VAL_SHIFT ;
   test_bus_sel= data & DSC2B0_DSC_MISC_CTRL0_TEST_BUS_SEL_MASK ;

   printf(" DSC2B0_DSC_MISC_CTRL0r(%0x) d=%0x rxSeqStart=%0x forceRxSeqDone=%0x\n",
          ws->accAddr, data, rxSeqStart, forceRxSeqDone ) ;
   printf("   forceRxSeqDone_val=%0x rxSeqStart_AN_disable=%0x test_bus_sel_bit4=%0x\n",
          forceRxSeqDone_val, rxSeqStart_AN_disable, test_bus_sel_bit4 ) ;
   printf("   cdrbr_sel_force=%0x cdrbr_sel_force_val=%0x osr_mode_force=%0x\n",
          cdrbr_sel_force, cdrbr_sel_force_val, osr_mode_force ) ;
   printf("   osr_mode_force_val=%0x test_bus_sel=%0x\n",
          osr_mode_force_val, test_bus_sel ) ;

   return SOC_E_NONE ;
}

int tscmod_diag_g_an_deep(tscmod_st *ws, int lane){
   uint16 data ;

   /* lane swap 0x9003 */
   SOC_IF_ERROR_RETURN
      (READ_WC40_MAIN0_LANE_SWAPr(ws->unit, ws, &data)) ;
   printf("  MAIN0_LANE_SWAPr(%0x) d=%0x rx[15:8]_tx[7:0]=%x\n",
          ws->accAddr, data, data) ;

   /* 0xc260 */
   SOC_IF_ERROR_RETURN
      (READ_WC40_UCSS_X4_FIRMWARE_MODEr(ws->unit, ws, &data));
   printf(" UCSS_X4_FIRMWARE_MODEr(%0x) d=%0x \n", ws->accAddr, data) ;

   tscmod_diag_g_speed(ws, lane) ;

   /* 0xc252 */
   SOC_IF_ERROR_RETURN
      (READ_WC40_CL72_TX_FIR_TAP_REGISTERr(ws->unit, ws, &data));
   printf(" CL72_TX_FIR_TAP_REGISTERr(%0x) d=%0x \n", ws->accAddr, data) ;

   /* 0xc253 */
   SOC_IF_ERROR_RETURN
      (READ_WC40_CL72_MISC1_CONTROLr(ws->unit, ws, &data));
   printf(" READ_WC40_CL72_MISC1_CONTROLr(%0x) d=%0x \n", ws->accAddr, data) ;

   /* 0x925a */
   SOC_IF_ERROR_RETURN
      (READ_WC40_AN_X1_TIMERS_CL72_MAX_WAIT_TIMERr(ws->unit, ws, &data));
   printf(" AN_X1_TIMERS_CL72_MAX_WAIT_TIMERr(%0x) d=%0x \n", ws->accAddr, data) ;

   tscmod_diag_rd_dsc_misc_ctrl0r(ws) ;

   /* 0xa000 */
   SOC_IF_ERROR_RETURN
      (READ_WC40_TX_X2_CONTROL0_MLD_SWAP_COUNTr(ws->unit, ws, &data));
   printf(" TX_X2_CONTROL0_MLD_SWAP_COUNTr(%0x) d=%0x \n", ws->accAddr, data) ;

   /* 0x9248 */
   SOC_IF_ERROR_RETURN
      (READ_WC40_AN_X1_CONTROL_CONFIG_CONTROLr(ws->unit, ws, &data));
   printf(" AN_X1_CONTROL_CONFIG_CONTROLr(%0x) d=%0x \n", ws->accAddr, data) ;

   /* 0x924a */
   SOC_IF_ERROR_RETURN
      (TSC_REG_READ(ws->unit, ws, 0x00, 0x0000924a, &data)) ;
   printf(" Addr(%0x) d=%0x \n", ws->accAddr, data) ;

   /* 0x9249 */
   SOC_IF_ERROR_RETURN
      (READ_WC40_AN_X1_CONTROL_PLL_RESET_TIMERr(ws->unit, ws, &data));
   printf(" AN_X1_CONTROL_PLL_RESET_TIMERr(%0x) d=%0x \n", ws->accAddr, data) ;

   /* 0x9254 */
   SOC_IF_ERROR_RETURN
      (READ_WC40_AN_X1_TIMERS_CL73_ERRORr(ws->unit, ws, &data));
   printf(" AN_X1_TIMERS_CL73_ERRORr(%0x) d=%0x \n", ws->accAddr, data) ;


   /* set cl73_bam_code set 0xc187 */
   SOC_IF_ERROR_RETURN
      (READ_WC40_AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BAM_ABILITIESr(ws->unit, ws, &data));
   printf(" AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BAM_ABILITIESr(%0x) d=%0x \n", ws->accAddr, data) ;

   /* 0xc188 */
   SOC_IF_ERROR_RETURN
      (READ_WC40_AN_X4_ABILITIES_CONTROLSr(ws->unit, ws, &data));
   printf(" AN_X4_ABILITIES_CONTROLS(%0x) d=%0x pd_1g_kx=%0d pd_10g_kx4=%0d\n", ws->accAddr, data,
          (data>>1)&0x1, data&0x1) ;

   /* 0x9257 */
   SOC_IF_ERROR_RETURN
      (READ_WC40_AN_X1_TIMERS_LINK_FAIL_INHIBIT_TIMER_CL72r(ws->unit, ws, &data));
   printf(" AN_X1_TIMERS_LINK_FAIL_INHIBIT_TIMER_CL72r(%0x) d=%0x \n", ws->accAddr, data) ;

   /* 0x9258 */
   SOC_IF_ERROR_RETURN
      (READ_WC40_AN_X1_TIMERS_LINK_FAIL_INHIBIT_TIMER_NOT_CL72r(ws->unit, ws, &data));
   printf(" AN_X1_TIMERS_LINK_FAIL_INHIBIT_TIMER_NOT_CL72r(%0x) d=%0x \n", ws->accAddr, data) ;

   /* 0x9212 */
   SOC_IF_ERROR_RETURN
      (READ_WC40_TX_X1_STATUS0_TLA_STATUSr(ws->unit, ws, &data));
   printf(" TX_X1_STATUS0_TLA_STATUSr(%0x) d=%0x \n", ws->accAddr, data) ;

   /* 9010 */
   tscmod_diag_anapll_status(ws) ;

   /* 0xc150 */
   tscmod_diag_rx_x4_status1_pma_pmd_live_status(ws) ;

   /* 0xc154 */
   tscmod_diag_rx_x4_status1_pcs_live_status(ws) ;

   return SOC_E_NONE ;
}

int tscmod_diag_g_an(tscmod_st *ws, int lane)
{
   uint16 data ; int np, rcv_ack, msg_page, comply_ack, toggle, oui ;
   uint16 np_mask, rcv_ack_mask, msg_page_mask, comply_ack_mask, toggle_mask, oui_mask ;
   int    oui3, oui2, oui1, udc2, udc1;
   int    np_shift, rcv_ack_shift, msg_page_shift, comply_ack_shift, toggle_shift ;
   int    cl37_bam_enable, cl73_bam_enable, cl73_hpam_enable, cl73_enable, cl37_sgmii_enable,
          cl37_enable, cl37_restart, cl73_restart ;
   int    b100g, b40g_cr4, b40g_kr4, b10g_kr, b10g_kx4, b1g_kx, b_pause, b_fec, b_np, b_rf ;
   int    b20g_kr2, b20g_cr2, cl73_bam, hpam_20gkr2 ;

   printf("%-22s: u=%0d p=%0d",
          FUNCTION_NAME(), ws->unit, ws->port);
   printf("  autoneg mode(%s)\n", e2s_tscmod_an_type[ws->an_type]);

   /* 0xc181 */
   SOC_IF_ERROR_RETURN
      (READ_WC40_AN_X4_ABILITIES_LOCAL_DEVICE_CL37_BASE_ABILITIESr
       (ws->unit, ws, &data));
   printf("  AN_X4_ABILITIES_LOCAL_DEVICE_CL37_BASE_ABILITIES(%0x) d=%0x\n",
          ws->accAddr, data) ;

   /* 0xc180 */
   SOC_IF_ERROR_RETURN
      (READ_WC40_AN_X4_ABILITIES_ENABLESr(ws->unit, ws, &data));
   printf("  AN_X4_ABILITIES_ENABLES(%0x) d=%0x\n", ws->accAddr, data) ;
   cl37_bam_enable =(data&AN_X4_ABILITIES_ENABLES_CL37_BAM_ENABLE_MASK)>>
                    AN_X4_ABILITIES_ENABLES_CL37_BAM_ENABLE_SHIFT ;
   cl73_bam_enable =(data&AN_X4_ABILITIES_ENABLES_CL73_BAM_ENABLE_MASK)>>
                    AN_X4_ABILITIES_ENABLES_CL73_BAM_ENABLE_SHIFT ;
   cl73_hpam_enable=(data&AN_X4_ABILITIES_ENABLES_CL73_HPAM_ENABLE_MASK)>>
                    AN_X4_ABILITIES_ENABLES_CL73_HPAM_ENABLE_SHIFT ;
   cl73_enable     =(data&AN_X4_ABILITIES_ENABLES_CL73_ENABLE_MASK)>>
                    AN_X4_ABILITIES_ENABLES_CL73_ENABLE_SHIFT ;
   cl37_sgmii_enable=(data&AN_X4_ABILITIES_ENABLES_CL37_SGMII_ENABLE_MASK)>>
                    AN_X4_ABILITIES_ENABLES_CL37_SGMII_ENABLE_SHIFT ;
   cl37_enable     =(data&AN_X4_ABILITIES_ENABLES_CL37_ENABLE_MASK) >>
                    AN_X4_ABILITIES_ENABLES_CL37_ENABLE_SHIFT ;
   cl37_restart    =(data&AN_X4_ABILITIES_ENABLES_CL37_AN_RESTART_MASK)>>
                    AN_X4_ABILITIES_ENABLES_CL37_AN_RESTART_SHIFT ;
   cl73_restart    =(data&AN_X4_ABILITIES_ENABLES_CL73_AN_RESTART_MASK)>>
                    AN_X4_ABILITIES_ENABLES_CL73_AN_RESTART_SHIFT ;
   printf("     bam37_en=%0d, bam73_en=%0d hpbam73=%0d cl73_en=%0d \
 cl37_sgmii_en=%0d cl37_en=%0d\n", cl37_bam_enable, cl73_bam_enable,
          cl73_hpam_enable, cl73_enable, cl37_sgmii_enable, cl37_enable) ;
   printf("     cl37_re=%0d cl73_re=%0d\n",  cl37_restart, cl73_restart);

   /* 0xc183 BAM37 */
   SOC_IF_ERROR_RETURN
      (READ_WC40_AN_X4_ABILITIES_LOCAL_DEVICE_OVER1G_ABILITIES_1r(ws->unit, ws, &data));
   printf("  AN_X4_ABILITIES_LOCAL_DEVICE_OVER1G_ABILITIES_1r(%0x) d=%0x  HG2=%0d FEC=%0d CL72=%0d\n",
          ws->accAddr, data, (data&0x8000)>>15, (data&0x4000)>>14, (data&0x2000)>>13) ;
   printf("      40G=%0d 32.7G=%0d 31.5G=%0d 25.45G=%0d 21G_X4=%0d 20G_X2_CX4=%0d 20G_X2=%0d",
          (data&0x1000)>>12, (data&0x800)>>11, (data&0x400)>>10, (data&0x200)>>9, (data&0x100)>>8, (data&0x80)>>7, (data&0x40)>>6) ;
   printf("20G_X4=%0d 20G_X4_CX4=%0d\n 16G_X4=%0d 15.75G_X2=%0d 15G_X4=%0d 13G_X4=%0d\n",
          (data&0x20)>>5, (data&0x10)>>4, (data&0x8)>>3, (data&0x4)>>2, (data&0x2)>>1, (data&0x1)) ;

   /* 0xc184 BAM37 */
   SOC_IF_ERROR_RETURN
      (READ_WC40_AN_X4_ABILITIES_LOCAL_DEVICE_OVER1G_ABILITIES_0r(ws->unit, ws, &data));
   printf("  AN_X4_ABILITIES_LOCAL_DEVICE_OVER1G_ABILITIES_0r(%0x) d=%0x  12.7G_X2=%0d 12.5_X4=%0d 12G_X4=%0d 10.5G_X2=%0d\n",
          ws->accAddr, data, (data&0x400)>>10, (data&0x200)>>9, (data&0x100)>>8, (data&0x80)>>7) ;
   printf("      10G_X2_CX4=%0d 10G_X2=%0d 10G_X4_CX4=%0d 10G_X4=%0d 6G_X4=%0d 5G_X4=%0d 2.5G=%0d\n",
          (data&0x40)>>6, (data&0x20)>>5, (data&0x10)>>4, (data&0x8)>>3, (data&0x4)>>2, (data&0x2)>>1, (data&0x1)) ;


   /* 0xc185 */
   SOC_IF_ERROR_RETURN
      (READ_WC40_AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BASE_ABILITIES_1r(ws->unit, ws, &data));
   printf(" AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BASE_ABILITIES_1r(%0x) d=%0x \n", ws->accAddr, data) ;

   /* 0xc186 */
   SOC_IF_ERROR_RETURN
      (READ_WC40_AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BASE_ABILITIES_0r(ws->unit, ws, &data));
   b100g = (data&AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BASE_ABILITIES_0_BASE_100GBASE_CR10_MASK)>>
      AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BASE_ABILITIES_0_BASE_100GBASE_CR10_SHIFT ;
   b40g_cr4 = (data&AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BASE_ABILITIES_0_BASE_40GBASE_CR4_MASK)>>
      AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BASE_ABILITIES_0_BASE_40GBASE_CR4_SHIFT ;
   b40g_kr4 = (data&AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BASE_ABILITIES_0_BASE_40GBASE_KR4_MASK)>>
      AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BASE_ABILITIES_0_BASE_40GBASE_KR4_SHIFT ;
   b10g_kr  = (data&AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BASE_ABILITIES_0_BASE_10GBASE_KR_MASK)>>
      AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BASE_ABILITIES_0_BASE_10GBASE_KR_SHIFT ;
   b10g_kx4 = (data&AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BASE_ABILITIES_0_BASE_10GBASE_KX4_MASK)>>
      AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BASE_ABILITIES_0_BASE_10GBASE_KX4_SHIFT ;
   b1g_kx   = (data&AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BASE_ABILITIES_0_BASE_1000BASE_KX_MASK)>>
      AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BASE_ABILITIES_0_BASE_1000BASE_KX_SHIFT ;
   b_pause  = (data&AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BASE_ABILITIES_0_CL73_PAUSE_MASK)>>
      AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BASE_ABILITIES_0_CL73_PAUSE_SHIFT ;
   b_fec    = (data&AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BASE_ABILITIES_0_FEC_MASK)>>
      AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BASE_ABILITIES_0_FEC_SHIFT ;
   b_np     = (data&AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BASE_ABILITIES_0_NEXT_PAGE_MASK)>>
      AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BASE_ABILITIES_0_NEXT_PAGE_SHIFT ;
   b_rf     = (data&AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BASE_ABILITIES_0_CL73_REMOTE_FAULT_MASK)>>
      AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BASE_ABILITIES_0_CL73_REMOTE_FAULT_SHIFT ;
   printf(" AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BASE_ABILITIES_0r(%0x) d=%0x 100g=%0d 40g_cr4=%0d 40g_kr4=%0d\n",
          ws->accAddr, data, b100g, b40g_cr4, b40g_kr4) ;
   printf("   10g_kr=%0d 10g_kx4=%0d 1g=%0d pause=%x fec=%0d np=%0d rf=%0d\n", b10g_kr, b10g_kx4, b1g_kx, b_pause, b_fec, b_np, b_rf) ;


   /* set cl73_bam_code set 0xc187 */
   SOC_IF_ERROR_RETURN
      (READ_WC40_AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BAM_ABILITIESr(ws->unit, ws, &data));
   b20g_kr2 = (data&AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BAM_ABILITIES_BAM_20GBASE_KR2_MASK)>>
      AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BAM_ABILITIES_BAM_20GBASE_KR2_SHIFT;
   b20g_cr2 = (data&AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BAM_ABILITIES_BAM_20GBASE_CR2_MASK)>>
      AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BAM_ABILITIES_BAM_20GBASE_CR2_SHIFT ;
   cl73_bam = (data&AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BAM_ABILITIES_CL73_BAM_CODE_MASK)>>
      AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BAM_ABILITIES_CL73_BAM_CODE_SHIFT ;
   hpam_20gkr2 = (data&AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BAM_ABILITIES_HPAM_20GKR2_MASK)>>
      AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BAM_ABILITIES_HPAM_20GKR2_SHIFT ;
   printf("  AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BAM_ABILITIESr(%0x) d=%0x 20g_kr2=%0d 20g_cr2=%0d cl73bam=%0d hpam_20gkr2=%0d\n",
          ws->accAddr, data, b20g_kr2, b20g_cr2, cl73_bam, hpam_20gkr2) ;

   np_mask        = (1<<15) ;   np_shift        = 15 ;
   rcv_ack_mask   = (1<<14) ;   rcv_ack_shift   = 14 ;
   msg_page_mask  = (1<<13) ;   msg_page_shift  = 13 ;
   comply_ack_mask= (1<<12) ;   comply_ack_shift= 12 ;
   toggle_mask    = (1<<11) ;   toggle_shift    = 11 ;
   oui_mask       = 0x7ff ;

   /* 0xc190 LP MP5 UP1 */
   SOC_IF_ERROR_RETURN
      (READ_WC40_AN_X4_HW_LP_PAGES_LP_MP5_UP1r(ws->unit, ws, &data));
   np          = (data & np_mask ) >> np_shift ;
   rcv_ack     = (data & rcv_ack_mask ) >> rcv_ack_shift ;
   msg_page    = (data & msg_page_mask) >> msg_page_shift ;
   comply_ack  = (data & comply_ack_mask) >> comply_ack_shift ;
   toggle      = (data & toggle_mask) >> toggle_shift ;
   oui         = (data & oui_mask) ;
   oui3        = oui << 13 ;
   printf("  N=np, R=rcv_ack, M=msg_page, C=comply_ack, T=toogle\n") ;
   printf("  AN_X4_HW_LP_PAGES_LP_MP5_UP1r(%0x) d=%0x N=%0d R=%0d M=%0d C=%0d\
 T=%0d [10:0]=%0x\n",
          ws->accAddr, data, np, rcv_ack, msg_page, comply_ack, toggle, oui) ;
   /* 0xc191 LP MP5 UP2 */
   SOC_IF_ERROR_RETURN
      (READ_WC40_AN_X4_HW_LP_PAGES_LP_MP5_UP2r(ws->unit, ws, &data));
   np          = (data & np_mask ) >> np_shift ;
   rcv_ack     = (data & rcv_ack_mask ) >> rcv_ack_shift ;
   msg_page    = (data & msg_page_mask) >> msg_page_shift ;
   comply_ack  = (data & comply_ack_mask) >> comply_ack_shift ;
   toggle      = (data & toggle_mask) >> toggle_shift ;
   oui         = (data & oui_mask) ;
   oui2        = oui << 2 ;
   printf("  AN_X4_HW_LP_PAGES_LP_MP5_UP2r(%0x) d=%0x N=%0d R=%0d M=%0d C=%0d\
  T=%0d [10:0]=%0x\n",
          ws->accAddr, data, np, rcv_ack, msg_page, comply_ack, toggle, oui) ;
   /* 0xc192 LP MP5 UP3 */
   SOC_IF_ERROR_RETURN
      (READ_WC40_AN_X4_HW_LP_PAGES_LP_MP5_UP3r(ws->unit, ws, &data));
   np          = (data & np_mask ) >> np_shift ;
   rcv_ack     = (data & rcv_ack_mask ) >> rcv_ack_shift ;
   msg_page    = (data & msg_page_mask) >> msg_page_shift ;
   comply_ack  = (data & comply_ack_mask) >> comply_ack_shift ;
   toggle      = (data & toggle_mask) >> toggle_shift ;
   oui         = (data & oui_mask) ;
   oui1        = oui >> 9 ;
   udc2        = (oui & 0x1ff) << 11 ;
   printf("  AN_X4_HW_LP_PAGES_LP_MP5_UP3r(%0x) d=%0x N=%0d R=%0d M=%0d C=%0d\
 T=%0d [10:0]=%0x\n",
          ws->accAddr, data, np, rcv_ack, msg_page, comply_ack, toggle, oui) ;
   /* 0xc193 LP MP5 UP4 */
   SOC_IF_ERROR_RETURN
      (READ_WC40_AN_X4_HW_LP_PAGES_LP_MP5_UP4r(ws->unit, ws, &data));
   np          = (data & np_mask ) >> np_shift ;
   rcv_ack     = (data & rcv_ack_mask ) >> rcv_ack_shift ;
   msg_page    = (data & msg_page_mask) >> msg_page_shift ;
   comply_ack  = (data & comply_ack_mask) >> comply_ack_shift ;
   toggle      = (data & toggle_mask) >> toggle_shift ;
   oui         = (data & oui_mask) ;
   udc1        = oui ;
   printf("  AN_X4_HW_LP_PAGES_LP_MP5_UP4r(%0x) d=%0x N=%0d R=%0d M=%0d C=%0d\
 T=%0d [10:0]=%0x\n",
          ws->accAddr, data, np, rcv_ack, msg_page, comply_ack, toggle, oui) ;
   printf("      OUI=%x UDC=%x\n", (oui1|oui2|oui3), (udc2|udc1)) ;
   /* 0xc194 LP MP1024 UP1 */
   SOC_IF_ERROR_RETURN
      (READ_WC40_AN_X4_HW_LP_PAGES_LP_MP1024_UP1r(ws->unit, ws, &data));
   np          = (data & np_mask ) >> np_shift ;
   rcv_ack     = (data & rcv_ack_mask ) >> rcv_ack_shift ;
   msg_page    = (data & msg_page_mask) >> msg_page_shift ;
   comply_ack  = (data & comply_ack_mask) >> comply_ack_shift ;
   toggle      = (data & toggle_mask) >> toggle_shift ;
   oui         = (data & oui_mask) ;
   printf("  AN_X4_HW_LP_PAGES_LP_MP1024_UP1r(%0x) d=%0x N=%0d R=%0d M=%0d\
 C=%0d T=%0d [10:0]=%0x\n",
          ws->accAddr, data, np, rcv_ack, msg_page, comply_ack, toggle, oui) ;
   printf("      20GCX4=%0d 16GX4=%0d 15GX4=%0d 13GX4=%0d 12.5GX4=%0d\
 12GX4=%0d 10GCX4=%d 10GX4=%0d\n",
          (data&0x400)>10, (data&0x200)>>9, (data&0x100)>>8, (data&0x80)>>7,
          (data&0x40)>>6, (data&0x20)>>5, (data&0x10)>>4, (data&0x8)>>3);
   printf("      6GX4=%0d 5GX4=%0d 2.5GX1=%0d\n",
          (data&0x4)>>2, (data&2)>>1, data&1) ;
   /* 0xc195 LP MP1024 UP2 */
   SOC_IF_ERROR_RETURN
      (READ_WC40_AN_X4_HW_LP_PAGES_LP_MP1024_UP2r(ws->unit, ws, &data));
   np          = (data & np_mask ) >> np_shift ;
   rcv_ack     = (data & rcv_ack_mask ) >> rcv_ack_shift ;
   msg_page    = (data & msg_page_mask) >> msg_page_shift ;
   comply_ack  = (data & comply_ack_mask) >> comply_ack_shift ;
   toggle      = (data & toggle_mask) >> toggle_shift ;
   oui         = (data & oui_mask) ;
   printf("  AN_X4_HW_LP_PAGES_LP_MP1024_UP2r(%0x) d=%0x N=%0d R=%0d M=%0d\
 C=%0d T=%0d [10:0]=%0x\n",
          ws->accAddr, data, np, rcv_ack, msg_page, comply_ack, toggle, oui) ;
   /* 0xc196 LP MP1024 UP3 */
   SOC_IF_ERROR_RETURN
      (READ_WC40_AN_X4_HW_LP_PAGES_LP_MP1024_UP3r(ws->unit, ws, &data));
   np          = (data & np_mask ) >> np_shift ;
   rcv_ack     = (data & rcv_ack_mask ) >> rcv_ack_shift ;
   msg_page    = (data & msg_page_mask) >> msg_page_shift ;
   comply_ack  = (data & comply_ack_mask) >> comply_ack_shift ;
   toggle      = (data & toggle_mask) >> toggle_shift ;
   oui         = (data & oui_mask) ;
   printf("  AN_X4_HW_LP_PAGES_LP_MP1024_UP3r(%0x) d=%0x N=%0d R=%0d M=%0d\
 C=%0d T=%0d [10:0]=%0x\n",
          ws->accAddr, data, np, rcv_ack, msg_page, comply_ack, toggle, oui) ;
   printf("       21GX4=%0d 24.45GX4=%0d 31.5GX4=%0d 32.7G=%0d 40GX4=%0d\
 CL72=%0d FEC=%0d HG=%0d\n",
          (data&0x200)>>9, (data&0x100)>>8, (data&0x80)>>7, (data&0x40)>>6,
          (data&0x20)>>5, (data&0x4)>>2, (data&2)>>1, data&1) ;
   /* 0xc197 LP MP1024 UP4 */
   SOC_IF_ERROR_RETURN
      (READ_WC40_AN_X4_HW_LP_PAGES_LP_MP1024_UP4r(ws->unit, ws, &data));
   np          = (data & np_mask ) >> np_shift ;
   rcv_ack     = (data & rcv_ack_mask ) >> rcv_ack_shift ;
   msg_page    = (data & msg_page_mask) >> msg_page_shift ;
   comply_ack  = (data & comply_ack_mask) >> comply_ack_shift ;
   toggle      = (data & toggle_mask) >> toggle_shift ;
   oui         = (data & oui_mask) ;
   printf("  AN_X4_HW_LP_PAGES_LP_MP1024_UP4r(%0x) d=%0x N=%0d R=%0d M=%0d\
 C=%0d T=%0d [10:0]=%0x\n",
          ws->accAddr, data, np, rcv_ack, msg_page, comply_ack, toggle, oui) ;
   printf("       LastPage=%0d 1GCX1=%0d 10GCX1=%0d 15.75GX2=%0d 20GCX2=%0d\
 20GX2=%0d 12.7GX2=%0d 10.5GX2=%0d 10GCX2=%0d 10GX2=%0d 20GX4=%0d\n",
          (data&0x400)>10, (data&0x200)>>9, (data&0x100)>>8, (data&0x80)>>7,
          (data&0x40)>>6, (data&0x20)>>5, (data&0x10)>>4, (data&0x8)>>3,
          (data&0x4)>>2, (data&2)>>1, data&1) ;
   /* 0xc198 LP Base page1 */
   SOC_IF_ERROR_RETURN
      (READ_WC40_AN_X4_HW_LP_PAGES_LP_BASE_PAGE1r(ws->unit, ws, &data));
   np          = (data & np_mask ) >> np_shift ;
   rcv_ack     = (data & rcv_ack_mask ) >> rcv_ack_shift ;
   printf("  AN_X4_HW_LP_PAGES_LP_BASE_PAGE1r(%0x) d=%0x N=%0d R=%0d\n",
          ws->accAddr, data, np, rcv_ack) ;
   if(ws->an_type==TSCMOD_CL37||ws->an_type==TSCMOD_CL37_BAM||ws->an_type==TSCMOD_CL37_10G){
      printf("        RF=%x Pause=%x Duplex=%x\n",
             (data&0x3000)>>12, (data&0x180)>>7, (data&0x60)>>5);
   } else if(ws->an_type==TSCMOD_CL37_SGMII) {
      printf("        Duplex=%x Spd=%x SGMII=%0d\n",
             (data&0x1000)>>12, (data&0xc00)>>10, data&1);
   } else if(ws->an_type==TSCMOD_CL73||ws->an_type==TSCMOD_CL73_BAM){
      printf("        RF=%x Pause=%x Echo=%x Sel=%x\n",
             (data&0x2000)>>13, (data&0xc00)>>10, (data&0x3e0)>>5, (data&0x1f));
   }
   /* 0xc199 LP Base page2 */
   SOC_IF_ERROR_RETURN
      (READ_WC40_AN_X4_HW_LP_PAGES_LP_BASE_PAGE2r(ws->unit, ws, &data));
   oui         = (data & oui_mask) ;
   printf("  AN_X4_HW_LP_PAGES_LP_BASE_PAGE2r(%0x) d=%0x [10:0]=%0x\n",
          ws->accAddr, data, oui) ;
   printf("        100GCR10=%0d 40GCR4=%0d 40GKR4=%0d 10GKR=%0d 10GKX4=%0d\
 1GKX=%0d Nonce=%x\n",
          (data&0x400)>10, (data&0x200)>>9, (data&0x100)>>8, (data&0x80)>>7,
          (data&0x40)>>6, (data&0x20)>>5, data&0x1f) ;
   /* 0xc19a LP Base page3 */
   SOC_IF_ERROR_RETURN
      (READ_WC40_AN_X4_HW_LP_PAGES_LP_BASE_PAGE3r(ws->unit, ws, &data));
   oui         = (data & oui_mask) ;
   printf("  AN_X4_HW_LP_PAGES_LP_BASE_PAGE3r(%0x) d=%0x FEC=%0x\n",
          ws->accAddr, data, (data&0xc000)>>14) ;
   /* SW manager */
   SOC_IF_ERROR_RETURN
      (READ_WC40_AN_X4_SW_MANAGEMENT_SW_CONTROL_STATUSr(ws->unit, ws, &data));
   printf("  SW_MANAGEMENT_SW_CONTROL_STATUSr(%0x) d=%0x comp_sw=%0d\
 ld_ctl_v=%0d lp_status_v=%0d\n",
          ws->accAddr, data, (data&4)>>2,(data&2)>>1, (data&1)) ;
   SOC_IF_ERROR_RETURN
      (READ_WC40_AN_X4_SW_MANAGEMENT_LD_CONTROLr(ws->unit, ws, &data));
   printf("  SW_MANAGEMENT_LD_CONTROLr(%0x) d=%0x sw_an=%0d sw_hcd=%0d\n",
          ws->accAddr, data, (data&2)>>1, (data&1)) ;
   printf("------------\n") ;
   SOC_IF_ERROR_RETURN                                      /* 0xc1a8 */
      (READ_WC40_AN_X4_SW_MANAGEMENT_AN_PAGE_SEQUENCER_STATUSr
       (ws->unit, ws, &data));
   printf("  AN_PAGE_SEQUENCER_STATUSr(%0x) d=%0x sgmii_mode=%0d hp_mode=%0d\
 rx_sgmii_mismatch=%0d rx_bp=%0d\n",
          ws->accAddr, data, (data&0x8000)>>15, (data&0x4000)>>14,
          (data&0x2000)>>13, (data&0x1000)>>12) ;
   printf("     rx_np=%0d         rx_mp_null=%0d      rx_mp_oui=%0d\
  rx_mp_ov1g=%0d       rx_mp_mismatch=%0d rx_up3=%0d\n",
          (data&0x800)>>11, (data&0x400)>>10, (data&0x200)>>9, (data&0x100)>>8,
          (data&0x80)>>7, (data&0x40)>>6);
   printf("     rx_up_oui_mis=%0d rx_up_oui_match=%0d rx_invalid=%0d\
 rx_np_toggle_err=%0d cl37_done=%0d      cl73_done=%0d\n",
          (data&0x20)>>5, (data&0x10)>>4, (data&0x8)>>3, (data&0x4)>>2,
          (data&2)>>1, data&1);
   /* 0xc1a9 */
   SOC_IF_ERROR_RETURN
      (READ_WC40_AN_X4_SW_MANAGEMENT_AN_PAGE_EXCHANGEER_STATUSr
       (ws->unit, ws, &data));
   printf("  AN_PAGE_EXCHANGEER_STATUSr(%0x) d=%0x next_pg_wait=%0d link_ok=%0d\
 good_check=%0d an_restart=%0d config_nonzero=%0d\n",
          ws->accAddr, data, (data&0x2000)>>13, (data&0x1000)>>12,
          (data&0x800)>>11, (data&0x400)>>10, (data&0x200)>>9) ;
   printf("     consist_mismatch=%0d comp_ack=%0d ack_det=%0d ability_det=%0d\
 an_en=%0d err_st=%0d disable_link=%0d idle_det=%0d config_restart=%0d\n",
          (data&0x100)>>8, (data&0x80)>>7, (data&0x40)>>6, (data&0x20)>>5,
          (data&0x10)>>4, (data&0x8)>>3, (data&0x4)>>2, (data&2)>>1, data&1) ;
   /* 0xc1aa */
   SOC_IF_ERROR_RETURN
      (READ_WC40_AN_X4_SW_MANAGEMENT_AN_PAGE_DECODER_STATUSr
       (ws->unit, ws, &data));
   printf("  AN_PAGE_DECODER_STATUSr(%0x) d=%0x\n", ws->accAddr, data) ;
   /* 0xc1ab */
   SOC_IF_ERROR_RETURN
      (READ_WC40_AN_X4_SW_MANAGEMENT_AN_ABILITY_RESOLUTION_STATUSr
       (ws->unit, ws, &data));
   printf("  AN_ABILITY_RESOLUTION_STATUSr(%0x) d=%0x err=%0d hcd_duplex=%0d\
 pause=%x hcd_speed=0x%0x fec=%0d cl72=%0d higig2=%0d switch_to_cl37=%0d\n",
          ws->accAddr, data, (data&0x2000)>>13, (data&0x1000)>>12,
          (data&0xc00)>>10, (data&0x3f0)>>4, (data&0x8)>>3, (data&0x4)>>2,
          (data&2)>>1, data&1) ;
   SOC_IF_ERROR_RETURN
      (READ_WC40_AN_X4_SW_MANAGEMENT_AN_MISC_STATUSr(ws->unit, ws, &data));
   /* 0xc1ac */
   printf("  AN_MISC_STATUSr(%0x) d=%0x complete=%0d retry_cnt=%0d\
 fault_in_base=%0d pd_completed=%0d an_active=%0d\n",
          ws->accAddr, data, (data&0x8000)>>15, (data&0x7e00)>>9,
          (data&0x100)>>8, (data&0x80)>>7, (data&0x40)>>6);
   printf("    an_fail_count=%0d pd_in_progress=%0d hcd_kx(1)_or_kx4(0)=%0d\n",
          (data&0x3c)>>2, (data&2)>>1, data&1);
   /* 0xc1ad */
   SOC_IF_ERROR_RETURN
      (READ_WC40_AN_X4_SW_MANAGEMENT_TLA_SEQUENCER_STATUSr
       (ws->unit, ws, &data));
   printf("  TLA_SEQUENCER_STATUSr(%0x) d=%0x\n", ws->accAddr, data) ;
   SOC_IF_ERROR_RETURN
      (READ_WC40_AN_X4_SW_MANAGEMENT_AN_SEQ_UNEXPECTED_PAGEr
       (ws->unit, ws, &data));
   printf("  AN_SEQ_UNEXPECTED_PAGEr(%0x) d=%0x\n", ws->accAddr, data) ;


   printf("  ------------------------------------\n") ;
   tscmod_diag_g_an_deep(ws, lane) ;

   return SOC_E_NONE ;
}

int tscmod_diag_g_dsc(tscmod_st *ws, int lane) {

   uint16 data ;
   int cdr_freq_override_en, cdr_freq_en, cdr_freq_upd_en, cdr_integ_reg_clr, cdr_phase_err_frz, cdros_peak_polarity ;
   int cdros_zero_polarity, cdros_phase_sat_ctrl, cdros_falling_edge, cdros_rising_edge, cdrbr_third_vec_en, cdrbr_polarity ;
   int cdrbr_p1_slicer_only, cdrbr_m1_slicer_only, cdros_xfi_integ_sat_sel ;
   int cdr_freq_override_val ;
   int cdros_phase_err_offset, cdrbr_phase_err_offset ;
   int pi_cw_rst, interp_ctrl_dsel, interp_ctrl_cap, pi_phase_invert, pi_phase_step_mult ;
   int phase_step_dir, phase_step, phase_delta, phase_strobe, phase_rotate_override ;
   int clk90_offset_override, dual_phase_override ;
   int dfe_abs_sum_max, random_tapsel_disable, cor_neg2_en, cor_neg1_en, vga_polarity ;
   int dfe_polarity, trnsum_tap0_only, sum_m1err, trnsum_en, dfe_vga_clken ;
   int dfe_llms_update_count_sel, dfe_llms_en, dfe_cdros_qphase_mult_en, dfe_tap_2_dcd, dfe_tap_1_do, dfe_tap_1_de ;
   int vga3_override_val, vga3_override_en, dfe_vga_write_tapsel, dfe_vga_write_val;
   int dfe_vga_write_en ;
   int trnsum_otap_en, trnsum_etap_en ;
   int trnsum_otap_sign, trnsum_etap_sign ;
   int vga_cor_sel_e, vga_cor_sel_o ;
   int dfe_cor_sel_e, dfe_cor_sel_o ;
   int disable_cl72_rcving_ctrl_frame, pattern, pattern_bit_en, pattern_en ;
   int osx2_diag_ctrl, voffset, hoffset, diagnostics_en ;
   int supplement_info, ready_for_cmd, error_found, cmd_info, gp_uC_req ;
   int cl72_timer_en, acq2_timeout, acq1_timeout, acqcdr_timeout ;
   int acqvga_timeout, bypass_os_integ_xfer, dfe_frzval, dfe_frcfrz ;
   int vga_frzval, vga_frcfrz, dsc_clr_val, dsc_clr_frc ;
   int sw_handover, eee_acq_2_timeout, eee_acq_phase_timeout, skip_acq ;
   int train2_req, train1_req, soft_ack, train_mode_en ;
   int pause_level_trigger_en, os_dfe_en, dfe_taps2_5_en, dfe_gain_acq2 ;
   int dfe_gain_acq1, vga_gain_acq2, vga_gain_acq1, vga_gain_acqcdr, vga_gain_acqvga ;
   int cdros45_bwsel_prop_offset, cdros38_bwsel_prop_offset, cdros_bwsel_prop_acq1_2 ;
   int cdros_bwsel_prop_acqcdr, cdros_bwsel_prop_acqvga ;
   int cdros_bwsel_integ_eee_acq2, cdros_bwsel_integ_acq1_2, cdros_bwsel_integ_acqcdr ;
   int cdros_bwsel_integ_acqvga ;
   int cdrbr_bwsel_prop_acq2, cdrbr_bwsel_prop_acq1, cdrbr_bwsel_prop_acqcdr ;
   int cdrbr_bwsel_prop_acqphase ;
   int cdrbr_bwsel_integ_acq2, cdrbr_bwsel_integ_acq1, cdrbr_bwsel_integ_acqcdr ;
   int cdrbr_bwsel_integ_acqphase ;
   int cdros_bwsel_prop_eee_acqphase, phase_sat_ctrl_100fx ;
   int pi_phase_step_mult_100fx, cdros_bwsel_integ_100fx, cdros_bwsel_prop_100fx ;
   int force_p1_ctrl_strobe, force_p1_odd_ctrl, force_p1_evn_ctrl, p1_odd_ctrl ;
   int p1_evn_ctrl ;
   int force_d_ctrl_strobe, force_d_odd_ctrl, force_d_evn_ctrl, d_odd_ctrl, d_evn_ctrl ;
   int force_m1_ctrl_strobe, force_m1_odd_ctrl, force_m1_evn_ctrl, m1_odd_ctrl, m1_evn_ctrl ;
   int rx_pf_hiz, force_rx_m1_thresh_zero, rx_m1_thresh_zero, rx_thresh_sel ;
   int force_rx_pf_ctrl, rx_pf_ctrl ;
   int br_offset_pd, br_en_hgain, br_en_dfe_clk, br_pd_ch_p1, osr_offset_pd ;
   int osr_en_hgain, osr_en_dfe_clk, osr_pd_ch_p1 ;

   int integ_reg ;
   int integ_reg_xfer ;
   int rx_fifo_os8_error, sm_br_cdr_enabled, br_cdr_enabled ;
   int oscdr_mode, phase_err ;
   int clk90_phase_offset, phase_cntr ;
   int phs_interp_status ;
   int dfe_tap_1_bin, vga_sum ;
   int dfe_tap_3_bin, dfe_tap_2_bin ;
   int dfe_tap_5_bin, dfe_tap_4_bin ;
   int trnsum ;
   int dsc_state_eee_acq_2, dsc_state ;
   int dsc_state_eee_acq_cdr_phase, dsc_state_eee_quiet, dsc_state_done ;
   int dsc_state_measure, dsc_state_train_2, dsc_state_train_1, dsc_state_soft_ack ;
   int dsc_state_acq_2, dsc_state_acq_1, dsc_state_acq_cdr, dsc_state_acq_cdrbr_phase ;
   int dsc_state_os_integ_xfer, dsc_state_acq_vga, dsc_state_setup, dsc_state_pause ;
   int dsc_state_init ;
   int pd_ch_p1, en_dfe_clk, en_hgain, offset_pd, pf_ctrl_bin ;
   int slicer_offset_po, slicer_offset_pe ;
   int slicer_offset_zo, slicer_offset_ze ;
   int slicer_offset_mo, slicer_offset_me ;
   int bypass_tx_postc_cal, bypass_br_vga, postc_metric_ctrl, hysteresis_en ;
   int slicer_cal_linear_srch, bypass_br_pf_cal, bypass_osx2_pf_cal, bypass_osx1_pf_cal ;
   int bypass_data_slicer_recal, bypass_osx45_slicer_cal, bypass_phase_slicer_cal ;
   int bypass_br_data_slicer_cal, bypass_os_data_slicer_cal, restart_tuning ;
   int tuning_sm_en ;
   int pf_ctrl_br_offset, pf_ctrl_osx1_offset, pf_ctrl_osx2_offset, pf_ctrl_br_init ;
   int pf_ctrl_os_init ;
   int vga_max_val, vga_min_val ;
   int dfe_max_val, dfe_min_val ;
   int br_pf_tap_en, osx1_pf_tap_en ;
   int msr_br_vga_timeout, hysteresis_timeout, msr_postc_timeout ;
   int br_vga_lms_gain, postc_dfe_lms_gain, cdr_phase_inversion_timeout, msr_pf_timeout ;
   int br_vga_trn2_timeout, br_pf_trn2_timeout, br_pf_trn1_timeout ;
   int br_postc_tap_en, osx2_pf_tap_en ;
   int osx2_postc_tap_en, osx1_postc_tap_en ;
   int msr_slicer_slow_timeout, msr_slicer_fast_timeout, default_trn2_timeout ;
   int pf_fine_en, pf_max_val, bypass_osx1_slicer_cal, bypass_osx2_slicer_cal ;
   int eee_lfsr_cnt ;
   int tuning_done, srch_state, tuning_state, dsc_state_sm ;
   int postc_metric ;
   int dfe_max, dfe_min, vga_max, vga_min, pf_max, pf_min ;
   int trnsum_br_vga ;
   int trnsum_pf ;
   int tuning_state_br_tx_postc, tuning_state_br_pf, tuning_state_br_vga;
   int tuning_state_br_slicer_po, tuning_state_br_slicer_pe, tuning_state_br_slicer_mo ;
   int tuning_state_br_slicer_me, tuning_state_br_slicer_do, tuning_state_br_slicer_de ;
   int tuning_state_os_tx_postc, tuning_state_os_pf, tuning_state_os_slicer_mo ;
   int tuning_state_os_slicer_me, tuning_state_os_slicer_do, tuning_state_os_slicer_de ;
   int tuning_state_wait_for_lock ;
   int tuning_state_hysteresis, tuning_state_tuning_done, srch_state_srch_done ;
   int srch_state_srch_wait_undo_setup, srch_state_srch_undo_setup, srch_state_srch_wait_msr ;
   int srch_state_srch_set_msr, srch_state_srch_wait_trn, srch_state_srch_set_trn;
   int srch_state_srch_wait, srch_state_srch_setup, srch_state_srch_disable ;



   printf("%-22s u=%0d p=%0d\n", FUNCTION_NAME(), ws->unit, ws->port) ;

   /* 0xC200 - 0xC20E, Dsc1b0: DSC CDR/DFE Control Registers */

   /* 0xC200 */

   SOC_IF_ERROR_RETURN ( READ_WC40_DSC1B0_CDR_CTRL0r(ws->unit,ws,&data));

   cdros_xfi_integ_sat_sel= (data & DSC1B0_CDR_CTRL0_CDROS_XFI_INTEG_SAT_SEL_MASK) >>
                      DSC1B0_CDR_CTRL0_CDROS_XFI_INTEG_SAT_SEL_SHIFT ;
   cdrbr_m1_slicer_only= (data & DSC1B0_CDR_CTRL0_CDRBR_M1_SLICER_ONLY_MASK) >>
                   DSC1B0_CDR_CTRL0_CDRBR_M1_SLICER_ONLY_SHIFT ;
   cdrbr_p1_slicer_only= (data & DSC1B0_CDR_CTRL0_CDRBR_P1_SLICER_ONLY_MASK) >>
                   DSC1B0_CDR_CTRL0_CDRBR_P1_SLICER_ONLY_SHIFT ;
   cdrbr_polarity= (data & DSC1B0_CDR_CTRL0_CDRBR_POLARITY_MASK) >>
             DSC1B0_CDR_CTRL0_CDRBR_POLARITY_SHIFT ;
   cdrbr_third_vec_en= (data & DSC1B0_CDR_CTRL0_CDRBR_THIRD_VEC_EN_MASK) >>
                 DSC1B0_CDR_CTRL0_CDRBR_THIRD_VEC_EN_SHIFT ;
   cdros_rising_edge= (data & DSC1B0_CDR_CTRL0_CDROS_RISING_EDGE_MASK) >>
                DSC1B0_CDR_CTRL0_CDROS_RISING_EDGE_SHIFT ;
   cdros_falling_edge= (data & DSC1B0_CDR_CTRL0_CDROS_FALLING_EDGE_MASK) >>
                 DSC1B0_CDR_CTRL0_CDROS_FALLING_EDGE_SHIFT ;
   cdros_phase_sat_ctrl= (data & DSC1B0_CDR_CTRL0_CDROS_PHASE_SAT_CTRL_MASK) >>
                   DSC1B0_CDR_CTRL0_CDROS_PHASE_SAT_CTRL_SHIFT ;
   cdros_peak_polarity= (data & DSC1B0_CDR_CTRL0_CDROS_PEAK_POLARITY_MASK) >>
                  DSC1B0_CDR_CTRL0_CDROS_PEAK_POLARITY_SHIFT ;
   cdros_zero_polarity= (data & DSC1B0_CDR_CTRL0_CDROS_ZERO_POLARITY_MASK) >>
                  DSC1B0_CDR_CTRL0_CDROS_ZERO_POLARITY_SHIFT ;
   cdr_phase_err_frz= (data & DSC1B0_CDR_CTRL0_CDR_PHASE_ERR_FRZ_MASK) >>
                  DSC1B0_CDR_CTRL0_CDR_PHASE_ERR_FRZ_SHIFT ;
   cdr_integ_reg_clr= (data & DSC1B0_CDR_CTRL0_CDR_INTEG_REG_CLR_MASK) >>
                  DSC1B0_CDR_CTRL0_CDR_INTEG_REG_CLR_SHIFT ;
   cdr_freq_upd_en= (data & DSC1B0_CDR_CTRL0_CDR_FREQ_UPD_EN_MASK) >>
                DSC1B0_CDR_CTRL0_CDR_FREQ_UPD_EN_SHIFT ;
   cdr_freq_en= (data & DSC1B0_CDR_CTRL0_CDR_FREQ_EN_MASK) >>
            DSC1B0_CDR_CTRL0_CDR_FREQ_EN_SHIFT ;
   cdr_freq_override_en= data & DSC1B0_CDR_CTRL0_CDR_FREQ_OVERRIDE_EN_MASK ;

   printf(" DSC1B0_CDR_CTRL0r(%0x) d=%0x cdros_xfi_integ_sat_sel=%0x cdrbr_m1_slicer_only=%0x\n",
          ws->accAddr, data, cdros_xfi_integ_sat_sel, cdrbr_m1_slicer_only ) ;
   printf("   cdrbr_p1_slicer_only=%0x cdrbr_polarity=%0x cdrbr_third_vec_en=%0x cdros_rising_edge=%0x\n",
          cdrbr_p1_slicer_only, cdrbr_polarity, cdrbr_third_vec_en, cdros_rising_edge ) ;
   printf("   cdros_falling_edge=%0x cdros_phase_sat_ctrl=%0x cdros_peak_polarity=%0x cdros_zero_polarity=%0x\n",
          cdros_falling_edge, cdros_phase_sat_ctrl, cdros_peak_polarity, cdros_zero_polarity ) ;
   printf("   cdr_phase_err_frz=%0x cdr_integ_reg_clr=%0x cdr_freq_upd_en=%0x cdr_freq_en=%0x\n",
          cdr_phase_err_frz, cdr_integ_reg_clr, cdr_freq_upd_en, cdr_freq_en ) ;
   printf("   cdr_freq_override_en=%0x\n",
          cdr_freq_override_en ) ;


   /* 0xC201 */

   SOC_IF_ERROR_RETURN ( READ_WC40_DSC1B0_CDR_CTRL1r(ws->unit,ws,&data));

   cdr_freq_override_val= data & DSC1B0_CDR_CTRL1_CDR_FREQ_OVERRIDE_VAL_MASK ;

   printf(" DSC1B0_CDR_CTRL1r(%0x) d=%0x cdr_freq_override_val=%0x\n",
          ws->accAddr, data, cdr_freq_override_val ) ;


   /* 0xC202 */

   SOC_IF_ERROR_RETURN ( READ_WC40_DSC1B0_CDR_CTRL2r(ws->unit,ws,&data));

   cdros_phase_err_offset= (data & DSC1B0_CDR_CTRL2_CDROS_PHASE_ERR_OFFSET_MASK) >>
                           DSC1B0_CDR_CTRL2_CDROS_PHASE_ERR_OFFSET_SHIFT ;
   cdrbr_phase_err_offset= data & DSC1B0_CDR_CTRL2_CDRBR_PHASE_ERR_OFFSET_MASK ;

   printf(" DSC1B0_CDR_CTRL1r(%0x) d=%0x cdros_phase_err_offset=%0x cdrbr_phase_err_offset=%0x\n",
          ws->accAddr, data, cdros_phase_err_offset, cdrbr_phase_err_offset ) ;



   /* 0xC203 */

   SOC_IF_ERROR_RETURN ( READ_WC40_DSC1B0_PI_CTRL0r(ws->unit,ws,&data));

   pi_cw_rst= (data & DSC1B0_PI_CTRL0_PI_CW_RST_MASK) >>
              DSC1B0_PI_CTRL0_PI_CW_RST_SHIFT ;
   interp_ctrl_dsel= (data & DSC1B0_PI_CTRL0_INTERP_CTRL_DSEL_MASK) >>
                     DSC1B0_PI_CTRL0_INTERP_CTRL_DSEL_SHIFT ;
   interp_ctrl_cap= (data & DSC1B0_PI_CTRL0_INTERP_CTRL_CAP_MASK) >>
                    DSC1B0_PI_CTRL0_INTERP_CTRL_CAP_SHIFT                      ;
   pi_phase_invert= (data & DSC1B0_PI_CTRL0_PI_PHASE_INVERT_MASK) >>
                    DSC1B0_PI_CTRL0_PI_PHASE_INVERT_SHIFT ;
   pi_phase_step_mult= data & DSC1B0_PI_CTRL0_PI_PHASE_STEP_MULT_MASK ;

   printf(" DSC1B0_PI_CTRL0r(%0x) d=%0x pi_cw_rst=%0x interp_ctrl_dsel=%0x\n",
          ws->accAddr, data, pi_cw_rst, interp_ctrl_dsel ) ;
   printf("   interp_ctrl_cap=%0x pi_phase_invert=%0x pi_phase_step_mult=%0x\n",
          interp_ctrl_cap, pi_phase_invert, pi_phase_step_mult ) ;



   /* 0xC204 */

   SOC_IF_ERROR_RETURN ( READ_WC40_DSC1B0_PI_CTRL1r(ws->unit,ws,&data));

   phase_step_dir= (data & DSC1B0_PI_CTRL1_PI_PHASE_STEP_DIR_MASK ) >>
                   DSC1B0_PI_CTRL1_PI_PHASE_STEP_DIR_SHIFT ;
   phase_step= (data & DSC1B0_PI_CTRL1_PI_PHASE_STEP_MASK) >>
               DSC1B0_PI_CTRL1_PI_PHASE_STEP_SHIFT ;
   phase_delta= (data & DSC1B0_PI_CTRL1_PI_PHASE_DELTA_MASK) >>
                DSC1B0_PI_CTRL1_PI_PHASE_DELTA_SHIFT ;
   phase_strobe= (data & DSC1B0_PI_CTRL1_PI_PHASE_STROBE_MASK) >>
                 DSC1B0_PI_CTRL1_PI_PHASE_STROBE_SHIFT ;
   phase_rotate_override= (data & DSC1B0_PI_CTRL1_PI_PHASE_ROTATE_OVERRIDE_MASK) >>
                          DSC1B0_PI_CTRL1_PI_PHASE_ROTATE_OVERRIDE_SHIFT ;
   clk90_offset_override= (data & DSC1B0_PI_CTRL1_PI_CLK90_OFFSET_OVERRIDE_MASK) >>
                          DSC1B0_PI_CTRL1_PI_CLK90_OFFSET_OVERRIDE_SHIFT ;
   dual_phase_override= data & DSC1B0_PI_CTRL1_PI_DUAL_PHASE_OVERRIDE_MASK ;

   printf(" DSC1B0_PI_CTRL1r(%0x) d=%0x phase_step_dir=%0x phase_step=%0x\n",
          ws->accAddr, data, phase_step_dir, phase_step ) ;
   printf("   phase_delta=%0x phase_strobe=%0x phase_rotate_override=%0x clk90_offset_override=%0x\n",
          phase_delta, phase_strobe, phase_rotate_override, clk90_offset_override ) ;
   printf("   dual_phase_override=%0x\n",
          dual_phase_override ) ;


   /* 0xC205 */

   SOC_IF_ERROR_RETURN ( READ_WC40_DSC1B0_DFE_VGA_CTRL0r(ws->unit,ws,&data));

   dfe_abs_sum_max= (data & DSC1B0_DFE_VGA_CTRL0_DFE_ABS_SUM_MAX_MASK) >>
                    DSC1B0_DFE_VGA_CTRL0_DFE_ABS_SUM_MAX_SHIFT ;
   random_tapsel_disable= (data & DSC1B0_DFE_VGA_CTRL0_RANDOM_TAPSEL_DISABLE_MASK) >>
                          DSC1B0_DFE_VGA_CTRL0_RANDOM_TAPSEL_DISABLE_SHIFT ;
   cor_neg2_en= (data & DSC1B0_DFE_VGA_CTRL0_COR_NEG2_EN_MASK) >>
               DSC1B0_DFE_VGA_CTRL0_COR_NEG2_EN_SHIFT ;
   cor_neg1_en= (data & DSC1B0_DFE_VGA_CTRL0_COR_NEG1_EN_MASK) >>
                DSC1B0_DFE_VGA_CTRL0_COR_NEG1_EN_SHIFT ;
   vga_polarity= (data & DSC1B0_DFE_VGA_CTRL0_VGA_POLARITY_MASK) >>
                 DSC1B0_DFE_VGA_CTRL0_VGA_POLARITY_SHIFT ;
   dfe_polarity= (data & DSC1B0_DFE_VGA_CTRL0_DFE_POLARITY_MASK) >>
                 DSC1B0_DFE_VGA_CTRL0_DFE_POLARITY_SHIFT ;
   trnsum_tap0_only= (data & DSC1B0_DFE_VGA_CTRL0_TRNSUM_TAP0_ONLY_MASK) >>
                     DSC1B0_DFE_VGA_CTRL0_TRNSUM_TAP0_ONLY_SHIFT ;
   sum_m1err= (data & DSC1B0_DFE_VGA_CTRL0_SUM_M1ERR_MASK) >>
              DSC1B0_DFE_VGA_CTRL0_SUM_M1ERR_SHIFT ;
   trnsum_en= (data & DSC1B0_DFE_VGA_CTRL0_TRNSUM_EN_MASK) >>
              DSC1B0_DFE_VGA_CTRL0_TRNSUM_EN_SHIFT ;
   dfe_vga_clken= data & DSC1B0_DFE_VGA_CTRL0_DFE_VGA_CLKEN_MASK ;

   printf(" DSC1B0_DFE_VGA_CTRL0r(%0x) d=%0x dfe_abs_sum_max=%0x random_tapsel_disable=%0x\n",
          ws->accAddr, data, dfe_abs_sum_max, random_tapsel_disable ) ;
   printf("   cor_neg2_en=%0x cor_neg1_en=%0x vga_polarity=%0x dfe_polarity=%0x\n",
          cor_neg2_en, cor_neg1_en, vga_polarity, dfe_polarity ) ;
   printf("   trnsum_tap0_only=%0x sum_m1err=%0x trnsum_en=%0x dfe_vga_clken=%0x\n",
          trnsum_tap0_only, sum_m1err, trnsum_en, dfe_vga_clken ) ;

   /* 0xC206 */

   SOC_IF_ERROR_RETURN ( READ_WC40_DSC1B0_DFE_VGA_CTRL1r(ws->unit,ws,&data));

   dfe_llms_update_count_sel= (data & DSC1B0_DFE_VGA_CTRL1_DFE_LLMS_UPDATE_COUNT_SEL_MASK ) >>
                          DSC1B0_DFE_VGA_CTRL1_DFE_LLMS_UPDATE_COUNT_SEL_SHIFT ;
   dfe_llms_en= (data & DSC1B0_DFE_VGA_CTRL1_DFE_LLMS_EN_MASK) >>
            DSC1B0_DFE_VGA_CTRL1_DFE_LLMS_EN_SHIFT ;
   dfe_cdros_qphase_mult_en= (data & DSC1B0_DFE_VGA_CTRL1_DFE_CDROS_QPHASE_MULT_EN_MASK) >>
                         DSC1B0_DFE_VGA_CTRL1_DFE_CDROS_QPHASE_MULT_EN_SHIFT ;
   dfe_tap_2_dcd= (data & DSC1B0_DFE_VGA_CTRL1_DFE_TAP_2_DCD_MASK) >>
              DSC1B0_DFE_VGA_CTRL1_DFE_TAP_2_DCD_SHIFT ;
   dfe_tap_1_do= (data & DSC1B0_DFE_VGA_CTRL1_DFE_TAP_1_DO_MASK) >>
             DSC1B0_DFE_VGA_CTRL1_DFE_TAP_1_DO_SHIFT ;
   dfe_tap_1_de= data & DSC1B0_DFE_VGA_CTRL1_DFE_TAP_1_DE_MASK ;

   printf(" DSC1B0_DFE_VGA_CTRL1r(%0x) d=%0x dfe_llms_update_count_sel=%0x dfe_llms_en=%0x\n",
          ws->accAddr, data, dfe_llms_update_count_sel, dfe_llms_en ) ;
   printf("   dfe_cdros_qphase_mult_en=%0x dfe_tap_2_dcd=%0x dfe_tap_1_do=%0x dfe_tap_1_de=%0x\n",
          dfe_cdros_qphase_mult_en, dfe_tap_2_dcd, dfe_tap_1_do, dfe_tap_1_de ) ;


   /* 0xC207 */

   SOC_IF_ERROR_RETURN ( READ_WC40_DSC1B0_DFE_VGA_CTRL2r(ws->unit,ws,&data));

   vga3_override_val= (data & DSC1B0_DFE_VGA_CTRL2_VGA3_OVERRIDE_VAL_MASK) >>
                      DSC1B0_DFE_VGA_CTRL2_VGA3_OVERRIDE_VAL_SHIFT;
   vga3_override_en= (data & DSC1B0_DFE_VGA_CTRL2_VGA3_OVERRIDE_EN_MASK) >>
                     DSC1B0_DFE_VGA_CTRL2_VGA3_OVERRIDE_EN_SHIFT ;
   dfe_vga_write_tapsel= (data & DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_TAPSEL_MASK) >>
                         DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_TAPSEL_SHIFT ;
   dfe_vga_write_val= (data & DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_VAL_MASK) >>
                      DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_VAL_SHIFT ;
   dfe_vga_write_en= data & DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_EN_MASK ;


   printf(" DSC1B0_DFE_VGA_CTRL2r(%0x) d=%0x vga3_override_val=%0x vga3_override_en=%0x\n",
          ws->accAddr, data, vga3_override_val, vga3_override_en ) ;
   printf("   dfe_vga_write_tapsel=%0x dfe_vga_write_val=%0x dfe_vga_write_en=%0x\n",
          dfe_vga_write_tapsel, dfe_vga_write_val, dfe_vga_write_en ) ;


   /* 0xC208 */

   SOC_IF_ERROR_RETURN ( READ_WC40_DSC1B0_DFE_VGA_CTRL3r(ws->unit,ws,&data));

   trnsum_otap_en= (data & DSC1B0_DFE_VGA_CTRL3_TRNSUM_OTAP_EN_MASK) >>
                   DSC1B0_DFE_VGA_CTRL3_TRNSUM_OTAP_EN_SHIFT ;
   trnsum_etap_en= data & DSC1B0_DFE_VGA_CTRL3_TRNSUM_ETAP_EN_MASK ;

   printf(" DSC1B0_DFE_VGA_CTRL3r(%0x) d=%0x trnsum_otap_en=%0x trnsum_etap_en=%0x\n",
          ws->accAddr, data, trnsum_otap_en, trnsum_etap_en ) ;


   /* 0xC209 */

   SOC_IF_ERROR_RETURN ( READ_WC40_DSC1B0_DFE_VGA_CTRL4r(ws->unit,ws,&data));

   trnsum_otap_sign= (data & DSC1B0_DFE_VGA_CTRL4_TRNSUM_OTAP_SIGN_MASK) >>
                     DSC1B0_DFE_VGA_CTRL4_TRNSUM_OTAP_SIGN_SHIFT ;
   trnsum_etap_sign= data & DSC1B0_DFE_VGA_CTRL4_TRNSUM_ETAP_SIGN_MASK ;

   printf(" DSC1B0_DFE_VGA_CTRL4r(%0x) d=%0x trnsum_otap_sign=%0x trnsum_etap_sign=%0x\n",
          ws->accAddr, data, trnsum_otap_sign, trnsum_etap_sign ) ;


   /* 0xC20A */

   SOC_IF_ERROR_RETURN ( READ_WC40_DSC1B0_DFE_VGA_CTRL5r(ws->unit,ws,&data));

   vga_cor_sel_e= (data & DSC1B0_DFE_VGA_CTRL5_VGA_COR_SEL_E_MASK) >>
                  DSC1B0_DFE_VGA_CTRL5_VGA_COR_SEL_E_SHIFT ;
   vga_cor_sel_o= data & DSC1B0_DFE_VGA_CTRL5_VGA_COR_SEL_O_MASK ;


   printf(" DSC1B0_DFE_VGA_CTRL5r(%0x) d=%0x vga_cor_sel_e=%0x vga_cor_sel_o=%0x\n",
	  ws->accAddr, data, vga_cor_sel_e, vga_cor_sel_o ) ;


   /* 0xC20B */

   SOC_IF_ERROR_RETURN ( READ_WC40_DSC1B0_DFE_VGA_CTRL6r(ws->unit,ws,&data));

   dfe_cor_sel_e= (data & DSC1B0_DFE_VGA_CTRL6_DFE_COR_SEL_E_MASK) >>
                  DSC1B0_DFE_VGA_CTRL6_DFE_COR_SEL_E_SHIFT ;
   dfe_cor_sel_o= data & DSC1B0_DFE_VGA_CTRL6_DFE_COR_SEL_O_MASK ;

   printf(" DSC1B0_DFE_VGA_CTRL6r(%0x) d=%0x dfe_cor_sel_e=%0x dfe_cor_sel_o=%0x\n",
          ws->accAddr, data, dfe_cor_sel_e, dfe_cor_sel_o ) ;


   /* 0xC20C */

   SOC_IF_ERROR_RETURN ( READ_WC40_DSC1B0_DFE_VGA_CTRL7r(ws->unit,ws,&data));

   disable_cl72_rcving_ctrl_frame= (data & DSC1B0_DFE_VGA_CTRL7_DISABLE_CL72_RCVING_CTRL_FRAME_MASK) >>
                                   DSC1B0_DFE_VGA_CTRL7_DISABLE_CL72_RCVING_CTRL_FRAME_SHIFT ;
   pattern= (data & DSC1B0_DFE_VGA_CTRL7_PATTERN_MASK) >>
            DSC1B0_DFE_VGA_CTRL7_PATTERN_SHIFT ;
   pattern_bit_en= (data & DSC1B0_DFE_VGA_CTRL7_PATTERN_BIT_EN_MASK) >>
                   DSC1B0_DFE_VGA_CTRL7_PATTERN_BIT_EN_SHIFT ;
   pattern_en= data & DSC1B0_DFE_VGA_CTRL7_PATTERN_EN_MASK ;

   printf(" DSC1B0_DFE_VGA_CTRL7r(%0x) d=%0x disable_cl72_rcving_ctrl_frame=%0x pattern=%0x\n",
          ws->accAddr, data, disable_cl72_rcving_ctrl_frame, pattern ) ;
   printf("   pattern_bit_en=%0x pattern_en=%0x\n",
          pattern_bit_en, pattern_en ) ;


   /* 0xC20D */

   SOC_IF_ERROR_RETURN ( READ_WC40_DSC1B0_DSC_DIAG_CTRL0r(ws->unit,ws,&data));

   osx2_diag_ctrl= (data & DSC1B0_DSC_DIAG_CTRL0_OSX2_DIAG_CTRL_MASK) >>
                   DSC1B0_DSC_DIAG_CTRL0_OSX2_DIAG_CTRL_SHIFT ;
   voffset= (data & DSC1B0_DSC_DIAG_CTRL0_VOFFSET_MASK) >>
            DSC1B0_DSC_DIAG_CTRL0_VOFFSET_SHIFT ;
   hoffset= (data & DSC1B0_DSC_DIAG_CTRL0_HOFFSET_MASK) >>
            DSC1B0_DSC_DIAG_CTRL0_HOFFSET_SHIFT ;
   diagnostics_en= data & DSC1B0_DSC_DIAG_CTRL0_DIAGNOSTICS_EN_MASK ;

   printf(" DSC1B0_DSC_DIAG_CTRL0r(%0x) d=%0x osx2_diag_ctrl=%0x voffset=%0x\n",
          ws->accAddr, data, osx2_diag_ctrl, voffset ) ;
   printf("   hoffset=%0x diagnostics_en=%0x\n",
          hoffset, diagnostics_en ) ;


   /* 0xC20E */

   SOC_IF_ERROR_RETURN ( READ_WC40_DSC1B0_UC_CTRLr(ws->unit,ws,&data));

   supplement_info= (data & DSC1B0_UC_CTRL_SUPPLEMENT_INFO_MASK) >>
                    DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT ;
   ready_for_cmd= (data & DSC1B0_UC_CTRL_READY_FOR_CMD_MASK) >>
                  DSC1B0_UC_CTRL_READY_FOR_CMD_SHIFT ;
   error_found= (data & DSC1B0_UC_CTRL_ERROR_FOUND_MASK) >>
                DSC1B0_UC_CTRL_ERROR_FOUND_SHIFT ;
   cmd_info= (data & DSC1B0_UC_CTRL_CMD_INFO_MASK) >>
             DSC1B0_UC_CTRL_CMD_INFO_SHIFT ;
   gp_uC_req= data & DSC1B0_UC_CTRL_GP_UC_REQ_MASK ;

   printf(" DSC1B0_UC_CTRLr(%0x) d=%0x supplement_info=%0x ready_for_cmd=%0x\n",
          ws->accAddr, data, supplement_info, ready_for_cmd ) ;
   printf("   error_found=%0x cmd_info=%0x gp_uC_req=%0x\n",
          error_found, cmd_info, gp_uC_req ) ;


   /* 0xC210 - 0xC21E, Dsc2b0: DSC Acquisition FSM & Analog Control Registers */

   /* 0xC210 */

   SOC_IF_ERROR_RETURN ( READ_WC40_DSC2B0_ACQ_SM_CTRL0r(ws->unit,ws,&data));

   cl72_timer_en= (data & DSC2B0_ACQ_SM_CTRL0_CL72_TIMER_EN_MASK) >>
                  DSC2B0_ACQ_SM_CTRL0_CL72_TIMER_EN_SHIFT ;
   acq2_timeout= (data & DSC2B0_ACQ_SM_CTRL0_ACQ2_TIMEOUT_MASK) >>
                 DSC2B0_ACQ_SM_CTRL0_ACQ2_TIMEOUT_SHIFT ;
   acq1_timeout= (data & DSC2B0_ACQ_SM_CTRL0_ACQ1_TIMEOUT_MASK) >>
                 DSC2B0_ACQ_SM_CTRL0_ACQ1_TIMEOUT_SHIFT ;
   acqcdr_timeout= data & DSC2B0_ACQ_SM_CTRL0_ACQCDR_TIMEOUT_MASK ;

   printf(" DSC2B0_ACQ_SM_CTRL0r(%0x) d=%0x cl72_timer_en=%0x acq2_timeout=%0x\n",
          ws->accAddr, data, cl72_timer_en, acq2_timeout ) ;
   printf("  acq1_timeout=%0x acqcdr_timeout=%0x\n",
          acq1_timeout, acqcdr_timeout ) ;


   /* 0xC211 */

   SOC_IF_ERROR_RETURN ( READ_WC40_DSC2B0_ACQ_SM_CTRL1r(ws->unit,ws,&data));

   acqvga_timeout= (data & DSC2B0_ACQ_SM_CTRL1_ACQVGA_TIMEOUT_MASK) >>
                   DSC2B0_ACQ_SM_CTRL1_ACQVGA_TIMEOUT_SHIFT ;
   bypass_os_integ_xfer= (data & DSC2B0_ACQ_SM_CTRL1_BYPASS_OS_INTEG_XFER_MASK) >>
                         DSC2B0_ACQ_SM_CTRL1_BYPASS_OS_INTEG_XFER_SHIFT ;
   dfe_frzval= (data & DSC2B0_ACQ_SM_CTRL1_DFE_FRZVAL_MASK) >>
               DSC2B0_ACQ_SM_CTRL1_DFE_FRZVAL_SHIFT ;
   dfe_frcfrz= (data & DSC2B0_ACQ_SM_CTRL1_DFE_FRCFRZ_MASK) >>
               DSC2B0_ACQ_SM_CTRL1_DFE_FRCFRZ_SHIFT ;
   vga_frzval= (data & DSC2B0_ACQ_SM_CTRL1_VGA_FRZVAL_MASK) >>
               DSC2B0_ACQ_SM_CTRL1_VGA_FRZVAL_SHIFT ;
   vga_frcfrz= (data & DSC2B0_ACQ_SM_CTRL1_VGA_FRCFRZ_MASK) >>
               DSC2B0_ACQ_SM_CTRL1_VGA_FRCFRZ_SHIFT ;
   dsc_clr_val= (data & DSC2B0_ACQ_SM_CTRL1_DSC_CLR_VAL_MASK) >>
                DSC2B0_ACQ_SM_CTRL1_DSC_CLR_VAL_SHIFT ;
   dsc_clr_frc= data & DSC2B0_ACQ_SM_CTRL1_DSC_CLR_FRC_MASK ;

   printf(" DSC2B0_ACQ_SM_CTRL1r(%0x) d=%0x acqvga_timeout=%0x bypass_os_integ_xfer=%0x\n",
          ws->accAddr, data, acqvga_timeout, bypass_os_integ_xfer ) ;
   printf("   dfe_frzval=%0x dfe_frcfrz=%0x vga_frzval=%0x vga_frcfrz=%0x\n",
          dfe_frzval, dfe_frcfrz, vga_frzval, vga_frcfrz ) ;
   printf("   dsc_clr_val=%0x dsc_clr_frc=%0x\n",
          dsc_clr_val, dsc_clr_frc ) ;


   /* 0xC212 */

   SOC_IF_ERROR_RETURN ( READ_WC40_DSC2B0_ACQ_SM_CTRL2r(ws->unit,ws,&data));

   sw_handover= (data & DSC2B0_ACQ_SM_CTRL2_SW_HANDOVER_MASK) >>
                DSC2B0_ACQ_SM_CTRL2_SW_HANDOVER_SHIFT ;
   eee_acq_2_timeout= (data & DSC2B0_ACQ_SM_CTRL2_EEE_ACQ_2_TIMEOUT_MASK) >>
                      DSC2B0_ACQ_SM_CTRL2_EEE_ACQ_2_TIMEOUT_SHIFT ;
   eee_acq_phase_timeout= (data & DSC2B0_ACQ_SM_CTRL2_EEE_ACQ_PHASE_TIMEOUT_MASK) >>
                          DSC2B0_ACQ_SM_CTRL2_EEE_ACQ_PHASE_TIMEOUT_SHIFT ;
   skip_acq= (data & DSC2B0_ACQ_SM_CTRL2_SKIP_ACQ_MASK) >>
             DSC2B0_ACQ_SM_CTRL2_SKIP_ACQ_SHIFT ;
   train2_req= (data & DSC2B0_ACQ_SM_CTRL2_TRAIN2_REQ_MASK) >>
               DSC2B0_ACQ_SM_CTRL2_TRAIN2_REQ_SHIFT ;
   train1_req= (data & DSC2B0_ACQ_SM_CTRL2_TRAIN1_REQ_MASK) >>
               DSC2B0_ACQ_SM_CTRL2_TRAIN1_REQ_SHIFT ;
   soft_ack= (data & DSC2B0_ACQ_SM_CTRL2_SOFT_ACK_MASK) >>
             DSC2B0_ACQ_SM_CTRL2_SOFT_ACK_SHIFT ;
   train_mode_en= data & DSC2B0_ACQ_SM_CTRL2_TRAIN_MODE_EN_MASK ;

   printf(" DSC2B0_ACQ_SM_CTRL2r(%0x) d=%0x sw_handover=%0x eee_acq_2_timeout=%0x\n",
          ws->accAddr, data, sw_handover, eee_acq_2_timeout) ;
   printf("   eee_acq_phase_timeout=%0x skip_acq=%0x train2_req=%0x train1_req=%0x\n",
          eee_acq_phase_timeout, skip_acq, train2_req, train1_req  ) ;
   printf("   soft_ack=%0x train_mode_en=%0x\n",
          soft_ack, train_mode_en ) ;


   /* 0xC213 */

   SOC_IF_ERROR_RETURN ( READ_WC40_DSC2B0_ACQ_SM_CTRL3r(ws->unit,ws,&data));

   pause_level_trigger_en= (data & DSC2B0_ACQ_SM_CTRL3_PAUSE_LEVEL_TRIGGER_EN_MASK) >>
                           DSC2B0_ACQ_SM_CTRL3_PAUSE_LEVEL_TRIGGER_EN_SHIFT ;
   os_dfe_en= (data & DSC2B0_ACQ_SM_CTRL3_OS_DFE_EN_MASK) >>
              DSC2B0_ACQ_SM_CTRL3_OS_DFE_EN_SHIFT ;
   dfe_taps2_5_en= (data & DSC2B0_ACQ_SM_CTRL3_DFE_TAPS2_5_EN_MASK) >>
                   DSC2B0_ACQ_SM_CTRL3_DFE_TAPS2_5_EN_SHIFT ;
   dfe_gain_acq2= (data & DSC2B0_ACQ_SM_CTRL3_DFE_GAIN_ACQ2_MASK) >>
                  DSC2B0_ACQ_SM_CTRL3_DFE_GAIN_ACQ2_SHIFT ;
   dfe_gain_acq1= (data & DSC2B0_ACQ_SM_CTRL3_DFE_GAIN_ACQ1_MASK) >>
                  DSC2B0_ACQ_SM_CTRL3_DFE_GAIN_ACQ1_SHIFT ;
   vga_gain_acq2= (data & DSC2B0_ACQ_SM_CTRL3_VGA_GAIN_ACQ2_MASK) >>
                  DSC2B0_ACQ_SM_CTRL3_VGA_GAIN_ACQ2_SHIFT ;
   vga_gain_acq1= (data & DSC2B0_ACQ_SM_CTRL3_VGA_GAIN_ACQ1_MASK) >>
                  DSC2B0_ACQ_SM_CTRL3_VGA_GAIN_ACQ1_SHIFT ;
   vga_gain_acqcdr= (data & DSC2B0_ACQ_SM_CTRL3_VGA_GAIN_ACQCDR_MASK) >>
                    DSC2B0_ACQ_SM_CTRL3_VGA_GAIN_ACQCDR_SHIFT ;
   vga_gain_acqvga= data & DSC2B0_ACQ_SM_CTRL3_VGA_GAIN_ACQVGA_MASK ;

   printf(" DSC2B0_ACQ_SM_CTRL3r((%0x) d=%0x pause_level_trigger_en=%0x os_dfe_en=%0x\n",
          ws->accAddr, data, pause_level_trigger_en, os_dfe_en ) ;
   printf("   dfe_taps2_5_en=%0x dfe_gain_acq2=%0x dfe_gain_acq1=%0x vga_gain_acq2=%0x\n",
          dfe_taps2_5_en, dfe_gain_acq2, dfe_gain_acq1, vga_gain_acq2 ) ;
   printf("   vga_gain_acq1=%0x vga_gain_acqcdr=%0x vga_gain_acqvga=%0x\n",
          vga_gain_acq1, vga_gain_acqcdr, vga_gain_acqvga ) ;

   /* 0xC214 */

   SOC_IF_ERROR_RETURN ( READ_WC40_DSC2B0_ACQ_SM_CTRL4r(ws->unit,ws,&data));

   cdros45_bwsel_prop_offset= (data & DSC2B0_ACQ_SM_CTRL4_CDROS45_BWSEL_PROP_OFFSET_MASK) >>
                              DSC2B0_ACQ_SM_CTRL4_CDROS45_BWSEL_PROP_OFFSET_SHIFT ;
   cdros38_bwsel_prop_offset= (data & DSC2B0_ACQ_SM_CTRL4_CDROS38_BWSEL_PROP_OFFSET_MASK) >>
	                      DSC2B0_ACQ_SM_CTRL4_CDROS38_BWSEL_PROP_OFFSET_SHIFT ;
   cdros_bwsel_prop_acq1_2= (data & DSC2B0_ACQ_SM_CTRL4_CDROS_BWSEL_PROP_ACQ1_2_MASK) >>
                            DSC2B0_ACQ_SM_CTRL4_CDROS_BWSEL_PROP_ACQ1_2_SHIFT ;
   cdros_bwsel_prop_acqcdr= (data & DSC2B0_ACQ_SM_CTRL4_CDROS_BWSEL_PROP_ACQCDR_MASK) >>
                            DSC2B0_ACQ_SM_CTRL4_CDROS_BWSEL_PROP_ACQCDR_SHIFT ;
   cdros_bwsel_prop_acqvga= data & DSC2B0_ACQ_SM_CTRL4_CDROS_BWSEL_PROP_ACQVGA_MASK ;

   printf(" DSC2B0_ACQ_SM_CTRL4r(%0x) d=%0x cdros45_bwsel_prop_offset=%0x cdros38_bwsel_prop_offset=%0x\n",
          ws->accAddr, data, cdros45_bwsel_prop_offset, cdros38_bwsel_prop_offset ) ;
   printf("   cdros_bwsel_prop_acq1_2=%0x cdros_bwsel_prop_acqcdr=%0x cdros_bwsel_prop_acqvga=%0x\n",
          cdros_bwsel_prop_acq1_2, cdros_bwsel_prop_acqcdr, cdros_bwsel_prop_acqvga ) ;

   /* 0xC215 */

   SOC_IF_ERROR_RETURN ( READ_WC40_DSC2B0_ACQ_SM_CTRL5r(ws->unit,ws,&data));

   cdros_bwsel_integ_eee_acq2= (data & DSC2B0_ACQ_SM_CTRL5_CDROS_BWSEL_INTEG_EEE_ACQ2_MASK) >>
                               DSC2B0_ACQ_SM_CTRL5_CDROS_BWSEL_INTEG_EEE_ACQ2_SHIFT ;
   cdros_bwsel_integ_acq1_2= (data & DSC2B0_ACQ_SM_CTRL5_CDROS_BWSEL_INTEG_ACQ1_2_MASK) >>
                             DSC2B0_ACQ_SM_CTRL5_CDROS_BWSEL_INTEG_ACQ1_2_SHIFT ;
   cdros_bwsel_integ_acqcdr= (data & DSC2B0_ACQ_SM_CTRL5_CDROS_BWSEL_INTEG_ACQCDR_MASK) >>
                             DSC2B0_ACQ_SM_CTRL5_CDROS_BWSEL_INTEG_ACQCDR_SHIFT ;
   cdros_bwsel_integ_acqvga= data & DSC2B0_ACQ_SM_CTRL5_CDROS_BWSEL_INTEG_ACQVGA_MASK ;

   printf(" DSC2B0_ACQ_SM_CTRL5r(%0x) d=%0x cdros_bwsel_integ_eee_acq2=%0x cdros_bwsel_integ_acq1_2=%0x\n",
          ws->accAddr, data, cdros_bwsel_integ_eee_acq2, cdros_bwsel_integ_acq1_2 ) ;
   printf("   cdros_bwsel_integ_acqcdr=%0x cdros_bwsel_integ_acqvga=%0x\n",
          cdros_bwsel_integ_acqcdr, cdros_bwsel_integ_acqvga ) ;


   /* 0xC216 */

   SOC_IF_ERROR_RETURN ( READ_WC40_DSC2B0_ACQ_SM_CTRL6r(ws->unit,ws,&data));

   cdrbr_bwsel_prop_acq2= (data & DSC2B0_ACQ_SM_CTRL6_CDRBR_BWSEL_PROP_ACQ2_MASK) >>
                          DSC2B0_ACQ_SM_CTRL6_CDRBR_BWSEL_PROP_ACQ2_SHIFT ;
   cdrbr_bwsel_prop_acq1= (data & DSC2B0_ACQ_SM_CTRL6_CDRBR_BWSEL_PROP_ACQ1_MASK) >>
                          DSC2B0_ACQ_SM_CTRL6_CDRBR_BWSEL_PROP_ACQ1_SHIFT ;
   cdrbr_bwsel_prop_acqcdr= (data & DSC2B0_ACQ_SM_CTRL6_CDRBR_BWSEL_PROP_ACQCDR_MASK) >>
                            DSC2B0_ACQ_SM_CTRL6_CDRBR_BWSEL_PROP_ACQCDR_SHIFT ;
   cdrbr_bwsel_prop_acqphase= data & DSC2B0_ACQ_SM_CTRL6_CDRBR_BWSEL_PROP_ACQPHASE_MASK ;

   printf(" DSC2B0_ACQ_SM_CTRL6r(%0x) d=%0x cdrbr_bwsel_prop_acq2=%0x cdrbr_bwsel_prop_acq1=%0x\n",
          ws->accAddr, data, cdrbr_bwsel_prop_acq2, cdrbr_bwsel_prop_acq1 ) ;
   printf("   cdrbr_bwsel_prop_acqcdr=%0x cdrbr_bwsel_prop_acqphase=%0x\n",
          cdrbr_bwsel_prop_acqcdr, cdrbr_bwsel_prop_acqphase ) ;


   /* 0xC217 */

   SOC_IF_ERROR_RETURN ( READ_WC40_DSC2B0_ACQ_SM_CTRL7r(ws->unit,ws,&data));

   cdrbr_bwsel_integ_acq2= (data & DSC2B0_ACQ_SM_CTRL7_CDRBR_BWSEL_INTEG_ACQ2_MASK) >>
                           DSC2B0_ACQ_SM_CTRL7_CDRBR_BWSEL_INTEG_ACQ2_SHIFT ;
   cdrbr_bwsel_integ_acq1= (data & DSC2B0_ACQ_SM_CTRL7_CDRBR_BWSEL_INTEG_ACQ1_MASK) >>
                           DSC2B0_ACQ_SM_CTRL7_CDRBR_BWSEL_INTEG_ACQ1_SHIFT ;
   cdrbr_bwsel_integ_acqcdr= (data & DSC2B0_ACQ_SM_CTRL7_CDRBR_BWSEL_INTEG_ACQCDR_MASK) >>
                             DSC2B0_ACQ_SM_CTRL7_CDRBR_BWSEL_INTEG_ACQCDR_SHIFT ;
   cdrbr_bwsel_integ_acqphase= data & DSC2B0_ACQ_SM_CTRL7_CDRBR_BWSEL_INTEG_ACQPHASE_MASK ;

   printf(" DSC2B0_ACQ_SM_CTRL7r(%0x) d=%0x cdrbr_bwsel_integ_acq2=%0x cdrbr_bwsel_integ_acq1=%0x\n",
          ws->accAddr, data, cdrbr_bwsel_integ_acq2, cdrbr_bwsel_integ_acq1 ) ;
   printf("   cdrbr_bwsel_integ_acqcdr=%0x cdrbr_bwsel_integ_acqphase=%0x\n",
          cdrbr_bwsel_integ_acqcdr, cdrbr_bwsel_integ_acqphase ) ;


   /* 0xC218 */

   SOC_IF_ERROR_RETURN ( READ_WC40_DSC2B0_ACQ_SM_CTRL8r(ws->unit,ws,&data));

   cdros_bwsel_prop_eee_acqphase= (data & DSC2B0_ACQ_SM_CTRL8_CDROS_BWSEL_PROP_EEE_ACQPHASE_MASK) >>
                                  DSC2B0_ACQ_SM_CTRL8_CDROS_BWSEL_PROP_EEE_ACQPHASE_SHIFT ;
   phase_sat_ctrl_100fx= (data & DSC2B0_ACQ_SM_CTRL8_PHASE_SAT_CTRL_100FX_MASK) >>
                         DSC2B0_ACQ_SM_CTRL8_PHASE_SAT_CTRL_100FX_SHIFT ;
   pi_phase_step_mult_100fx= (data & DSC2B0_ACQ_SM_CTRL8_PI_PHASE_STEP_MULT_100FX_MASK) >>
                             DSC2B0_ACQ_SM_CTRL8_PI_PHASE_STEP_MULT_100FX_SHIFT ;
   cdros_bwsel_integ_100fx= (data & DSC2B0_ACQ_SM_CTRL8_CDROS_BWSEL_INTEG_100FX_MASK) >>
                            DSC2B0_ACQ_SM_CTRL8_CDROS_BWSEL_INTEG_100FX_SHIFT ;
   cdros_bwsel_prop_100fx= data & DSC2B0_ACQ_SM_CTRL8_CDROS_BWSEL_PROP_100FX_MASK ;

   printf(" DSC2B0_ACQ_SM_CTRL8r(%0x) d=%0x cdros_bwsel_prop_eee_acqphase=%0x phase_sat_ctrl_100fx=%0x\n",
          ws->accAddr, data, cdros_bwsel_prop_eee_acqphase, phase_sat_ctrl_100fx ) ;
   printf("   pi_phase_step_mult_100fx=%0x cdros_bwsel_integ_100fx=%0x cdros_bwsel_prop_100fx=%0x\n",
          pi_phase_step_mult_100fx, cdros_bwsel_integ_100fx, cdros_bwsel_prop_100fx ) ;


   /* 0xC219 */

   SOC_IF_ERROR_RETURN ( READ_WC40_DSC2B0_DSC_ANA_CTRL0r(ws->unit,ws,&data));

   force_p1_ctrl_strobe= (data & DSC2B0_DSC_ANA_CTRL0_FORCE_P1_CTRL_STROBE_MASK) >>
                         DSC2B0_DSC_ANA_CTRL0_FORCE_P1_CTRL_STROBE_SHIFT ;
   force_p1_odd_ctrl= (data & DSC2B0_DSC_ANA_CTRL0_FORCE_P1_ODD_CTRL_MASK) >>
                      DSC2B0_DSC_ANA_CTRL0_FORCE_P1_ODD_CTRL_SHIFT ;
   force_p1_evn_ctrl= (data & DSC2B0_DSC_ANA_CTRL0_FORCE_P1_EVN_CTRL_MASK) >>
                      DSC2B0_DSC_ANA_CTRL0_FORCE_P1_EVN_CTRL_SHIFT ;
   p1_odd_ctrl= (data & DSC2B0_DSC_ANA_CTRL0_P1_ODD_CTRL_MASK) >>
                DSC2B0_DSC_ANA_CTRL0_P1_ODD_CTRL_SHIFT ;
   p1_evn_ctrl= data & DSC2B0_DSC_ANA_CTRL0_P1_EVN_CTRL_MASK ;

   printf(" DSC2B0_DSC_ANA_CTRL0r(%0x) d=%0x force_p1_ctrl_strobe=%0x force_p1_odd_ctrl=%0x\n",
          ws->accAddr, data, force_p1_ctrl_strobe, force_p1_odd_ctrl ) ;
   printf("   force_p1_evn_ctrl=%0x p1_odd_ctrl=%0x p1_evn_ctrl=%0x\n",
          force_p1_evn_ctrl, p1_odd_ctrl, p1_evn_ctrl ) ;


   /* 0xC21A */

   SOC_IF_ERROR_RETURN ( READ_WC40_DSC2B0_DSC_ANA_CTRL1r(ws->unit,ws,&data));

   force_d_ctrl_strobe= (data & DSC2B0_DSC_ANA_CTRL1_FORCE_D_CTRL_STROBE_MASK) >>
                        DSC2B0_DSC_ANA_CTRL1_FORCE_D_CTRL_STROBE_SHIFT ;
   force_d_odd_ctrl= (data & DSC2B0_DSC_ANA_CTRL1_FORCE_D_ODD_CTRL_MASK) >>
                     DSC2B0_DSC_ANA_CTRL1_FORCE_D_ODD_CTRL_SHIFT ;
   force_d_evn_ctrl= (data & DSC2B0_DSC_ANA_CTRL1_FORCE_D_EVN_CTRL_MASK) >>
                     DSC2B0_DSC_ANA_CTRL1_FORCE_D_EVN_CTRL_SHIFT ;
   d_odd_ctrl= (data & DSC2B0_DSC_ANA_CTRL1_D_ODD_CTRL_MASK) >>
               DSC2B0_DSC_ANA_CTRL1_D_ODD_CTRL_SHIFT ;
   d_evn_ctrl= data & DSC2B0_DSC_ANA_CTRL1_D_EVN_CTRL_MASK ;

   printf(" DSC2B0_DSC_ANA_CTRL1r(%0x) d=%0x force_d_ctrl_strobe=%0x force_d_odd_ctrl=%0x\n",
          ws->accAddr, data, force_d_ctrl_strobe, force_d_odd_ctrl ) ;
   printf("   force_d_evn_ctrl=%0x d_odd_ctrl=%0x d_evn_ctrl=%0x\n",
          force_d_evn_ctrl, d_odd_ctrl, d_evn_ctrl ) ;


   /* 0xC21B */

   SOC_IF_ERROR_RETURN ( READ_WC40_DSC2B0_DSC_ANA_CTRL2r(ws->unit,ws,&data));

   force_m1_ctrl_strobe= (data & DSC2B0_DSC_ANA_CTRL2_FORCE_M1_CTRL_STROBE_MASK) >>
                         DSC2B0_DSC_ANA_CTRL2_FORCE_M1_CTRL_STROBE_SHIFT ;
   force_m1_odd_ctrl= (data & DSC2B0_DSC_ANA_CTRL2_FORCE_M1_ODD_CTRL_MASK) >>
                      DSC2B0_DSC_ANA_CTRL2_FORCE_M1_ODD_CTRL_SHIFT ;
   force_m1_evn_ctrl= (data & DSC2B0_DSC_ANA_CTRL2_FORCE_M1_EVN_CTRL_MASK) >>
                      DSC2B0_DSC_ANA_CTRL2_FORCE_M1_EVN_CTRL_SHIFT ;
   m1_odd_ctrl= (data & DSC2B0_DSC_ANA_CTRL2_M1_ODD_CTRL_MASK) >>
                DSC2B0_DSC_ANA_CTRL2_M1_ODD_CTRL_SHIFT ;
   m1_evn_ctrl= data & DSC2B0_DSC_ANA_CTRL2_M1_EVN_CTRL_MASK ;

   printf(" DSC2B0_DSC_ANA_CTRL2r(%0x) d=%0x force_m1_ctrl_strobe=%0x force_m1_odd_ctrl=%0x\n",
          ws->accAddr, data, force_m1_ctrl_strobe, force_m1_odd_ctrl ) ;
   printf("   force_m1_evn_ctrl=%0x m1_odd_ctrl=%0x m1_evn_ctrl=%0x\n",
          force_m1_evn_ctrl, m1_odd_ctrl, m1_evn_ctrl ) ;


   /* 0xC21C */

   SOC_IF_ERROR_RETURN ( READ_WC40_DSC2B0_DSC_ANA_CTRL3r(ws->unit,ws,&data));

   rx_pf_hiz= (data & DSC2B0_DSC_ANA_CTRL3_RX_PF_HIZ_MASK) >>
              DSC2B0_DSC_ANA_CTRL3_RX_PF_HIZ_SHIFT ;
   force_rx_m1_thresh_zero= (data & DSC2B0_DSC_ANA_CTRL3_FORCE_RX_M1_THRESH_ZERO_MASK) >>
                            DSC2B0_DSC_ANA_CTRL3_FORCE_RX_M1_THRESH_ZERO_SHIFT ;
   rx_m1_thresh_zero= (data & DSC2B0_DSC_ANA_CTRL3_RX_M1_THRESH_ZERO_MASK) >>
                      DSC2B0_DSC_ANA_CTRL3_RX_M1_THRESH_ZERO_SHIFT ;
   rx_thresh_sel= (data & DSC2B0_DSC_ANA_CTRL3_RX_THRESH_SEL_MASK) >>
                  DSC2B0_DSC_ANA_CTRL3_RX_THRESH_SEL_SHIFT ;
   force_rx_pf_ctrl= (data & DSC2B0_DSC_ANA_CTRL3_FORCE_RX_PF_CTRL_MASK) >>
                     DSC2B0_DSC_ANA_CTRL3_FORCE_RX_PF_CTRL_SHIFT ;
   rx_pf_ctrl= data & DSC2B0_DSC_ANA_CTRL3_RX_PF_CTRL_MASK ;

   printf(" DSC2B0_DSC_ANA_CTRL3r(%0x) d=%0x rx_pf_hiz=%0x force_rx_m1_thresh_zero=%0x\n",
          ws->accAddr, data, rx_pf_hiz, force_rx_m1_thresh_zero ) ;
   printf("   rx_m1_thresh_zero=%0x rx_thresh_sel=%0x force_rx_pf_ctrl=%0x rx_pf_ctrl=%0x\n",
          rx_m1_thresh_zero, rx_thresh_sel, force_rx_pf_ctrl, rx_pf_ctrl ) ;


   /* 0xC21D */

   SOC_IF_ERROR_RETURN ( READ_WC40_DSC2B0_DSC_ANA_CTRL4r(ws->unit,ws,&data));

   br_offset_pd= (data & DSC2B0_DSC_ANA_CTRL4_BR_OFFSET_PD_MASK) >>
                 DSC2B0_DSC_ANA_CTRL4_BR_OFFSET_PD_SHIFT ;
   br_en_hgain= (data & DSC2B0_DSC_ANA_CTRL4_BR_EN_HGAIN_MASK) >>
                DSC2B0_DSC_ANA_CTRL4_BR_EN_HGAIN_SHIFT ;
   br_en_dfe_clk= (data & DSC2B0_DSC_ANA_CTRL4_BR_EN_DFE_CLK_MASK) >>
                  DSC2B0_DSC_ANA_CTRL4_BR_EN_DFE_CLK_SHIFT ;
   br_pd_ch_p1= (data & DSC2B0_DSC_ANA_CTRL4_BR_PD_CH_P1_MASK) >>
                DSC2B0_DSC_ANA_CTRL4_BR_PD_CH_P1_SHIFT ;
   osr_offset_pd= (data & DSC2B0_DSC_ANA_CTRL4_OSR_OFFSET_PD_MASK) >>
                  DSC2B0_DSC_ANA_CTRL4_OSR_OFFSET_PD_SHIFT ;
   osr_en_hgain= (data & DSC2B0_DSC_ANA_CTRL4_OSR_EN_HGAIN_MASK) >>
                 DSC2B0_DSC_ANA_CTRL4_OSR_EN_HGAIN_SHIFT ;
   osr_en_dfe_clk= (data & DSC2B0_DSC_ANA_CTRL4_OSR_EN_DFE_CLK_MASK) >>
                   DSC2B0_DSC_ANA_CTRL4_OSR_EN_DFE_CLK_SHIFT ;
   osr_pd_ch_p1= data & DSC2B0_DSC_ANA_CTRL4_OSR_PD_CH_P1_MASK ;

   printf(" DSC2B0_DSC_ANA_CTRL4r(%0x) d=%0x br_offset_pd=%0x br_en_hgain=%0x\n",
          ws->accAddr, data, br_offset_pd, br_en_hgain ) ;
   printf("   br_en_dfe_clk=%0x br_pd_ch_p1=%0x osr_offset_pd=%0x osr_en_hgain=%0x\n",
          br_en_dfe_clk, br_pd_ch_p1, osr_offset_pd, osr_en_hgain ) ;
   printf("   osr_en_dfe_clk=%0x osr_pd_ch_p1=%0x\n",
          osr_en_dfe_clk, osr_pd_ch_p1 ) ;


   tscmod_diag_rd_dsc_misc_ctrl0r(ws) ;


   /* 0xC220 - 0xC22E, Dsc3b0: DSC Status Registers */

   /* 0xC220 */

   SOC_IF_ERROR_RETURN ( READ_WC40_DSC3B0_CDR_STATUS0r(ws->unit,ws,&data));

   integ_reg= data & DSC3B0_CDR_STATUS0_INTEG_REG_MASK ;

   printf(" DSC3B0_CDR_STATUS0r(%0x) d=%0x integ_reg=%0x\n",
          ws->accAddr, data, integ_reg ) ;


   /* 0xC221 */

   SOC_IF_ERROR_RETURN ( READ_WC40_DSC3B0_CDR_STATUS1r(ws->unit,ws,&data));

   integ_reg_xfer= data & DSC3B0_CDR_STATUS1_INTEG_REG_XFER_MASK ;

   printf(" DSC3B0_CDR_STATUS1r(%0x) d=%0x integ_reg_xfer=%0x\n",
          ws->accAddr, data, integ_reg_xfer ) ;


   /* 0xC222 */

   SOC_IF_ERROR_RETURN ( READ_WC40_DSC3B0_CDR_STATUS2r(ws->unit,ws,&data));

   rx_fifo_os8_error= (data & DSC3B0_CDR_STATUS2_RX_FIFO_OS8_ERROR_MASK) >>
                      DSC3B0_CDR_STATUS2_RX_FIFO_OS8_ERROR_SHIFT ;
   sm_br_cdr_enabled= (data & DSC3B0_CDR_STATUS2_SM_BR_CDR_ENABLED_MASK) >>
                      DSC3B0_CDR_STATUS2_SM_BR_CDR_ENABLED_SHIFT ;
   br_cdr_enabled= (data & DSC3B0_CDR_STATUS2_BR_CDR_ENABLED_MASK) >>
                   DSC3B0_CDR_STATUS2_BR_CDR_ENABLED_SHIFT ;
   oscdr_mode= (data & DSC3B0_CDR_STATUS2_OSCDR_MODE_MASK) >>
               DSC3B0_CDR_STATUS2_OSCDR_MODE_SHIFT ;
   phase_err= data &  DSC3B0_CDR_STATUS2_PHASE_ERR_MASK;

   printf(" DSC3B0_CDR_STATUS2r(%0x) d=%0x rx_fifo_os8_error=%0x sm_br_cdr_enabled=%0x\n",
          ws->accAddr, data, rx_fifo_os8_error, sm_br_cdr_enabled ) ;
   printf("   br_cdr_enabled=%0x oscdr_mode=%0x phase_err=%0x\n",
          br_cdr_enabled, oscdr_mode, phase_err ) ;


   /* 0xC223 */

   SOC_IF_ERROR_RETURN ( READ_WC40_DSC3B0_PI_STATUS0r(ws->unit,ws,&data));

   clk90_phase_offset= (data & DSC3B0_PI_STATUS0_CLK90_PHASE_OFFSET_MASK) >>
                       DSC3B0_PI_STATUS0_CLK90_PHASE_OFFSET_SHIFT ;
   phase_cntr= data & DSC3B0_PI_STATUS0_PHASE_CNTR_MASK ;

   printf(" DSC3B0_PI_STATUS0r(%0x) d=%0x clk90_phase_offset=%0x phase_cntr=%0x\n",
          ws->accAddr, data, clk90_phase_offset, phase_cntr ) ;


   /* 0xC224 */

   SOC_IF_ERROR_RETURN ( READ_WC40_DSC3B0_PI_STATUS1r(ws->unit,ws,&data));

   phs_interp_status= data &  DSC3B0_PI_STATUS1_PHS_INTERP_STATUS_MASK;

   printf(" DSC3B0_PI_STATUS1r(%0x) d=%0x phs_interp_status=%0x\n",
          ws->accAddr, data, phs_interp_status ) ;


   /* 0xC225 */

   SOC_IF_ERROR_RETURN ( READ_WC40_DSC3B0_DFE_VGA_STATUS0r(ws->unit,ws,&data));

   dfe_tap_1_bin= (data & DSC3B0_DFE_VGA_STATUS0_DFE_TAP_1_BIN_MASK) >>
                  DSC3B0_DFE_VGA_STATUS0_DFE_TAP_1_BIN_SHIFT ;
   vga_sum= data & DSC3B0_DFE_VGA_STATUS0_VGA_SUM_MASK ;

   printf(" DSC3B0_DFE_VGA_STATUS0r(%0x) d=%0x dfe_tap_1_bin=%0x vga_sum=%0x\n",
          ws->accAddr, data, dfe_tap_1_bin, vga_sum ) ;


   /* 0xC226 */

   SOC_IF_ERROR_RETURN ( READ_WC40_DSC3B0_DFE_VGA_STATUS1r(ws->unit,ws,&data));

   dfe_tap_3_bin= (data & DSC3B0_DFE_VGA_STATUS1_DFE_TAP_3_BIN_MASK) >>
                  DSC3B0_DFE_VGA_STATUS1_DFE_TAP_3_BIN_SHIFT ;
   dfe_tap_2_bin= data & DSC3B0_DFE_VGA_STATUS1_DFE_TAP_2_BIN_MASK ;

   printf(" DSC3B0_DFE_VGA_STATUS1r(%0x) d=%0x dfe_tap_3_bin=%0x dfe_tap_2_bin=%0x\n",
          ws->accAddr, data, dfe_tap_3_bin, dfe_tap_2_bin ) ;


   /* 0xC227 */

   SOC_IF_ERROR_RETURN ( READ_WC40_DSC3B0_DFE_VGA_STATUS2r(ws->unit,ws,&data));

   dfe_tap_5_bin= (data & DSC3B0_DFE_VGA_STATUS2_DFE_TAP_5_BIN_MASK) >>
                  DSC3B0_DFE_VGA_STATUS2_DFE_TAP_5_BIN_SHIFT ;
   dfe_tap_4_bin= data & DSC3B0_DFE_VGA_STATUS2_DFE_TAP_4_BIN_MASK ;

   printf(" DSC3B0_DFE_VGA_STATUS2r(%0x) d=%0x dfe_tap_5_bin=%0x dfe_tap_4_bin=%0x\n",
          ws->accAddr, data, dfe_tap_5_bin, dfe_tap_4_bin ) ;


   /* 0xC228 */

   SOC_IF_ERROR_RETURN ( READ_WC40_DSC3B0_DFE_VGA_STATUS3r(ws->unit,ws,&data));

   trnsum= data & DSC3B0_DFE_VGA_STATUS3_TRNSUM_MASK ;

   printf(" DSC3B0_DFE_VGA_STATUS3r(%0x) d=%0x trnsum=%0x\n",
          ws->accAddr, data, trnsum ) ;


   /* 0xC229 */

   SOC_IF_ERROR_RETURN ( READ_WC40_DSC3B0_ACQ_SM_STATUS0r(ws->unit,ws,&data));


   dsc_state_eee_acq_2= (data & DSC3B0_ACQ_SM_STATUS0_DSC_STATE_EEE_ACQ_2_MASK) >>
                        DSC3B0_ACQ_SM_STATUS0_DSC_STATE_EEE_ACQ_2_SHIFT ;
   dsc_state= data & DSC3B0_ACQ_SM_STATUS0_DSC_STATE_MASK ;

   printf("DSC3B0_ACQ_SM_STATUS0r (%0x) d=%0x dsc_state_eee_acq_2=%0x dsc_state=%0x\n",
          ws->accAddr, data, dsc_state_eee_acq_2, dsc_state ) ;


   /* 0xC22A */

   SOC_IF_ERROR_RETURN ( READ_WC40_DSC3B0_ACQ_SM_STATUS1r(ws->unit,ws,&data));

   dsc_state_eee_acq_cdr_phase= (data & DSC3B0_ACQ_SM_STATUS1_DSC_STATE_EEE_ACQ_CDR_PHASE_MASK) >>
                                DSC3B0_ACQ_SM_STATUS1_DSC_STATE_EEE_ACQ_CDR_PHASE_SHIFT ;
   dsc_state_eee_quiet= (data & DSC3B0_ACQ_SM_STATUS1_DSC_STATE_EEE_QUIET_MASK) >>
                        DSC3B0_ACQ_SM_STATUS1_DSC_STATE_EEE_QUIET_SHIFT ;
   dsc_state_done= (data & DSC3B0_ACQ_SM_STATUS1_DSC_STATE_DONE_MASK) >>
                   DSC3B0_ACQ_SM_STATUS1_DSC_STATE_DONE_SHIFT ;
   dsc_state_measure= (data & DSC3B0_ACQ_SM_STATUS1_DSC_STATE_MEASURE_MASK) >>
                      DSC3B0_ACQ_SM_STATUS1_DSC_STATE_MEASURE_SHIFT ;
   dsc_state_train_2= (data & DSC3B0_ACQ_SM_STATUS1_DSC_STATE_TRAIN_2_MASK) >>
                      DSC3B0_ACQ_SM_STATUS1_DSC_STATE_TRAIN_2_SHIFT ;
   dsc_state_train_1= (data & DSC3B0_ACQ_SM_STATUS1_DSC_STATE_TRAIN_1_MASK) >>
                      DSC3B0_ACQ_SM_STATUS1_DSC_STATE_TRAIN_1_SHIFT ;
   dsc_state_soft_ack= (data & DSC3B0_ACQ_SM_STATUS1_DSC_STATE_SOFT_ACK_MASK) >>
                       DSC3B0_ACQ_SM_STATUS1_DSC_STATE_SOFT_ACK_SHIFT ;
   dsc_state_acq_2= (data & DSC3B0_ACQ_SM_STATUS1_DSC_STATE_ACQ_2_MASK) >>
                    DSC3B0_ACQ_SM_STATUS1_DSC_STATE_ACQ_2_SHIFT ;
   dsc_state_acq_1= (data & DSC3B0_ACQ_SM_STATUS1_DSC_STATE_ACQ_1_MASK) >>
                    DSC3B0_ACQ_SM_STATUS1_DSC_STATE_ACQ_1_SHIFT ;
   dsc_state_acq_cdr= (data & DSC3B0_ACQ_SM_STATUS1_DSC_STATE_ACQ_CDR_MASK) >>
                      DSC3B0_ACQ_SM_STATUS1_DSC_STATE_ACQ_CDR_SHIFT ;
   dsc_state_acq_cdrbr_phase= (data & DSC3B0_ACQ_SM_STATUS1_DSC_STATE_ACQ_CDRBR_PHASE_MASK) >>
                              DSC3B0_ACQ_SM_STATUS1_DSC_STATE_ACQ_CDRBR_PHASE_SHIFT ;
   dsc_state_os_integ_xfer= (data & DSC3B0_ACQ_SM_STATUS1_DSC_STATE_OS_INTEG_XFER_MASK) >>
                            DSC3B0_ACQ_SM_STATUS1_DSC_STATE_OS_INTEG_XFER_SHIFT ;
   dsc_state_acq_vga= (data & DSC3B0_ACQ_SM_STATUS1_DSC_STATE_ACQ_VGA_MASK) >>
                      DSC3B0_ACQ_SM_STATUS1_DSC_STATE_ACQ_VGA_SHIFT ;
   dsc_state_setup= (data & DSC3B0_ACQ_SM_STATUS1_DSC_STATE_SETUP_MASK) >>
                    DSC3B0_ACQ_SM_STATUS1_DSC_STATE_SETUP_SHIFT ;
   dsc_state_pause= (data & DSC3B0_ACQ_SM_STATUS1_DSC_STATE_PAUSE_MASK) >>
                    DSC3B0_ACQ_SM_STATUS1_DSC_STATE_PAUSE_SHIFT ;
   dsc_state_init= data & DSC3B0_ACQ_SM_STATUS1_DSC_STATE_INIT_MASK ;

   printf(" DSC3B0_ACQ_SM_STATUS1r(%0x) d=%0x dsc_state_eee_acq_cdr_phase=%0x dsc_state_eee_quiet=%0x\n",
          ws->accAddr, data, dsc_state_eee_acq_cdr_phase, dsc_state_eee_quiet ) ;
   printf("   dsc_state_done=%0x dsc_state_measure=%0x dsc_state_train_2=%0x dsc_state_train_1=%0x\n",
          dsc_state_done, dsc_state_measure, dsc_state_train_2, dsc_state_train_1 ) ;
   printf("   dsc_state_soft_ack=%0x dsc_state_acq_2=%0x dsc_state_acq_1=%0x dsc_state_acq_cdr=%0x\n",
          dsc_state_soft_ack, dsc_state_acq_2, dsc_state_acq_1, dsc_state_acq_cdr ) ;
   printf("   dsc_state_acq_cdrbr_phase=%0x dsc_state_os_integ_xfer=%0x dsc_state_acq_vga=%0x\n",
          dsc_state_acq_cdrbr_phase, dsc_state_os_integ_xfer, dsc_state_acq_vga ) ;
   printf("   dsc_state_setup=%0x dsc_state_pause=%0x dsc_state_init=%0x\n",
          dsc_state_setup, dsc_state_pause, dsc_state_init ) ;


   /* 0xC22B */

   SOC_IF_ERROR_RETURN ( READ_WC40_DSC3B0_ANA_STATUS0r(ws->unit,ws,&data));

   pd_ch_p1= (data & DSC3B0_ANA_STATUS0_PD_CH_P1_MASK) >>
             DSC3B0_ANA_STATUS0_PD_CH_P1_SHIFT ;
   en_dfe_clk= (data & DSC3B0_ANA_STATUS0_EN_DFE_CLK_MASK) >>
               DSC3B0_ANA_STATUS0_EN_DFE_CLK_SHIFT ;
   en_hgain= (data & DSC3B0_ANA_STATUS0_EN_HGAIN_MASK) >>
             DSC3B0_ANA_STATUS0_EN_HGAIN_SHIFT ;
   offset_pd= (data & DSC3B0_ANA_STATUS0_OFFSET_PD_MASK) >>
              DSC3B0_ANA_STATUS0_OFFSET_PD_SHIFT ;
   pf_ctrl_bin= data & DSC3B0_ANA_STATUS0_PF_CTRL_BIN_MASK ;

   printf(" DSC3B0_ANA_STATUS0r(%0x) d=%0x pd_ch_p1=%0x en_dfe_clk=%0x\n",
          ws->accAddr, data, pd_ch_p1, en_dfe_clk ) ;
   printf("   en_hgain=%0x offset_pd=%0x pf_ctrl_bin=%0x\n",
          en_hgain, offset_pd, pf_ctrl_bin ) ;


   /* 0xC22C */

   SOC_IF_ERROR_RETURN ( READ_WC40_DSC3B0_ANA_STATUS1r(ws->unit,ws,&data));

   slicer_offset_po= (data & DSC3B0_ANA_STATUS1_SLICER_OFFSET_PO_MASK) >>
                     DSC3B0_ANA_STATUS1_SLICER_OFFSET_PO_SHIFT ;
   slicer_offset_pe= data & DSC3B0_ANA_STATUS1_SLICER_OFFSET_PE_MASK ;

   printf(" DSC3B0_ANA_STATUS1r(%0x) d=%0x slicer_offset_po=%0x slicer_offset_pe=%0x\n",
          ws->accAddr, data, slicer_offset_po, slicer_offset_pe ) ;


   /* 0xC22D */

   SOC_IF_ERROR_RETURN ( READ_WC40_DSC3B0_ANA_STATUS2r(ws->unit,ws,&data));

   slicer_offset_zo= (data & DSC3B0_ANA_STATUS2_SLICER_OFFSET_ZO_MASK) >>
                     DSC3B0_ANA_STATUS2_SLICER_OFFSET_ZO_SHIFT ;
   slicer_offset_ze= data & DSC3B0_ANA_STATUS2_SLICER_OFFSET_ZE_MASK ;

   printf(" DSC3B0_ANA_STATUS2r(%0x) d=%0x slicer_offset_zo=%0x slicer_offset_ze=%0x\n",
          ws->accAddr, data, slicer_offset_zo, slicer_offset_ze ) ;


   /* 0xC22E */

   SOC_IF_ERROR_RETURN ( READ_WC40_DSC3B0_ANA_STATUS3r(ws->unit,ws,&data));

   slicer_offset_mo= (data & DSC3B0_ANA_STATUS3_SLICER_OFFSET_MO_MASK) >>
                     DSC3B0_ANA_STATUS3_SLICER_OFFSET_MO_SHIFT ;
   slicer_offset_me= data & DSC3B0_ANA_STATUS3_SLICER_OFFSET_ME_MASK ;

   printf(" DSC3B0_ANA_STATUS3r(%0x) d=%0x slicer_offset_mo=%0x slicer_offset_me=%0x\n",
          ws->accAddr, data, slicer_offset_mo, slicer_offset_me ) ;



   /* 0xC230 - 0xC23C, Dsc4b0: DSC Tuning FSM Control Registers */

   /* 0xC230 */

   SOC_IF_ERROR_RETURN ( READ_WC40_DSC4B0_TUNING_SM_CTRL0r(ws->unit,ws,&data));

   bypass_tx_postc_cal= (data & DSC4B0_TUNING_SM_CTRL0_BYPASS_TX_POSTC_CAL_MASK) >>
                        DSC4B0_TUNING_SM_CTRL0_BYPASS_TX_POSTC_CAL_SHIFT ;
   bypass_br_vga= (data & DSC4B0_TUNING_SM_CTRL0_BYPASS_BR_VGA_MASK) >>
                  DSC4B0_TUNING_SM_CTRL0_BYPASS_BR_VGA_SHIFT ;
   postc_metric_ctrl= (data & DSC4B0_TUNING_SM_CTRL0_POSTC_METRIC_CTRL_MASK) >>
                      DSC4B0_TUNING_SM_CTRL0_POSTC_METRIC_CTRL_SHIFT ;
   hysteresis_en= (data & DSC4B0_TUNING_SM_CTRL0_HYSTERESIS_EN_MASK) >>
                  DSC4B0_TUNING_SM_CTRL0_HYSTERESIS_EN_SHIFT ;
   slicer_cal_linear_srch= (data & DSC4B0_TUNING_SM_CTRL0_SLICER_CAL_LINEAR_SRCH_MASK) >>
                        DSC4B0_TUNING_SM_CTRL0_SLICER_CAL_LINEAR_SRCH_SHIFT ;
   bypass_br_pf_cal= (data & DSC4B0_TUNING_SM_CTRL0_BYPASS_BR_PF_CAL_MASK) >>
                     DSC4B0_TUNING_SM_CTRL0_BYPASS_BR_PF_CAL_SHIFT ;
   bypass_osx2_pf_cal= (data & DSC4B0_TUNING_SM_CTRL0_BYPASS_OSX2_PF_CAL_MASK) >>
                       DSC4B0_TUNING_SM_CTRL0_BYPASS_OSX2_PF_CAL_SHIFT ;
   bypass_osx1_pf_cal= (data & DSC4B0_TUNING_SM_CTRL0_BYPASS_OSX1_PF_CAL_MASK) >>
                       DSC4B0_TUNING_SM_CTRL0_BYPASS_OSX1_PF_CAL_SHIFT ;
   bypass_data_slicer_recal= (data & DSC4B0_TUNING_SM_CTRL0_BYPASS_DATA_SLICER_RECAL_MASK) >>
                             DSC4B0_TUNING_SM_CTRL0_BYPASS_DATA_SLICER_RECAL_SHIFT ;
   bypass_osx45_slicer_cal= (data & DSC4B0_TUNING_SM_CTRL0_BYPASS_OSX45_SLICER_CAL_MASK) >>
                            DSC4B0_TUNING_SM_CTRL0_BYPASS_OSX45_SLICER_CAL_SHIFT ;
   bypass_phase_slicer_cal= (data & DSC4B0_TUNING_SM_CTRL0_BYPASS_PHASE_SLICER_CAL_MASK) >>
                            DSC4B0_TUNING_SM_CTRL0_BYPASS_PHASE_SLICER_CAL_SHIFT ;
   bypass_br_data_slicer_cal= (data & DSC4B0_TUNING_SM_CTRL0_BYPASS_BR_DATA_SLICER_CAL_MASK) >>
                              DSC4B0_TUNING_SM_CTRL0_BYPASS_BR_DATA_SLICER_CAL_SHIFT ;
   bypass_os_data_slicer_cal= (data & DSC4B0_TUNING_SM_CTRL0_BYPASS_OS_DATA_SLICER_CAL_MASK) >>
                              DSC4B0_TUNING_SM_CTRL0_BYPASS_OS_DATA_SLICER_CAL_SHIFT ;
   restart_tuning= (data & DSC4B0_TUNING_SM_CTRL0_RESTART_TUNING_MASK) >>
                   DSC4B0_TUNING_SM_CTRL0_RESTART_TUNING_SHIFT ;
   tuning_sm_en= data & DSC4B0_TUNING_SM_CTRL0_TUNING_SM_EN_MASK ;

   printf(" DSC4B0_TUNING_SM_CTRL0r(%0x) d=%0x bypass_tx_postc_cal=%0x bypass_br_vga=%0x\n",
          ws->accAddr, data, bypass_tx_postc_cal, bypass_br_vga ) ;
   printf("   postc_metric_ctrl=%0x hysteresis_en=%0x slicer_cal_linear_srch=%0x bypass_br_pf_cal=%0x\n",
          postc_metric_ctrl, hysteresis_en, slicer_cal_linear_srch, bypass_br_pf_cal ) ;
   printf("   bypass_osx2_pf_cal=%0x bypass_osx1_pf_cal=%0x bypass_data_slicer_recal=%0x\n",
          bypass_osx2_pf_cal, bypass_osx1_pf_cal, bypass_data_slicer_recal ) ;
   printf("   bypass_osx45_slicer_cal=%0x bypass_phase_slicer_cal=%0x bypass_br_data_slicer_cal=%0x\n",
          bypass_osx45_slicer_cal, bypass_phase_slicer_cal, bypass_br_data_slicer_cal ) ;
   printf("   bypass_os_data_slicer_cal=%0x restart_tuning=%0x tuning_sm_en=%0x\n",
          bypass_os_data_slicer_cal, restart_tuning, tuning_sm_en ) ;




   /* 0xC231 */

   SOC_IF_ERROR_RETURN ( READ_WC40_DSC4B0_TUNING_SM_CTRL1r(ws->unit,ws,&data));

   pf_ctrl_br_offset= (data & DSC4B0_TUNING_SM_CTRL1_PF_CTRL_BR_OFFSET_MASK) >>
                      DSC4B0_TUNING_SM_CTRL1_PF_CTRL_BR_OFFSET_SHIFT ;
   pf_ctrl_osx1_offset= (data & DSC4B0_TUNING_SM_CTRL1_PF_CTRL_OSX1_OFFSET_MASK) >>
                        DSC4B0_TUNING_SM_CTRL1_PF_CTRL_OSX1_OFFSET_SHIFT ;
   pf_ctrl_osx2_offset= (data & DSC4B0_TUNING_SM_CTRL1_PF_CTRL_OSX2_OFFSET_MASK) >>
                        DSC4B0_TUNING_SM_CTRL1_PF_CTRL_OSX2_OFFSET_SHIFT ;
   pf_ctrl_br_init= (data & DSC4B0_TUNING_SM_CTRL1_PF_CTRL_BR_INIT_MASK) >>
                    DSC4B0_TUNING_SM_CTRL1_PF_CTRL_BR_INIT_SHIFT ;
   pf_ctrl_os_init= data & DSC4B0_TUNING_SM_CTRL1_PF_CTRL_OS_INIT_MASK ;

   printf(" DSC4B0_TUNING_SM_CTRL1r(%0x) d=%0x pf_ctrl_br_offset=%0x pf_ctrl_osx1_offset=%0x\n",
          ws->accAddr, data, pf_ctrl_br_offset, pf_ctrl_osx1_offset ) ;
   printf("   pf_ctrl_osx2_offset=%0x pf_ctrl_br_init=%0x pf_ctrl_os_init=%0x\n",
          pf_ctrl_osx2_offset, pf_ctrl_br_init, pf_ctrl_os_init) ;


   /* 0xC232 */

   SOC_IF_ERROR_RETURN ( READ_WC40_DSC4B0_TUNING_SM_CTRL2r(ws->unit,ws,&data));

   vga_max_val= (data & DSC4B0_TUNING_SM_CTRL2_VGA_MAX_VAL_MASK) >>
                DSC4B0_TUNING_SM_CTRL2_VGA_MAX_VAL_SHIFT ;
   vga_min_val= data & DSC4B0_TUNING_SM_CTRL2_VGA_MIN_VAL_MASK ;

   printf(" DSC4B0_TUNING_SM_CTRL2r(%0x) d=%0x vga_max_val=%0x vga_min_val=%0x\n",
          ws->accAddr, data, vga_max_val, vga_min_val ) ;


   /* 0xC233 */

   SOC_IF_ERROR_RETURN ( READ_WC40_DSC4B0_TUNING_SM_CTRL3r(ws->unit,ws,&data));

   dfe_max_val= (data & DSC4B0_TUNING_SM_CTRL3_DFE_MAX_VAL_MASK) >>
                DSC4B0_TUNING_SM_CTRL3_DFE_MAX_VAL_SHIFT ;
   dfe_min_val= data & DSC4B0_TUNING_SM_CTRL3_DFE_MIN_VAL_MASK ;

   printf(" DSC4B0_TUNING_SM_CTRL3r(%0x) d=%0x dfe_max_val=%0x dfe_min_val=%0x\n",
          ws->accAddr, data, dfe_max_val, dfe_min_val ) ;


   /* 0xC234 */

   SOC_IF_ERROR_RETURN ( READ_WC40_DSC4B0_TUNING_SM_CTRL4r(ws->unit,ws,&data));

   br_pf_tap_en= (data & DSC4B0_TUNING_SM_CTRL4_BR_PF_TAP_EN_MASK) >>
                 DSC4B0_TUNING_SM_CTRL4_BR_PF_TAP_EN_SHIFT ;
   osx1_pf_tap_en= data & DSC4B0_TUNING_SM_CTRL4_OSX1_PF_TAP_EN_MASK ;

   printf(" DSC4B0_TUNING_SM_CTRL4r(%0x) d=%0x br_pf_tap_en=%0x osx1_pf_tap_en=%0x\n",
          ws->accAddr, data, br_pf_tap_en, osx1_pf_tap_en ) ;


   /* 0xC235 */

   SOC_IF_ERROR_RETURN ( READ_WC40_DSC4B0_TUNING_SM_CTRL5r(ws->unit,ws,&data));

   msr_br_vga_timeout= (data & DSC4B0_TUNING_SM_CTRL5_MSR_BR_VGA_TIMEOUT_MASK) >>
                       DSC4B0_TUNING_SM_CTRL5_MSR_BR_VGA_TIMEOUT_SHIFT ;
   hysteresis_timeout= (data & DSC4B0_TUNING_SM_CTRL5_HYSTERESIS_TIMEOUT_MASK) >>
                       DSC4B0_TUNING_SM_CTRL5_HYSTERESIS_TIMEOUT_SHIFT ;
   msr_postc_timeout= data & DSC4B0_TUNING_SM_CTRL5_MSR_POSTC_TIMEOUT_MASK ;

   printf(" DSC4B0_TUNING_SM_CTRL5r(%0x) d=%0x msr_br_vga_timeout=%0x hysteresis_timeout=%0x\n",
          ws->accAddr, data, msr_br_vga_timeout, hysteresis_timeout ) ;
   printf("   msr_postc_timeout=%0x\n",
          msr_postc_timeout ) ;


   /* 0xC236 */

   SOC_IF_ERROR_RETURN ( READ_WC40_DSC4B0_TUNING_SM_CTRL6r(ws->unit,ws,&data));

   br_vga_lms_gain= (data & DSC4B0_TUNING_SM_CTRL6_BR_VGA_LMS_GAIN_MASK) >>
                    DSC4B0_TUNING_SM_CTRL6_BR_VGA_LMS_GAIN_SHIFT ;
   postc_dfe_lms_gain= (data & DSC4B0_TUNING_SM_CTRL6_POSTC_DFE_LMS_GAIN_MASK) >>
                       DSC4B0_TUNING_SM_CTRL6_POSTC_DFE_LMS_GAIN_SHIFT ;
   cdr_phase_inversion_timeout= (data & DSC4B0_TUNING_SM_CTRL6_CDR_PHASE_INVERSION_TIMEOUT_MASK ) >>
                                DSC4B0_TUNING_SM_CTRL6_CDR_PHASE_INVERSION_TIMEOUT_SHIFT ;
   msr_pf_timeout= data & DSC4B0_TUNING_SM_CTRL6_MSR_PF_TIMEOUT_MASK ;

   printf(" DSC4B0_TUNING_SM_CTRL6r(%0x) d=%0x br_vga_lms_gain=%0x postc_dfe_lms_gain=%0x\n",
          ws->accAddr, data, br_vga_lms_gain, postc_dfe_lms_gain ) ;
   printf("   cdr_phase_inversion_timeout=%0x msr_pf_timeout=%0x\n",
          cdr_phase_inversion_timeout, msr_pf_timeout ) ;


   /* 0xC237 */

   SOC_IF_ERROR_RETURN ( READ_WC40_DSC4B0_TUNING_SM_CTRL7r(ws->unit,ws,&data));

   br_vga_trn2_timeout= (data & DSC4B0_TUNING_SM_CTRL7_BR_VGA_TRN2_TIMEOUT_MASK) >>
                        DSC4B0_TUNING_SM_CTRL7_BR_VGA_TRN2_TIMEOUT_SHIFT ;
   br_pf_trn2_timeout= (data & DSC4B0_TUNING_SM_CTRL7_BR_PF_TRN2_TIMEOUT_MASK) >>
                       DSC4B0_TUNING_SM_CTRL7_BR_PF_TRN2_TIMEOUT_SHIFT ;
   br_pf_trn1_timeout= data & DSC4B0_TUNING_SM_CTRL7_BR_PF_TRN1_TIMEOUT_MASK ;

   printf(" DSC4B0_TUNING_SM_CTRL7r(%0x) d=%0x br_vga_trn2_timeout=%0x br_pf_trn2_timeout=%0x\n",
          ws->accAddr, data, br_vga_trn2_timeout, br_pf_trn2_timeout ) ;
   printf("   br_pf_trn1_timeout=%0x\n",
          br_pf_trn1_timeout ) ;


   /* 0xC238 */

   SOC_IF_ERROR_RETURN ( READ_WC40_DSC4B0_TUNING_SM_CTRL8r(ws->unit,ws,&data));

   br_postc_tap_en= (data & DSC4B0_TUNING_SM_CTRL8_BR_POSTC_TAP_EN_MASK) >>
                    DSC4B0_TUNING_SM_CTRL8_BR_POSTC_TAP_EN_SHIFT ;
   osx2_pf_tap_en= data & DSC4B0_TUNING_SM_CTRL8_OSX2_PF_TAP_EN_MASK ;

   printf(" DSC4B0_TUNING_SM_CTRL8r(%0x) d=%0x br_postc_tap_en=%0x osx2_pf_tap_en=%0x\n",
          ws->accAddr, data, br_postc_tap_en, osx2_pf_tap_en ) ;


   /* 0xC239 */

   SOC_IF_ERROR_RETURN ( READ_WC40_DSC4B0_TUNING_SM_CTRL9r(ws->unit,ws,&data));

   osx2_postc_tap_en= (data & DSC4B0_TUNING_SM_CTRL9_OSX2_POSTC_TAP_EN_MASK) >>
                      DSC4B0_TUNING_SM_CTRL9_OSX2_POSTC_TAP_EN_SHIFT ;
   osx1_postc_tap_en= data & DSC4B0_TUNING_SM_CTRL9_OSX1_POSTC_TAP_EN_MASK ;

   printf(" DSC4B0_TUNING_SM_CTRL9r(%0x) d=%0x osx2_postc_tap_en=%0x osx1_postc_tap_en=%0x\n",
          ws->accAddr, data, osx2_postc_tap_en, osx1_postc_tap_en ) ;


   /* 0xC23A */

   SOC_IF_ERROR_RETURN ( READ_WC40_DSC4B0_TUNING_SM_CTRL10r(ws->unit,ws,&data));

   msr_slicer_slow_timeout= (data & DSC4B0_TUNING_SM_CTRL10_MSR_SLICER_SLOW_TIMEOUT_MASK) >>
                            DSC4B0_TUNING_SM_CTRL10_MSR_SLICER_SLOW_TIMEOUT_SHIFT ;
   msr_slicer_fast_timeout= (data & DSC4B0_TUNING_SM_CTRL10_MSR_SLICER_FAST_TIMEOUT_MASK) >>
                            DSC4B0_TUNING_SM_CTRL10_MSR_SLICER_FAST_TIMEOUT_SHIFT ;
   default_trn2_timeout= data & DSC4B0_TUNING_SM_CTRL10_DEFAULT_TRN2_TIMEOUT_MASK ;

   printf(" DSC4B0_TUNING_SM_CTRL10r(%0x) d=%0x msr_slicer_slow_timeout=%0x msr_slicer_fast_timeout=%0x\n",
          ws->accAddr, data, msr_slicer_slow_timeout, msr_slicer_fast_timeout ) ;
   printf("   default_trn2_timeout=%0x\n",
          default_trn2_timeout ) ;


   /* 0xC23B */

   SOC_IF_ERROR_RETURN ( READ_WC40_DSC4B0_TUNING_SM_CTRL11r(ws->unit,ws,&data));

   pf_fine_en= (data & DSC4B0_TUNING_SM_CTRL11_PF_FINE_EN_MASK) >>
               DSC4B0_TUNING_SM_CTRL11_PF_FINE_EN_SHIFT ;
   pf_max_val= (data & DSC4B0_TUNING_SM_CTRL11_PF_MAX_VAL_MASK) >>
                DSC4B0_TUNING_SM_CTRL11_PF_MAX_VAL_SHIFT ;
   bypass_osx1_slicer_cal= (data & DSC4B0_TUNING_SM_CTRL11_BYPASS_OSX1_SLICER_CAL_MASK) >>
                           DSC4B0_TUNING_SM_CTRL11_BYPASS_OSX1_SLICER_CAL_SHIFT ;
   bypass_osx2_slicer_cal= data & DSC4B0_TUNING_SM_CTRL11_BYPASS_OSX2_SLICER_CAL_MASK ;

   printf(" DSC4B0_TUNING_SM_CTRL11r(%0x) d=%0x pf_fine_en=%0x pf_max_val=%0x\n",
          ws->accAddr, data, pf_fine_en, pf_max_val ) ;
   printf("   bypass_osx1_slicer_cal=%0x bypass_osx2_slicer_cal=%0x\n",
          bypass_osx1_slicer_cal, bypass_osx2_slicer_cal ) ;


   /* 0xC23C */

   SOC_IF_ERROR_RETURN ( READ_WC40_DSC4B0_ACQ_SM_CTRL9r(ws->unit,ws,&data));

   eee_lfsr_cnt= data & DSC4B0_ACQ_SM_CTRL9_EEE_LFSR_CNT_MASK ;

   printf(" DSC4B0_ACQ_SM_CTRL9r(%0x) d=%0x eee_lfsr_cnt=%0x\n",
          ws->accAddr, data, eee_lfsr_cnt ) ;





   /* 0xC240 - 0xC246, Dsc5b0: DSC Tuning FSM Status Registers */

   /* 0xC240 */

   SOC_IF_ERROR_RETURN ( READ_WC40_DSC5B0_TUNING_SM_STATUS0r(ws->unit,ws,&data));

   tuning_done= (data & DSC5B0_TUNING_SM_STATUS0_TUNING_DONE_MASK) >>
                DSC5B0_TUNING_SM_STATUS0_TUNING_DONE_SHIFT ;
   srch_state= (data & DSC5B0_TUNING_SM_STATUS0_SRCH_STATE_MASK) >>
               DSC5B0_TUNING_SM_STATUS0_SRCH_STATE_SHIFT ;
   tuning_state= (data & DSC5B0_TUNING_SM_STATUS0_TUNING_STATE_MASK) >>
                 DSC5B0_TUNING_SM_STATUS0_TUNING_STATE_SHIFT ;
   dsc_state_sm= data & DSC5B0_TUNING_SM_STATUS0_DSC_STATE_MASK ;

   printf(" DSC5B0_TUNING_SM_STATUS0r(%0x) d=%0x tuning_done=%0x srch_state=%0x\n",
          ws->accAddr, data, tuning_done, srch_state ) ;
   printf("   tuning_state=%0x dsc_state=%0x\n",
          tuning_state, dsc_state_sm ) ;


   /* 0xC241 */

   SOC_IF_ERROR_RETURN ( READ_WC40_DSC5B0_TUNING_SM_STATUS1r(ws->unit,ws,&data));

   postc_metric= data & DSC5B0_TUNING_SM_STATUS1_POSTC_METRIC_MASK ;

   printf(" DSC5B0_TUNING_SM_STATUS1r(%0x) d=%0x postc_metric=%0x\n",
          ws->accAddr, data, postc_metric ) ;


   /* 0xC242 */

   SOC_IF_ERROR_RETURN ( READ_WC40_DSC5B0_TUNING_SM_STATUS2r(ws->unit,ws,&data));

   dfe_max= (data & DSC5B0_TUNING_SM_STATUS2_DFE_MAX_MASK) >>
            DSC5B0_TUNING_SM_STATUS2_DFE_MAX_SHIFT ;
   dfe_min= (data & DSC5B0_TUNING_SM_STATUS2_DFE_MIN_MASK) >>
            DSC5B0_TUNING_SM_STATUS2_DFE_MIN_SHIFT ;
   vga_max= (data & DSC5B0_TUNING_SM_STATUS2_VGA_MAX_MASK) >>
            DSC5B0_TUNING_SM_STATUS2_VGA_MAX_SHIFT ;
   vga_min= (data & DSC5B0_TUNING_SM_STATUS2_VGA_MIN_MASK) >>
            DSC5B0_TUNING_SM_STATUS2_VGA_MIN_SHIFT ;
   pf_max= (data & DSC5B0_TUNING_SM_STATUS2_PF_MAX_MASK) >>
           DSC5B0_TUNING_SM_STATUS2_PF_MAX_SHIFT ;
   pf_min= data & DSC5B0_TUNING_SM_STATUS2_PF_MIN_MASK ;

   printf(" DSC5B0_TUNING_SM_STATUS2r(%0x) d=%0x dfe_max=%0x dfe_min=%0x\n",
          ws->accAddr, data, dfe_max, dfe_min ) ;
   printf("   vga_max=%0x vga_min=%0x pf_max=%0x pf_min=%0x\n",
          vga_max, vga_min, pf_max, pf_min ) ;


   /* 0xC243 */

   SOC_IF_ERROR_RETURN ( READ_WC40_DSC5B0_TUNING_SM_STATUS3r(ws->unit,ws,&data));

   trnsum_br_vga= data & DSC5B0_TUNING_SM_STATUS3_TRNSUM_BR_VGA_MASK ;

   printf(" DSC5B0_TUNING_SM_STATUS3r(%0x) d=%0x trnsum_br_vga=%0x\n",
          ws->accAddr, data, trnsum_br_vga ) ;


   /* 0xC244 */

   SOC_IF_ERROR_RETURN ( READ_WC40_DSC5B0_TUNING_SM_STATUS4r(ws->unit,ws,&data));

   trnsum_pf= data & DSC5B0_TUNING_SM_STATUS4_TRNSUM_PF_MASK ;

   printf(" DSC5B0_TUNING_SM_STATUS4r(%0x) d=%0x trnsum_pf=%0x\n",
          ws->accAddr, data, trnsum_pf ) ;


   /* 0xC245 */

   SOC_IF_ERROR_RETURN ( READ_WC40_DSC5B0_TUNING_SM_STATUS5r(ws->unit,ws,&data));


   tuning_state_br_tx_postc= (data & DSC5B0_TUNING_SM_STATUS5_TUNING_STATE_BR_TX_POSTC_MASK) >>
                             DSC5B0_TUNING_SM_STATUS5_TUNING_STATE_BR_TX_POSTC_SHIFT ;
   tuning_state_br_pf= (data & DSC5B0_TUNING_SM_STATUS5_TUNING_STATE_BR_PF_MASK) >>
                       DSC5B0_TUNING_SM_STATUS5_TUNING_STATE_BR_PF_SHIFT ;
   tuning_state_br_vga= (data & DSC5B0_TUNING_SM_STATUS5_TUNING_STATE_BR_VGA_MASK) >>
                        DSC5B0_TUNING_SM_STATUS5_TUNING_STATE_BR_VGA_SHIFT ;
   tuning_state_br_slicer_po= (data & DSC5B0_TUNING_SM_STATUS5_TUNING_STATE_BR_SLICER_PO_MASK) >>
                              DSC5B0_TUNING_SM_STATUS5_TUNING_STATE_BR_SLICER_PO_SHIFT ;
   tuning_state_br_slicer_pe= (data & DSC5B0_TUNING_SM_STATUS5_TUNING_STATE_BR_SLICER_PE_MASK) >>
                              DSC5B0_TUNING_SM_STATUS5_TUNING_STATE_BR_SLICER_PE_SHIFT ;
   tuning_state_br_slicer_mo= (data & DSC5B0_TUNING_SM_STATUS5_TUNING_STATE_BR_SLICER_MO_MASK) >>
                              DSC5B0_TUNING_SM_STATUS5_TUNING_STATE_BR_SLICER_MO_SHIFT ;
   tuning_state_br_slicer_me= (data & DSC5B0_TUNING_SM_STATUS5_TUNING_STATE_BR_SLICER_ME_MASK) >>
                              DSC5B0_TUNING_SM_STATUS5_TUNING_STATE_BR_SLICER_ME_SHIFT ;
   tuning_state_br_slicer_do= (data & DSC5B0_TUNING_SM_STATUS5_TUNING_STATE_BR_SLICER_DO_MASK) >>
                              DSC5B0_TUNING_SM_STATUS5_TUNING_STATE_BR_SLICER_DO_SHIFT ;
   tuning_state_br_slicer_de= (data & DSC5B0_TUNING_SM_STATUS5_TUNING_STATE_BR_SLICER_DE_MASK) >>
                              DSC5B0_TUNING_SM_STATUS5_TUNING_STATE_BR_SLICER_DE_SHIFT ;
   tuning_state_os_tx_postc= (data & DSC5B0_TUNING_SM_STATUS5_TUNING_STATE_OS_TX_POSTC_MASK) >>
                             DSC5B0_TUNING_SM_STATUS5_TUNING_STATE_OS_TX_POSTC_SHIFT ;
   tuning_state_os_pf= (data & DSC5B0_TUNING_SM_STATUS5_TUNING_STATE_OS_PF_MASK) >>
                       DSC5B0_TUNING_SM_STATUS5_TUNING_STATE_OS_PF_SHIFT ;
   tuning_state_os_slicer_mo= (data & DSC5B0_TUNING_SM_STATUS5_TUNING_STATE_OS_SLICER_MO_MASK) >>
                              DSC5B0_TUNING_SM_STATUS5_TUNING_STATE_OS_SLICER_MO_SHIFT ;
   tuning_state_os_slicer_me= (data & DSC5B0_TUNING_SM_STATUS5_TUNING_STATE_OS_SLICER_ME_MASK) >>
                              DSC5B0_TUNING_SM_STATUS5_TUNING_STATE_OS_SLICER_ME_SHIFT ;
   tuning_state_os_slicer_do= (data & DSC5B0_TUNING_SM_STATUS5_TUNING_STATE_OS_SLICER_DO_MASK) >>
                              DSC5B0_TUNING_SM_STATUS5_TUNING_STATE_OS_SLICER_DO_SHIFT ;
   tuning_state_os_slicer_de= (data & DSC5B0_TUNING_SM_STATUS5_TUNING_STATE_OS_SLICER_DE_MASK) >>
        	              DSC5B0_TUNING_SM_STATUS5_TUNING_STATE_OS_SLICER_DE_SHIFT ;
   tuning_state_wait_for_lock= data & DSC5B0_TUNING_SM_STATUS5_TUNING_STATE_WAIT_FOR_LOCK_MASK ;

   printf(" DSC5B0_TUNING_SM_STATUS5r(%0x) d=%0x tuning_state_br_tx_postc=%0x tuning_state_br_pf=%0x\n",
          ws->accAddr, data, tuning_state_br_tx_postc, tuning_state_br_pf ) ;
   printf("   tuning_state_br_vga=%0x tuning_state_br_slicer_po=%0x tuning_state_br_slicer_pe=%0x\n",
          tuning_state_br_vga, tuning_state_br_slicer_po, tuning_state_br_slicer_pe ) ;
   printf("   tuning_state_br_slicer_mo=%0x tuning_state_br_slicer_me=%0x tuning_state_br_slicer_do=%0x\n",
          tuning_state_br_slicer_mo, tuning_state_br_slicer_me, tuning_state_br_slicer_do ) ;
   printf("   tuning_state_br_slicer_de=%0x tuning_state_os_tx_postc=%0x tuning_state_os_pf=%0x\n",
          tuning_state_br_slicer_de, tuning_state_os_tx_postc, tuning_state_os_pf ) ;
   printf("   tuning_state_os_slicer_mo=%0x tuning_state_os_slicer_me=%0x tuning_state_os_slicer_do=%0x\n",
          tuning_state_os_slicer_mo, tuning_state_os_slicer_me, tuning_state_os_slicer_do ) ;
   printf("   tuning_state_os_slicer_de=%0x tuning_state_wait_for_lock=%0x\n",
          tuning_state_os_slicer_de, tuning_state_wait_for_lock ) ;


   /* 0xC246 */

   SOC_IF_ERROR_RETURN ( READ_WC40_DSC5B0_TUNING_SM_STATUS6r(ws->unit,ws,&data));

   tuning_state_hysteresis= (data & DSC5B0_TUNING_SM_STATUS6_TUNING_STATE_HYSTERESIS_MASK) >>
                            DSC5B0_TUNING_SM_STATUS6_TUNING_STATE_HYSTERESIS_SHIFT ;
   tuning_state_tuning_done= (data & DSC5B0_TUNING_SM_STATUS6_TUNING_STATE_TUNING_DONE_MASK) >>
                             DSC5B0_TUNING_SM_STATUS6_TUNING_STATE_TUNING_DONE_SHIFT ;
   srch_state_srch_done= (data & DSC5B0_TUNING_SM_STATUS6_SRCH_STATE_SRCH_DONE_MASK) >>
                         DSC5B0_TUNING_SM_STATUS6_SRCH_STATE_SRCH_DONE_SHIFT ;
   srch_state_srch_wait_undo_setup= (data & DSC5B0_TUNING_SM_STATUS6_SRCH_STATE_SRCH_WAIT_UNDO_SETUP_MASK) >>
                                    DSC5B0_TUNING_SM_STATUS6_SRCH_STATE_SRCH_WAIT_UNDO_SETUP_SHIFT ;
   srch_state_srch_undo_setup= (data & DSC5B0_TUNING_SM_STATUS6_SRCH_STATE_SRCH_UNDO_SETUP_MASK) >>
                               DSC5B0_TUNING_SM_STATUS6_SRCH_STATE_SRCH_UNDO_SETUP_SHIFT ;
   srch_state_srch_wait_msr= (data & DSC5B0_TUNING_SM_STATUS6_SRCH_STATE_SRCH_WAIT_MSR_MASK) >>
                             DSC5B0_TUNING_SM_STATUS6_SRCH_STATE_SRCH_WAIT_MSR_SHIFT ;
   srch_state_srch_set_msr= (data & DSC5B0_TUNING_SM_STATUS6_SRCH_STATE_SRCH_SET_MSR_MASK) >>
                            DSC5B0_TUNING_SM_STATUS6_SRCH_STATE_SRCH_SET_MSR_SHIFT ;
   srch_state_srch_wait_trn= (data & DSC5B0_TUNING_SM_STATUS6_SRCH_STATE_SRCH_WAIT_TRN_MASK) >>
                             DSC5B0_TUNING_SM_STATUS6_SRCH_STATE_SRCH_WAIT_TRN_SHIFT ;
   srch_state_srch_set_trn= (data & DSC5B0_TUNING_SM_STATUS6_SRCH_STATE_SRCH_SET_TRN_MASK) >>
                            DSC5B0_TUNING_SM_STATUS6_SRCH_STATE_SRCH_SET_TRN_SHIFT ;
   srch_state_srch_wait= (data & DSC5B0_TUNING_SM_STATUS6_SRCH_STATE_SRCH_WAIT_MASK) >>
                         DSC5B0_TUNING_SM_STATUS6_SRCH_STATE_SRCH_WAIT_SHIFT ;
   srch_state_srch_setup= (data & DSC5B0_TUNING_SM_STATUS6_SRCH_STATE_SRCH_SETUP_MASK) >>
                          DSC5B0_TUNING_SM_STATUS6_SRCH_STATE_SRCH_SETUP_SHIFT ;
   srch_state_srch_disable= data & DSC5B0_TUNING_SM_STATUS6_SRCH_STATE_SRCH_DISABLE_MASK ;

   printf(" DSC5B0_TUNING_SM_STATUS6r(%0x) d=%0x tuning_state_hysteresis=%0x tuning_state_tuning_done=%0x\n",
          ws->accAddr, data, tuning_state_hysteresis, tuning_state_tuning_done ) ;
   printf("   srch_state_srch_done=%0x srch_state_srch_wait_undo_setup=%0x srch_state_srch_undo_setup=%0x\n",
          srch_state_srch_done, srch_state_srch_wait_undo_setup, srch_state_srch_undo_setup ) ;
   printf("   srch_state_srch_wait_msr=%0x srch_state_srch_set_msr=%0x srch_state_srch_wait_trn=%0x\n",
          srch_state_srch_wait_msr, srch_state_srch_set_msr, srch_state_srch_wait_trn ) ;
   printf("   srch_state_srch_set_trn=%0x srch_state_srch_wait=%0x srch_state_srch_setup=%0x\n",
          srch_state_srch_set_trn, srch_state_srch_wait, srch_state_srch_setup ) ;
   printf("   srch_state_srch_disable=%0x\n",
          srch_state_srch_disable );


   return SOC_E_NONE ;
}


int tscmod_diag_g_dsc_cfg(tscmod_st *ws, int lane)
{
   uint16 data ; int link_ctl_force, link_ctl_forceval, tap_main_kx,
                    muxsel_forceval, muxsel, hw_tuning, restart ;
   int pre_t, main_t, post_t ;
   /* CL72 */
   printf("%-22s: u=%0d p=%0d\n",
          FUNCTION_NAME(), ws->unit, ws->port);

   SOC_IF_ERROR_RETURN
      (READ_WC40_CL72_MISC1_CONTROLr(ws->unit, ws, &data));
   printf(" CL72_KX_(%0x) d=%0x tap=%0x\n", ws->accAddr, data,
          (data&CL72_MISC1_CONTROL_TX_FIR_TAP_MAIN_KX_INIT_VAL_MASK)>>CL72_MISC1_CONTROL_TX_FIR_TAP_MAIN_KX_INIT_VAL_SHIFT) ;

   /* 0x9110 */
   SOC_IF_ERROR_RETURN
      (READ_WC40_CL72_SHARED_AN_OS_DEFAULT_CONTROLr(ws->unit, ws, &data));
   pre_t  = (data & CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_PRE_MASK) >>CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_PRE_SHIFT;
   main_t = (data & CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_MAIN_MASK) >>CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_MAIN_SHIFT;
   post_t = (data & CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_POST_MASK) >>CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_POST_SHIFT;
   printf(" CL72_OS_(%0x) d=%0x  pre=0x%0x main=%0x post=%0x\n", ws->accAddr, data, pre_t, main_t, post_t) ;

   /* 0x9111 */
   SOC_IF_ERROR_RETURN
      (READ_WC40_CL72_SHARED_AN_BR_DEFAULT_CONTROLr(ws->unit, ws, &data));
   pre_t  = (data & CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_PRE_MASK) >>CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_PRE_SHIFT;
   main_t = (data & CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_MAIN_MASK) >>CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_MAIN_SHIFT;
   post_t = (data & CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_POST_MASK) >>CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_POST_SHIFT;
   printf(" CL72_BR_(%0x) d=%0x  pre=0x%0x main=%0x post=%0x\n", ws->accAddr, data, pre_t, main_t, post_t) ;

   SOC_IF_ERROR_RETURN
      (READ_WC40_TX_X4_CONTROL0_KR_DEFAULT_CONTROLr(ws->unit, ws, &data));
   pre_t  = (data & CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_PRE_MASK) >>CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_PRE_SHIFT;
   main_t = (data & CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_MAIN_MASK) >>CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_MAIN_SHIFT;
   post_t = (data & CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_POST_MASK) >>CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_POST_SHIFT;
   printf(" CL72_KR_(%0x) d=%0x  pre=0x%0x main=%0x post=%0x\n", ws->accAddr, data, pre_t, main_t, post_t) ;

   SOC_IF_ERROR_RETURN
      (READ_WC40_CL72_SHARED_AN_TWOP5_DEFAULT_CONTROLr(ws->unit, ws, &data));
   pre_t  = (data & CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_PRE_MASK) >>CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_PRE_SHIFT;
   main_t = (data & CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_MAIN_MASK) >>CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_MAIN_SHIFT;
   post_t = (data & CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_POST_MASK) >>CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_POST_SHIFT;
   printf(" CL72_2p5_(%0x) d=%0x  pre=0x%0x main=%0x post=%0x\n", ws->accAddr, data, pre_t, main_t, post_t) ;


   SOC_IF_ERROR_RETURN
      (READ_WC40_CL72_TX_FIR_TAP_REGISTERr(ws->unit, ws, &data));
   printf(" CL72_TX_FIR_TAP(%0x) d=%0x\n", ws->accAddr, data) ;

   SOC_IF_ERROR_RETURN
      (READ_WC40_CL72_MISC1_CONTROLr(ws->unit, ws, &data));
   link_ctl_force = (data & CL72_MISC1_CONTROL_LINK_CONTROL_FORCE_MASK )
      >> CL72_MISC1_CONTROL_LINK_CONTROL_FORCE_SHIFT ;
   link_ctl_forceval = (data & CL72_MISC1_CONTROL_LINK_CONTROL_FORCEVAL_MASK)
      >> CL72_MISC1_CONTROL_LINK_CONTROL_FORCEVAL_SHIFT ;
   tap_main_kx = (data & CL72_MISC1_CONTROL_TX_FIR_TAP_MAIN_KX_INIT_VAL_MASK)
      >> CL72_MISC1_CONTROL_TX_FIR_TAP_MAIN_KX_INIT_VAL_SHIFT ;
   muxsel_forceval = (data & CL72_MISC1_CONTROL_TAP_DEFAULT_MUXSEL_FORCEVAL_MASK )
      >> CL72_MISC1_CONTROL_TAP_DEFAULT_MUXSEL_FORCEVAL_SHIFT ;
   muxsel = (data & CL72_MISC1_CONTROL_TAP_DEFAULT_MUXSEL_FORCE_MASK )
      >> CL72_MISC1_CONTROL_TAP_DEFAULT_MUXSEL_FORCE_SHIFT;
   printf(" CL72_MISC1_CONTROL(%0x) d=%0x link_ctl_force=%0d link_ctl_forceval=%0d\n",
          ws->accAddr, data, link_ctl_force, link_ctl_forceval) ;
   printf("     tap_main_kx=%0x muxsel_forceval=%0x muxsel=%0x\n",
          tap_main_kx, muxsel_forceval, muxsel) ;

   /* 0xc230 */
   SOC_IF_ERROR_RETURN
      (READ_WC40_DSC4B0_TUNING_SM_CTRL0r(ws->unit, ws, &data));
   hw_tuning = (data & DSC4B0_TUNING_SM_CTRL0_TUNING_SM_EN_MASK) >>
      DSC4B0_TUNING_SM_CTRL0_TUNING_SM_EN_SHIFT ;
   restart = (data & DSC4B0_TUNING_SM_CTRL0_RESTART_TUNING_MASK )>>
      DSC4B0_TUNING_SM_CTRL0_RESTART_TUNING_SHIFT ;
   printf(" DSC_TUNING_SM_CTRL0(%0x) d=%0x hw_tuning=%0d restart=%0d\n",
          ws->accAddr, data, hw_tuning, restart) ;

   /* 0xc260 */
   SOC_IF_ERROR_RETURN
      (READ_WC40_UCSS_X4_FIRMWARE_MODEr(ws->unit, ws, &data));
   printf(" UCSS_X4_FIRMWARE_MODEr(%0x) d=%0x \n", ws->accAddr, data) ;

   return SOC_E_NONE ;
}

int tscmod_diag_g_pattern(tscmod_st *ws, int lane)
{
   uint16 data, dataL, dataH ;  uint16 prbs_tx_en, prbs_rx_en, src_sel, bus_width_tx,
   bus_width_rx, clr_txcnt, clr_rxcnt, patgen_en, rx_fixed_chk, number_bit ;
   int lock, lock_lost, err, addrH, addrL ;
   printf("%-22s: u=%0d p=%0d\n",
          FUNCTION_NAME(), ws->unit, ws->port);
   SOC_IF_ERROR_RETURN
      (READ_WC40_PATGEN0_PATGENCTRLr(ws->unit, ws, &data)) ;
   number_bit = (data &PATGEN0_PATGENCTRL_NUMBER_BIT_MASK)
      >> PATGEN0_PATGENCTRL_NUMBER_BIT_SHIFT ;
   rx_fixed_chk =(data& PATGEN0_PATGENCTRL_RX_FIXED_PATTERN_CHECK_ENABLE_MASK)
      >> PATGEN0_PATGENCTRL_RX_FIXED_PATTERN_CHECK_ENABLE_SHIFT ;
   bus_width_rx =(data&PATGEN0_PATGENCTRL_TESTGEN_BUS_WIDTH_RX_MASK)
      >>PATGEN0_PATGENCTRL_TESTGEN_BUS_WIDTH_RX_SHIFT ;
   bus_width_tx =(data&PATGEN0_PATGENCTRL_TESTGEN_BUS_WIDTH_TX_MASK)
      >>PATGEN0_PATGENCTRL_TESTGEN_BUS_WIDTH_TX_SHIFT ;
   clr_rxcnt =(data&PATGEN0_PATGENCTRL_CLR_RXCNT_MASK)
      >>PATGEN0_PATGENCTRL_CLR_RXCNT_SHIFT;
   clr_txcnt =(data&PATGEN0_PATGENCTRL_CLR_TXCNT_MASK)
      >>PATGEN0_PATGENCTRL_CLR_TXCNT_SHIFT;
   patgen_en=(data&PATGEN0_PATGENCTRL_PATGEN_EN_MASK)
      >>PATGEN0_PATGENCTRL_PATGEN_EN_SHIFT ;
   prbs_rx_en=(data&PATGEN0_PATGENCTRL_PRBS_RX_EN_MASK)
      >>PATGEN0_PATGENCTRL_PRBS_RX_EN_SHIFT;
   prbs_tx_en=(data&PATGEN0_PATGENCTRL_PRBS_TX_EN_MASK)
      >>PATGEN0_PATGENCTRL_PRBS_TX_EN_SHIFT;
   src_sel = (data&PATGEN0_PATGENCTRL_TESTSOURCE_SEL_MASK)
      >>PATGEN0_PATGENCTRL_TESTSOURCE_SEL_SHIFT ;
   printf(" PATGEN0_PATGENCTRLr(%0x) d=%0x prbs_en tx=%0d rx=%0d src_sel=%0d \
patgen_en=%0d bus_w tx=%0d rx=%0d\n", ws->accAddr, data, prbs_tx_en,
        prbs_rx_en, src_sel, patgen_en,bus_width_tx,bus_width_rx) ;

   printf("     clr_cnt tx=%0d rx=%0d rx_fixed_chk=%0d num_bits=%0d\n",
          clr_txcnt,  clr_rxcnt,rx_fixed_chk,  number_bit) ;

   SOC_IF_ERROR_RETURN
      (READ_WC40_PATGEN0_TESTMISCSTATUSr(ws->unit, ws, &data));
   SOC_IF_ERROR_RETURN
      (READ_WC40_PATGEN0_PRBSSTAT0r(ws->unit, ws, &dataH));
   addrH = ws->accAddr ;
   SOC_IF_ERROR_RETURN
      (READ_WC40_PATGEN0_PRBSSTAT1r(ws->unit, ws, &dataL));
   addrL = ws->accAddr ;
   lock      = (data&PATGEN0_TESTMISCSTATUS_PRBS_LOCK_MASK) ? 1: 0 ;
   lock_lost = (dataH & PATGEN0_PRBSSTAT0_PRBS_LOCK_LOST_LH_MASK)?1:0 ;
   err       = dataH & ~PATGEN0_PRBSSTAT0_PRBS_LOCK_LOST_LH_MASK ;
   err       = err <<16 | dataL ;
   printf(" PATGEN0_TESTMISCSTATUSr(%0x) d=%0x", ws->accAddr, data) ;
   printf("   prbs lck=%0d lost=%0d err=%0d(H%x L%x)\n", lock, lock_lost, err, dataH, dataL);
   printf(" PATGEN0_PRBSSTAT0r(%0x) d=%0x PATGEN0_PRBSSTAT1r(%0x) d=%0x\n", addrH, dataH,addrL, dataL) ;
   return SOC_E_NONE ;
}

int tscmod_diag_g_path(tscmod_st *ws, int lane)
{
   uint16 data ; uint16 tx_rx_pcs, rx_tx_pcs, rx_tx_pmd ;
   int tx_flip, rx_flip ;

   printf("%-22s: u=%0d p=%0d\n",
          FUNCTION_NAME(), ws->unit, ws->port);
   /* loopback */
   SOC_IF_ERROR_RETURN
      (READ_WC40_MAIN0_LOOPBACK_CONTROLr(ws->unit, ws, &data));
   printf(" MAIN0_LOOPBACK_CONTROLr(%0x) d=%0x\n", ws->accAddr, data) ;
   tx_rx_pcs = (data &MAIN0_LOOPBACK_CONTROL_LOCAL_PCS_LOOPBACK_ENABLE_MASK)>>
                      MAIN0_LOOPBACK_CONTROL_LOCAL_PCS_LOOPBACK_ENABLE_SHIFT ;
   rx_tx_pcs = (data &MAIN0_LOOPBACK_CONTROL_REMOTE_PCS_LOOPBACK_ENABLE_MASK)>>
                      MAIN0_LOOPBACK_CONTROL_REMOTE_PCS_LOOPBACK_ENABLE_SHIFT;

   rx_tx_pmd = (data &MAIN0_LOOPBACK_CONTROL_REMOTE_PMD_LOOPBACK_ENABLE_MASK)>>
                      MAIN0_LOOPBACK_CONTROL_REMOTE_PMD_LOOPBACK_ENABLE_SHIFT;
   printf("     gloop(tx->rx) pcs=%x; rloop(rx->tx) pcs=%x pmd=%x\n",
          tx_rx_pcs, rx_tx_pcs, rx_tx_pmd) ;

   /* polarity tx: 0xc011   rx: 0xc021 */
   SOC_IF_ERROR_RETURN (READ_WC40_ANATX_ACONTROL0r(ws->unit, ws, &data));
   tx_flip = (data & ANATX_ACONTROL0_TXPOL_FLIP_MASK) >>
      ANATX_ACONTROL0_TXPOL_FLIP_SHIFT;
   printf("  ANATX_ACONTROL0r(%0x) d=%0x tx_flip=%x\n",
          ws->accAddr, data, tx_flip);
   SOC_IF_ERROR_RETURN
          (READ_WC40_ANARX_ACONTROL0r(ws->unit, ws, &data));
   rx_flip = (data & ANARX_ACONTROL0_RXPOL_FLIP_MASK) >>
                     ANARX_ACONTROL0_RXPOL_FLIP_SHIFT;
   printf("  ANARX_ACONTROL0r(%0x) d=%0x rx_flip=%x\n",
          ws->accAddr, data, rx_flip) ;

   /* lane swap 0x9003 */
   SOC_IF_ERROR_RETURN
      (READ_WC40_MAIN0_LANE_SWAPr(ws->unit, ws, &data)) ;
   printf("  MAIN0_LANE_SWAPr(%0x) d=%0x rx[15:8]_tx[7:0]=%x\n",
          ws->accAddr, data, data) ;

   /* pi */
   SOC_IF_ERROR_RETURN
      (READ_WC40_TX_X1_CONTROL0_TX_PI_CONTROL1r(ws->unit, ws, &data)) ;
   printf("  TX_PI_CONTROL1r(%0x) d=%0x pi_en=%0x\n",
          ws->accAddr, data, (data&TX_X1_CONTROL0_TX_PI_CONTROL1_TX_PI_EN_MASK)>>TX_X1_CONTROL0_TX_PI_CONTROL1_TX_PI_EN_SHIFT) ;

   SOC_IF_ERROR_RETURN
      (READ_WC40_TX_X1_CONTROL0_TX_PI_CONTROL2r(ws->unit, ws, &data)) ;
   printf("  TX_PI_CONTROL2r(%0x) d=%0x pi_rloop=%0x pi_sel=%0x\n",
          ws->accAddr, data, (data& TX_X1_CONTROL0_TX_PI_CONTROL2_TX_PI_LOOPBACK_MODE_MASK) >>
          TX_X1_CONTROL0_TX_PI_CONTROL2_TX_PI_LOOPBACK_MODE_SHIFT,
          (data & TX_X1_CONTROL0_TX_PI_CONTROL2_TX_PI_RX_LANE_SEL_MASK) >>
          TX_X1_CONTROL0_TX_PI_CONTROL2_TX_PI_RX_LANE_SEL_SHIFT ) ;
   return SOC_E_NONE ;
}

int tscmod_diag_g_cl72(tscmod_st *ws, int lane)
{
   uint16 data ; uint16 tr_en, tr_restart, tr_fail, tr_st, frame_lock, rcv_st;
   uint16 tap_f, tap_pre, tap_main, tap_post ;
   uint16 link_ctl_f, link_ctl_fval, muxsel_f, muxsel_fval, coarse_lck, rx_trained;

   printf("%-22s: u=%0d p=%0d\n",
          FUNCTION_NAME(), ws->unit, ws->port);

   /* 0x800_0096 */
   SOC_IF_ERROR_RETURN
      (READ_WC40_DEV1_IEEE9_PMD_10GBASE_KR_PMD_CONTROL_REGISTER_150r(ws->unit, ws, &data));
   tr_en = (data & DEV1_IEEE9_PMD_10GBASE_KR_PMD_CONTROL_REGISTER_150_TRAINING_ENABLE_MASK)>>
      DEV1_IEEE9_PMD_10GBASE_KR_PMD_CONTROL_REGISTER_150_TRAINING_ENABLE_SHIFT ;
   tr_restart = (data & DEV1_IEEE9_PMD_10GBASE_KR_PMD_CONTROL_REGISTER_150_RESTART_TRAINING_MASK) >>
      DEV1_IEEE9_PMD_10GBASE_KR_PMD_CONTROL_REGISTER_150_RESTART_TRAINING_SHIFT ;
   printf(" PMD_10GBASE_KR_PMD_CONTROL_150(%0x) d=%0x tr_en=%0x tr_re=%0x \n", ws->accAddr, data, tr_en, tr_restart) ;

   /* 0x800_0097 */
   SOC_IF_ERROR_RETURN
      (READ_WC40_DEV1_IEEE9_PMD_10GBASE_KR_PMD_STATUS_REGISTER_151r(ws->unit, ws, &data));
   tr_fail = (data & DEV1_IEEE9_PMD_10GBASE_KR_PMD_STATUS_REGISTER_151_TRAINING_FAILURE_MASK)>>
      DEV1_IEEE9_PMD_10GBASE_KR_PMD_STATUS_REGISTER_151_TRAINING_FAILURE_SHIFT ;
   tr_st = (data & DEV1_IEEE9_PMD_10GBASE_KR_PMD_STATUS_REGISTER_151_TRAINING_STATUS_MASK)>>
      DEV1_IEEE9_PMD_10GBASE_KR_PMD_STATUS_REGISTER_151_TRAINING_STATUS_SHIFT ;
   frame_lock = (data & DEV1_IEEE9_PMD_10GBASE_KR_PMD_STATUS_REGISTER_151_FRAME_LOCK_MASK)>>
      DEV1_IEEE9_PMD_10GBASE_KR_PMD_STATUS_REGISTER_151_FRAME_LOCK_SHIFT ;
   rcv_st = (data & DEV1_IEEE9_PMD_10GBASE_KR_PMD_STATUS_REGISTER_151_RECEIVER_STATUS_MASK)>>
      DEV1_IEEE9_PMD_10GBASE_KR_PMD_STATUS_REGISTER_151_RECEIVER_STATUS_SHIFT ;
   printf(" PMD_10GBASE_KR_PMD_STATUS_151(%0x) d=%0x fail=%0x tr_st=%0d frame_lck=%0d rcv_st=%0d\n",
          ws->accAddr, data, tr_fail, tr_st, frame_lock, rcv_st) ;

    /* 0xc252 */
   SOC_IF_ERROR_RETURN
      (READ_WC40_CL72_TX_FIR_TAP_REGISTERr(ws->unit, ws, &data));
   tap_f = (data & CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_FORCE_MASK)>>CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_FORCE_SHIFT;
   tap_post=(data& CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_POST_MASK)>>CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_POST_SHIFT;
   tap_main=(data& CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_MAIN_MASK)>>CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_MAIN_SHIFT;
   tap_pre =(data& CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_PRE_MASK)>>CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_PRE_SHIFT;
   printf(" CL72_TX_FIR_TAP_REGISTERr(%0x) d=%0x f=%0d post=%0x main=%0x pre=%0x\n", ws->accAddr, data, tap_f, tap_post, tap_main, tap_pre) ;

   /* 0xc253 */
   SOC_IF_ERROR_RETURN
      (READ_WC40_CL72_MISC1_CONTROLr(ws->unit, ws, &data));
   link_ctl_f = (data &CL72_MISC1_CONTROL_LINK_CONTROL_FORCE_MASK)>>CL72_MISC1_CONTROL_LINK_CONTROL_FORCE_SHIFT ;
   link_ctl_fval = (data&CL72_MISC1_CONTROL_LINK_CONTROL_FORCEVAL_MASK)>>CL72_MISC1_CONTROL_LINK_CONTROL_FORCEVAL_SHIFT;
   muxsel_f   = (data &CL72_MISC1_CONTROL_TAP_DEFAULT_MUXSEL_FORCE_MASK)>>CL72_MISC1_CONTROL_TAP_DEFAULT_MUXSEL_FORCE_SHIFT;
   muxsel_fval= (data &CL72_MISC1_CONTROL_TAP_DEFAULT_MUXSEL_FORCEVAL_MASK)>>CL72_MISC1_CONTROL_TAP_DEFAULT_MUXSEL_FORCEVAL_SHIFT;
   coarse_lck = (data &CL72_MISC1_CONTROL_TR_COARSE_LOCK_MASK)>>CL72_MISC1_CONTROL_TR_COARSE_LOCK_SHIFT;
   rx_trained = (data &CL72_MISC1_CONTROL_RX_TRAINED_MASK)>>CL72_MISC1_CONTROL_RX_TRAINED_SHIFT ;
   printf("  CL72_MISC1_CONTROLr(%0x) d=%0x link_ctl_force=%0d link_ctl_fval=%0x\n", ws->accAddr, data, link_ctl_f, link_ctl_fval) ;
   printf("    muxsel_force=%0d  muxsel_fval=%x coarse_lck=%0x rx_trained=%0x\n", muxsel_f, muxsel_fval, coarse_lck, rx_trained) ;

   /* 0xc25c */
   SOC_IF_ERROR_RETURN
      (READ_WC40_CL72_CL72_READY_FOR_CMD_REGISTERr(ws->unit, ws, &data));
   printf("  CL72_CL72_READY_FOR_CMD_REGISTERr(%0x) d=%0x\n",  ws->accAddr, data) ;

   SOC_IF_ERROR_RETURN
      (READ_WC40_AN_X1_TIMERS_LINK_FAIL_INHIBIT_TIMER_CL72r(ws->unit, ws, &data));
   printf("  AN_X1_TIMERS_LINK_FAIL_INHIBIT_TIMER_CL72r(%0x) d=%0x\n", ws->accAddr, data) ;

   return SOC_E_NONE ;

}

/* use ws->accData to pass in address */
/* mode = 0 -> read 16 bytes for BCM console */
/* mode = 1 -> read 2 bytes */
int tscmod_diag_g_rd_ram(tscmod_st *ws, int mode, int lane)
{
   int i, rv ;   uint16 addr, addr2, len;  uint16 b0, b1 ;
   if(mode) {
      addr = ws->accData  ;
   } else {
      addr = ws->accData & 0xffe0 ; /* 8-byte aligned, assume the address is byte address*/
   }

   if(mode==0)
      printf("%-22s: u=%0d p=%0d ram addr=0x%0x",
             FUNCTION_NAME(), ws->unit, ws->port, addr);

   len = mode ? 1 : 16 ;

   for(i=0; i<len; i++) {
      if(mode==0) {
         if(i%8==0) { printf("\noffset 0x%02x: ", i*2) ; }
         if(i%8==4) { printf("   ") ; }
      }
      addr2 = addr + 2*i ;

      ws->accAddr          = addr2 ;
      ws->per_lane_control = TSCMOD_UC_SYNC_RAM_MODE | TSCMOD_UC_SYNC_RD_MODE ;
      tscmod_tier1_selector("UC_SYNC_CMD", ws, &rv) ;

      b0 = ws->accData ;
      b1 = (b0>>8) & 0xff ;
      b0 = b0 & 0xff ;
      if(mode==0) printf("%02x_%02x ", b0, b1) ;
   }
   if(mode==0) printf("\n") ;
   return SOC_E_NONE ;
}

int tscmod_diag_g_rd_fec_err(tscmod_st *ws, int lane)
{
   uint16 data ;  int cc_err, uc_err ;

   SOC_IF_ERROR_RETURN
      (READ_WC40_DEV1_IEEE10_PMD_CORRECTEDBLKSLr(ws->unit, ws, &data));
   cc_err = data ;
   SOC_IF_ERROR_RETURN
      (READ_WC40_DEV1_IEEE10_PMD_CORRECTEDBLKSHr(ws->unit, ws, &data));
   cc_err |= (data<<16) ;

   SOC_IF_ERROR_RETURN
      (READ_WC40_DEV1_IEEE10_PMD_UNCORRECTEDBLKSLr(ws->unit, ws, &data));
   uc_err = data ;
   SOC_IF_ERROR_RETURN
      (READ_WC40_DEV1_IEEE10_PMD_UNCORRECTEDBLKSHr(ws->unit, ws, &data));
   uc_err |= (data<<16) ;

   printf("  FEC cc_err=%0d(%x) uc_err=%0d(%x)\n", cc_err, cc_err, uc_err, uc_err)  ;
   return SOC_E_NONE ;
}

/* read out internal uC state */
/*   the only write is to write to 0xffcc */
int tscmod_diag_g_rd_uC(tscmod_st *ws, int lane)
{
   int uc_mode, sfr_sm, rv;  uint16 data, offset ;
   uint16 b0, b1 ;  int idx, addr ;
   uint16 rdata0, rdata1, rdata2, rdata3 ;

   uc_mode = 5 ;
   SOC_IF_ERROR_RETURN
      (MODIFY_WC40_UC_COMMAND3r(ws->unit, ws,
                                (uc_mode<<UC_COMMAND3_GEN_STATUS_SEL_SHIFT),
                                UC_COMMAND3_GEN_STATUS_SEL_MASK)) ;
   SOC_IF_ERROR_RETURN
      (READ_WC40_UC_SFR_STATUSr(ws->unit, ws, &data));

   sfr_sm = data ;

   printf("%-22s: u=%0d p=%0d uC states:\n", FUNCTION_NAME(), ws->unit, ws->port);

   ws->accAddr          = 0x8 ;
   ws->per_lane_control = TSCMOD_UC_SYNC_RAM_MODE | TSCMOD_UC_SYNC_RD_MODE ;
   tscmod_tier1_selector("UC_SYNC_CMD", ws, &rv) ;
   b0 = ws->accData ;
   b1 = (b0>>8) & 0xff ;
   b0 = b0 & 0xff ;
   printf("  crc=%02x%02x ", b0,b1) ;

   ws->accAddr          = 0x0 ;
   ws->per_lane_control = TSCMOD_UC_SYNC_RAM_MODE | TSCMOD_UC_SYNC_RD_MODE ;
   tscmod_tier1_selector("UC_SYNC_CMD", ws, &rv) ;
   b0 = ws->accData ;
   b1 = (b0>>8) & 0xff ;
   b0 = b0 & 0xff ;
   printf("  ver=%02x%02x ", b0,b1) ;

   ws->accAddr          = 0x2 ;
   ws->per_lane_control = TSCMOD_UC_SYNC_RAM_MODE | TSCMOD_UC_SYNC_RD_MODE ;
   tscmod_tier1_selector("UC_SYNC_CMD", ws, &rv) ;
   b0 = ws->accData ;
   b1 = (b0>>8) & 0xff ;
   b0 = b0 & 0xff ;
   printf(" target=%02x%02x ", b0,b1) ;

   ws->accAddr          = 0x4 ;
   ws->per_lane_control = TSCMOD_UC_SYNC_RAM_MODE | TSCMOD_UC_SYNC_RD_MODE ;
   tscmod_tier1_selector("UC_SYNC_CMD", ws, &rv) ;
   b0 = ws->accData ;
   b1 = (b0>>8) & 0xff ;
   b0 = b0 & 0xff ;
   printf("  sub_ver=%02x%02x\n", b0,b1) ;

   offset = (ws->this_lane) * 0x100 ;
   data   = 0x400+ offset ;
   printf("  sfr_sm=0x%x      this_lane=%0d   offset=%x\n", sfr_sm, ws->this_lane, data) ;

   ws->accAddr          = 0x400 + offset ;
   ws->per_lane_control = TSCMOD_UC_SYNC_RAM_MODE | TSCMOD_UC_SYNC_RD_MODE ;
   tscmod_tier1_selector("UC_SYNC_CMD", ws, &rv) ;
   b0 = ws->accData ;
   b1 = (b0>>8) & 0xff ;
   b0 = b0 & 0xff ;
   printf("  asp=%02x         osr_mode=%02x\n", b0, b1) ;

   ws->accAddr          = 0x402 + offset ;
   ws->per_lane_control = TSCMOD_UC_SYNC_RAM_MODE | TSCMOD_UC_SYNC_RD_MODE ;
   tscmod_tier1_selector("UC_SYNC_CMD", ws, &rv) ;
   b0 = ws->accData ;
   b1 = (b0>>8) & 0xff ;
   b0 = b0 & 0xff ;
   printf("  spd_grp=%02x     firmware_mode=%02x\n", b0, b1) ;

   ws->accAddr          = 0x404 + offset ;
   ws->per_lane_control = TSCMOD_UC_SYNC_RAM_MODE | TSCMOD_UC_SYNC_RD_MODE ;
   tscmod_tier1_selector("UC_SYNC_CMD", ws, &rv) ;
   b0 = ws->accData ;
   b1 = (b0>>8) & 0xff ;
   b0 = b0 & 0xff ;
   printf("  os_en=%02x       os_dfe_en=%02x\n", b0, b1) ;

   ws->accAddr          = 0x406 + offset ;
   ws->per_lane_control = TSCMOD_UC_SYNC_RAM_MODE | TSCMOD_UC_SYNC_RD_MODE ;
   tscmod_tier1_selector("UC_SYNC_CMD", ws, &rv) ;
   b0 = ws->accData ;
   b1 = (b0>>8) & 0xff ;
   b0 = b0 & 0xff ;
   printf("  br_dfe_en=%02x   dfe_taps2_5_en=%02x\n", b0, b1) ;

   ws->accAddr          = 0x408 + offset ;
   ws->per_lane_control = TSCMOD_UC_SYNC_RAM_MODE | TSCMOD_UC_SYNC_RD_MODE ;
   tscmod_tier1_selector("UC_SYNC_CMD", ws, &rv) ;
   b0 = ws->accData ;
   b1 = (b0>>8) & 0xff ;
   b0 = b0 & 0xff ;
   printf("  need_br_dfe=%02x lane_initialized=%02x\n", b0, b1) ;

   ws->accAddr          = 0x40a + offset ;
   ws->per_lane_control = TSCMOD_UC_SYNC_RAM_MODE | TSCMOD_UC_SYNC_RD_MODE ;
   tscmod_tier1_selector("UC_SYNC_CMD", ws, &rv) ;
   b0 = ws->accData ;
   b1 = (b0>>8) & 0xff ;
   b0 = b0 & 0xff ;
   printf("  stop_lane_op=%02x graceful_stop_trig=%02x\n", b0, b1) ;

   ws->accAddr          = 0x472 + offset ;
   ws->per_lane_control = TSCMOD_UC_SYNC_RAM_MODE | TSCMOD_UC_SYNC_RD_MODE ;
   tscmod_tier1_selector("UC_SYNC_CMD", ws, &rv) ;
   b0 = ws->accData ;
   b1 = (b0>>8) & 0xff ;
   b0 = b0 & 0xff ;
   printf("  pll_lock=%02x \n", b0) ;

   ws->accAddr          = 0x47c + offset ;
   ws->per_lane_control = TSCMOD_UC_SYNC_RAM_MODE | TSCMOD_UC_SYNC_RD_MODE ;
   tscmod_tier1_selector("UC_SYNC_CMD", ws, &rv) ;
   b0 = ws->accData ;
   b1 = (b0>>8) & 0xff ;
   b0 = b0 & 0xff ;
   printf("  spd_mode_status_lo=%02x hi=%02x\n", b0,b1) ;

   ws->accAddr          = 0x47e + offset ;
   ws->per_lane_control = TSCMOD_UC_SYNC_RAM_MODE | TSCMOD_UC_SYNC_RD_MODE ;
   tscmod_tier1_selector("UC_SYNC_CMD", ws, &rv) ;
   b0 = ws->accData ;
   b1 = (b0>>8) & 0xff ;
   b0 = b0 & 0xff ;
   printf("  state_travse_lo=%02x hi=%02x\n", b0,b1) ;

   addr = 0 ;
   for(idx=0; idx<8;idx++) {
      addr                 = 0x480 + 2*idx + offset ;
      ws->accAddr          = addr ;
      ws->per_lane_control = TSCMOD_UC_SYNC_RAM_MODE | TSCMOD_UC_SYNC_RD_MODE ;
      tscmod_tier1_selector("UC_SYNC_CMD", ws, &rv) ;
      b0 = ws->accData ;
      b1 = (b0>>8) & 0xff ;
      b0 = b0 & 0xff ;
      printf("  debug_%0d_addr(%x)_lo=%02x hi=%02x\n", idx, addr, b0,b1) ;
   }

   addr = 0 ;
   for(idx=0; idx<4;idx++) {
      addr = 0x4d0 + 0x100 * idx ;
      ws->accAddr          = addr ;
      ws->per_lane_control = TSCMOD_UC_SYNC_RAM_MODE | TSCMOD_UC_SYNC_RD_MODE ;
      tscmod_tier1_selector("UC_SYNC_CMD", ws, &rv) ;
      b0 = ws->accData ;
      b1 = (b0>>8) & 0xff ;
      b0 = b0 & 0xff ;
      printf("  debug_%0d_addr(%x)_lo=%02x hi=%02x\n", idx, addr, b0,b1) ;
   }

   printf("Diagnosis: sync_cmd=") ;
   /* TSCMOD_UC_SYNC_CMDr has one copy per core. TSC623_SDK_42585_A1 */
   rv = TSC_REG_READ(ws->unit, ws, 0x0, TSCMOD_UC_SYNC_CMDr, &rdata0) ;
   printf(" 0x%x", rdata0) ;
   rv = TSC_REG_READ(ws->unit, ws, 0x0, TSCMOD_UC_SYNC_LADDr, &rdata0) ;
   rv = TSC_REG_READ(ws->unit, ws, 0x0, TSCMOD_UC_SYNC_HADDr, &rdata1) ;
   printf(" Haddr=%0x Laddr=%0x", rdata1, rdata0) ;
   printf("\n") ;

   rv = TSC_REG_WRITE(ws->unit, ws, 0x0, 0xffcc, 0x3404) ;
   rv = TSC_REG_READ(ws->unit, ws, 0x0, 0xffc6, &rdata0) ;
   rv = TSC_REG_READ(ws->unit, ws, 0x0, 0xffc6, &rdata1) ;
   rv = TSC_REG_READ(ws->unit, ws, 0x0, 0xffc6, &rdata2) ;
   rv = TSC_REG_READ(ws->unit, ws, 0x0, 0xffc6, &rdata3) ;
   printf("Diagnosis: u=%0d p=%0d 0xffcc=0x3404 0xffc6=%x %x %x %x\n",
          ws->unit, ws->port, rdata0, rdata1, rdata2, rdata3) ;
   rv = TSC_REG_WRITE(ws->unit, ws, 0x0, 0xffcc, 0x5404) ;
   rv = TSC_REG_READ(ws->unit, ws, 0x0, 0xffc6, &rdata0) ;
   rv = TSC_REG_READ(ws->unit, ws, 0x0, 0xffc6, &rdata1) ;
   rv = TSC_REG_READ(ws->unit, ws, 0x0, 0xffc6, &rdata2) ;
   rv = TSC_REG_READ(ws->unit, ws, 0x0, 0xffc6, &rdata3) ;
   printf("Diagnosis: u=%0d p=%0d 0xffcc=0x5404 0xffc6=%x %x %x %x\n",
          ws->unit, ws->port, rdata0, rdata1, rdata2, rdata3) ;
   return SOC_E_NONE ;
}

int tscmod_diag_g_rxlos(tscmod_st *ws) 
{
    printf("%-22s: u=%0d p=%0d restart_time=%0dus linkup_time=%0dus\n",
           FUNCTION_NAME(), ws->unit, ws->port, ws->rxlos_restart_time, ws->rxlos_linkup_time);
    return SOC_E_NONE ;
}

int tscmod_diag_g_info(tscmod_st *ws)
{
   int i ;
   printf("Syntax: phy diag <port> peek fb=<N> l=<n>\n") ;
   printf("        l=lane number or address\n") ;
   for(i=0; i< TSCMOD_DIAG_G_TYPE_ILLEGAL; i++) {
      if(i==TSCMOD_DIAG_G_MISC1) {
         printf("%-22s: fb=%0d uC ram read\n", e2s_tscmod_diag_general_type[i], i) ;
      } else if(i==TSCMOD_DIAG_G_MISC3) {
         printf("%-22s: fb=%0d uC state read\n", e2s_tscmod_diag_general_type[i], i) ;
      } else {
         printf("%-22s: fb=%0d\n", e2s_tscmod_diag_general_type[i], i) ;
      }
   }
   return SOC_E_NONE ;
}


/* lane should have the selected lane info */
int tscmod_diag_general(tscmod_st *ws)
{
   int lane_s, lane_e, lane, lid, func, rv ;
   int tmp_sel, tmp_dxgxs, tmp_lane ;
   func = ws->per_lane_control >> 4 ;
   lane = ws->per_lane_control & 0xf ;

   rv        = SOC_E_NONE ;
   tmp_sel   = ws->lane_select ;
   tmp_dxgxs = ws->dxgxs ;
   tmp_lane  = ws->this_lane ;


   if( (func >= TSCMOD_DIAG_G_TYPE_ILLEGAL)||(func==TSCMOD_DIAG_G_INFO)) {
      tscmod_diag_g_info(ws) ;
      return SOC_E_NONE ;
   }
   if(func == TSCMOD_DIAG_G_MISC1){
      lane_s = 0 ; lane_e = 0 ;
   } else {
      if( lane < 4 ) {
         lane_s = lane ;
         lane_e = lane ;
      } else {
         if( lane == 4 ) {
            lane_s = 0 ; lane_e = 1 ;
         } else if (lane==5 ) {
            lane_s = 2 ; lane_e = 3 ;
         } else {
            lane_s = 0 ;
            lane_e = 3 ;
         }
      }
   }
   ws->dxgxs = 0 ;
   for(lid = lane_s ; lid <= lane_e ; lid++) {
      if( func < TSCMOD_DIAG_G_TYPE_ILLEGAL ) {
         printf("Diagnosis register peeks FB=[%s] l=%0d this_lane=%0d\n",
                e2s_tscmod_diag_general_type[func], lid, ws->this_lane) ;
      }
      ws->lane_select = getLaneSelect(lid) ;
      ws->this_lane   = lid ;

      if( func == TSCMOD_DIAG_G_PORT ) {           /* = 0 */
         rv = tscmod_diag_g_port(ws, lane) ;
      } else if ( func == TSCMOD_DIAG_G_SPEED ) {  /* = 2 */
         tscmod_diag_g_speed(ws, lane) ;
      } else if ( func == TSCMOD_DIAG_G_AN ) {     /* = 3 */
         tscmod_diag_g_an(ws, lane) ;
      } else if ( func == TSCMOD_DIAG_G_DSC ) {
         tscmod_diag_g_dsc(ws, lane) ;
      } else if ( func == TSCMOD_DIAG_G_DSC_CFG ) {
         tscmod_diag_g_dsc_cfg(ws, lane) ;
      } else if ( func == TSCMOD_DIAG_G_UCODE ){
         tscmod_diag_g_ucode(ws, lane) ;
      } else if ( func == TSCMOD_DIAG_G_PATTERN ){
         tscmod_diag_g_pattern(ws, lane) ;
      } else if ( func == TSCMOD_DIAG_G_PATH ) {
         tscmod_diag_g_path(ws, lane) ;
      } else if ( func == TSCMOD_DIAG_G_CL72 ) {
         tscmod_diag_g_cl72(ws, lane) ;
      } else if ( func == TSCMOD_DIAG_G_MISC1 ){
         tscmod_diag_g_rd_ram(ws, 0, lane) ;
      } else if ( func == TSCMOD_DIAG_G_MISC2 ){
         tscmod_diag_g_rd_fec_err(ws, lane) ;
      } else if ( func == TSCMOD_DIAG_G_MISC3 ){
         tscmod_diag_g_rd_uC(ws, lane) ;
      } else if ( func == TSCMOD_DIAG_G_RXLOS ){
         tscmod_diag_g_rxlos(ws) ;
      }
   }

   ws->lane_select = tmp_sel ;
   ws->dxgxs       = tmp_dxgxs ;
   ws->this_lane   = tmp_lane ;

   return (SOC_E_NONE|rv);
}


int tscmod_diag_link(tscmod_st *ws)
{
   uint16         data16, data_ber, latched;  int linkup, latched_l;
   linkup    = 0 ;
   latched_l = 0 ;
   data_ber  = 0 ;
   data16    = 0 ;
   latched   = 0 ;

   SOC_IF_ERROR_RETURN
      (READ_WC40_RX_X4_STATUS1_PCS_LIVE_STATUSr(ws->unit, ws, &data16));
   linkup = (data16 & RX_X4_STATUS1_PCS_LIVE_STATUS_LINK_STATUS_MASK) >>
                      RX_X4_STATUS1_PCS_LIVE_STATUS_LINK_STATUS_SHIFT ;

   if(ws->per_lane_control == TSCMOD_DIAG_LINK_LIVE) {
      latched_l=0 ;
   } else {
      SOC_IF_ERROR_RETURN
         (READ_WC40_RX_X4_STATUS1_PCS_LATCHED_STATUS_0r(ws->unit, ws, &latched));
      latched_l=(latched &RX_X4_STATUS1_PCS_LATCHED_STATUS_0_LINK_STATUS_LL_MASK)>>
                          RX_X4_STATUS1_PCS_LATCHED_STATUS_0_LINK_STATUS_LL_SHIFT;
   }
   if((linkup==1) && (latched_l==0)) {
     ws->accData = TRUE ;

     if((ws->ctrl_type  & TSCMOD_CTRL_TYPE_LINK_BER) ){
        /* 0xc14e */
        SOC_IF_ERROR_RETURN
           (READ_WC40_RX_X4_STATUS0_BERCOUNTr(ws->unit, ws, &data_ber));
        data_ber = (data_ber & RX_X4_STATUS0_BERCOUNT_BER_COUNT_PER_LN_MASK)>>
                               RX_X4_STATUS0_BERCOUNT_BER_COUNT_PER_LN_SHIFT ;
        ws->linkup_bert_cnt[0] = ws->linkup_bert_cnt[0] + data_ber ;
        if(data_ber) {
           printf("%s u=%0d p=%0d ctrl_type=%x bercnt=%0d %0d\n", FUNCTION_NAME(),
                  ws->unit, ws->port, ws->ctrl_type, data_ber, ws->linkup_bert_cnt[0]) ;
        }
     }
   } else {
      ws->accData = FALSE ;
      if(linkup & latched_l ) {
         if(ws->verbosity & TSCMOD_DBG_LINK)
            printf("%s u=%0d p=%0d HOLDDN PCS_LIVE_STATUS=%x LATCHED=%x this_lane=%0d\
 rtn=%0d\n", FUNCTION_NAME(), ws->unit, ws->port, linkup, latched_l, ws->accData, ws->this_lane);
      }
   }
#if defined (_DV_TRIDENT2)
    printf("%s u=%0d p=%0d PCS_LIVE_STATUS=%x LATCHED_L=%x rtn=%0d\n",
           FUNCTION_NAME(), ws->unit, ws->port, linkup, latched_l, ws->accData) ;
#else
    if(ws->verbosity & TSCMOD_DBG_LINK)
         printf("%s u=%0d p=%0d PCS_LIVE_STATUS=%x(reg=0x%x) LATCHED_L=%x(reg=0x%x) rtn=%0d this_lane=%0d data=%x\n",
                FUNCTION_NAME(), ws->unit, ws->port, linkup, data16, latched_l, latched, ws->accData, ws->this_lane, data16);
#endif


   return SOC_E_NONE;
}

int tscmod_diag_internal_tfc(tscmod_st *ws)
{
  return SOC_E_NONE;
}

int tscmod_diag_dfe(tscmod_st *ws)
{
  return SOC_E_NONE;
}

/*
void printTaps(int lane, uint16 data)
{
  return;
}
*/

int tscmod_diag_ieee(tscmod_st *ws)
{
  return SOC_E_NONE;
}

int tscmod_diag_eee(tscmod_st *ws)
{
    uint16 data ; 
    SOC_IF_ERROR_RETURN
        (READ_WC40_RX_X4_CONTROL0_PCS_CONTROL_0r(ws->unit, ws, &data));
    if(data&RX_X4_CONTROL0_PCS_CONTROL_0_LPI_ENABLE_MASK) {
        ws->accData = 1 ;
    } else {
        ws->accData = 0 ;
    }
    return SOC_E_NONE;
}


#define MAX_LOOPS 47
#define MIN_RUNTIME  1
#define INDEX_UNINITIALIZED  (-1)
#define SPD_UP_MAX_RUNTIME 64
#ifdef BER_EXTRAP_SPD_UP
  #define MAX_RUNTIME SPD_UP_MAX_RUNTIME
#else
  #define MAX_RUNTIME 256
#endif
#define HI_CONFIDENCE_ERR_CNT 100    /* bit errors to determine high confidence */
#define LO_CONFIDENCE_MIN_ERR_CNT 10 /* bit errors, exit condition for low confidence */
#define HI_CONFIDENCE_MIN_ERR_CNT 20 /* bit errors exit condition for high confidence */
#define VEYE_UNIT 1.75
#define HEYE_UNIT 3.125
#define TSC_UTIL_MAX_ROUND_DIGITS (8)


typedef struct {
    int total_errs[MAX_LOOPS];
    int total_elapsed_time[MAX_LOOPS];
    int mono_flags[MAX_LOOPS];
    int max_loops;
    int offset_max;
    int veye_cnt;
    uint32 rate;      /* frequency in KHZ */
    int first_good_ber_idx;
    int first_small_errcnt_idx;
    int target_ber;    /* this is the target BER example, -18, representing 10^(-18) */
    int tolerance;     /* this is the percentage  specify +/- % opening/margin of the target_ber */
} TSCMOD_EYE_DIAG_INFOt;

#ifndef __KERNEL__

float _tscmod_util_round_real( float original_value, int decimal_places )
{
   float shift_digits[TSC_UTIL_MAX_ROUND_DIGITS+1] = { 0.0, 10.0, 100.0, 1000.0,
             10000.0, 100000.0, 1000000.0, 10000000.0, 100000000.0 };
   float shifted;
   float rounded;
   if (decimal_places > TSC_UTIL_MAX_ROUND_DIGITS ) {
      printf("ERROR: Maximum digits to the right of decimal for rounding "
             "exceeded. Max %d, requested %d\n",
             TSC_UTIL_MAX_ROUND_DIGITS, decimal_places);
      return 0.0;
   }
   /* shift to preserve the desired digits to the right of the decimal */
   shifted = original_value * shift_digits[decimal_places];

   /* convert to integer and back to float to truncate undesired precision */
   shifted = (float)(floor(shifted+0.5));

   /* shift back to place decimal point correctly */
   rounded = shifted / shift_digits[decimal_places];
   return rounded;
}

int _tscmod_avg_vga_dfe(tscmod_st* ws, int tapsel, int *avg_value)
{
   uint16 vga_frzval;
   uint16 vga_frcfrz;
   uint16 dfe_frzval;
   uint16 dfe_frcfrz;
   uint16 frzval;
   uint16 frcfrz;
   int val_min = 0;
   int val_max = 0;
   int val_avg;
   int val_curr = 0;
   uint16 avg_cnt;
   uint16 loop_var;
   uint16 data16;
   int rem;

   avg_cnt = 40;
   val_avg = 0;

   SOC_IF_ERROR_RETURN
      (READ_WC40_DSC2B0_ACQ_SM_CTRL1r(ws->unit,ws, &data16));
   vga_frzval = (data16 & DSC2B0_ACQ_SM_CTRL1_VGA_FRZVAL_MASK) >>
      DSC2B0_ACQ_SM_CTRL1_VGA_FRZVAL_SHIFT;
   vga_frcfrz = (data16 & DSC2B0_ACQ_SM_CTRL1_VGA_FRCFRZ_MASK) >>
      DSC2B0_ACQ_SM_CTRL1_VGA_FRCFRZ_SHIFT;
   dfe_frzval = (data16 & DSC2B0_ACQ_SM_CTRL1_DFE_FRZVAL_MASK) >>
      DSC2B0_ACQ_SM_CTRL1_DFE_FRZVAL_SHIFT;
   dfe_frcfrz = (data16 & DSC2B0_ACQ_SM_CTRL1_DFE_FRCFRZ_MASK) >>
      DSC2B0_ACQ_SM_CTRL1_DFE_FRCFRZ_SHIFT;

   if (tapsel == 0) {
       frzval = vga_frzval;
       frcfrz = vga_frcfrz;
       val_min = 47;
       val_max = 0;
   } else {
       frzval = (dfe_frzval & (1 << (tapsel-1))) / (1 << (tapsel-1));
       frcfrz = dfe_frcfrz;
       if (tapsel == 1) {
           val_min = 63;
           val_max = 0;
       } else if ((tapsel == 2) || (tapsel == 3)) {
           val_min = 31;
           val_max = -31;
       } else if ((tapsel == 4) || (tapsel == 5)) {
           val_min = 15;
           val_max = -15;
       }
   }

   if ((frzval == 1) && (frcfrz == 1)) {
       avg_cnt = 1;
   }

   /* Const DSC_3_ADDR = &H8220 */
   for (loop_var = 0; loop_var < avg_cnt; loop_var++) {
      switch(tapsel) {
      case 0:
         SOC_IF_ERROR_RETURN
            (READ_WC40_DSC3B0_DFE_VGA_STATUS0r(ws->unit,ws, &data16));
         val_curr = data16 & DSC3B0_DFE_VGA_STATUS0_VGA_SUM_MASK;
         break;

      case 1:
         SOC_IF_ERROR_RETURN
            (READ_WC40_DSC3B0_DFE_VGA_STATUS0r(ws->unit,ws,&data16));
         val_curr = (data16 & DSC3B0_DFE_VGA_STATUS0_DFE_TAP_1_BIN_MASK) >>
            DSC3B0_DFE_VGA_STATUS0_DFE_TAP_1_BIN_SHIFT;
         break;

      case 2:
         SOC_IF_ERROR_RETURN
            (READ_WC40_DSC3B0_DFE_VGA_STATUS1r(ws->unit,ws, &data16));
         val_curr = data16 & DSC3B0_DFE_VGA_STATUS1_DFE_TAP_2_BIN_MASK;
         if (val_curr >= 32) {
            val_curr -= 64;
         }
         break;

      case 3:
         SOC_IF_ERROR_RETURN
            (READ_WC40_DSC3B0_DFE_VGA_STATUS1r(ws->unit,ws,&data16));
         val_curr = (data16 & DSC3B0_DFE_VGA_STATUS1_DFE_TAP_3_BIN_MASK) >>
            DSC3B0_DFE_VGA_STATUS1_DFE_TAP_3_BIN_SHIFT;
         if (val_curr >= 32) {
            val_curr -= 64;
         }
         break;

      case 4:
         SOC_IF_ERROR_RETURN
            (READ_WC40_DSC3B0_DFE_VGA_STATUS2r(ws->unit,ws, &data16));
         val_curr = data16 & DSC3B0_DFE_VGA_STATUS2_DFE_TAP_4_BIN_MASK;
         if (val_curr >= 16) {
            val_curr -= 32;
         }
         break;

      case 5:
         SOC_IF_ERROR_RETURN
            (READ_WC40_DSC3B0_DFE_VGA_STATUS2r(ws->unit,ws,&data16));
         val_curr = (data16 & DSC3B0_DFE_VGA_STATUS2_DFE_TAP_5_BIN_MASK) >>
            DSC3B0_DFE_VGA_STATUS2_DFE_TAP_5_BIN_SHIFT;
         if (val_curr >= 16) {
            val_curr -= 32;
         }
         break;

      default:
         break;
      }

      LOG_INFO(BSL_LS_SOC_PHY,
               (BSL_META_U(ws->unit,
                           "TSC_VEYE : val_curr = %d, val_avg = %d, tap_select = %d\n"),
                val_curr,val_avg,tapsel));

      val_avg = (val_avg + val_curr);

      if (val_curr < val_min) {
          val_min = val_curr;
      }

      if (val_curr > val_max) {
          val_max = val_curr;
      }
      /* delay 20ms */
#ifdef _SDK_TSCMOD_
     sal_usleep(20000);  /* 10ms */
#endif
   } /* end for loop */

   /* average value */
   rem = val_avg % avg_cnt;
   val_avg = val_avg / avg_cnt;

   /* add one if remainder is more than half */
   val_avg += (2 * rem) / avg_cnt;
   *avg_value = val_avg;

   LOG_INFO(BSL_LS_SOC_PHY,
            (BSL_META_U(ws->unit,
                        "TSC_VEYE : val_avg = %d, tap_select = %d\n"), val_avg,tapsel));
   return SOC_E_NONE;
}

int  _tscmod_set_hoff(tscmod_st* ws,  int hoff)
{
   int rv = 0;

   /* write horizontal offset */
   SOC_IF_ERROR_RETURN
      (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws,
                  hoff << DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT,
                  DSC1B0_UC_CTRL_SUPPLEMENT_INFO_MASK));

   SOC_IF_ERROR_RETURN
      (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, 0x0, DSC1B0_UC_CTRL_READY_FOR_CMD_MASK));
   SOC_IF_ERROR_RETURN
      (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, 0x0, DSC1B0_UC_CTRL_ERROR_FOUND_MASK));
   SOC_IF_ERROR_RETURN
      (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, 0x0, DSC1B0_UC_CTRL_CMD_INFO_MASK));
   SOC_IF_ERROR_RETURN
      (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws,
         (0x2| (hoff << DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT)),
		   (DSC1B0_UC_CTRL_GP_UC_REQ_MASK|
                  DSC1B0_UC_CTRL_SUPPLEMENT_INFO_MASK)));

   /* wait for uC ready for command:  bit7=1    */
   rv = tscmod_regbit_set_wait_check(ws, TSC40_DSC1B0_UC_CTRLr,
                                    DSC1B0_UC_CTRL_READY_FOR_CMD_MASK,1, TSC40_PLL_WAIT*1000);

   ws->accData = ((ws->accData) & DSC1B0_UC_CTRL_ERROR_FOUND_MASK) >> DSC1B0_UC_CTRL_ERROR_FOUND_SHIFT;

   if ((rv <0) || (ws->accData != 0)) {
      printf("TSC_EYE : uController not ready pass 1!: u=%d n", ws->unit);
   }
   return rv;
}

int  _tscmod_get_max_hoffset_right(tscmod_st* ws)
{
   int rv = 0;
   uint16 data;


   SOC_IF_ERROR_RETURN
      (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws,
                  127 << DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT,
                  DSC1B0_UC_CTRL_SUPPLEMENT_INFO_MASK));
   SOC_IF_ERROR_RETURN
      (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, 0x0, DSC1B0_UC_CTRL_READY_FOR_CMD_MASK));
   SOC_IF_ERROR_RETURN
      (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, 0x0, DSC1B0_UC_CTRL_ERROR_FOUND_MASK));
   SOC_IF_ERROR_RETURN
      (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, 0x0, DSC1B0_UC_CTRL_CMD_INFO_MASK));
   SOC_IF_ERROR_RETURN
      (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws,
		  (0x2| (127 << DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT)),
		  (DSC1B0_UC_CTRL_GP_UC_REQ_MASK|
                  DSC1B0_UC_CTRL_SUPPLEMENT_INFO_MASK)));

   /* wait for uC ready for command:  bit7=1    */
   rv = tscmod_regbit_set_wait_check(ws, TSC40_DSC1B0_UC_CTRLr,
                                    DSC1B0_UC_CTRL_READY_FOR_CMD_MASK,1,TSC40_PLL_WAIT*1000);

   ws->accData = ((ws->accData) & DSC1B0_UC_CTRL_ERROR_FOUND_MASK) >> DSC1B0_UC_CTRL_ERROR_FOUND_SHIFT;

   if ((rv <0) || (ws->accData != 1)) {
      printf("TSC_EYE : uController not ready pass 1!: u=%d n", ws->unit);
      return (SOC_E_TIMEOUT);

   }
   /* read back the maximum offset */
   SOC_IF_ERROR_RETURN
      (READ_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, &data));
   data &= DSC1B0_UC_CTRL_SUPPLEMENT_INFO_MASK;
   data >>= DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT;
   ws->accData = data;

   return rv;
}

int  _tscmod_get_max_hoffset_left(tscmod_st* ws)
{
   int rv = 0;
    uint16 data;


    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws,
                  -128 << DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT,
                  DSC1B0_UC_CTRL_SUPPLEMENT_INFO_MASK));
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, 0x0, DSC1B0_UC_CTRL_READY_FOR_CMD_MASK));
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, 0x0, DSC1B0_UC_CTRL_ERROR_FOUND_MASK));
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, 0x0, DSC1B0_UC_CTRL_CMD_INFO_MASK));
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws,
                                     (0x2| (-128 << DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT)),
                                     (DSC1B0_UC_CTRL_GP_UC_REQ_MASK|
                  DSC1B0_UC_CTRL_SUPPLEMENT_INFO_MASK)));

    /* wait for uC ready for command:  bit7=1    */
    rv = tscmod_regbit_set_wait_check(ws, TSC40_DSC1B0_UC_CTRLr,
             DSC1B0_UC_CTRL_READY_FOR_CMD_MASK,1, TSC40_PLL_WAIT*1000);
    ws->accData = ((ws->accData) & DSC1B0_UC_CTRL_ERROR_FOUND_MASK) >> DSC1B0_UC_CTRL_ERROR_FOUND_SHIFT;

    if ((rv < 0) || (ws->accData != 1)) {
            printf("TSC_EYE : uController not ready pass 1!: u=%d n", ws->unit);
            return (SOC_E_TIMEOUT);
    }
    /* read back the maximum offset */
    SOC_IF_ERROR_RETURN
        (READ_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, &data));
    data &= DSC1B0_UC_CTRL_SUPPLEMENT_INFO_MASK;
    data >>= DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT;
    ws->accData = -data;

    return rv;
}

int  _tscmod_set_voff(tscmod_st* ws, int voff)
{
   int rv = 0;

   /* write vertical offset */
   SOC_IF_ERROR_RETURN
      (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws,
                  voff << DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT,
                  DSC1B0_UC_CTRL_SUPPLEMENT_INFO_MASK));

   SOC_IF_ERROR_RETURN
      (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, 0x0, DSC1B0_UC_CTRL_READY_FOR_CMD_MASK));
   SOC_IF_ERROR_RETURN
      (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, 0x0, DSC1B0_UC_CTRL_ERROR_FOUND_MASK));
   SOC_IF_ERROR_RETURN
      (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, 0x0, DSC1B0_UC_CTRL_CMD_INFO_MASK));
   SOC_IF_ERROR_RETURN
      (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws,
                                     (0x3| (voff << DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT)),
                                     (DSC1B0_UC_CTRL_GP_UC_REQ_MASK|
                  DSC1B0_UC_CTRL_SUPPLEMENT_INFO_MASK)));

   /* wait for uC ready for command:  bit7=1    */
   rv = tscmod_regbit_set_wait_check(ws, TSC40_DSC1B0_UC_CTRLr,
                                    DSC1B0_UC_CTRL_READY_FOR_CMD_MASK,1, TSC40_PLL_WAIT*1000);
   ws->accData = ((ws->accData) & DSC1B0_UC_CTRL_ERROR_FOUND_MASK) >> DSC1B0_UC_CTRL_ERROR_FOUND_SHIFT;

   if ((rv <0) || (ws->accData != 0)) {
      printf("WC_EYE set_voffset: uController not ready pass 1!: u=%d\n", ws->unit);
   }

   return rv;
}
int  _tscmod_get_min_voffset(tscmod_st* ws)
{
   int rv = 0;
   uint16 data;

   SOC_IF_ERROR_RETURN
      (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws,
                   -128 << DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT,
                   DSC1B0_UC_CTRL_SUPPLEMENT_INFO_MASK));
   SOC_IF_ERROR_RETURN
      (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, 0x0, DSC1B0_UC_CTRL_READY_FOR_CMD_MASK));
   SOC_IF_ERROR_RETURN
      (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, 0x0, DSC1B0_UC_CTRL_ERROR_FOUND_MASK));
   SOC_IF_ERROR_RETURN
      (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, 0x0, DSC1B0_UC_CTRL_CMD_INFO_MASK));
   SOC_IF_ERROR_RETURN
      (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws,
                                     (0x3| (-128 << DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT)),
                                     (DSC1B0_UC_CTRL_GP_UC_REQ_MASK|
                  DSC1B0_UC_CTRL_SUPPLEMENT_INFO_MASK)));

   /* wait for uC ready for command:  bit7=1    */
   rv = tscmod_regbit_set_wait_check(ws, TSC40_DSC1B0_UC_CTRLr,
                                    DSC1B0_UC_CTRL_READY_FOR_CMD_MASK,1, TSC40_PLL_WAIT*1000);
   ws->accData = ((ws->accData) & DSC1B0_UC_CTRL_ERROR_FOUND_MASK) >> DSC1B0_UC_CTRL_ERROR_FOUND_SHIFT;

   if ((rv <0) || (ws->accData != 1)) {
      printf("WC_EYE : uController not ready pass 1!: u=%d n", ws->unit);
      return (SOC_E_TIMEOUT);
   }
   /* read back the maximum offset */
   SOC_IF_ERROR_RETURN
      (READ_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, &data));
   data &= DSC1B0_UC_CTRL_SUPPLEMENT_INFO_MASK;
   data >>= DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT;
   ws->accData = data - 256;

   return rv;
}
int  _tscmod_get_max_voffset(tscmod_st* ws)
{
   int rv = 0;
   uint16 data;

   SOC_IF_ERROR_RETURN
      (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws,
                  127 << DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT,
                  DSC1B0_UC_CTRL_SUPPLEMENT_INFO_MASK));
   SOC_IF_ERROR_RETURN
      (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, 0x0, DSC1B0_UC_CTRL_READY_FOR_CMD_MASK));
   SOC_IF_ERROR_RETURN
      (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, 0x0, DSC1B0_UC_CTRL_ERROR_FOUND_MASK));
   SOC_IF_ERROR_RETURN
      (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, 0x0, DSC1B0_UC_CTRL_CMD_INFO_MASK));
   SOC_IF_ERROR_RETURN
      (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws,
                                   (0x3| (127 << DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT)),
                                     (DSC1B0_UC_CTRL_GP_UC_REQ_MASK|
                  DSC1B0_UC_CTRL_SUPPLEMENT_INFO_MASK)));

   /* wait for uC ready for command:  bit7=1    */
   rv = tscmod_regbit_set_wait_check(ws, TSC40_DSC1B0_UC_CTRLr,
                                    DSC1B0_UC_CTRL_READY_FOR_CMD_MASK,1, TSC40_PLL_WAIT*1000);
   ws->accData = ((ws->accData) & DSC1B0_UC_CTRL_ERROR_FOUND_MASK) >> DSC1B0_UC_CTRL_ERROR_FOUND_SHIFT;

   if ((rv <0) || (ws->accData != 1)) {
      printf("WC_EYE : uController not ready pass 1!: u=%d n", ws->unit);
      return (SOC_E_TIMEOUT);
   }
   /* read back the maximum offset */
   SOC_IF_ERROR_RETURN
      (READ_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, &data));
   data &= DSC1B0_UC_CTRL_SUPPLEMENT_INFO_MASK;
   data >>= DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT;
   ws->accData = data;

   return rv;
}

int  _tscmod_get_init_voffset(tscmod_st* ws)
{
   int rv = 0;
   uint16 data;

   /* XXXX using reserve bits */
   SOC_IF_ERROR_RETURN
      (READ_WC40_DSC2B0_DSC_ANA_CTRL3r(ws->unit, ws, &data));
   data = (data & DSC2B0_DSC_ANA_CTRL3_RESERVED0_MASK)
      >> DSC2B0_DSC_ANA_CTRL3_RESERVED0_SHIFT;
   ws->accData = data;

   return rv;
}

int _tscmod_enable_livelink_diagnostics(tscmod_st* ws)
{
   /* first enable livelink bit also */
   SOC_IF_ERROR_RETURN(MODIFY_WC40_DSC1B0_DSC_DIAG_CTRL0r(ws->unit, ws,
                 0x2,   0x2));

   /* 0x820d[0]=1 enable diagnostic */
   SOC_IF_ERROR_RETURN(MODIFY_WC40_DSC1B0_DSC_DIAG_CTRL0r(ws->unit, ws,
             DSC1B0_DSC_DIAG_CTRL0_DIAGNOSTICS_EN_MASK,
             DSC1B0_DSC_DIAG_CTRL0_DIAGNOSTICS_EN_MASK));

   return (SOC_E_NONE);
}

int _tscmod_disable_livelink_diagnostics(tscmod_st* ws)
{
   /* disable diagnostics */
   SOC_IF_ERROR_RETURN(MODIFY_WC40_DSC1B0_DSC_DIAG_CTRL0r(ws->unit, ws, 0,
            DSC1B0_DSC_DIAG_CTRL0_DIAGNOSTICS_EN_MASK));

   /* next disable livelink bit also */
   SOC_IF_ERROR_RETURN(MODIFY_WC40_DSC1B0_DSC_DIAG_CTRL0r(ws->unit, ws,
                0x0,   0x2));

   return (SOC_E_NONE);
}
int _tscmod_enable_deadlink_diagnostics(tscmod_st* ws)
{
   int dfe_vga[6];
    int loop_var;
    uint16 data16;
    /* 0x820d[0]=1 enable diagnostic */
    SOC_IF_ERROR_RETURN(MODIFY_WC40_DSC1B0_DSC_DIAG_CTRL0r(ws->unit, ws,
             DSC1B0_DSC_DIAG_CTRL0_DIAGNOSTICS_EN_MASK,
             DSC1B0_DSC_DIAG_CTRL0_DIAGNOSTICS_EN_MASK));

    /* save original DFE/VGA settings */
    SOC_IF_ERROR_RETURN
        (READ_WC40_DSC2B0_ACQ_SM_CTRL1r(ws->unit, ws, &data16));
    ws->vga_frzval = data16 & DSC2B0_ACQ_SM_CTRL1_VGA_FRZVAL_MASK;
    ws->vga_frcfrz = data16 & DSC2B0_ACQ_SM_CTRL1_VGA_FRCFRZ_MASK;
    ws->dfe_frzval = data16 & DSC2B0_ACQ_SM_CTRL1_DFE_FRZVAL_MASK;
    ws->dfe_frcfrz = data16 & DSC2B0_ACQ_SM_CTRL1_DFE_FRCFRZ_MASK;

    for (loop_var = 0; loop_var <= 5; loop_var++) {
        (void)_tscmod_avg_vga_dfe (ws,loop_var,&dfe_vga[loop_var]);
        if (loop_var == 0) {
            /* freeze VGA */
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_DSC2B0_ACQ_SM_CTRL1r(ws->unit, ws,
                      DSC2B0_ACQ_SM_CTRL1_VGA_FRZVAL_MASK |
                      DSC2B0_ACQ_SM_CTRL1_VGA_FRCFRZ_MASK,
                      DSC2B0_ACQ_SM_CTRL1_VGA_FRZVAL_MASK |
                      DSC2B0_ACQ_SM_CTRL1_VGA_FRCFRZ_MASK));

            /* select VGA */
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_DSC1B0_DFE_VGA_CTRL2r(ws->unit, ws,
                  loop_var << DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_TAPSEL_SHIFT,
                  DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_TAPSEL_MASK));

            /* set the VGA value */
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_DSC1B0_DFE_VGA_CTRL2r(ws->unit, ws,
                   dfe_vga[loop_var] << DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_VAL_SHIFT,
                  DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_VAL_MASK));

            /* VGA/DFE write enable */
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_DSC1B0_DFE_VGA_CTRL2r(ws->unit, ws,
                  DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_EN_MASK,
                  DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_EN_MASK));

            /* VGA/DFE write disable */
            SOC_IF_ERROR_RETURN
                (MODIFY_WC40_DSC1B0_DFE_VGA_CTRL2r(ws->unit, ws,
                  0,
                  DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_EN_MASK));
        }
    }
    /* freeze the DFE */
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DSC2B0_ACQ_SM_CTRL1r(ws->unit, ws,
          (0x1f << DSC2B0_ACQ_SM_CTRL1_DFE_FRZVAL_SHIFT) |
          DSC2B0_ACQ_SM_CTRL1_DFE_FRCFRZ_MASK,
          DSC2B0_ACQ_SM_CTRL1_DFE_FRZVAL_MASK |
          DSC2B0_ACQ_SM_CTRL1_DFE_FRCFRZ_MASK));

    for (loop_var = 1; loop_var <= 5; loop_var++) {
        /* set DFE taps */
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_DSC1B0_DFE_VGA_CTRL2r(ws->unit, ws,
              loop_var << DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_TAPSEL_SHIFT,
              DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_TAPSEL_MASK));

        /* set the DFE tap value */
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_DSC1B0_DFE_VGA_CTRL2r(ws->unit, ws,
               dfe_vga[loop_var] << DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_VAL_SHIFT,
              DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_VAL_MASK));

        /* VGA/DFE write enable */
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_DSC1B0_DFE_VGA_CTRL2r(ws->unit, ws,
              DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_EN_MASK,
              DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_EN_MASK));

        /* VGA/DFE write disable */
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_DSC1B0_DFE_VGA_CTRL2r(ws->unit, ws,0,
              DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_EN_MASK));
    }

    /* display the average value written to VGA/DFE */
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(ws->unit,
                         "TSC_EYE avg dfe/vga: u=%d p=%d, VGA:0x%x, dfe1:0x%x, dfe2:0x%x, dfe3:0x%x, "
                         "dfe4:0x%x, dfe5:0x%x\n"), ws->unit, ws->port, dfe_vga[0],dfe_vga[1],
              dfe_vga[2],dfe_vga[3],dfe_vga[4],dfe_vga[5]));

    return (SOC_E_NONE);
}
int _tscmod_disable_deadlink_diagnostics(tscmod_st* ws)
{
   /* restore the  vga/dfe freeze value before exit */
   SOC_IF_ERROR_RETURN
      (MODIFY_WC40_DSC2B0_ACQ_SM_CTRL1r(ws->unit, ws,
                      ws->vga_frzval,
                      DSC2B0_ACQ_SM_CTRL1_VGA_FRZVAL_MASK));
   SOC_IF_ERROR_RETURN
      (MODIFY_WC40_DSC2B0_ACQ_SM_CTRL1r(ws->unit, ws,
                      ws->dfe_frzval,
                      DSC2B0_ACQ_SM_CTRL1_DFE_FRZVAL_MASK));
   SOC_IF_ERROR_RETURN
      (MODIFY_WC40_DSC2B0_ACQ_SM_CTRL1r(ws->unit, ws,
                      ws->vga_frcfrz,
                      DSC2B0_ACQ_SM_CTRL1_VGA_FRCFRZ_MASK));
   SOC_IF_ERROR_RETURN
      (MODIFY_WC40_DSC2B0_ACQ_SM_CTRL1r(ws->unit, ws,
                      ws->dfe_frcfrz,
                      DSC2B0_ACQ_SM_CTRL1_DFE_FRCFRZ_MASK));

   /* disable diagnostics */
   SOC_IF_ERROR_RETURN(MODIFY_WC40_DSC1B0_DSC_DIAG_CTRL0r(ws->unit, ws, 0,
               DSC1B0_DSC_DIAG_CTRL0_DIAGNOSTICS_EN_MASK));

    return (SOC_E_NONE);

}
int _tscmod_start_deadlink_diagnostics(tscmod_st* ws)
{
   SOC_IF_ERROR_RETURN(tscmod_prbs_check(ws));
   SOC_IF_ERROR_RETURN(tscmod_prbs_check(ws));  /* XXXX required or not */
   return (SOC_E_NONE);
}

int _tscmod_read_deadlink_diagnostics(tscmod_st* ws)
{
   int data ;
   SOC_IF_ERROR_RETURN(tscmod_prbs_check(ws));
   /* not lock or temp lost lock all treated the same */
   if (ws->accData < 0) {
      data = 0x7fff;
   } else {
      data = ws->accData;
   }
   ws->accData = data;
   return (SOC_E_NONE);
}
int _tscmod_start_livelink_diagnostics(tscmod_st* ws)
{
   int rv = 0;
   SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, 0x0, DSC1B0_UC_CTRL_SUPPLEMENT_INFO_MASK));
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, 0x0, DSC1B0_UC_CTRL_READY_FOR_CMD_MASK));
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, 0x0, DSC1B0_UC_CTRL_ERROR_FOUND_MASK));
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, 0x0, DSC1B0_UC_CTRL_CMD_INFO_MASK));
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, 0x6, DSC1B0_UC_CTRL_GP_UC_REQ_MASK));

    /* wait for uC ready for command:  bit7=1    */
    rv = tscmod_regbit_set_wait_check(ws, TSC40_DSC1B0_UC_CTRLr,
             DSC1B0_UC_CTRL_READY_FOR_CMD_MASK,1, TSC40_PLL_WAIT*1000);
    ws->accData = ((ws->accData) & DSC1B0_UC_CTRL_ERROR_FOUND_MASK) >> DSC1B0_UC_CTRL_ERROR_FOUND_SHIFT;

    if ((rv <0) || (ws->accData != 0)) {
       printf("TSC_EYE start_live_link : uController not ready pass 1!: u=%d\n", ws->unit);
    }

    return rv;
}
int _tscmod_stop_livelink_diagnostics(tscmod_st* ws)
{
   int rv = 0;

   SOC_IF_ERROR_RETURN (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws,
                                                     (0x1 << DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT),
                                                     DSC1B0_UC_CTRL_SUPPLEMENT_INFO_MASK));
   SOC_IF_ERROR_RETURN
      (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, 0x0, DSC1B0_UC_CTRL_READY_FOR_CMD_MASK));
   SOC_IF_ERROR_RETURN
      (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, 0x0, DSC1B0_UC_CTRL_ERROR_FOUND_MASK));
   SOC_IF_ERROR_RETURN
      (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, 0x0, DSC1B0_UC_CTRL_CMD_INFO_MASK));
   SOC_IF_ERROR_RETURN
      (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws,
                                     (0x6| (0x1 << DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT)),
                                     (DSC1B0_UC_CTRL_GP_UC_REQ_MASK|
                  DSC1B0_UC_CTRL_SUPPLEMENT_INFO_MASK)));

   /* wait for uC ready for command:  bit7=1    */
   rv = tscmod_regbit_set_wait_check(ws,  TSC40_DSC1B0_UC_CTRLr,
                                    DSC1B0_UC_CTRL_READY_FOR_CMD_MASK,1, TSC40_PLL_WAIT*1000);
   ws->accData = ((ws->accData) & DSC1B0_UC_CTRL_ERROR_FOUND_MASK) >> DSC1B0_UC_CTRL_ERROR_FOUND_SHIFT;

   if ((rv <0) || (ws->accData != 0)) {
      printf("TSC_EYE start_live_link : uController not ready pass 1!: u=%d\n", ws->unit);
   }

   return rv ;
}
int _tscmod_clear_livelink_diagnostics(tscmod_st* ws)
{
   int rv = 0;

   SOC_IF_ERROR_RETURN (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws,
                                                     (0x3 << DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT),
                                                     DSC1B0_UC_CTRL_SUPPLEMENT_INFO_MASK));
   SOC_IF_ERROR_RETURN
      (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, 0x0, DSC1B0_UC_CTRL_READY_FOR_CMD_MASK));
   SOC_IF_ERROR_RETURN
      (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, 0x0, DSC1B0_UC_CTRL_ERROR_FOUND_MASK));
   SOC_IF_ERROR_RETURN
      (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, 0x0, DSC1B0_UC_CTRL_CMD_INFO_MASK));
   SOC_IF_ERROR_RETURN (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws,
                                                    (0x6| (0x3 << DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT)),
                                                    (DSC1B0_UC_CTRL_GP_UC_REQ_MASK|
                  DSC1B0_UC_CTRL_SUPPLEMENT_INFO_MASK)));

   /* wait for uC ready for command:  bit7=1    */
   rv = tscmod_regbit_set_wait_check(ws, TSC40_DSC1B0_UC_CTRLr,
                                    DSC1B0_UC_CTRL_READY_FOR_CMD_MASK,1, TSC40_PLL_WAIT*1000);
   ws->accData = ((ws->accData) & DSC1B0_UC_CTRL_ERROR_FOUND_MASK) >> DSC1B0_UC_CTRL_ERROR_FOUND_SHIFT;

   if ((rv <0) || (ws->accData != 0)) {
      printf("TSC_EYE start_live_link : uController not ready pass 1!: u=%d\n", ws->unit);
   }

   return rv;
}
int _tscmod_read_livelink_diagnostics(tscmod_st* ws)
{
   int rv = 0;
   int i = 0;
   uint32 error_counter = 0;
   uint16 data;


   for (i = 0; i < 4; i++) {
      SOC_IF_ERROR_RETURN (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws,
                                                       (0x2 << DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT),
                                                       DSC1B0_UC_CTRL_SUPPLEMENT_INFO_MASK));
      SOC_IF_ERROR_RETURN
            (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, 0x0, DSC1B0_UC_CTRL_READY_FOR_CMD_MASK));
      SOC_IF_ERROR_RETURN
            (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, 0x0, DSC1B0_UC_CTRL_ERROR_FOUND_MASK));
      SOC_IF_ERROR_RETURN
            (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, 0x0, DSC1B0_UC_CTRL_CMD_INFO_MASK));
      SOC_IF_ERROR_RETURN (MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws,
                                                       (0x6| (0x2 << DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT)),
                                                       (DSC1B0_UC_CTRL_GP_UC_REQ_MASK|
                  DSC1B0_UC_CTRL_SUPPLEMENT_INFO_MASK)));
      /* wait for uC ready for command:  bit7=1    */
      rv = tscmod_regbit_set_wait_check(ws, TSC40_DSC1B0_UC_CTRLr,
                    DSC1B0_UC_CTRL_READY_FOR_CMD_MASK,1, TSC40_PLL_WAIT*1000);
      ws->accData = ((ws->accData) & DSC1B0_UC_CTRL_ERROR_FOUND_MASK) >> DSC1B0_UC_CTRL_ERROR_FOUND_SHIFT;
      if ((rv <0) || (ws->accData != 0)) {
        printf("TSC_EYE : uController not ready pass 1!: u=%d \n", ws->unit);
        return (SOC_E_TIMEOUT);
      }
      SOC_IF_ERROR_RETURN
            (READ_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, &data));
      data &= DSC1B0_UC_CTRL_SUPPLEMENT_INFO_MASK;
      data >>= DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT;

      error_counter +=  data << (8 * i);
   }
   ws->accData  = error_counter;
   return SOC_E_NONE;
}

#if 0
int _tscmod_2d_livelink_eye_margin_data_get(tscmod_st* ws)
{
return SOC_E_NONE;
}
int _tscmod_2d_prbs_eye_margin_data_get(tscmod_st* ws)
{return SOC_E_NONE;
}
#endif  /* end of if 0 */


#ifndef __KERNEL__

int _tscmod_eye_margin_data_get(tscmod_st* ws, TSCMOD_EYE_DIAG_INFOt *pInfo,int type)
{

   int runtime_loop;
   int max_runtime_loop;
   int curr_runtime;
   int offset[MAX_LOOPS];
   int offset_max;
   int loop_var;
   int veye_cnt;
   int hi_confidence_cnt;
   uint32 prbs_status=0;
   int rv = SOC_E_NONE;
   uint16 data16;
   uint16 vga_frzval;
   uint16 vga_frcfrz;
   uint16 dfe_frzval;
   uint16 dfe_frcfrz;
   int pll_div[16] = {32,36,40,42,48,50,52,54,60,64,66,68,70,80,92,100};
   int ref_clk_freq[8] = {25000,100000,125000,156250,187500,161250,50000,106250};
   uint16 clk90_p_offset;
   int dfe_vga[6];
   int tmp=0;
   int max_total_time=0;
   int prbs_stky_flag;
   int prbs_stky_cnt;
   int max_stky_cnt;

   curr_runtime = 0 ;
   /* check what rate the test will run for */
   SOC_IF_ERROR_RETURN(READ_WC40_ANAPLL_STATUSr(ws->unit,ws,&data16));

   /* get the ref_clk divider first */
   pInfo->rate = pll_div[data16 & ANAPLL_STATUS_PLL_MODE_AFE_MASK];

   /* get the ref_clk frequency */
   SOC_IF_ERROR_RETURN(READ_WC40_MAIN0_SETUPr(ws->unit,ws,&data16));

   data16 = (data16 &MAIN0_SETUP_REFCLK_SEL_MASK) >>MAIN0_SETUP_REFCLK_SEL_SHIFT  ;
   pInfo->rate *= ref_clk_freq[data16];

   if(ws->time_upper_bound>=4) {
       max_total_time = ws->time_upper_bound ; 
   } else {
       max_total_time = MAX_RUNTIME;
   }

   /* max loop_cnt for BER test with test time doubling every iteration */
   /* max_runtime_loop = log(MAX_RUNTIME/MIN_RUNTIME)/log(2); */
   max_runtime_loop = 80;
   pInfo->max_loops = MAX_LOOPS;

   LOG_INFO(BSL_LS_SOC_PHY,
            (BSL_META_U(ws->unit,
                        "TSC_VEYE : max_runtime_loop: %d u=%d p=%d\n"),
             max_runtime_loop, ws->unit, ws->port));

   /* Initialize BER array */
   for( loop_var = 0; loop_var < pInfo->max_loops; loop_var++ ) {
      pInfo->total_errs[loop_var] = 0;
   }

   /* 0x820d[0]=1 enable diagnostic */
   SOC_IF_ERROR_RETURN(MODIFY_WC40_DSC1B0_DSC_DIAG_CTRL0r(ws->unit, ws,
             DSC1B0_DSC_DIAG_CTRL0_DIAGNOSTICS_EN_MASK,
             DSC1B0_DSC_DIAG_CTRL0_DIAGNOSTICS_EN_MASK));

   /* save original DFE/VGA settings */
   SOC_IF_ERROR_RETURN(READ_WC40_DSC2B0_ACQ_SM_CTRL1r(ws->unit, ws, &data16));
   vga_frzval = data16 & DSC2B0_ACQ_SM_CTRL1_VGA_FRZVAL_MASK;
   vga_frcfrz = data16 & DSC2B0_ACQ_SM_CTRL1_VGA_FRCFRZ_MASK;
   dfe_frzval = data16 & DSC2B0_ACQ_SM_CTRL1_DFE_FRZVAL_MASK;
   dfe_frcfrz = data16 & DSC2B0_ACQ_SM_CTRL1_DFE_FRCFRZ_MASK;

   for (loop_var = 0; loop_var <= 5; loop_var++) {
      (void)_tscmod_avg_vga_dfe(ws, loop_var, &dfe_vga[loop_var]);
      if (loop_var == 0) {
         /* freeze VGA */
         SOC_IF_ERROR_RETURN
            (MODIFY_WC40_DSC2B0_ACQ_SM_CTRL1r(ws->unit,ws,
                      DSC2B0_ACQ_SM_CTRL1_VGA_FRZVAL_MASK |
                      DSC2B0_ACQ_SM_CTRL1_VGA_FRCFRZ_MASK,
                      DSC2B0_ACQ_SM_CTRL1_VGA_FRZVAL_MASK |
                      DSC2B0_ACQ_SM_CTRL1_VGA_FRCFRZ_MASK));

         /* select VGA */
         SOC_IF_ERROR_RETURN
            (MODIFY_WC40_DSC1B0_DFE_VGA_CTRL2r(ws->unit,ws,
                  loop_var << DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_TAPSEL_SHIFT,
                  DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_TAPSEL_MASK));

         /* set the VGA value */
         SOC_IF_ERROR_RETURN
            (MODIFY_WC40_DSC1B0_DFE_VGA_CTRL2r(ws->unit,ws,
                   dfe_vga[loop_var] << DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_VAL_SHIFT,
                  DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_VAL_MASK));

         /* VGA/DFE write enable */
         SOC_IF_ERROR_RETURN
                (MODIFY_WC40_DSC1B0_DFE_VGA_CTRL2r(ws->unit,ws,
                  DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_EN_MASK,
                  DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_EN_MASK));

         /* VGA/DFE write disable */
         SOC_IF_ERROR_RETURN
                (MODIFY_WC40_DSC1B0_DFE_VGA_CTRL2r(ws->unit,ws,
                  0,
                  DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_EN_MASK));
      }
   }
   /* freeze the DFE */
   SOC_IF_ERROR_RETURN
      (MODIFY_WC40_DSC2B0_ACQ_SM_CTRL1r(ws->unit,ws,
        (0x1f << DSC2B0_ACQ_SM_CTRL1_DFE_FRZVAL_SHIFT) |
        DSC2B0_ACQ_SM_CTRL1_DFE_FRCFRZ_MASK,
        DSC2B0_ACQ_SM_CTRL1_DFE_FRZVAL_MASK |
        DSC2B0_ACQ_SM_CTRL1_DFE_FRCFRZ_MASK));

   for (loop_var = 1; loop_var <= 5; loop_var++) {
      /* set DFE taps */
      SOC_IF_ERROR_RETURN
         (MODIFY_WC40_DSC1B0_DFE_VGA_CTRL2r(ws->unit,ws,
              loop_var << DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_TAPSEL_SHIFT,
              DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_TAPSEL_MASK));

      /* set the DFE tap value */
      SOC_IF_ERROR_RETURN
         (MODIFY_WC40_DSC1B0_DFE_VGA_CTRL2r(ws->unit,ws,
               dfe_vga[loop_var] << DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_VAL_SHIFT,
              DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_VAL_MASK));

      /* VGA/DFE write enable */
      SOC_IF_ERROR_RETURN
         (MODIFY_WC40_DSC1B0_DFE_VGA_CTRL2r(ws->unit,ws,
              DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_EN_MASK,
              DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_EN_MASK));

      /* VGA/DFE write disable */
      SOC_IF_ERROR_RETURN
         (MODIFY_WC40_DSC1B0_DFE_VGA_CTRL2r(ws->unit,ws,
              0,
              DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_EN_MASK));
   }

   LOG_INFO(BSL_LS_SOC_PHY,
            (BSL_META_U(ws->unit,
                        "TSC_EYE avg dfe/vga: u=%d p=%d, VGA:0d%d, dfe1:0d%d, dfe2:0d%d, dfe3:0d%d, "
                        "dfe4:0d%d, dfe5:0d%d\n"), ws->unit, ws->port, dfe_vga[0],dfe_vga[1],
             dfe_vga[2],dfe_vga[3],dfe_vga[4],dfe_vga[5]));

   if (type == TSC_UTIL_HEYE_L) {
      /* Write max to get left eye offset range */
      SOC_IF_ERROR_RETURN(WRITE_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, 0x7f02));
   } else if (type == TSC_UTIL_HEYE_R) {
      /* Write min to get right eye offset range */
      SOC_IF_ERROR_RETURN(WRITE_WC40_DSC1B0_UC_CTRLr(ws->unit, ws,0x8002));

   } else if (type == TSC_UTIL_VEYE_U) {
      /* write max to get vertical offset range */
      SOC_IF_ERROR_RETURN(WRITE_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, 0x7f03));
   } else {  /* vertical eye */
      printf("%s Internal: Bad type:%d passed in\n", FUNCTION_NAME(), type);
   }

   /* wait for uC ready for command:  bit7=1  */
   rv = tscmod_regbit_set_wait_check(ws, TSC40_DSC1B0_UC_CTRLr,
                        DSC1B0_UC_CTRL_READY_FOR_CMD_MASK, 1, TSC40_PLL_WAIT);


   if (rv == SOC_E_TIMEOUT) {
      printf("TSC_EYE : uController not ready pass 1!: u=%d\n", ws->unit);
      return (SOC_E_TIMEOUT);
   }

   /* read out the max value */
   SOC_IF_ERROR_RETURN(READ_WC40_DSC1B0_UC_CTRLr(ws->unit, ws,  &data16));
   offset_max = (data16 >> 8) & 0xff;

   if (type == TSC_UTIL_HEYE_L || type == TSC_UTIL_HEYE_R) {
      offset_max -= 4;
      pInfo->offset_max = offset_max;
   } else {
      pInfo->offset_max = offset_max;
   }

   SOC_IF_ERROR_RETURN(READ_WC40_DSC2B0_ACQ_SM_CTRL1r(ws->unit, ws, &data16));

   LOG_INFO(BSL_LS_SOC_PHY,
            (BSL_META_U(ws->unit,
                        "TSC_EYE : offset_max %d DSC2B0_ctrl 0x%x u=%d p=%d\n"),
             offset_max,data16, ws->unit, ws->port));

   if (offset_max >= MAX_LOOPS) {
        LOG_ERROR(BSL_LS_SOC_PHY,
                  (BSL_META_U(ws->unit,
                              "WC_EYE ERROR: offset_max %d greater than MAX %d  u=%d p=%d\n"),
                   offset_max,MAX_LOOPS, ws->unit, ws->port));
        return SOC_E_NONE;
    }

#ifdef _SDK_TSCMOD_
   sal_usleep(10000);  /* 10ms */
#endif

   hi_confidence_cnt = 0;
   veye_cnt = 0;

   /* big loop */
   for (loop_var = 0; loop_var < offset_max; loop_var++) {
      offset[loop_var] = offset_max-loop_var;
      veye_cnt += 1;
      /* Set a offset */
      if (type == TSC_UTIL_HEYE_R) { data16 =-offset[loop_var];
      } else                      { data16 = offset[loop_var];
      }
      /* write vertical offset */
      SOC_IF_ERROR_RETURN(MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws,
                  data16 << DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT,
                  DSC1B0_UC_CTRL_SUPPLEMENT_INFO_MASK));

      /* 0x8223 register read out */
      SOC_IF_ERROR_RETURN(READ_WC40_DSC3B0_PI_STATUS0r(ws->unit, ws, &clk90_p_offset));
      clk90_p_offset >>= 7;
      clk90_p_offset &= 0x7f;
      tmp = (short)data16;

      LOG_INFO(BSL_LS_SOC_PHY,
               (BSL_META_U(ws->unit,
                           "Starting BER msmt at offset: %d clk90_p_offset: 0x%x loop_var=%0d u=%d p=%d\n"),
                tmp,clk90_p_offset, loop_var, ws->unit, ws->port));

      if (type == TSC_UTIL_HEYE_L || type == TSC_UTIL_HEYE_R) {
         data16 = 2;
      } else {
         data16 = 3;
      }

      /* set offset cmd */
      SOC_IF_ERROR_RETURN(MODIFY_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, data16,
              DSC1B0_UC_CTRL_GP_UC_REQ_MASK));

      /* wait for uC ready for command:  bit7=1    */
      rv = tscmod_regbit_set_wait_check(ws, TSC40_DSC1B0_UC_CTRLr,
             DSC1B0_UC_CTRL_READY_FOR_CMD_MASK,1, TSC40_PLL_WAIT);

      if (rv == SOC_E_TIMEOUT) {
         printf("TSC_EYE: uC waits for offset fail: u=%d p=%d\n",ws->unit,ws->port);
         return (SOC_E_TIMEOUT);
      }

      pInfo->total_errs[loop_var] = 0;
      pInfo->total_elapsed_time[loop_var] = 0;

      /* enable PRBS before this function. Clear PRBS error. read a few times */
      SOC_IF_ERROR_RETURN(tscmod_prbs_check(ws));
      SOC_IF_ERROR_RETURN(tscmod_prbs_check(ws));
      SOC_IF_ERROR_RETURN(tscmod_prbs_check(ws));

      for (runtime_loop = 0; runtime_loop <= max_runtime_loop; runtime_loop++) {
         if (runtime_loop == 0) {
            curr_runtime = 1 * MIN_RUNTIME;
         } else if (curr_runtime < (4 * MIN_RUNTIME)) {
            curr_runtime = (1 << (runtime_loop - 1)) * MIN_RUNTIME;
         }
         LOG_INFO(BSL_LS_SOC_PHY,
                  (BSL_META_U(ws->unit,
                              "Starting prbs run for %d seconds run_loop=%0d: u=%d p=%d\n"),
                   curr_runtime, runtime_loop, ws->unit, ws->port));

         prbs_stky_flag = 1;
         prbs_stky_cnt = 0;
         if (curr_runtime < 4) {
            max_stky_cnt = 4;
         } else {
            max_stky_cnt = 2;
         }

         while (prbs_stky_flag) {
            /* wait for specified amount of time to collect the PRBS error */
#ifdef _SDK_TSCMOD_
            sal_usleep(curr_runtime * 1000000);
#endif
            SOC_IF_ERROR_RETURN(tscmod_prbs_check(ws));

            prbs_status = ws->accData;
            prbs_stky_flag = 0;
            if (prbs_status == -1) { /* not lock */
               printf("PRBS not locked, loop_num %d status=%d u=%d p=%d\n",
                        loop_var,prbs_status,ws->unit, ws->port);
               prbs_status = 0x7fff;  /* max error */
            } else if (prbs_status == -2) {
               printf("PRBS momentarily lost lock, loop_num %d u=%d p=%d\n",
                        loop_var,ws->unit, ws->port);
               /* legacy PRBS momentarily lose link, retry limited times */
               prbs_stky_cnt += 1;
               if (prbs_stky_cnt < max_stky_cnt) {
                  prbs_stky_flag = 1;
               } else {
                  prbs_status = 0x7fff; /* default if retry fails */
               }
            }
         }

         pInfo->total_errs[loop_var] +=  prbs_status ;
         pInfo->total_elapsed_time[loop_var] +=  curr_runtime;

         if ((pInfo->total_errs[loop_var] >= HI_CONFIDENCE_ERR_CNT) ||
             (pInfo->total_elapsed_time[loop_var] >= max_total_time)) {
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(ws->unit,
                                 "TSC_EYE: done PRBS err count: u=%d p=%d, total_err: %d"
                                 " elapsed_time: %d, loop_num: %d\n"), ws->unit, ws->port,
                      pInfo->total_errs[loop_var],pInfo->total_elapsed_time[loop_var],loop_var));
            break;
         }
      }

      LOG_INFO(BSL_LS_SOC_PHY,
               (BSL_META_U(ws->unit,
                           "finishing BER msmt at offset: %d clk90_p_offset: 0x%x loop_var=%0d u=%d p=%d\n"),
                tmp,clk90_p_offset, loop_var, ws->unit, ws->port));

      /* Determine high-confidence iterations */
      if (pInfo->total_errs[loop_var] >= HI_CONFIDENCE_ERR_CNT) {
         hi_confidence_cnt = hi_confidence_cnt + 1;
      }

      if (((hi_confidence_cnt >= 2) && (pInfo->total_errs[loop_var] <
                                        HI_CONFIDENCE_MIN_ERR_CNT)) ||
          ((hi_confidence_cnt <  2) &&
           (pInfo->total_errs[loop_var] < LO_CONFIDENCE_MIN_ERR_CNT)) ) {
         break;  /* exit for loop */
      }
   }   /* for loop_var */

   /* Undo setup */
   if (type == TSC_UTIL_HEYE_L || type == TSC_UTIL_HEYE_R) {
      data16 = 2;
   } else {
      data16 = 3;
   }

   /* set vertical offset back to 0 */
   SOC_IF_ERROR_RETURN(WRITE_WC40_DSC1B0_UC_CTRLr(ws->unit, ws, data16));

   /* wait for uC ready for command:  bit7=1 */
   rv = tscmod_regbit_set_wait_check(ws, TSC40_DSC1B0_UC_CTRLr,
                        DSC1B0_UC_CTRL_READY_FOR_CMD_MASK,1, TSC40_PLL_WAIT);

   if (rv == SOC_E_TIMEOUT) {
      printf("TSC_VEYE : uC waits for offset=0 fail!: u=%d p=%d\n",ws->unit, ws->port);
      return (SOC_E_TIMEOUT);
   }

   SOC_IF_ERROR_RETURN(MODIFY_WC40_DSC2B0_ACQ_SM_CTRL1r(ws->unit,ws,vga_frzval,
                    DSC2B0_ACQ_SM_CTRL1_VGA_FRZVAL_MASK));
   SOC_IF_ERROR_RETURN(MODIFY_WC40_DSC2B0_ACQ_SM_CTRL1r(ws->unit,ws,dfe_frzval,
                    DSC2B0_ACQ_SM_CTRL1_DFE_FRZVAL_MASK));
   SOC_IF_ERROR_RETURN(MODIFY_WC40_DSC2B0_ACQ_SM_CTRL1r(ws->unit,ws,vga_frcfrz,
                    DSC2B0_ACQ_SM_CTRL1_VGA_FRCFRZ_MASK));
   SOC_IF_ERROR_RETURN(MODIFY_WC40_DSC2B0_ACQ_SM_CTRL1r(ws->unit,ws,dfe_frcfrz,
                    DSC2B0_ACQ_SM_CTRL1_DFE_FRCFRZ_MASK));

   /* disable diagnostics */
   SOC_IF_ERROR_RETURN(MODIFY_WC40_DSC1B0_DSC_DIAG_CTRL0r(ws->unit, ws, 0,
             DSC1B0_DSC_DIAG_CTRL0_DIAGNOSTICS_EN_MASK));

   /* Clear prbs monitor. Ignore the return value and error count. */
   SOC_IF_ERROR_RETURN (tscmod_prbs_check(ws));
   pInfo->veye_cnt = veye_cnt;
   return SOC_E_NONE;
}


STATIC int
_tscmod_eye_margin_ber_cal(TSCMOD_EYE_DIAG_INFOt *pInfo, int type)
{
#ifdef COMPILER_HAS_DOUBLE
    COMPILER_DOUBLE bers[MAX_LOOPS]; /* computed bit error rate */
    COMPILER_DOUBLE margins[MAX_LOOPS]; /* Eye margin @ each measurement*/
    int loop_var;
    COMPILER_DOUBLE eye_unit;
    COMPILER_DOUBLE curr_ber_log;
    COMPILER_DOUBLE prev_ber_log = 0;
    COMPILER_DOUBLE good_ber_level = -7.8;

    /* Initialize BER array */
    for( loop_var = 0; loop_var < MAX_LOOPS; loop_var++ ) {
        bers[loop_var] = 0.0;
    }

    if (type == TSC_UTIL_HEYE_L || type == TSC_UTIL_HEYE_R) {
        eye_unit = HEYE_UNIT;
    } else {
        eye_unit = VEYE_UNIT;
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META("\nBER measurement at each offset, num_data_points: %d\n"),
pInfo->veye_cnt));

    for (loop_var = 0; loop_var < pInfo->veye_cnt; loop_var++) {
        margins[loop_var] = (pInfo->offset_max-loop_var)*eye_unit;
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META("BER measurement at offset: %f\n"), margins[loop_var]));
        if(type == TSC_UTIL_HEYE_L || type == TSC_UTIL_HEYE_R) {
            printf("[H=%d V=0] ", pInfo->offset_max-loop_var) ;
        } else {
            printf("[H=0 V=%d] ", pInfo->offset_max-loop_var) ;
        }
        /* Compute BER */
        if (pInfo->total_errs[loop_var] == 0) {
            bers[loop_var] = 1.0/(COMPILER_DOUBLE)pInfo->total_elapsed_time[loop_var]/pInfo->rate;
            bers[loop_var] /= 1000;

            LOG_WARN(BSL_LS_SOC_PHY,
                     (BSL_META("BER @ %04f %% = 1e%04f (%d errors in %d seconds)\n"),
(COMPILER_DOUBLE)((pInfo->offset_max-loop_var)*eye_unit),
                      1.0*(log10(bers[loop_var])),
                      pInfo->total_errs[loop_var],
                      pInfo->total_elapsed_time[loop_var]));
        } else {
            bers[loop_var] = (COMPILER_DOUBLE)(pInfo->total_errs[loop_var])/
                             (COMPILER_DOUBLE)pInfo->total_elapsed_time[loop_var]/pInfo->rate;

            /* the rate unit is KHZ, add -3(log10(1/1000)) for actual display  */
            bers[loop_var] /= 1000;
            LOG_WARN(BSL_LS_SOC_PHY,
                     (BSL_META("BER @ %2.2f%% = 1e%2.2f (%d errors in %d seconds)\n"),
(pInfo->offset_max-loop_var)*eye_unit,
                      log10(bers[loop_var]),
                      pInfo->total_errs[loop_var],
                      pInfo->total_elapsed_time[loop_var]));
        }
        curr_ber_log = log10(bers[loop_var]);

        /* Detect and record nonmonotonic data points */
        if ((loop_var > 0) && (curr_ber_log > prev_ber_log)) {
            pInfo->mono_flags[loop_var] = 1;
        }
        prev_ber_log = curr_ber_log;

        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META("cur_be_log %2.2f\n"), curr_ber_log));
        /* find the first data point with good BER */
        if ((curr_ber_log <= good_ber_level) &&
            (pInfo->first_good_ber_idx == INDEX_UNINITIALIZED)) {
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META("cur_be_log %2.2f, loop_var %d\n"),
curr_ber_log,loop_var));
            pInfo->first_good_ber_idx = loop_var;
        }
        if ((pInfo->total_errs[loop_var] < HI_CONFIDENCE_MIN_ERR_CNT) &&
            (pInfo->first_small_errcnt_idx == INDEX_UNINITIALIZED)) {
             /* find the first data point with small error count */
             pInfo->first_small_errcnt_idx = loop_var;
        }

    }   /* for loop_var */
    return SOC_E_NONE;
#else
    return SOC_E_UNAVAIL;
#endif

}

STATIC int
_tscmod_eye_margin_diagram_cal(TSCMOD_EYE_DIAG_INFOt *pInfo, int type)
{
#ifdef COMPILER_HAS_DOUBLE
    COMPILER_DOUBLE lbers[MAX_LOOPS]; /*Internal linear scale sqrt(-log(ber))*/
    COMPILER_DOUBLE margins[MAX_LOOPS]; /* Eye margin @ each measurement*/
    COMPILER_DOUBLE bers[MAX_LOOPS]; /* computed bit error rate */
    int delta_n;
    COMPILER_DOUBLE Exy = 0.0;
    COMPILER_DOUBLE Eyy = 0.0;
    COMPILER_DOUBLE Exx = 0.0;
    COMPILER_DOUBLE Ey  = 0.0;
    COMPILER_DOUBLE Ex  = 0.0;
    COMPILER_DOUBLE alpha;
    COMPILER_DOUBLE beta;
    COMPILER_DOUBLE proj_ber;
    COMPILER_DOUBLE proj_margin_12;
    COMPILER_DOUBLE proj_margin_15;
    COMPILER_DOUBLE proj_margin_18;
    COMPILER_DOUBLE sq_err2;
    COMPILER_DOUBLE ierr;
    int beta_max=0;
    int ber_conf_scale[20];
    int start_n;
    int stop_n;
    int low_confidence;
    int loop_index;
    COMPILER_DOUBLE outputs[4];
    COMPILER_DOUBLE eye_unit;
    int n_mono;

    /* Initialize BER confidence scale */
    ber_conf_scale[0] = 3.02;
    ber_conf_scale[1] = 4.7863;
    ber_conf_scale[2] = 3.1623;
    ber_conf_scale[3] = 2.6303;
    ber_conf_scale[4] = 2.2909;
    ber_conf_scale[5] = 2.138;
    ber_conf_scale[6] = 1.9953;
    ber_conf_scale[7] = 1.9055;
    ber_conf_scale[8] = 1.8197;
    ber_conf_scale[9] = 1.7783;
    ber_conf_scale[10] = 1.6982;
    ber_conf_scale[11] = 1.6596;
    ber_conf_scale[12] = 1.6218;
    ber_conf_scale[13] = 1.6218;
    ber_conf_scale[14] = 1.5849;
    ber_conf_scale[15] = 1.5488;
    ber_conf_scale[16] = 1.5488;
    ber_conf_scale[17] = 1.5136;
    ber_conf_scale[18] = 1.5136;
    ber_conf_scale[19] = 1.4791;

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META("first_good_ber_idx: %d, first_small_errcnt_idx: %d\n"),
pInfo->first_good_ber_idx,pInfo->first_small_errcnt_idx));
    
    for(loop_index=0; loop_index<MAX_LOOPS; loop_index++) {
        lbers[loop_index] = 0 ;
        margins[loop_index] = 0 ;
        bers[loop_index] = 0;
    }

    /* Find the highest data point to use, currently based on at least 1E-8 BER level */
    if (pInfo->first_good_ber_idx == INDEX_UNINITIALIZED) {
        start_n = pInfo->veye_cnt;
    } else {
        start_n = pInfo->first_good_ber_idx;
    }
    stop_n = pInfo->veye_cnt;

    /* Find the lowest data point to use */
    if (pInfo->first_small_errcnt_idx == INDEX_UNINITIALIZED) {
        stop_n = pInfo->veye_cnt;
    } else {
        stop_n = pInfo->first_small_errcnt_idx;
    }

    /* determine the number of non-monotonic outliers */
    n_mono = 0;
    for (loop_index = start_n; loop_index < stop_n; loop_index++) {
        if (pInfo->mono_flags[loop_index] == 1) {
            n_mono = n_mono + 1;
        }
    }

    if (type == TSC_UTIL_HEYE_L || type == TSC_UTIL_HEYE_R) {
        eye_unit = HEYE_UNIT;
    } else {
        eye_unit = VEYE_UNIT;
    }

    for (loop_index = 0; loop_index < pInfo->veye_cnt; loop_index++) {
        if (pInfo->total_errs[loop_index] == 0) {
            bers[loop_index] = 1.0/(COMPILER_DOUBLE)pInfo->total_elapsed_time[loop_index]/pInfo->rate;
        } else {
            bers[loop_index] = (COMPILER_DOUBLE)pInfo->total_errs[loop_index]/
                               (COMPILER_DOUBLE)pInfo->total_elapsed_time[loop_index]/pInfo->rate;
        }
        bers[loop_index] /= 1000;
        margins[loop_index] = (pInfo->offset_max-loop_index)*eye_unit;
    }

    if( start_n >= pInfo->veye_cnt ) {
        outputs[0] = - _tscmod_util_round_real(log(bers[pInfo->veye_cnt-1])/log(10), 1);
        outputs[1] = -100.0;
        outputs[2] = -100.0;
        outputs[3] = -100.0;
    /*  No need to print out the decimal portion of the BER */
    LOG_CLI((BSL_META("BER *worse* than 1e-%d\n"), (int)outputs[0]));
    LOG_CLI((BSL_META("Negative margin @ 1e-12, 1e-15 & 1e-18\n")));
    } else {
        low_confidence = 0;
    if( (stop_n-start_n - n_mono) < 2  ) {
        /* Code triggered when less than 2 statistically valid extrapolation points */
            for( loop_index = stop_n; loop_index < pInfo->veye_cnt; loop_index++ ) {
            if( pInfo->total_errs[loop_index] < 20 ) {
            bers[loop_index] = ber_conf_scale[pInfo->total_errs[loop_index]] * bers[loop_index];
        } else {
            bers[loop_index] = ber_conf_scale[19] * bers[loop_index];
        }
                pInfo->mono_flags[loop_index] = 0;    /* remove flags; or assess again */
        }
        /* Add artificial point at 100% margin to enable interpolation */
            margins[pInfo->veye_cnt] = 100.0;
            bers[pInfo->veye_cnt] = 0.1;
            low_confidence = 1;
            stop_n = pInfo->veye_cnt + 1;
    }

    /* Below this point the code assumes statistically valid point available */
        delta_n = stop_n - start_n - n_mono;

        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META("start_n: %d, stop_n: %d, veye: %d, n_mono: %d\n"),
start_n,stop_n,pInfo->veye_cnt,n_mono));

    /* Find all the correlations */
    for( loop_index = start_n; loop_index < stop_n; loop_index++ ) {
        lbers[loop_index] = (COMPILER_DOUBLE)sqrt(-log(bers[loop_index]));
    }

        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META("\tstart=%d, stop=%d, low_confidence=%d\n"),
start_n, stop_n, low_confidence));
        for (loop_index=start_n; loop_index < stop_n; loop_index++){
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META("\ttotal_errs[%d]=0x%08x\n"),
loop_index,(int)pInfo->total_errs[loop_index]));
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META("\tbers[%d]=%f\n"),
loop_index,bers[loop_index]));
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META("\tlbers[%d]=%f\n"),
loop_index,lbers[loop_index]));
    }

    for( loop_index = start_n; loop_index < stop_n; loop_index++ ) {
            if (pInfo->mono_flags[loop_index] == 0) {
            Exy = Exy + margins[loop_index] * lbers[loop_index]/(COMPILER_DOUBLE)delta_n;
            Eyy = Eyy + lbers[loop_index]*lbers[loop_index]/(COMPILER_DOUBLE)delta_n;
            Exx = Exx + margins[loop_index]*margins[loop_index]/(COMPILER_DOUBLE)delta_n;
            Ey  = Ey + lbers[loop_index]/(COMPILER_DOUBLE)delta_n;
            Ex  = Ex + margins[loop_index]/(COMPILER_DOUBLE)delta_n;
            }
    }

    /* Compute fit slope and offset */
        alpha = (Exy - Ey * Ex)/(Exx - Ex * Ex);
        beta = Ey - Ex * alpha;

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META("Exy=%f, Eyy=%f, Exx=%f, Ey=%f,Ex=%f alpha=%f, beta=%f\n"),
Exy,Eyy,Exx,Ey,Ex,alpha,beta));

    /* JPA> Due to the limit of floats, I need to test for a maximum Beta of 9.32 */
    if(beta > 9.32){
      beta_max=1;
      LOG_CLI((BSL_META("\n\tWARNING: intermediate float variable is maxed out, "
                        "what this means is:\n")));
      LOG_CLI((BSL_META("\t\t- The *extrapolated* minimum BER will be "
                        "reported as 1E-37.\n")));
      LOG_CLI((BSL_META("\t\t- This may occur if the channel is near "
                        "ideal (e.g. test loopback)\n")));
      LOG_CLI((BSL_META("\t\t- While not discrete, reporting an extrapolated "
                        "BER < 1E-37 is numerically corect, and informative\n\n")));
    }


        proj_ber = exp(-beta * beta);
        proj_margin_12 = (sqrt(-log(1e-12)) - beta)/alpha;
        proj_margin_15 = (sqrt(-log(1e-15)) - beta)/alpha;
        proj_margin_18 = (sqrt(-log(1e-18)) - beta)/alpha;

        sq_err2 = 0;
    for( loop_index = start_n; loop_index < stop_n; loop_index++ ) {
      ierr = (lbers[loop_index] - (alpha*margins[loop_index] + beta));
      sq_err2 = sq_err2 + ierr*ierr/(COMPILER_DOUBLE)delta_n;
        }

        outputs[0] = -_tscmod_util_round_real(log(proj_ber)/log(10),1);
        outputs[1] = _tscmod_util_round_real(proj_margin_18,1);
        outputs[2] = _tscmod_util_round_real(proj_margin_12,1);
        outputs[3] = _tscmod_util_round_real(proj_margin_15,1);

        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META("\t\tlog1e-12=%f, sq=%f\n"),(COMPILER_DOUBLE)log(1e-12),(COMPILER_DOUBLE)sqrt(-log(1e-12))));
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META("\t\talpha=%f\n"),alpha));
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META("\t\tbeta=%f\n"),beta));
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META("\t\tproj_ber=%f\n"),proj_ber));
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META("\t\tproj_margin12=%f\n"),proj_margin_12));
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META("\t\tproj_margin12=%f\n"),proj_margin_15));
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META("\t\tproj_margin18=%f\n"),proj_margin_18));
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META("\t\toutputs[0]=%f\n"),outputs[0]));
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META("\t\toutputs[1]=%f\n"),outputs[1]));
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META("\t\toutputs[2]=%f\n"),outputs[2]));

        /* Extrapolated results, low confidence */
        if( low_confidence == 1 ) {
          if(beta_max){
            LOG_CLI((BSL_META("BER(extrapolated) is *better* than 1e-37\n")));
            LOG_CLI((BSL_META("Margin @ 1e-12    is *better* than %f\n"), outputs[2]));
            LOG_CLI((BSL_META("Margin @ 1e-15    is *better* than %f\n"), outputs[3]));
            LOG_CLI((BSL_META("Margin @ 1e-18    is *better* than %f\n"), outputs[1]));
          }
          else{
            LOG_CLI((BSL_META("BER(extrapolated) is *better* than 1e-%f\n"),
                     outputs[0]));
            LOG_CLI((BSL_META("Margin @ 1e-12    is *better* than %f\n"), outputs[2]));
            LOG_CLI((BSL_META("Margin @ 1e-15    is *better* than %f\n"), outputs[3]));
            LOG_CLI((BSL_META("Margin @ 1e-18    is *better* than %f\n"), outputs[1]));
          }

        /* JPA> Extrapolated results, high confidence */
        } else {
          if(beta_max){
            LOG_CLI((BSL_META("BER(extrapolated) = 1e-37\n")));
            LOG_CLI((BSL_META("Margin @ 1e-12    is *better* than %f\n"), outputs[2]));
            LOG_CLI((BSL_META("Margin @ 1e-15    is *better* than %f\n"), outputs[3]));
            LOG_CLI((BSL_META("Margin @ 1e-18    is *better* than %f\n"), outputs[1]));
          }
          else{
          LOG_CLI((BSL_META("BER(extrapolated) = 1e-%4.2f\n"), outputs[0]));
          LOG_CLI((BSL_META("Margin @ 1e-12    = %4.2f%%\n"), outputs[2]));
          LOG_CLI((BSL_META("Margin @ 1e-15    = %4.2f%%\n"), outputs[3]));
          LOG_CLI((BSL_META("Margin @ 1e-18    = %4.2f%%\n"), outputs[1]));
          }
        }
    }
    return SOC_E_NONE;
#else
    return SOC_E_UNAVAIL;
#endif
}

STATIC int
_tscmod_eye_margin(tscmod_st *ws, int type)
{
    int lane_s;
    int lane_e;
    int i;
    TSCMOD_EYE_DIAG_INFOt eye_info[4];
    int tmp_sel, tmp_dxgxs, tmp_lane ;

    /* clear the memory */
    TSCMOD_MEM_SET(eye_info, 0, 4*sizeof(TSCMOD_EYE_DIAG_INFOt));

    for (i = 0; i < 4; i++) {
         eye_info[i].first_good_ber_idx     = INDEX_UNINITIALIZED;
         eye_info[i].first_small_errcnt_idx = INDEX_UNINITIALIZED;
    }

    tmp_sel   = ws->lane_select ;
    tmp_dxgxs = ws->dxgxs ;
    tmp_lane  = ws->this_lane ;
    if(ws->port_type   == TSCMOD_SINGLE_PORT ) {
       lane_s = 0 ; lane_e = 4 ;
    } else if (ws->port_type == TSCMOD_DXGXS ) {
       if((ws->dxgxs & 3)==2) { lane_s = 2; lane_e = 4 ; }
       else { lane_s = 0; lane_e = 2 ; }
    } else {
       lane_s = ws->this_lane ; lane_e = lane_s + 1 ;
    }

    printf("\nPort %d : Start BER extrapolation for ", ws->port);
    switch(type) {
    case TSC_UTIL_VEYE_U:
       printf("VEYE_U\n") ; break ;
    case TSC_UTIL_HEYE_R:
       printf("HEYE_R\n") ; break ;
    case TSC_UTIL_HEYE_L:
       printf("HEYE_L\n") ; break ;
    default:
       printf("UNKNOWN\n") ; break ;
    }
    printf("Port %d : Test time varies from a few minutes to over 20 minutes. "
                 "Please wait ...\n", ws->port);

    for (i = lane_s; i < lane_e; i++) {
        if((ws->eye_lane==0)||(ws->eye_lane & (1<<(i-lane_s)))) {
            printf("\nStart test for lane %d\n", i);
            ws->this_lane = i ;
            ws->lane_select = getLaneSelect(i) ;
            ws->dxgxs       = 0 ;
            SOC_IF_ERROR_RETURN
                (_tscmod_eye_margin_data_get(ws, &eye_info[i],type));
            SOC_IF_ERROR_RETURN
                (_tscmod_eye_margin_ber_cal(&eye_info[i],type));
            SOC_IF_ERROR_RETURN
                (_tscmod_eye_margin_diagram_cal(&eye_info[i],type));
        }
    }

    ws->lane_select = tmp_sel  ;  /* restore */
    ws->this_lane   = tmp_lane ;
    ws->dxgxs       = tmp_dxgxs;
    return SOC_E_NONE;
}

#endif   /* #ifdef __KERNEL__ */


/* to get the ppm units, the 2's complement value
must be divided by 83.89 for dsc c0 and 83.89*1.65 for dsc b0 */

int tscmod_diag_rx_ppm(tscmod_st *ws)
{
   uint16 data, idata;
   float fdata;
   char  datac=' ';

   SOC_IF_ERROR_RETURN
       (READ_WC40_DSC3B0_CDR_STATUS0r(ws->unit, ws, &data));
   /* data = ~data + 1;  take the 2s compliment) */
   idata = data;if (data & 0x8000) {idata = ((1 + ~(data & 0x7fff)) & 0x7fff); datac = '-';}
   if (ws->model_type == TSCMOD_A0 ) {
      fdata = (((float)idata) / 83.89);
   } else {
      fdata = (((float)idata) / (1.65 * 83.89));
   }
   printf("%s PPM: 0x%02x(%c%f)\n", FUNCTION_NAME(), data, datac, fdata);
   return SOC_E_NONE;
}

#endif /* #ifndef _KERNEL_ */


/*!

\details
Perturb main data slicer horizontally/vertically. Measure BER to estimate error
rate for different slicer states. We use PRBS for (user selected polynomial)
for BER test. PRBS is sent for various durations, BERs recorded and real BER
is extrapolated using linear fit algorithm.

Slicer perturbation direction is controlled by #tscmod_st::tscmod_eye_slicer.
Vertical   movement is 24 steps from 0 to 40% in 1.75%
Horizontal movement is 64 steps from -0.5UI to +0.5UI

You can choose one mode of slicer pertabation only at a time. For vertical, the
algorithm will choose the direction. For horizontal you can choose
horizontal-left or horizonal-right with #tscmod_st::per_lane_control as follows.
(note: it is not one-hot coded)

\li #tscmod_st::per_lane_control 0x1 chooses vertical perturbation
\li #tscmod_st::per_lane_control 0x2 chooses horizontal-right perturbation
\li #tscmod_st::per_lane_control 0x3 chooses horizontal-left  perturbation

*/

int tscmod_diag_eye(tscmod_st *ws)
{
  int rv = 0;
  if(ws->verbosity&(TSCMOD_DBG_FLOW|TSCMOD_DBG_FUNC))
     printf("%-22s: u=%0d p=%0d cntl=0x%0x accData=%0d\n", FUNCTION_NAME(),
            ws->unit, ws->port, ws->per_lane_control, ws->accData);

#ifndef __KERNEL__
  switch (ws->per_lane_control) {
#if 0
    case TSC_UTIL_2D_LIVELINK_EYESCAN:
        rv = _tscmod_2d_livelink_eye_margin_data_get(ws);
        break;
    case TSC_UTIL_2D_PRBS_EYESCAN:
        rv = _tscmod_2d_prbs_eye_margin_data_get(ws);
        break;
#endif
    case TSC_UTIL_ENABLE_LIVELINK:
        rv = _tscmod_enable_livelink_diagnostics(ws);
        break;
    case TSC_UTIL_DISABLE_LIVELINK:
        rv = _tscmod_disable_livelink_diagnostics(ws);
        break;
    case TSC_UTIL_ENABLE_DEADLINK:
        rv = _tscmod_enable_deadlink_diagnostics(ws);
        break;
    case TSC_UTIL_DISABLE_DEADLINK:
        rv = _tscmod_disable_deadlink_diagnostics(ws);
        break;
    case TSC_UTIL_SET_VOFFSET:
        rv = _tscmod_set_voff(ws, ws->accData);
        break;
    case TSC_UTIL_SET_HOFFSET:
        rv = _tscmod_set_hoff(ws, ws->accData);
        break;
    case TSC_UTIL_GET_MAX_VOFFSET:
        rv = _tscmod_get_max_voffset(ws);
        break;
    case TSC_UTIL_GET_MIN_VOFFSET:
        rv= _tscmod_get_min_voffset(ws);
        break;
    case TSC_UTIL_GET_INIT_VOFFSET:
        rv= _tscmod_get_init_voffset(ws);
        break;
    case TSC_UTIL_GET_MAX_LEFT_HOFFSET:
        rv = _tscmod_get_max_hoffset_left(ws);
        break;
    case TSC_UTIL_GET_MAX_RIGHT_HOFFSET:
        rv = _tscmod_get_max_hoffset_right(ws);
        break;
    case TSC_UTIL_START_LIVELINK:
        rv = _tscmod_start_livelink_diagnostics(ws);
        break;
    case TSC_UTIL_START_DEADLINK:
        rv = _tscmod_start_deadlink_diagnostics(ws);
        break;
    case TSC_UTIL_STOP_LIVELINK:
        rv = _tscmod_stop_livelink_diagnostics(ws);
        break;
    case TSC_UTIL_CLEAR_LIVELINK:
        rv = _tscmod_clear_livelink_diagnostics(ws);
        break;
    case TSC_UTIL_READ_LIVELINK:
        rv = _tscmod_read_livelink_diagnostics(ws);
        break;
    case TSC_UTIL_READ_DEADLINK:
        rv = _tscmod_read_deadlink_diagnostics(ws);
        break;
    case TSC_UTIL_VEYE_U:
       rv =  _tscmod_eye_margin(ws,  TSC_UTIL_VEYE_U) ;
       break ;
    case TSC_UTIL_HEYE_R:
       rv =  _tscmod_eye_margin(ws,  TSC_UTIL_HEYE_R) ;
       break ;
    case TSC_UTIL_HEYE_L:
       rv =  _tscmod_eye_margin(ws,  TSC_UTIL_HEYE_L) ;
       break ;
    default:
        break;
  }
#endif /* # !__KERNEL__ */

  return rv;
}

int tscmod_diag_slicers(tscmod_st *ws)
{
  uint16 data, p1even, p1odd, deven, dodd, m1even, m1odd;
  int    ip1even, ip1odd, ideven, idodd, im1even, im1odd;
  char   p1evenc=' ',p1oddc=' ', devenc=' ', doddc=' ', m1evenc=' ', m1oddc=' ';

  SOC_IF_ERROR_RETURN(READ_WC40_DSC2B0_DSC_ANA_CTRL0r (ws->unit, ws, &data));
  p1even = (data & DSC2B0_DSC_ANA_CTRL0_P1_EVN_CTRL_MASK) >>
            DSC2B0_DSC_ANA_CTRL0_P1_EVN_CTRL_SHIFT;
  p1odd  = (data & DSC2B0_DSC_ANA_CTRL0_P1_ODD_CTRL_MASK) >>
            DSC2B0_DSC_ANA_CTRL0_P1_ODD_CTRL_SHIFT;
  if (ws->accData == 1) ws->accData =  ((p1even << 8) | p1odd);

  SOC_IF_ERROR_RETURN(READ_WC40_DSC2B0_DSC_ANA_CTRL2r (ws->unit, ws, &data));
  m1even = (data & DSC2B0_DSC_ANA_CTRL2_M1_EVN_CTRL_MASK) >>
                DSC2B0_DSC_ANA_CTRL2_M1_EVN_CTRL_SHIFT;
  m1odd  = (data & DSC2B0_DSC_ANA_CTRL2_M1_ODD_CTRL_MASK) >>
                DSC2B0_DSC_ANA_CTRL2_M1_ODD_CTRL_SHIFT;
  if (ws->accData == 2) ws->accData =  ((m1even << 8) | m1odd);

  SOC_IF_ERROR_RETURN(READ_WC40_DSC2B0_DSC_ANA_CTRL1r (ws->unit, ws, &data));
  deven = (data & DSC2B0_DSC_ANA_CTRL1_D_EVN_CTRL_MASK) >>
                DSC2B0_DSC_ANA_CTRL1_D_EVN_CTRL_SHIFT;
  dodd  = (data & DSC2B0_DSC_ANA_CTRL1_D_ODD_CTRL_MASK) >>
                DSC2B0_DSC_ANA_CTRL1_D_ODD_CTRL_SHIFT;
  if (ws->accData == 3) ws->accData =  ((deven << 8) | dodd);

  ip1even = p1even;
  ip1odd  = p1odd;
  ideven  = deven;
  idodd   = dodd;
  im1even = m1even;
  im1odd  = m1odd;
  if (p1even & 0x20) {ip1even = ((1 + ~(p1even & 0x1f)) & 0x1f); p1evenc = '-';}
  if (p1odd  & 0x20) {ip1odd = ((1 + ~(p1odd  & 0x1f)) & 0x1f); p1oddc  = '-';}
  if (deven  & 0x20) {ideven = ((1 + ~(deven  & 0x1f)) & 0x1f); devenc  = '-';}
  if (dodd   & 0x20) {idodd  = ((1 + ~(dodd   & 0x1f)) & 0x1f); doddc   = '-';}
  if (m1even & 0x20) {im1even = ((1 + ~(m1even & 0x1f)) & 0x1f); m1evenc = '-';}
  if (m1odd  & 0x20) {im1odd = ((1 + ~(m1odd  & 0x1f)) & 0x1f); m1oddc  = '-';}

  printf("%s P1Eeven:0x%02x(%c%d) P1Odd:0x%02x(%c%d) DEven:0x%02x(%c%d) DOdd:0x%02x(%c%d) M1Even:0x%02x(%c%d) M1Odd:0x%02x(%c%d)\n", FUNCTION_NAME(),
         p1even,p1evenc, ip1even,
         p1odd ,p1oddc,  ip1odd,
         deven ,devenc,  ideven,
         dodd  ,doddc,   idodd,
         m1even,m1evenc, im1even,
         m1odd ,m1oddc,  im1odd);
  return SOC_E_NONE;
}


int tscmod_diag_tx_amps(tscmod_st *ws)
{
   uint16 data ; uint16 idrv, predrv, p2_cf;
   SOC_IF_ERROR_RETURN(READ_WC40_ANATX_TX_DRIVERr(ws->unit, ws, &data));

   idrv  =(data & ANATX_TX_DRIVER_IDRIVER_MASK)
      >>     ANATX_TX_DRIVER_IDRIVER_SHIFT;
   predrv=(data & ANATX_TX_DRIVER_IPREDRIVER_MASK)
      >>  ANATX_TX_DRIVER_IPREDRIVER_SHIFT;
   p2_cf =(data & ANATX_TX_DRIVER_POST2_COEFF_MASK)
      >> ANATX_TX_DRIVER_POST2_COEFF_SHIFT;

   if(ws->per_lane_control&0x10) {
       printf("Tx Amps. Lane %d: IDriver:%d PreDriver:%d P2_Coeff:%d\n",
                  ws->this_lane,idrv,predrv,p2_cf);
   }
   if ((ws->per_lane_control) == 0x1)        {
      ws->accData = predrv;
   } else if ((ws->per_lane_control) == 0x2) {
      ws->accData = idrv;
   } else if ((ws->per_lane_control) == 0x3) {
      ws->accData = p2_cf;
   } else {
       ws->accData = data ;
   }
   return SOC_E_NONE;
}

int tscmod_diag_prbs(tscmod_st* ws)
{
   uint16         data16 ;

   switch(ws->per_lane_control) {
   case TSCMOD_DIAG_PRBS_POLYNOMIAL_GET:
      SOC_IF_ERROR_RETURN
      (READ_WC40_PATGEN0_PRBSCTRLr(ws->unit, ws, &data16));
      ws->accData = (data16 & PATGEN0_PRBSCTRL_PRBS_SEL_TX_MASK)
         >> PATGEN0_PRBSCTRL_PRBS_SEL_TX_SHIFT;
      break ;
   case TSCMOD_DIAG_PRBS_CTRL_GET: 
      SOC_IF_ERROR_RETURN
      (READ_WC40_PATGEN0_PRBSCTRLr(ws->unit, ws, &data16));
      ws->accData = data16 ;
      break;  
   case TSCMOD_DIAG_PRBS_INVERT_DATA_GET:
      SOC_IF_ERROR_RETURN
      (READ_WC40_PATGEN0_PRBSCTRLr(ws->unit, ws, &data16));
      ws->accData = (data16 & PATGEN0_PRBSCTRL_PRBS_INV_TX_MASK)
                           >> PATGEN0_PRBSCTRL_PRBS_INV_TX_SHIFT;
      /* JIRA PHY-804 */
      ws->accData = (ws->accData) ? 0 : 1 ;
      break ;
   case TSCMOD_DIAG_PRBS_RX_INVERT_DATA_GET:
      SOC_IF_ERROR_RETURN
      (READ_WC40_PATGEN0_PRBSCTRLr(ws->unit, ws, &data16));
      ws->accData = (data16 & PATGEN0_PRBSCTRL_PRBS_INV_RX_MASK)
                           >> PATGEN0_PRBSCTRL_PRBS_INV_RX_SHIFT;
      ws->accData = (ws->accData) ? 0 : 1 ;
      break;     
   case TSCMOD_DIAG_PRBS_MODE_GET:  /* include TX and RX POLY */
      SOC_IF_ERROR_RETURN
      (READ_WC40_PATGEN0_PRBSCTRLr(ws->unit, ws, &data16));
      ws->accData = ((data16 & PATGEN0_PRBSCTRL_PRBS_SEL_TX_MASK)
                     >> PATGEN0_PRBSCTRL_PRBS_SEL_TX_SHIFT) |
                    ((data16 & PATGEN0_PRBSCTRL_PRBS_SEL_RX_MASK)
                     >> PATGEN0_PRBSCTRL_PRBS_SEL_RX_SHIFT);
      break ;
   case TSCMOD_DIAG_PRBS_CHECK_MODE_GET:
      SOC_IF_ERROR_RETURN
      (READ_WC40_PATGEN0_PRBSCTRLr(ws->unit, ws, &data16));
      ws->accData = (data16 & PATGEN0_PRBSCTRL_PRBS_CHECK_MODE_MASK)
                           >> PATGEN0_PRBSCTRL_PRBS_CHECK_MODE_SHIFT;
     break;    
   case TSCMOD_DIAG_PRBS_LANE_EN_GET:
   default:
      SOC_IF_ERROR_RETURN
      (READ_WC40_PATGEN0_PATGENCTRLr(ws->unit, ws, &data16));
      ws->accData = (data16 & PATGEN0_PATGENCTRL_PRBS_TX_EN_MASK)
         >>PATGEN0_PATGENCTRL_PRBS_TX_EN_SHIFT;
      break ;
   }
   return SOC_E_NONE;

}
/* TSCMOD_DIAG_TX_TAPS */
/* per_lane_control==0xa -> printf out tap info */
int tscmod_diag_tx_taps(tscmod_st *ws)
{
   uint16 data, dt, pre, mn, post, swap, lane;
   int    tmp_lane, tmp_sel ;
   tmp_lane = ws->this_lane ;
   tmp_sel  = ws->lane_select ;

   if(ws->this_lane>=4) {
      printf("%s FATAL: Internal. Bad lane:%d\n", FUNCTION_NAME(), ws->this_lane);
      return SOC_E_ERROR;
   }
   /* CRTSC-688 */
   SOC_IF_ERROR_RETURN
      (READ_WC40_MAIN0_LANE_SWAPr(ws->unit, ws, &swap)) ;

   for(lane=0;lane<4; lane++) {
      if(((swap&(0x3<<(2*lane)))>>(2*lane))==tmp_lane) {
         break ;
      }
   }
   ws->this_lane   = lane ;
   ws->lane_select = getLaneSelect(lane) ;

   SOC_IF_ERROR_RETURN
      (READ_WC40_ANATX_ASTATUS0r(ws->unit, ws, &dt)); /* 0xc010 */

   data = dt;
   /* using mask of lane 0. only */
   pre = (dt & ANATX_ASTATUS0_TX_FIR_TAP_PRE_MASK)
            >> ANATX_ASTATUS0_TX_FIR_TAP_PRE_SHIFT;
   mn  = (dt & ANATX_ASTATUS0_TX_FIR_TAP_MAIN_MASK)
            >> ANATX_ASTATUS0_TX_FIR_TAP_MAIN_SHIFT;
   post= (dt & ANATX_ASTATUS0_TX_FIR_TAP_POST_MASK)
            >> ANATX_ASTATUS0_TX_FIR_TAP_POST_SHIFT;

   if((ws->per_lane_control == 0xa)||(ws->verbosity&TSCMOD_DBG_TXDRV))
      printf("Tx_Taps, Lane:%d: Pre:0x%02x Main:0x%02x Post:0x%02x\n",
              tmp_lane,pre,mn,post) ;

   ws->this_lane  = tmp_lane ;
   ws->lane_select= tmp_sel ;
   SOC_IF_ERROR_RETURN
      (READ_WC40_CL72_TX_FIR_TAP_REGISTERr(ws->unit, ws, &dt)) ;

   pre = (dt & ANATX_ASTATUS0_TX_FIR_TAP_PRE_MASK)
            >> ANATX_ASTATUS0_TX_FIR_TAP_PRE_SHIFT;
   mn  = (dt & ANATX_ASTATUS0_TX_FIR_TAP_MAIN_MASK)
            >> ANATX_ASTATUS0_TX_FIR_TAP_MAIN_SHIFT;
   post= (dt & ANATX_ASTATUS0_TX_FIR_TAP_POST_MASK)
            >> ANATX_ASTATUS0_TX_FIR_TAP_POST_SHIFT;

   if(ws->verbosity&TSCMOD_DBG_TXDRV)
      printf("    Tx_Taps set Lane:%d: Pre:0x%02x Main:0x%02x Post:0x%02x reg=0x%0x(lane_readback=%0d data=%0x swap=%0x)\n",
              tmp_lane, pre,mn  ,post, dt, lane, data, swap&0xff) ;

   ws->accData = data ;
   return SOC_E_NONE;
}

/* TSCMOD_DIAG_RX_TAPS */
int tscmod_diag_rx_taps(tscmod_st* ws)
{
   return SOC_E_NONE;
}

/* use tscmod_diag_pmd instead */
int tscmod_diag_rx_sigdet(tscmod_st* ws)
{ 
   uint16 data ;  
   int sig_ok ;
   SOC_IF_ERROR_RETURN                   /* c150 */
      (READ_WC40_RX_X4_STATUS1_PMA_PMD_LIVE_STATUSr(ws->unit, ws, &data));
   
   sig_ok  =(data & RX_X4_STATUS1_PMA_PMD_LIVE_STATUS_SIGNAL_OK_MASK)
                 >> RX_X4_STATUS1_PMA_PMD_LIVE_STATUS_SIGNAL_OK_SHIFT ;
   ws->accData = sig_ok ;

   return SOC_E_NONE;
}


/* TSCMOD_DIAG_CL72.  CL72 is done (return 1) or not(return 0) if per_lane_control ==TSCMOD_DIAG_CL72_DONE  */
/*                    CL72 is started or not                   if per_lane_control ==TSCMOD_DIAG_CL72_START */
/*                    CL72 has coarse lock or not              if per_lane_control ==TSCMOD_DIAG_CL72_LOCK */

int tscmod_diag_cl72(tscmod_st* ws)
{
   uint16 data ;  uint16 done ;

   SOC_IF_ERROR_RETURN
      (READ_WC40_DEV1_IEEE9_PMD_10GBASE_KR_PMD_STATUS_REGISTER_151r(ws->unit, ws, &data));

   if(ws->per_lane_control == TSCMOD_DIAG_CL72_START ) {
      if(data&DEV1_IEEE9_PMD_10GBASE_KR_PMD_STATUS_REGISTER_151_TRAINING_STATUS_MASK){
         ws->accData = 1 ;
      } else {
         ws->accData = 0 ;
      }
      return SOC_E_NONE ;
   }
   if(ws->per_lane_control == TSCMOD_DIAG_CL72_LOCK) {
      SOC_IF_ERROR_RETURN
         (READ_WC40_CL72_MISC1_CONTROLr(ws->unit, ws, &data));
      if(data&CL72_MISC1_CONTROL_TR_COARSE_LOCK_MASK) {
         ws->accData = 1 ;
      } else {
         ws->accData = 0 ;
      }
      return SOC_E_NONE ;
   }

   if(ws->per_lane_control == TSCMOD_DIAG_CL72_RX_TRAINED) {
      SOC_IF_ERROR_RETURN
         (READ_WC40_CL72_MISC1_CONTROLr(ws->unit, ws, &data));
      if(data&CL72_MISC1_CONTROL_RX_TRAINED_MASK) done =1 ;
      else                                        done =0 ;
      ws->accData = done ;
      return SOC_E_NONE ;
   }


   if( (data &  DEV1_IEEE9_PMD_10GBASE_KR_PMD_STATUS_REGISTER_151_RECEIVER_STATUS_MASK) &&
       ((data & DEV1_IEEE9_PMD_10GBASE_KR_PMD_STATUS_REGISTER_151_TRAINING_STATUS_MASK)==0) )
      done = 1 ;
   else
      done = 0 ;

   if(done) {
      /* 0xc253 */
      SOC_IF_ERROR_RETURN
         (READ_WC40_CL72_MISC1_CONTROLr(ws->unit, ws, &data));
      if(data&CL72_MISC1_CONTROL_RX_TRAINED_MASK) done =1 ;
      else                                        done =0 ;
   }
   ws->accData = done ;
   return SOC_E_NONE ;
}

/* TSCMOD_DIAG_PLL  pll div value */
int tscmod_diag_pll(tscmod_st* ws)
{
   uint16 data ;
   int plldiv_vec, plldiv_r_val ;
   /* read back plldiv.  Same codes from tscmod_get_plldiv */
   SOC_IF_ERROR_RETURN (READ_WC40_MAIN0_SETUPr(ws->unit, ws, &data)) ;

   plldiv_vec = ( data & MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_MASK ) >>
                         MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_SHIFT ;
   switch(plldiv_vec) {  /* shared codes */
   case MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div32:   plldiv_r_val = 32 ;  break;
   case MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div36:   plldiv_r_val = 36 ;  break;
   case MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div40:   plldiv_r_val = 40 ;  break;
   case MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div42:   plldiv_r_val = 42 ;  break;
   case MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div48:   plldiv_r_val = 48 ;  break;
   case MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div50:   plldiv_r_val = 50 ;  break;
   case MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div52:   plldiv_r_val = 52 ;  break;
   case MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div54:   plldiv_r_val = 54 ;  break;
   case MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div60:   plldiv_r_val = 40 ;  break;
   case MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div64:   plldiv_r_val = 64 ;  break;
   case MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div66:   plldiv_r_val = 66 ;  break;
   case MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div68:   plldiv_r_val = 68 ;  break;
   case MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div70:   plldiv_r_val = 70 ;  break;
   case MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div80:   plldiv_r_val = 80 ;  break;
   case MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div92:   plldiv_r_val = 92 ;  break;
   case MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div100:  plldiv_r_val =100 ; break;
   default : plldiv_r_val = 40;
   }
   ws->accData = plldiv_r_val ;
   return SOC_E_NONE ;
}

/* TSCMOD_DIAG_PMD  pmd live or latch value and signal detect or latched value */
/*  per_lane_control = TSCMOD_DIAG_PMD_LOCK_LATCH or TSCMOD_DIAG_PMD_LOC_LIVE or
                       TSCMOD_DIAG_PMD_INFO_LATCH or TSCMOD_DIAG_PMD_INFO_LIVE.

    when per_lane_control=TSCMOD_DIAG_PMD_LOC_LATCH, if pmd_lock_latched_low=1,
    then pmd lock is not set.  if pmd_latched_low=0, then check pmd_live status
    TSCMOD_DIAG_PMD_INFO_LATCH or TSCMOD_DIAG_PMD_INFO_LIVE also include
    signal_detect (AFE_SIGNAL) alone pmd_lock.
*/

/* return PMD status */
int tscmod_diag_pmd(tscmod_st* ws)
{
   uint16 data ;
   int pmd_lck, pmd_lk_ll, sig_det, sig_det_ll ;

   SOC_IF_ERROR_RETURN                   /* c150 */
      (READ_WC40_RX_X4_STATUS1_PMA_PMD_LIVE_STATUSr(ws->unit, ws, &data));

   pmd_lck =(data & RX_X4_STATUS1_PMA_PMD_LIVE_STATUS_PMD_LOCK_MASK)
                  >>RX_X4_STATUS1_PMA_PMD_LIVE_STATUS_PMD_LOCK_SHIFT ;
   sig_det =(data & RX_X4_STATUS1_PMA_PMD_LIVE_STATUS_SIGNAL_OK_MASK)
                  >>RX_X4_STATUS1_PMA_PMD_LIVE_STATUS_SIGNAL_OK_SHIFT ;

   pmd_lk_ll =0 ; sig_det_ll = 0 ;
   switch(ws->per_lane_control) {
   case TSCMOD_DIAG_PMD_INFO_LIVE :
      ws->accData = 0 ;
      if(pmd_lck) ws->accData |= TSCMOD_DIAG_PMD_INFO_LOCK_BIT ;
      if(sig_det) ws->accData |= TSCMOD_DIAG_PMD_INFO_SDET_BIT ;
      break ;
   case TSCMOD_DIAG_PMD_INFO_LATCH:
      SOC_IF_ERROR_RETURN                  /* c151 */
         (READ_WC40_RX_X4_STATUS1_PMA_PMD_LATCHED_STATUSr(ws->unit, ws, &data));
      pmd_lk_ll = (data&RX_X4_STATUS1_PMA_PMD_LATCHED_STATUS_PMD_LOCK_LL_MASK)
                     >> RX_X4_STATUS1_PMA_PMD_LATCHED_STATUS_PMD_LOCK_LL_SHIFT ;
      sig_det_ll= (data&RX_X4_STATUS1_PMA_PMD_LATCHED_STATUS_SIGNAL_OK_LL_MASK)
                     >> RX_X4_STATUS1_PMA_PMD_LATCHED_STATUS_SIGNAL_OK_LL_SHIFT ;
      if(pmd_lk_ll) pmd_lck =0 ;
      if(sig_det_ll)sig_det = 0 ;
      ws->accData = 0 ;
      if(pmd_lck) ws->accData |= TSCMOD_DIAG_PMD_INFO_LOCK_BIT ;
      if(sig_det) ws->accData |= TSCMOD_DIAG_PMD_INFO_SDET_BIT ;
      break ;
   case TSCMOD_DIAG_PMD_LOCK_LATCH:
      SOC_IF_ERROR_RETURN                  /* c151 */
         (READ_WC40_RX_X4_STATUS1_PMA_PMD_LATCHED_STATUSr(ws->unit, ws, &data));

      pmd_lk_ll = (data&RX_X4_STATUS1_PMA_PMD_LATCHED_STATUS_PMD_LOCK_LL_MASK)
                     >> RX_X4_STATUS1_PMA_PMD_LATCHED_STATUS_PMD_LOCK_LL_SHIFT ;
      if(pmd_lk_ll) ws->accData = 0 ;
      else          ws->accData = pmd_lck ;
      break ;
   case TSCMOD_DIAG_PMD_LOCK_LIVE:
   default:
      ws->accData = pmd_lck ;
      break ;
   }
   return SOC_E_NONE ;
}

/* return HG info */
/* accData = 1 if it is HG */
/* accData = 0 if not      */
int tscmod_diag_hg(tscmod_st* ws)
{
   uint16 data  ;

   SOC_IF_ERROR_RETURN
      (READ_WC40_TX_X4_CONTROL0_ENCODE_0r(ws->unit, ws, &data)) ;

   if(data&TX_X4_CONTROL0_ENCODE_0_HG2_ENABLE_MASK) ws->accData = 1 ;
   else                                             ws->accData = 0 ;

   return SOC_E_NONE ;
}

/*!

*/
int tscmod_diag(tscmod_st *ws)
{
  uint32 vec ;
  int rv = SOC_E_NONE;

  if(ws->verbosity&(TSCMOD_DBG_FLOW|TSCMOD_DBG_FUNC))
     printf("%-22s: u=%0d p=%0d diag_type=%0x cntl=%0x\n", FUNCTION_NAME(),
            ws->unit, ws->port, ws->diag_type, ws->per_lane_control);
  vec = ws->diag_type ;

  if(ws->verbosity&(TSCMOD_DBG_FLOW|TSCMOD_DBG_FUNC)) {
     int i = 0 ;
     while(vec) {
        if(vec & 1) {
           if(i>=CNT_tscmod_diag_type)
              printf("out of range i=%0d vec=%x\n", i, vec) ;
           else
              printf("%s ", e2s_tscmod_diag_type[i]) ;
        }
        vec = (vec >> 1) ;
        i++ ;
     }
     printf(")\n") ;
  }

  if(ws->diag_type & TSCMOD_DIAG_GENERAL)    rv |= tscmod_diag_general(ws);
  if(ws->diag_type & TSCMOD_DIAG_LINK)       rv |= tscmod_diag_link(ws);
  if(ws->diag_type & TSCMOD_DIAG_ANEG)       rv |= tscmod_diag_autoneg(ws);
  if(ws->diag_type & TSCMOD_DIAG_TFC)        rv |= tscmod_diag_internal_tfc(ws);
  if(ws->diag_type & TSCMOD_DIAG_DFE)        rv |= tscmod_diag_dfe(ws);
  if(ws->diag_type & TSCMOD_DIAG_IEEE)       rv |= tscmod_diag_ieee(ws);
  if(ws->diag_type & TSCMOD_DIAG_TOPOLOGY)   rv |= tscmod_diag_topology(ws);
  if(ws->diag_type & TSCMOD_DIAG_EEE)        rv |= tscmod_diag_eee(ws);
  if(ws->diag_type & TSCMOD_DIAG_SLICERS)    rv |= tscmod_diag_slicers(ws);
  if(ws->diag_type & TSCMOD_DIAG_TX_AMPS)    rv |= tscmod_diag_tx_amps(ws);
  if(ws->diag_type & TSCMOD_DIAG_TX_TAPS)    rv |= tscmod_diag_tx_taps(ws);
  if(ws->diag_type & TSCMOD_DIAG_RX_TAPS)    rv |= tscmod_diag_rx_taps(ws);
  if(ws->diag_type & TSCMOD_DIAG_SPEED)      rv |= tscmod_diag_speed(ws);
  if(ws->diag_type & TSCMOD_DIAG_TX_LOOPBACK)rv |= tscmod_diag_tx_loopback(ws);
  if(ws->diag_type & TSCMOD_DIAG_RX_LOOPBACK)rv |= tscmod_diag_rx_loopback(ws);
  if(ws->diag_type & TSCMOD_DIAG_EYE)        rv |= tscmod_diag_eye(ws);
#ifndef __KERNEL__
  if(ws->diag_type & TSCMOD_DIAG_RX_PPM)     rv |= tscmod_diag_rx_ppm(ws);
#endif
  if(ws->diag_type & TSCMOD_DIAG_RX_SIGDET)  rv |= tscmod_diag_rx_sigdet(ws);
  if(ws->diag_type & TSCMOD_DIAG_PRBS)       rv |= tscmod_diag_prbs(ws);
  if(ws->diag_type & TSCMOD_DIAG_CL72)       rv |= tscmod_diag_cl72(ws);
  if(ws->diag_type & TSCMOD_DIAG_PLL)        rv |= tscmod_diag_pll(ws);
  if(ws->diag_type & TSCMOD_DIAG_PMD)        rv |= tscmod_diag_pmd(ws);
  if(ws->diag_type & TSCMOD_DIAG_HG)         rv |= tscmod_diag_hg(ws);
  return rv;
}

#endif  /* #ifdef INCLUDE_XGXS_TSCMOD */
