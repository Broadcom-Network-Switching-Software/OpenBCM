/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shared/bsl.h>
#include "wcmod_main.h"
#include "wcmod.h"
#include "wcmod_defines.h"
#ifdef _SDK_WCMOD_
#include "wcmod_phyreg.h"
#include <sal/core/thread.h>
#endif

int wcmod_pll_lock_wait(wcmod_st* pc);
extern int wcmod_firmware_set (wcmod_st *pc);
extern int _wcmod_asymmetric_mode(wcmod_st* pc);
extern wcmod_int_st _wcmod_int_st; 

/* int Wcmod_print_wcmod_st(wcmod_st*)            :: PRINT_WCMOD_ST         */
/* int Wcmod_100fx_control (wcmod_st*)            :: 100FX_CONTROL          */
/* int Wcmod_master_slave_control(wcmod_st* pc)   :: MASTER_SLAVE_CONTROL   */
/* int Wcmod_tx_lane_disable (wcmod_st*)          :: TX_LANE_DISABLE        */
/* int Wcmod_pcs_bypass_ctl(wcmod_st*)            :: PCS_BYPASS_CTL         */
/* int Wcmod_autoneg_control(wcmod_st*)           :: AUTONEG_CONTROL        */
/* int Wcmod_pll_sequencer_control(wcmod_st*)     :: PLL_SEQUENCER_CONTROL  */
/* int Wcmod_pll_lock_wait(pc);                   :: PLL_LOCK_WAIT          */
/* int Wcmod_set_port_mode(wcmod_st*)             :: SET_PORT_MODE          */
/* int Wcmod_parallel_detect_control(wcmod_st*)   :: PARALLEL_DETECT_CONTROL*/
/* int Wcmod_tx_loopback_control(wcmod_st*)       :: TX_LOOPBACK_CONTROL    */
/* int Wcmod_aer_lane_select(wcmod_st*)           :: AER_LANE_SELECT        */
/* int Wcmod_speed_intf_set(wcmod_st*)            :: SET_SPD_INTF           */
/* int Wcmod_rx_seq_control(wcmod_st*)            :: RX_SEQ_CONTROL         */
/* int Wcmod_soft_reset(wcmod_st*)                :: SOFT_RESET             */
/* int Wcmod_revid_read(wcmod_st*)                :: REVID_READ             */
/* int Wcmod_clause72_control(wcmod_st*)          :: CLAUSE72_CONTROL       */
/* int Wcmod_prbs_check(wcmod_st*)                :: PRBS_CHECK             */
/* int Wcmod_prbs_control(wcmod_st*)              :: PRBS_CONTROL           */
/* int Wcmod_prbs_decouple_control(wcmod_st*)     :: PRBS_DECOUPLE_CONTROL  */
/* int Wcmod_cjpat_crpat_control(wcmod_st*)       :: CJPAT_CRPAT_CONTROL    */
/* int Wcmod_cjpat_crpat_check  (wcmod_st*)       :: CJPAT_CRPAT_CHECK      */
/* int Wcmod_tx_bert_control(wcmod_st*)           :: TX_BERT_CONTROL        */
/* int Wcmod_lane_swap(wcmod_st *pc)              :: LANE_SWAP              */
/* int Wcmod_tx_rx_polarity(wcmod_st *pc)         :: TX_RX_POLARITY         */
/* int Wcmod_prog_data(wcmod_st *pc)              :: PROG_DATA              */
/* int Wcmod_core_reset(wcmod_st*)                :: CORE_RESET             */
/* int Wcmod_firmware_set (wcmod_st*)             :: FIRMWARE_SET           */
/* int Wcmod_BRCM64B66B_control(wcmod_st*)        :: BRCM64B66B_CONTROL     */
/* int Wcmod_FEC_control(wcmod_st*)               :: FEC_CONTROL            */
/* int Wcmod_tx_amp_control(wcmod_st*)            :: TX_AMP_CONTROL         */
/* int Wcmod_tx_pi_control(wcmod_st*)             :: TX_PI_CONTROL          */
/* int Wcmod_rx_p1_slicer_control(wcmod_st*)      :: RX_P1_SLICER_CONTROL   */
/* int Wcmod_rx_m1_slicer_control(wcmod_st*)      :: RX_M1_SLICER_CONTROL   */
/* int Wcmod_rx_d_slicer_control(wcmod_st*)       :: RX_Z_SLICER_CONTROL    */
/* int Wcmod_tx_main_tap_control(wcmod_st* pc)    :: TX_MAIN_TAP_CONTROL    */
/* int Wcmod_tx_pre_tap_control(wcmod_st* pc)     :: TX_PRE_TAP_CONTROL     */
/* int Wcmod_tx_post_tap_control(wcmod_st* pc)    :: TX_POST_TAP_CONTROL    */
/* int Wcmod_tx_tap_control(wcmod_st*)            :: TX_TAP_CONTROL         */
/* int Wcmod_rx_PF_control(wcmod_st*)             :: RX_PF_CONTROL          */
/* int Wcmod_rx_low_freq_PF_control(wcmod_st*)    :: RX_LOW_FREQ_PF_CONTROL */
/* int Wcmod_rx_DFE_tap1_control(wcmod_st*)       :: RX_DFE_TAP1_CONTROL    */
/* int Wcmod_rx_DFE_tap2_control(wcmod_st*)       :: RX_DFE_TAP2_CONTROL    */
/* int Wcmod_rx_DFE_tap3_control(wcmod_st*)       :: RX_DFE_TAP3_CONTROL    */
/* int Wcmod_rx_DFE_tap4_control(wcmod_st*)       :: RX_DFE_TAP4_CONTROL    */
/* int Wcmod_rx_DFE_tap5_control(wcmod_st*)       :: RX_DFE_TAP5_CONTROL    */
/* int Wcmod_rx_VGA_control(wcmod_st*)            :: RX_VGA_CONTROL         */
/* int Wcmod_power_control(wcmod_st*)             :: POWER_CONTROL          */
/* int Wcmod_duplex_control(wcmod_st*)            :: DUPLEX_CONTROL         */
/* int Wcmod_port_stat_display(wcmod_st*)         :: PORT_STAT_DISPLAY      */
/* int Wcmod_autoneg_set(wcmod_st*)               :: AUTONEG_SET            */
/* int Wcmod_scrambler_control(wcmod_st*)         :: SCRAMBLER_CONTROL      */
/* int Wcmod_rx_loopback_control(wcmod_st*)       :: RX_LOOPBACK_CONTROL    */
/* int Wcmod_rx_loopback_pcs_bypass(wcmod_st*)    :: RX_LOOPBACK_PCS_BYPASS */
/* int Wcmod_packet_size_IPG_set(wcmod_st*)       :: PACKET_SIZE_IPG_SET    */
/* int Wcmod_tx_rx_packets_check(wcmod_st*)       :: TX_RX_PACKETS_CHECK    */
/* int Wcmod_refclk_set(wcmod_st*)                :: REFCLK_SET             */
/* int Wcmod_asymmetric_mode_set(wcmod_st*)       :: ASYMMETRIC_MODE_SET    */
/* int Wcmod_EEE_pass_thru_set(wcmod_st*)         :: EEE_PASS_THRU_SET      */
/* int Wcmod_EEE_set(wcmod_st*)                   :: EEE_SET                */
                                                                            
/* int Wcmod_reg_read(wcmod_st*)                  :: REG_READ               */
/* int Wcmod_reg_write(wcmod_st*)                 :: REG_WRITE              */
                                                                            
/* int Wcmod_wait_bit(wcmod_st*)                  :: WAIT_BIT               */
/* int Wcmod_wait_time(wcmod_st*)                 :: WAIT_TIME              */

/*!
\brief <B>Do not directly call this function.</B>  It is called by the function
#wcmod_pll_sequencer_control(). This function waits for the the TX-PLL to lock.

\param  pc handle to current warpcore context (#wcmod_st)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.
*/

int wcmod_pll_lock_wait(wcmod_st* pc)
{
  int rv;

  rv = wcmod_regbit_set_wait_check(pc,WC40_XGXSBLK0_XGXSSTATUSr,
                   XGXSBLK0_XGXSSTATUS_TXPLL_LOCK_MASK,1,(WC40_PLL_WAIT*200));

  if (rv == SOC_E_TIMEOUT) {
    printf("%-22s: Error. Timeout TXPLL lock: port %d \n", FUNCTION_NAME(), pc->port);
    return (SOC_E_TIMEOUT);
  }
  return (SOC_E_NONE);
}

/*!
\brief Print Warpcore programmers context.

\param  pc handle to current warpcore context (#wcmod_st)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.

This function prints out the fields of the #wcmod_st struct. Currently it does
not print all fields. The user warpcore context (void pointer attached) cannot
be printed since this function does not know the fields.
*/

int wcmod_print_wcmod_st(wcmod_st* pc)
{
  printf("id/Unit/Prt:%d/%d/%d phyAd/portAd:%d/%x thisLane:%d LnNumIgn:%d\n",
         pc->id, pc->unit, pc->port, pc->phy_ad, pc->prt_ad, pc->this_lane,
         pc->lane_num_ignore);
  printf("perLaneCtrl/mask/1-8:%x/%x/%x/%x/%x/%x/%x/%x/%x/%x\n",
         pc->per_lane_control, pc->per_lane_control_x,
         pc->per_lane_control_1, pc->per_lane_control_2, pc->per_lane_control_3,
         pc->per_lane_control_4, pc->per_lane_control_5, pc->per_lane_control_6,
         pc->per_lane_control_7, pc->per_lane_control_8);
  printf("spd_intf:%s(%d),mdio_type:%s(%d), portType:%s(%d) osType:%s(%d)\n",
         e2s_wcmod_spd_intfc_set[pc->spd_intf],pc->spd_intf,
         e2s_wcmod_mdio_type[pc->mdio_type], pc->mdio_type,
         e2s_wcmod_port_type[pc->port_type],pc->port_type,
         e2s_wcmod_os_type[pc->os_type],pc->os_type);
  printf("dgxs:%d touched:%d laneSwap:%x Dplx:%d modelType:%d v:%d CT:%d\n",
         pc->dxgxs, pc->wc_touched, pc->lane_swap, pc->duplex,
         pc->model_type, pc->verbosity, pc->wcmod_sdk_ctrl_type);
  printf("AN type:%d rf:%d paus:%d, tech_ability:%d fec:%d higig2:%d cl72:%d\n",
         pc->an_type, pc->an_rf,     pc->an_pause, pc->an_tech_ability,
         pc->an_fec,  pc->an_higig2, pc->an_cl72); 
  printf("refclk:%d plldiv:%d\n", pc->refclk,  pc->plldiv); 
  return SOC_E_NONE;
}

/*!
\brief Squelch TX lane output.

\param  pc handle to current warpcore context (#wcmod_st)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.


\details This function disables transmission on a specific lane. No reset is
required to restart the transmission. Lane control is done through
#wcmod_st::per_lane_control. Set bits 0, 8, 16, or 24 to  <B>0 to disable
transmission on lanes 0, 1, 2, or 3,</B>. Set the bits to <B>1 to enable
transmission.

Does not work with B0 and previous versions.

This function modifies the following registers

\li DIGITAL4_MISC3_CONTROL (0x833c)

<B>How to call:</B><br>
<p>Call this function directly or through the function
#wcmod_tier1_selector() with the upper-case string "TX_LANE_DISABLE".</p>
*/

int wcmod_tx_lane_disable (wcmod_st* pc)
{
  uint16 mask;
  int cntl;

  cntl=(pc->per_lane_control & (0xff << (pc->this_lane * 8))) >> (pc->this_lane * 8);

  switch(pc->this_lane) {
    case 0:
      mask = TX0_TX_DRIVER_ELECIDLE_MASK;
      if (cntl) {
        if (pc->verbosity > 0)
          printf("%-22s: Lane(%d) TX disabled\n", FUNCTION_NAME(), pc->this_lane);
        SOC_IF_ERROR_RETURN(
		    MODIFY_WC40_TX0_TX_DRIVERr(pc->unit,pc,mask,mask));
      } else {
        if (pc->verbosity > 0)
          printf("%-22s: Lane(%d) TX enabled\n", FUNCTION_NAME(), pc->this_lane);
        SOC_IF_ERROR_RETURN(
                   MODIFY_WC40_TX0_TX_DRIVERr(pc->unit, pc, 0,mask));
      }
      break;
    case 1:
      mask = TX1_TX_DRIVER_ELECIDLE_MASK;
      if (cntl) {
        if (pc->verbosity > 0)
          printf("%-22s: Lane(%d) TX disabled\n", FUNCTION_NAME(), pc->this_lane);
        SOC_IF_ERROR_RETURN(
		    MODIFY_WC40_TX1_TX_DRIVERr(pc->unit,pc,mask,mask));
      } else {
        if (pc->verbosity > 0)
          printf("%-22s: Lane(%d) TX enabled\n", FUNCTION_NAME(), pc->this_lane);
        SOC_IF_ERROR_RETURN(
                   MODIFY_WC40_TX1_TX_DRIVERr(pc->unit, pc, 0,mask));
      }
      break;
    case 2:
      mask = TX2_TX_DRIVER_ELECIDLE_MASK;
      if (cntl) {
        if (pc->verbosity > 0)
          printf("%-22s: Lane(%d) TX disabled\n", FUNCTION_NAME(), pc->this_lane);
        SOC_IF_ERROR_RETURN(
		    MODIFY_WC40_TX2_TX_DRIVERr(pc->unit,pc,mask,mask));
      } else {
        if (pc->verbosity > 0)
          printf("%-22s: Lane(%d) TX enabled\n", FUNCTION_NAME(), pc->this_lane);
        SOC_IF_ERROR_RETURN(
                   MODIFY_WC40_TX2_TX_DRIVERr(pc->unit, pc, 0,mask));
      }
      break;
    case 3:
      mask = TX3_TX_DRIVER_ELECIDLE_MASK;
      if (cntl) {
        if (pc->verbosity > 0)
          printf("%-22s: Lane(%d) TX disabled\n", FUNCTION_NAME(), pc->this_lane);
        SOC_IF_ERROR_RETURN(
		    MODIFY_WC40_TX3_TX_DRIVERr(pc->unit,pc,mask,mask));
      } else {
        if (pc->verbosity > 0)
          printf("%-22s: Lane(%d) TX enabled\n", FUNCTION_NAME(), pc->this_lane);
        SOC_IF_ERROR_RETURN(
                   MODIFY_WC40_TX3_TX_DRIVERr(pc->unit, pc, 0,mask));
      }
      break;
    default:
      break;
  }
  return SOC_E_NONE;
}

/*!
\brief controls 100fx registers to enable/disable autodetect of 100fx

\param  pc handle to current warpcore context (#wcmod_st)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.


\details Set Bits 0, 8, 16 and/or 24 of #wcmod_st::per_lane_control to <B>0 to
disable</B> 100FX auto-detect, and <B>1 to enable</B>.

This function modifies the following registers

\li FX100_CONTROL1 (0x8400)
\li FX100_CONTROL3 (0x8402)

<B>How to call:</B><br>
<p>Call this function directly or through the function
#wcmod_tier1_selector() with the upper-case string "100FX_CONTROL".</p>
*/
int wcmod_100fx_control (wcmod_st* pc)
{
  uint16 data16, mask16;

  int cntl=(pc->per_lane_control & (0xff << pc->this_lane * 8)) >> pc->this_lane * 8;
  if (cntl) {
    if (pc->verbosity > 0) printf("%-22s: 100fx set\n", FUNCTION_NAME());
      data16 = SERDESDIGITAL_CONTROL1000X1_FIBER_MODE_1000X_MASK;

      MODIFY_WC40_SERDESDIGITAL_CONTROL1000X1r(pc->unit, pc, data16,
                            SERDESDIGITAL_CONTROL1000X1_FIBER_MODE_1000X_MASK);

      mask16 = FX100_CONTROL1_FAR_END_FAULT_EN_MASK |
               FX100_CONTROL1_FULL_DUPLEX_MASK |
               FX100_CONTROL1_ENABLE_MASK;
      data16 = mask16;
      MODIFY_WC40_FX100_CONTROL1r(pc->unit, pc, data16, mask16);

  } else {
    /* disable 100FX and 100FX auto-detect */
    SOC_IF_ERROR_RETURN
     (MODIFY_WC40_FX100_CONTROL1r(pc->unit, pc, 0, FX100_CONTROL1_ENABLE_MASK));
    /* disable 100FX idle detect */
    SOC_IF_ERROR_RETURN
      (MODIFY_WC40_FX100_CONTROL3r(pc->unit, pc,
                   FX100_CONTROL3_CORRELATOR_DISABLE_MASK,
                   FX100_CONTROL3_CORRELATOR_DISABLE_MASK));
  }
  return SOC_E_NONE;
}

/*!
\brief Controls Master Slave settings for 100/120/150G setups.

\param  pc handle to current warpcore context (#wcmod_st)

\returns The value #SOC_E_NONE upon successful completion of the function.

\details  In 100/120/150G mode, three WCs work in tandem, one as a master and
the other two as slaves. The master can control the slaves via straps. Software
overrides exist too. It is possible to bring up 100G and up completely in
software. The WCs are always in combo mode.

Use the ::wcmod_st::per_lane_control field of #wcmod_st as follows.
This is in combo mode, set only the first byte.
\li First bit:    0 -> Do nothing.
                  1 -> wakeup slave.

<B>How to call:</B><br>
<p>This function may be called directly or through the function
#wcmod_tier1_selector() using case-sensitive string "SLAVE_WAKEUP_CONTROL".</p>
*/
int wcmod_slave_wakeup_control (wcmod_st* pc)
{
  int cntl=(pc->per_lane_control & (0xff << pc->this_lane * 8)) >> pc->this_lane * 8;

  if (!cntl) {
    if (pc->verbosity > 0) printf("%-22s: Do nothing(%d).\n", FUNCTION_NAME(), cntl);
    return SOC_E_NONE;
  }
  if (pc->verbosity > 0) printf("%-22s: Wakeup slave(%d).\n", FUNCTION_NAME(), cntl);

  return SOC_E_NONE;
}

/*!
\brief Wake up the slave in 100G mode. Slaves need kickstart from the straps
connected to the master, or via a software override. It is possible to bring
100G up completely in software.

\param  pc handle to current warpcore context (#wcmod_st)

\returns The value #SOC_E_NONE upon successful completion of the function.

\details Set the ::wcmod_st::per_lane_control field of #wcmod_st as follows.
 This is always in combo mode, so you only set the first byte.
\li First nibble: 0 -> disabled. In the other  cases below 0 is always active.
                  1 -> WC is master, 4 lanes active (0,1,2,3)
                  2 -> WC is master, 3 lanes active (0,1,2  )
                  3 -> WC is master, 2 lanes active (0,1    )
                  4 -> WC is slave,  4 lanes active (0,1,2,3)
                  5 -> WC is slave,  3 lanes active (0,1,2  )
                  6 -> WC is slave,  2 lanes active (0,1    )

Other parameters like speed are set in the set_sped intfc.

<B>How to call:</B><br>
<p>This function may be called directly or through the function
#wcmod_tier1_selector() using case-sensitive string "MASTER_SLAVE_CONTROL".</p>
*/
int wcmod_master_slave_control (wcmod_st* pc)
{
  uint16 data, mask;
  int cntl = pc->per_lane_control & 0x7;

  MODIFY_WC40_XGXSBLK6_XGXSX2CONTROL2r(pc->unit, pc, 
                                        XGXSBLK6_XGXSX2CONTROL2_100G_BIT_MASK,
                                        XGXSBLK6_XGXSX2CONTROL2_100G_BIT_MASK);
  if (cntl) {
    if (cntl == 1 || cntl == 2 || cntl == 3) {
      if (pc->verbosity > 0) printf("%-22s: WC in master mode\n", FUNCTION_NAME());
    } else if (cntl == 4 || cntl == 5 || cntl == 6) {
      if (pc->verbosity > 0) printf("%-22s: WC in slave mode\n", FUNCTION_NAME());
    } else {
      printf("%-22s: Error: Bad 100G mode for WC\n", FUNCTION_NAME());
    }

    data = XGXSBLK10_MISC_CONTROL_MASTER_SLAVE_LANES_OVERRIDE_MASK;
    data &= ~XGXSBLK10_MISC_CONTROL_SLAVE_WAKEUP_OVERRIDE_MASK;
    data &= ~XGXSBLK10_MISC_CONTROL_SLAVE_WAKEUP_OVERRIDE_VALUE_MASK;
    data |= (cntl << XGXSBLK10_MISC_CONTROL_MASTER_SLAVE_LANES_OVERRIDE_VALUE_SHIFT);
    mask = XGXSBLK10_MISC_CONTROL_MASTER_SLAVE_LANES_OVERRIDE_MASK |
           XGXSBLK10_MISC_CONTROL_SLAVE_WAKEUP_OVERRIDE_MASK |
           XGXSBLK10_MISC_CONTROL_SLAVE_WAKEUP_OVERRIDE_VALUE_MASK |
           XGXSBLK10_MISC_CONTROL_MASTER_SLAVE_LANES_OVERRIDE_VALUE_MASK;
  } else {
    if (pc->verbosity > 0) printf("%-22s: Master/Slave Modes bypassed", FUNCTION_NAME());
    data = XGXSBLK10_MISC_CONTROL_MASTER_SLAVE_LANES_OVERRIDE_MASK;
    mask = XGXSBLK10_MISC_CONTROL_MASTER_SLAVE_LANES_OVERRIDE_MASK |
           XGXSBLK10_MISC_CONTROL_SLAVE_WAKEUP_OVERRIDE_MASK;
  }
  SOC_IF_ERROR_RETURN(
    MODIFY_WC40_XGXSBLK10_MISC_CONTROLr (pc->unit, pc, data, mask));
  return SOC_E_NONE;
}

/*!
\brief controls PCS Bypass Control function.

\param  pc handle to current warpcore context (#wcmod_st)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.


\details Set the ::wcmod_st::per_lane_control field of #wcmod_st to <B>0 to
disable</B> PCS  <B>1 to enable</B>.

This function modifies the following registers

\li TBD
\li TBD

<B>How to call:</B><br>
<p>Call this function directly or through the function
#wcmod_tier1_selector() with the upper-case string "PCS_BYPASS_CTL".</p>
*/

int wcmod_pcs_bypass_ctl (wcmod_st* pc)
{
  uint16 data;
  int cntl = (pc->per_lane_control & (0xff << pc->this_lane * 8)) >> pc->this_lane * 8;

  if (cntl) {
    if (pc->verbosity > 0)
      printf("%-22s: PCS Bypassed\n", FUNCTION_NAME());
    if (pc->model_type == WCMOD_QS_A0 || pc->model_type == WCMOD_QS_B0) {
      /* No concept of PCS Bypassing here. */
    } else {
      if (pc->model_type == WCMOD_WC_B0) {
        /*reset PLL */
        if (pc->spd_intf ==  WCMOD_SPD_PCSBYP_10P3125) {
          data = XGXSBLK0_XGXSCONTROL_TXCKO_DIV_MASK |
           XGXSBLK0_XGXSCONTROL_AFRST_EN_MASK |
                XGXSBLK0_XGXSCONTROL_EDEN_MASK |
         XGXSBLK0_XGXSCONTROL_CDET_EN_MASK |
                XGXSBLK0_XGXSCONTROL_RESERVED0_MASK |
                (XGXSBLK0_XGXSCONTROL_MODE_10G_IndLane_OS8
             << XGXSBLK0_XGXSCONTROL_MODE_10G_SHIFT);
        } else {
          data = XGXSBLK0_XGXSCONTROL_TXCKO_DIV_MASK |
             XGXSBLK0_XGXSCONTROL_AFRST_EN_MASK |
                 XGXSBLK0_XGXSCONTROL_EDEN_MASK |
             XGXSBLK0_XGXSCONTROL_CDET_EN_MASK |
                 XGXSBLK0_XGXSCONTROL_RESERVED0_MASK |
         (XGXSBLK0_XGXSCONTROL_MODE_10G_IndLane_OS5
          << XGXSBLK0_XGXSCONTROL_MODE_10G_SHIFT);
        }
        WRITE_WC40_XGXSBLK0_XGXSCONTROLr(pc->unit, pc, data);
        /*turn off 8b10b */
        WRITE_WC40_XGXSBLK1_LANECTRL2r(pc->unit, pc, 0);
        /*turn off 64b66b */
        WRITE_WC40_XGXSBLK1_LANECTRL0r(pc->unit, pc, 0);
        /*turn off PRBS */
        WRITE_WC40_XGXSBLK1_LANEPRBSr(pc->unit, pc, 0);
        /*next the PLL multiplier and ref clock */
        if (pc->spd_intf ==  WCMOD_SPD_PCSBYP_10P3125) {
          data = SERDESDIGITAL_MISC1_FORCE_SPEED_dr_10GBASE_CX4 |
                (SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div66
                  << SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT) |
                SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
                (SERDESDIGITAL_MISC1_REFCLK_SEL_clk_156p25MHz
                  << SERDESDIGITAL_MISC1_REFCLK_SEL_SHIFT);
        } else {
          data = SERDESDIGITAL_MISC1_FORCE_SPEED_dr_6363BRCM_X1 |
                (SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div40
                  << SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT) |
                SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
                (SERDESDIGITAL_MISC1_REFCLK_SEL_clk_156p25MHz
                  << SERDESDIGITAL_MISC1_REFCLK_SEL_SHIFT);
        }
        WRITE_WC40_SERDESDIGITAL_MISC1r(pc->unit, pc, data);
        /*next enable the fiber mode */
        MODIFY_WC40_SERDESDIGITAL_CONTROL1000X1r(pc->unit, pc,
                             SERDESDIGITAL_CONTROL1000X1_FIBER_MODE_1000X_MASK,
                             SERDESDIGITAL_CONTROL1000X1_FIBER_MODE_1000X_MASK);
        /*then turn comma detect and eden 8b/10b */
        MODIFY_WC40_XGXSBLK0_XGXSCONTROLr(pc->unit, pc,
            XGXSBLK0_XGXSCONTROL_CDET_EN_MASK | XGXSBLK0_XGXSCONTROL_EDEN_MASK,
            XGXSBLK0_XGXSCONTROL_CDET_EN_MASK | XGXSBLK0_XGXSCONTROL_EDEN_MASK);
      }
    }
  } else {
    if (pc->verbosity > 0)
      printf("%-22s: PCS Not Bypassed.\n", FUNCTION_NAME());
    if (pc->model_type == WCMOD_QS_A0 || pc->model_type == WCMOD_QS_B0) {
      /* No concept of PCS Bypass for these.. */
    }
  }
  return SOC_E_NONE;
}

/*!
\brief Controls the setting/resetting of autoneg advertisement registers.

\param  pc handle to current warpcore context (#wcmod_st)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.

\details
This function modifies the following registers:

<B>How to call:</B><br>
<p>Call this function directly or through the function
#wcmod_tier1_selector() with the upper-case string "AUTONEG_SET".</p>
*/

int wcmod_autoneg_set(wcmod_st* pc)               /* AUTONEG_SET */
{
  uint16 data16, mask16;

  int rf = pc->an_rf;
  int pause = pc->an_pause;
  int tech_ability = pc->an_tech_ability;
  int fec = pc->an_fec;
  if (pc->verbosity > 0) printf("%s an_type:%d tech_abi:%d/%d\n",
                      FUNCTION_NAME(), pc->an_type, tech_ability, pc->an_tech_ability);
  if (pc->an_type == WCMOD_CL73) {
  data16 = ((rf << AN_IEEE1BLK_AN_ADVERTISEMENT0_RF_SHIFT) |
            (pause << AN_IEEE1BLK_AN_ADVERTISEMENT0_PAUSE_SHIFT));
  mask16 = AN_IEEE1BLK_AN_ADVERTISEMENT0_RF_MASK |
            AN_IEEE1BLK_AN_ADVERTISEMENT0_PAUSE_MASK;
  if (pc->verbosity > 0) printf("%s data:%d mask:%d\n", FUNCTION_NAME(), data16, mask16);
  SOC_IF_ERROR_RETURN
    (MODIFY_WC40_AN_IEEE1BLK_AN_ADVERTISEMENT0r(pc->unit, pc, data16, mask16));
  data16 = (tech_ability << AN_IEEE1BLK_AN_ADVERTISEMENT1_TECHABILITY_SHIFT);
  SOC_IF_ERROR_RETURN
    (MODIFY_WC40_AN_IEEE1BLK_AN_ADVERTISEMENT1r(pc->unit, pc, data16,
      AN_IEEE1BLK_AN_ADVERTISEMENT1_TECHABILITY_MASK));
  if (pc->verbosity > 0) printf("%s mask:%d data:%d\n", FUNCTION_NAME(), data16, mask16);
  data16 = fec << AN_IEEE1BLK_AN_ADVERTISEMENT2_FEC_REQUESTED_SHIFT;
  SOC_IF_ERROR_RETURN
    (MODIFY_WC40_AN_IEEE1BLK_AN_ADVERTISEMENT2r(pc->unit, pc, data16,
      AN_IEEE1BLK_AN_ADVERTISEMENT2_FEC_REQUESTED_MASK));

  } else {
    return SOC_E_NONE;
  }
  return SOC_E_NONE;
}

/*!
\brief Controls cl37 aneg set/reset user pages 4, 3, 2 & 1 advertisement regs.

\param  pc handle to current warpcore context (#wcmod_st)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.

This function modifies the following registers:

\li TBD
\li TBD

<B>How to call:</B><br>
<p>Call this function directly or through the function
#wcmod_tier1_selector() with the upper-case string "AUTONEG_PAGE_SET".</p>
*/

int wcmod_autoneg_page_set(wcmod_st* pc)               /* AUTONEG_PAGE_SET */
{
  uint16 data16, mask16;
  int an_reg[5];
  int regNum, bitPos;
  int i, x, rf, pause, higig2, cl72, fec, speedmap;

  mask16   = 0xFFFF;
  rf       = pc->an_rf;
  pause    = pc->an_pause;
  higig2   = pc->an_higig2;
  cl72     = pc->an_cl72;
  fec      = pc->an_fec;
  speedmap = pc->an_tech_ability;

  an_reg[0] = an_reg[1] = an_reg[2] = an_reg[3] = an_reg[4] = 0;

  for (i=0; i < CNT_wcmod_tech_ability; i++) {
    if (pc->verbosity > 0)
      printf("%d %s %d\n", i, e2s_wcmod_tech_ability[i],
                                                     e2n_wcmod_tech_ability[i]);
    if (speedmap & (1 << i)) {
      x = e2n_wcmod_tech_ability[i];
      regNum = ((x & 0xf00) >> 8);   /* number to link to the adv. page  */
      bitPos = 1 << (x & 0x0ff);   /* bit position in the above register. */
      if (pc->verbosity > 0) 
        printf("regNum: %d bitPos: %d\n", regNum, bitPos);
      an_reg[regNum] |= bitPos;
      if (pc->verbosity > 0) 
        printf("regNum: %d reg: %x\n", regNum, an_reg[regNum]);
    }
  }
  /* Writing to AN adv0 cl73 (0x0x3800_0010) */
  if (pc->verbosity > 0) printf("an_type:%d spd_map:%x %x %x %x %x %x\n", pc->an_type, speedmap,
                        an_reg[0], an_reg[1], an_reg[2], an_reg[3], an_reg[4]);
  data16 = ((rf << AN_IEEE1BLK_AN_ADVERTISEMENT0_RF_SHIFT) |
            (pause << AN_IEEE1BLK_AN_ADVERTISEMENT0_PAUSE_SHIFT));
  mask16 = AN_IEEE1BLK_AN_ADVERTISEMENT0_RF_MASK |
            AN_IEEE1BLK_AN_ADVERTISEMENT0_PAUSE_MASK;
  SOC_IF_ERROR_RETURN
    (MODIFY_WC40_AN_IEEE1BLK_AN_ADVERTISEMENT0r(pc->unit, pc, data16, mask16));

  /* Writing to CL73 techAbility register bits 15:5 (0x0x3800_0011) */
  data16 = an_reg[0];
  SOC_IF_ERROR_RETURN
    (MODIFY_WC40_AN_IEEE1BLK_AN_ADVERTISEMENT1r(pc->unit, pc, data16, 0xffff));

  /* Writing to UP1 (0x8329) */
  data16 = an_reg[1];
  SOC_IF_ERROR_RETURN(MODIFY_WC40_DIGITAL3_UP1r(pc->unit, pc, data16, 0xffff));

  /* Writing to UP2 - (0x832A) */
  SOC_IF_ERROR_RETURN (MODIFY_WC40_DIGITAL3_UP2r(pc->unit, pc, 0x0, 0xffff));

  /* Writing to UP3 (0x832B) */
  data16 = an_reg[3] |
           (fec << DIGITAL3_UP3_FEC_SHIFT) |
           (higig2 << DIGITAL3_UP3_HIGIG2_SHIFT) |
           (cl72 << DIGITAL3_UP3_CL72_SHIFT);
  mask16 =  DIGITAL3_UP3_DATARATE_40G_MASK | 
            DIGITAL3_UP3_DATARATE_31P5G_MASK |
            DIGITAL3_UP3_DATARATE_25P45GX4_MASK |
            DIGITAL3_UP3_DATARATE_21GX4_MASK |
            DIGITAL3_UP3_CL72_MASK | DIGITAL3_UP3_FEC_MASK |
            DIGITAL3_UP3_HIGIG2_MASK;

  SOC_IF_ERROR_RETURN(MODIFY_WC40_DIGITAL3_UP3r(pc->unit, pc, data16, mask16));

  /* Writing to UP4 (0x835d) */
  data16 = an_reg[4];
  SOC_IF_ERROR_RETURN(MODIFY_WC40_DIGITAL6_UP4r(pc->unit, pc, data16, 0xffff));

  

  
  SOC_IF_ERROR_RETURN
    (MODIFY_WC40_CL73_USERB0_CL73_BAMCTRL1r(pc->unit, pc, 0x2000, 0x3fff));


  SOC_IF_ERROR_RETURN
    (MODIFY_WC40_CL73_USERB0_CL73_BAMCTRL2r(pc->unit, pc, 0x0, 0xffff));


  SOC_IF_ERROR_RETURN
    (MODIFY_WC40_CL73_USERB0_CL73_BAMCTRL3r(pc->unit, pc, 0x1, 0xffff));

  return SOC_E_NONE;
}

/*!
\brief Controls the setting/resetting of autoneg ability

\param  pc handle to current warpcore context (#wcmod_st)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.


This function programs auto-negotiation (AN) modes for the warpcore. It can
enable/disable clause37/clause73/BAM autonegotiation capabilities. Call this
function once for combo mode and once per lane in independent lane mode.

The autonegotiation mode is controlled by setting specific bits of
#wcmod_st::per_lane_control. Setting specific bit to 0 turns off the feature
and setting to 1 implies turns it on.

For lane 0, the 4 bits (0-3) are defined thus.
\li bit 0: controls clause37
\li bit 1: controls clause73
\li bit 2: controls clause37+BAM
\li bit 3: controls clause73+BAM

Bits associated with lane 1 (4-7), lane 2 (8-11) and lane 3 (12-15) have
similar definitions

Use this predefined constant for better readability.

\li #DO_NOT_USE_AN  (disable all autoneg modes)

The autonegotiation mode is indicated by setting the bit-wise ORing the field

\li #USE_CLAUSE_37_AN_LANE0
\li #USE_CLAUSE_73_AN_LANE0
\li #USE_CLAUSE_37_AN_WITH_BAM_LANE0
\li #USE_CLAUSE_73_AN_WITH_BAM_LANE0

where 0 (is for lane 0) should be replaced by 1, 2, or 3 for other lanes.

This function modifies the following registers:

\li IEEE0BLK_MIICNTL            (0x0000_0000)
\li AN_IEEE0BLK_AN_IEEECONTROL1 (0x3800_0000)

<B>How to call:</B><br>
<p>Call this function directly or through the function
#wcmod_tier1_selector() with the upper-case string "AUTONEG_CONTROL".</p>
*/

int wcmod_autoneg_control(wcmod_st* pc)
{
  uint16 data16 = 0, mask16 = 0;
  int cntl = (pc->per_lane_control & (0xff << pc->this_lane * 8)) >> pc->this_lane * 8;


  /* 0x8357[10:9] = 2'b1x
     * Set new advertisement page count identifier(hardware determines the highest common number of pages to send) that differentiates it from
     * HyperCore or other legacy devices(3 user pages)
     */
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DIGITAL6_UD_FIELDr(pc->unit, pc,
                1 << 10, 3 << 9));
  
  if (pc->verbosity > 0)
      printf("%-22s: p=%0d cntl=%x per_lane_cntl=%x lane=%0d select=%x\n",FUNCTION_NAME(), 
             pc->port, cntl, pc->per_lane_control, pc->this_lane, pc->lane_select);

  if ((cntl & USE_CLAUSE_37_AN) || (cntl & USE_CLAUSE_37_AN_WITH_BAM)) {
    if (pc->verbosity > 0)
        printf("%-22s: p=%d cl37 autoneg_enable\n",FUNCTION_NAME(), pc->port);
    if((pc->model_type == WCMOD_XN) || (pc->model_type == WCMOD_QS_A0) || (pc->model_type == WCMOD_QS)) {
      SOC_IF_ERROR_RETURN (MODIFY_WC40_IEEE0BLK_MIICNTLr(pc->unit, pc,
                                     IEEE0BLK_MIICNTL_AUTONEG_ENABLE_MASK,
                                     IEEE0BLK_MIICNTL_AUTONEG_ENABLE_MASK));
    } else {
      SOC_IF_ERROR_RETURN (MODIFY_WC40_COMBO_IEEE0_MIICNTLr(pc->unit, pc,
                                    COMBO_IEEE0_MIICNTL_AUTONEG_ENABLE_MASK,
                                    COMBO_IEEE0_MIICNTL_AUTONEG_ENABLE_MASK));
    }
  } else {
    /* disable cl37 an. */
    if (pc->verbosity > 0)
        printf("%-22s: p=%0d cl37 autoneg_disable\n",FUNCTION_NAME(), pc->port);
    if((pc->model_type == WCMOD_XN) || (pc->model_type == WCMOD_QS_A0) || (pc->model_type == WCMOD_QS)) {
      SOC_IF_ERROR_RETURN (MODIFY_WC40_IEEE0BLK_MIICNTLr(pc->unit, pc,
                            0, COMBO_IEEE0_MIICNTL_AUTONEG_ENABLE_MASK));
    } else  {
      SOC_IF_ERROR_RETURN (MODIFY_WC40_COMBO_IEEE0_MIICNTLr(pc->unit, pc,
                                  0, COMBO_IEEE0_MIICNTL_AUTONEG_ENABLE_MASK));
    }
  }
  if ((cntl & USE_CLAUSE_73_AN) || (cntl & USE_CLAUSE_73_AN_WITH_BAM)) {
    if (pc->verbosity > 0)
        printf("%-22s: p=%0d cl73 autoneg_enable\n",FUNCTION_NAME(), pc->port);
    SOC_IF_ERROR_RETURN((MODIFY_WC40_AN_IEEE0BLK_AN_IEEECONTROL1r(pc->unit, pc,
                AN_IEEE0BLK_AN_IEEECONTROL1_AN_ENABLE_MASK,
                AN_IEEE0BLK_AN_IEEECONTROL1_AN_ENABLE_MASK)));
    if ((cntl & USE_CLAUSE_73_AN_WITH_BAM)) {
        mask16 = CL73_USERB0_CL73_BAMCTRL1_CL73_BAMEN_MASK; 
        SOC_IF_ERROR_RETURN
          (MODIFY_WC40_CL73_USERB0_CL73_BAMCTRL1r(pc->unit, pc, mask16, mask16));   
    }
    /* restart is called at the end of this function 
    SOC_IF_ERROR_RETURN (MODIFY_WC40_AN_IEEE0BLK_AN_IEEECONTROL1r(pc->unit, pc,
                AN_IEEE0BLK_AN_IEEECONTROL1_RESTARTAN_MASK,
                AN_IEEE0BLK_AN_IEEECONTROL1_RESTARTAN_MASK));
    */
  } else {
    /* disable cl73 an */
    if (pc->verbosity > 0)
        printf("%-22s: p=%0d cl73 autoneg_disable\n",FUNCTION_NAME(), pc->port);
    SOC_IF_ERROR_RETURN
      (MODIFY_WC40_AN_IEEE0BLK_AN_IEEECONTROL1r(pc->unit, pc,0,
                AN_IEEE0BLK_AN_IEEECONTROL1_AN_ENABLE_MASK));
    mask16 = CL73_USERB0_CL73_BAMCTRL1_CL73_BAMEN_MASK; 
    SOC_IF_ERROR_RETURN
      (MODIFY_WC40_CL73_USERB0_CL73_BAMCTRL1r(pc->unit, pc, 0, mask16)); 

  }

  if (cntl & USE_CLAUSE_73_AN_WITH_BAM) {
    if (pc->verbosity > 0)
        printf("%-22s p=%0d Enabling cl73 with BAM\n",FUNCTION_NAME(), pc->port);
    data16 = CL73_USERB0_CL73_BAMCTRL1_CL73_BAMEN_MASK |
             CL73_USERB0_CL73_BAMCTRL1_CL73_BAM_STATION_MNGR_EN_MASK;
    mask16 = CL73_USERB0_CL73_BAMCTRL1_CL73_BAMEN_MASK |
             CL73_USERB0_CL73_BAMCTRL1_CL73_BAM_STATION_MNGR_EN_MASK;

    SOC_IF_ERROR_RETURN
      (MODIFY_WC40_CL73_USERB0_CL73_BAMCTRL1r(pc->unit, pc, data16, mask16));

  } else {
    if (pc->verbosity > 0)
        printf("%-22s p=%0d Disabling cl73 with BAM.\n",FUNCTION_NAME(), pc->port);
    mask16 = CL73_USERB0_CL73_BAMCTRL1_CL73_BAMNP_AFTER_BP_EN_MASK |
             CL73_USERB0_CL73_BAMCTRL1_CL73_BAM_STATION_MNGR_EN_MASK;
    SOC_IF_ERROR_RETURN
      (MODIFY_WC40_CL73_USERB0_CL73_BAMCTRL1r(pc->unit, pc, mask16, mask16));
  }

  if (cntl & USE_CLAUSE_37_AN_WITH_BAM) {
    if (pc->verbosity > 0)
        printf("%-22s p=%0d Enabling cl37+BAM\n",FUNCTION_NAME(), pc->port);
  data16 = DIGITAL6_MP5_NEXTPAGECTRL_BAM_MODE_MASK |
           DIGITAL6_MP5_NEXTPAGECTRL_TETON_MODE_MASK |
           DIGITAL6_MP5_NEXTPAGECTRL_TETON_MODE_UP3_EN_MASK;
  mask16 = DIGITAL6_MP5_NEXTPAGECTRL_BAM_MODE_MASK |
           DIGITAL6_MP5_NEXTPAGECTRL_TETON_MODE_MASK |
           DIGITAL6_MP5_NEXTPAGECTRL_TETON_MODE_UP3_EN_MASK;
  SOC_IF_ERROR_RETURN
    (MODIFY_WC40_DIGITAL6_MP5_NEXTPAGECTRLr(pc->unit, pc, data16, mask16));
  } else if (cntl & USE_CLAUSE_37_AN) {
      if (pc->verbosity > 0)
          printf ("%-22s p=%0d Enabling cl37\n",FUNCTION_NAME(), pc->port);
      data16 = DIGITAL6_MP5_NEXTPAGECTRL_TETON_MODE_MASK |
               DIGITAL6_MP5_NEXTPAGECTRL_TETON_MODE_UP3_EN_MASK;
      mask16 = DIGITAL6_MP5_NEXTPAGECTRL_BAM_MODE_MASK |
               DIGITAL6_MP5_NEXTPAGECTRL_TETON_MODE_MASK |
               DIGITAL6_MP5_NEXTPAGECTRL_TETON_MODE_UP3_EN_MASK;
      SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DIGITAL6_MP5_NEXTPAGECTRLr(pc->unit, pc, data16, mask16));
  } else {
    if (pc->verbosity > 0)
        printf("%-22s p=%0d Disabling cl37+BAM\n",FUNCTION_NAME(), pc->port);
    mask16 = DIGITAL6_MP5_NEXTPAGECTRL_BAM_MODE_MASK   |
             DIGITAL6_MP5_NEXTPAGECTRL_TETON_MODE_MASK |
             DIGITAL6_MP5_NEXTPAGECTRL_TETON_MODE_UP3_EN_MASK;
  SOC_IF_ERROR_RETURN
    (MODIFY_WC40_DIGITAL6_MP5_NEXTPAGECTRLr(pc->unit, pc, 0x0, mask16));
  }
/* if 73BAM and 37BAM is enabled together restart 37 AN only */
  if ((cntl & USE_CLAUSE_73_AN_WITH_BAM) && (cntl & USE_CLAUSE_37_AN_WITH_BAM)) {
    SOC_IF_ERROR_RETURN (MODIFY_WC40_COMBO_IEEE0_MIICNTLr(pc->unit, pc,
                         COMBO_IEEE0_MIICNTL_RESTART_AUTONEG_MASK,
                         COMBO_IEEE0_MIICNTL_RESTART_AUTONEG_MASK));
  } else if ((cntl & USE_CLAUSE_73_AN) || (cntl & USE_CLAUSE_73_AN_WITH_BAM)) {
        SOC_IF_ERROR_RETURN (MODIFY_WC40_AN_IEEE0BLK_AN_IEEECONTROL1r(pc->unit, pc,
                    AN_IEEE0BLK_AN_IEEECONTROL1_RESTARTAN_MASK,
                    AN_IEEE0BLK_AN_IEEECONTROL1_RESTARTAN_MASK));
  } else if ((cntl & USE_CLAUSE_37_AN) || (cntl & USE_CLAUSE_37_AN_WITH_BAM)) {
        SOC_IF_ERROR_RETURN (MODIFY_WC40_COMBO_IEEE0_MIICNTLr(pc->unit, pc,
                             COMBO_IEEE0_MIICNTL_RESTART_AUTONEG_MASK,
                             COMBO_IEEE0_MIICNTL_RESTART_AUTONEG_MASK));
  }

  return SOC_E_NONE;
} /* wcmod_autoneg_control(wcmod_st* pc) */

/*!
\brief Control turning on and off the PLL Sequencer.

\param  pc handle to current warpcore context (#wcmod_st)

\returns The value #SOC_E_NONE upon successful completion.

\details

This function turns on/off the PLL Sequencer.  To turn the PLL Sequencer on,
set #wcmod_st::per_lane_control to 1 to turn on the sequencer and set it to 0
turn it off. Call this function once for all lanes. Do not call it once per
lane, even in independent mode.

This function modifies the following registers:

\li XGXSBLK0_XGXSCONTROL (0x8000)

<B>How to call:</B><br>
<p>Call this function directly or through the function #wcmod_tier1_selector()
using the upper case string "PLL_SEQUENCER_CONTROL".</p>
*/

int wcmod_pll_sequencer_control(wcmod_st* pc)   /* PLL_SEQUENCER_CONTROL */
{
  int cntl = pc->per_lane_control;
  int tmplane_num_ignore;

  tmplane_num_ignore = pc->lane_num_ignore;
  pc->lane_num_ignore = 1; /* write only to lane 1. */

  if (cntl == WCMOD_ENABLE) {
    /* enable PLL sequencer */
    SOC_IF_ERROR_RETURN (MODIFY_WC40_XGXSBLK0_XGXSCONTROLr(pc->unit, pc,
                          XGXSBLK0_XGXSCONTROL_START_SEQUENCER_MASK,
                          XGXSBLK0_XGXSCONTROL_START_SEQUENCER_MASK));
    /* implement wait. Wait for 2 micro seconds... for PLL to lock */
    wcmod_pll_lock_wait(pc);
  } else if (cntl == WCMOD_PLL_ENABLE_NO_WAIT) {
    /* enable PLL sequencer without wait for the PLL lock */
    SOC_IF_ERROR_RETURN (MODIFY_WC40_XGXSBLK0_XGXSCONTROLr(pc->unit, pc,
                          XGXSBLK0_XGXSCONTROL_START_SEQUENCER_MASK,
                          XGXSBLK0_XGXSCONTROL_START_SEQUENCER_MASK));
  } else {
    SOC_IF_ERROR_RETURN (MODIFY_WC40_XGXSBLK0_XGXSCONTROLr(pc->unit, pc, 0,
                                    XGXSBLK0_XGXSCONTROL_START_SEQUENCER_MASK));
  }
  pc->lane_num_ignore = tmplane_num_ignore;
  return SOC_E_MULT_REG;  /* No errors. Just no need to call multiple times */
}

/* internal function */
int _wcmod_lane_enable(wcmod_st *pc)
{
  /* enable this lane now by disabling lane disable */
  SOC_IF_ERROR_RETURN(MODIFY_WC40_DIGITAL4_MISC3r(pc->unit, pc,
                                          0, DIGITAL4_MISC3_LANEDISABLE_MASK));
  return SOC_E_NONE;
}

int _wcmod_getRevDetails(wcmod_st* pc)
{
  uint16 data;
  int revLet, revNum, modelNum; /* needed for vers. and rev. id. */

  pc->model_type = WCMOD_MODEL_TYPE_ILLEGAL; /* assume non-standard model */

  /******************* DECODE SERDES ID ****************************/
  SOC_IF_ERROR_RETURN(READ_WC40_SERDESID_SERDESID0r(pc->unit,pc,&data));
  revLet = ((data & SERDESID_SERDESID0_REV_LETTER_MASK)
               >> SERDESID_SERDESID0_REV_LETTER_SHIFT);
  revNum = ((data & SERDESID_SERDESID0_REV_NUMBER_MASK)
               >> SERDESID_SERDESID0_REV_NUMBER_SHIFT);
  modelNum = ((data & SERDESID_SERDESID0_MODEL_NUMBER_MASK)
               >> SERDESID_SERDESID0_MODEL_NUMBER_SHIFT);

  if (modelNum == MODEL_WARPCORE) {                          /* Warpcore */
    if (revLet == 0) {
      if        (revNum == 0) { pc->model_type = WCMOD_WC_A0;
      } else if (revNum == 1) { pc->model_type = WCMOD_WC_A1;
      } else if (revNum == 2) { pc->model_type = WCMOD_WC_A2;
      } else {
        printf("%-22s: Bad revNum:%d for revLet:%d port:%d\n", FUNCTION_NAME(), revNum,revLet, pc->port);
        return SOC_E_ERROR;
      }
    } else if (revLet == 1) {
      if        (revNum == 0) { pc->model_type = WCMOD_WC_B0;
      } else if (revNum == 1) { pc->model_type = WCMOD_WC_B1;
      } else if (revNum == 2) { pc->model_type = WCMOD_WC_B2;
      } else {
        printf("%-22s: Bad revNum:%d for revLet:%d port:%d\n", FUNCTION_NAME(), revNum,revLet, pc->port);
        return SOC_E_ERROR;
      }
    } else if (revLet == 2) {
      if        (revNum == 0) { pc->model_type = WCMOD_WC_C0;
      } else if (revNum == 1) { pc->model_type = WCMOD_WC_C1;
      } else if (revNum == 2) { pc->model_type = WCMOD_WC_C2;
      } else {
        printf("%-22s: Bad revNum:%d for revLet:%d port:%d\n", FUNCTION_NAME(), revNum,revLet, pc->port);
        return SOC_E_ERROR;
      }
    } else if (revLet == 3) {
      if        (revNum == 0) { pc->model_type = WCMOD_WC_D0;
      } else if (revNum == 1) { pc->model_type = WCMOD_WC_D1;
      } else if (revNum == 2) { pc->model_type = WCMOD_WC_D2;
      } else {
        printf("%-22s: Bad revNum:%d for revLet:%d port:%d\n", FUNCTION_NAME(), revNum,revLet, pc->port);
        return SOC_E_ERROR;
      }
    } else {
      printf("%-22s: Bad revLet:%d for modelNum:%d port:%d\n",FUNCTION_NAME(),revLet,modelNum, pc->port);
      return SOC_E_ERROR;
    }
  } else if (modelNum == MODEL_XGXS_16G) {                      /* Xenia */
    pc->model_type = WCMOD_XN;
  } else if (modelNum == MODEL_WARPCORE_C)  {                   /*C*/
    if (revLet == 0) {
        pc->model_type = WCMOD_WC_C0;
    } else {
      printf("%-22s: Bad revLet:%d for modelNum:%d port:%d\n",FUNCTION_NAME(),revLet,modelNum, pc->port);
      return SOC_E_ERROR;
    }
  } else if (modelNum == MODEL_WARPLITE) {                      /*WarpLite*/
    if (revLet == 0) {
      if (revNum == 0 || revNum == 1) {
        pc->model_type = WCMOD_WL_A0;
      } else {
        printf("%-22s: Bad revNum:%d for revLet:%d port:%d\n",FUNCTION_NAME(),revNum,revLet, pc->port);
        return SOC_E_ERROR;
      }
    } else if (revLet == 1) {
      if (revNum == 0 || revNum == 1) {
        pc->model_type = WCMOD_WL_B0;
      } else {
        printf("%-22s: Bad revNum:%d for revLet:%d port:%d\n",FUNCTION_NAME(),revNum,revLet, pc->port);
        return SOC_E_ERROR;
      }
    } else {
      printf("%-22s: Bad revLet:%d for modelNum:%d port:%d\n",FUNCTION_NAME(),revLet,modelNum, pc->port);
      return SOC_E_ERROR;
    }
  } else if (modelNum == MODEL_ESM_SERDES) {
    pc->model_type = WCMOD_WL_A0;
  } else if (modelNum == MODEL_QUADSGMII) {
    pc->model_type = WCMOD_QS;
  } else if (modelNum == MODEL_QSGMII) {
    if (revNum == 0) { pc->model_type = WCMOD_QS_A0;
        /* } else if (revNum == 1) { pc->model_type = WCMOD_QS_B0;
        printf("%-22s: QS B0 modelNum:%d port:%d shouldn't be supported\n", FUNCTION_NAME(), modelNum, pc->port);
        */
    } else {
        printf("%-22s: Bad modelNum:%d revNum=%0d port:%d\n", FUNCTION_NAME(), modelNum, revNum, pc->port);
        return SOC_E_ERROR;
    }
  } else {
    printf("%-22s: Bad modelNum:%d port:%d\n", FUNCTION_NAME(), modelNum, pc->port);
    return SOC_E_ERROR;
  }
  if (pc->verbosity > 0) printf("%-22s: wcmod_revid:0x%x revNo.:%d revLet:%d modelNum:0x%x Model:%s\n",
                             FUNCTION_NAME(), data, revNum, revLet,
                             modelNum, e2s_wcmod_model_type[pc->model_type]);
  return SOC_E_NONE;
}

/*!
\brief Init routine sets various operating modes of warpcore.

\param  pc handle to current warpcore context (#wcmod_st)

\returns #SOC_E_NONE if successful. SOC_E_ERROR else.

\details

This function is called once per Warpcore. It cannot be called multiple times
and should be called immediately after reset. Elements of #wcmod_st should be
initialized to required values prior to calling this function. The following
sub-configurations are performed here.

\li PMA/PMD mux/demux
\li single/dual/quad combo/independent lane mode (#wcmod_st:::port_type)
\li oversampling rates for independent mode(OS5/OS8)(#wcmod_st:::os_type)
\li Set VCO rate for multi-port modes.
\li MAC interface.
\li Load Firmware. (In MDK/SDK this is done externally. The platform provides a
method to load firmware. WCMod cannot load firmware via MDIO.)
\li Configure the MAC interface.
\li Read the revision Id.

<B>How to call:</B><br>
<p>Call this function directly or through the function
#wcmod_tier1_selector() with the upper-case string "SET_PORT_MODE".</p>
*/

int wcmod_set_port_mode2(wcmod_st* pc)           /* SET_PORT_MODE */
{
  if (pc->verbosity > 0) printf("works\n");
  return SOC_E_MULT_REG;
}

int wcmod_set_port_mode(wcmod_st* pc)           /* SET_PORT_MODE */
{
  uint16 rv, mask, data, maskr, datar, laneValue;
  int tmp_lane_select;

  int laneSwap = pc->lane_swap;
  int unit = pc->unit;
  wcmod_os_type os = pc->os_type;

  if (pc->wc_touched == 0) {
    pc->wc_touched = 1;
  } else {
    if (pc->verbosity > 0) printf("%-22s: Skipping multiple inits\n",FUNCTION_NAME());
    return SOC_E_NONE;
  }

  SOC_IF_ERROR_RETURN(_wcmod_getRevDetails(pc));

  pc->lane_num_ignore = 1;
  /***************  COMBO/INDEP SINGLE/DUAL/QUAD PORT *****************/
  /* fix OS Type for XENIA */
  if ((pc->model_type == WCMOD_XN) || (pc->model_type == WCMOD_QS)) {
    pc->os_type = os = WCMOD_OS_IND_LANE;
    if (pc->verbosity > 0)
      printf("%-22s: OS fixed to %d for Xenia\n",FUNCTION_NAME(), pc->os_type);
  }
  /***** disable laneDisable and cl49 for TR3 *****/
  pc->lane_num_ignore = 0;
  tmp_lane_select     = pc->lane_select;        /* save this */
  pc->lane_select = WCMOD_LANE_BCST; /* go to broadcast mode */
  SOC_IF_ERROR_RETURN(MODIFY_WC40_DIGITAL4_MISC3r(pc->unit, pc, 0,
                                           DIGITAL4_MISC3_LANEDISABLE_MASK));
  /* disable cl72 */
  mask = CL72_USERB0_CL72_MISC1_CONTROL_LINK_CONTROL_FORCE_MASK |
         CL72_USERB0_CL72_MISC1_CONTROL_LINK_CONTROL_FORCEVAL_MASK;
  MODIFY_WC40_CL72_USERB0_CL72_MISC1_CONTROLr(pc->unit, pc, 
         CL72_USERB0_CL72_MISC1_CONTROL_LINK_CONTROL_FORCE_MASK, mask);

  /*set all the sync word and mask for all lanes */
  WRITE_WC40_RX66_SCW0r(pc->unit, pc, 0xE070);
  WRITE_WC40_RX66_SCW1r(pc->unit, pc, 0xC0D0);
  WRITE_WC40_RX66_SCW2r(pc->unit, pc, 0xA0B0);
  WRITE_WC40_RX66_SCW3r(pc->unit, pc, 0x8090);
  WRITE_WC40_RX66_SCW0_MASKr(pc->unit, pc, 0xF0F0);
  WRITE_WC40_RX66_SCW1_MASKr(pc->unit, pc, 0xF0F0);
  WRITE_WC40_RX66_SCW2_MASKr(pc->unit, pc, 0xF0F0);
  WRITE_WC40_RX66_SCW3_MASKr(pc->unit, pc, 0xF0F0);

  SOC_IF_ERROR_RETURN(MODIFY_WC40_RX66B1_BRCM_CL49_CONTROLr(pc->unit, pc,
                        RX66B1_BRCM_CL49_CONTROL_MDIO_BRCM6466_CL49_EN_MASK,
                        RX66B1_BRCM_CL49_CONTROL_MDIO_BRCM6466_CL49_EN_MASK));
  pc->lane_select        = tmp_lane_select;     /* restore */
  pc->lane_num_ignore    = 1;                   /* enable again */

  if (pc->port_type == WCMOD_COMBO) {
    if (pc->verbosity > 0)
      printf("%-22s: Combo Mode\n",FUNCTION_NAME());

    SOC_IF_ERROR_RETURN(MODIFY_WC40_SERDESDIGITAL_MISC1r(unit, pc, 0x0,
                              SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK));

    SOC_IF_ERROR_RETURN(MODIFY_WC40_XGXSBLK0_XGXSCONTROLr(unit, pc,
                                 (XGXSBLK0_XGXSCONTROL_MODE_10G_ComboCoreMode <<
                     XGXSBLK0_XGXSCONTROL_MODE_10G_SHIFT),
                     XGXSBLK0_XGXSCONTROL_MODE_10G_MASK));
    
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_XGXSBLK6_XGXSX2CONTROL2r(unit,pc, 
                     XGXSBLK6_XGXSX2CONTROL2_MAC_INF_TXCK_SEL_MASK,
                     XGXSBLK6_XGXSX2CONTROL2_MAC_INF_TXCK_SEL_MASK)) ;

  } else if (pc->port_type == WCMOD_INDEPENDENT) {
    if( pc->dual_type ) {
      pc->dxgxs = 0 ; 
      pc->lane_select = WCMOD_LANE_BCST;
      SOC_IF_ERROR_RETURN
          (MODIFY_WC40_XGXSBLK6_XGXSX2CONTROL2r(unit,pc, 
              (0x9 <<  XGXSBLK6_XGXSX2CONTROL2_MAC_INF_TXCK_SEL_SHIFT),
                       XGXSBLK6_XGXSX2CONTROL2_MAC_INF_TXCK_SEL_MASK)) ;
      pc->dxgxs = pc->dual_type ;
      pc->lane_select        = tmp_lane_select;     /* restore */
    } else {
      SOC_IF_ERROR_RETURN
          (MODIFY_WC40_XGXSBLK6_XGXSX2CONTROL2r(unit,pc, 
                    0, XGXSBLK6_XGXSX2CONTROL2_MAC_INF_TXCK_SEL_MASK)) ;
    }

    if (pc->verbosity > 0)
      printf("%-22s: Independent Mode\n",FUNCTION_NAME());
    switch(pc->refclk) {
      case 25 : datar=SERDESDIGITAL_MISC1_REFCLK_SEL_clk_25MHz;     break;
      case 50 : datar=SERDESDIGITAL_MISC1_REFCLK_SEL_clk_50Mhz;     break;
      case 100: datar=SERDESDIGITAL_MISC1_REFCLK_SEL_clk_100MHz;    break;
      case 106: datar=SERDESDIGITAL_MISC1_REFCLK_SEL_clk_106p25Mhz; break;
      case 125: datar=SERDESDIGITAL_MISC1_REFCLK_SEL_clk_125MHz;    break;
      case 156: datar=SERDESDIGITAL_MISC1_REFCLK_SEL_clk_156p25MHz; break;
      case 161: datar=SERDESDIGITAL_MISC1_REFCLK_SEL_clk_161p25Mhz; break;
      case 187: datar=SERDESDIGITAL_MISC1_REFCLK_SEL_clk_187p5MHz;  break;
      default : datar=SERDESDIGITAL_MISC1_REFCLK_SEL_clk_156p25MHz; 
    }
    maskr = SERDESDIGITAL_MISC1_REFCLK_SEL_MASK;
    datar = datar << SERDESDIGITAL_MISC1_REFCLK_SEL_SHIFT;

    /* os5/os8 (for indep. only) */
    if        (os == WCMOD_OS5) {
      SOC_IF_ERROR_RETURN (MODIFY_WC40_XGXSBLK0_XGXSCONTROLr(unit, pc,
                   (XGXSBLK0_XGXSCONTROL_MODE_10G_IndLane_OS5 <<
                    XGXSBLK0_XGXSCONTROL_MODE_10G_SHIFT),
                    XGXSBLK0_XGXSCONTROL_MODE_10G_MASK));

      data = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |  /* PP */
             (SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div40 <<
              SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT) | datar;
      mask = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
             SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_MASK | maskr;

      SOC_IF_ERROR_RETURN(MODIFY_WC40_SERDESDIGITAL_MISC1r(unit,pc,data, mask));
    } else if (os == WCMOD_OS_IND_LANE) { /* Xenia OS IndLane neither OS5/OS8 */
      SOC_IF_ERROR_RETURN (MODIFY_WC40_XGXSBLK0_XGXSCONTROLr(unit, pc,
               (XGXSBLK0_XGXSCONTROL_MODE_10G_IndLane_XENIA <<
                XGXSBLK0_XGXSCONTROL_MODE_10G_SHIFT),
               XGXSBLK0_XGXSCONTROL_MODE_10G_MASK));

      if (pc->refclk == 125) { /* For ESM_SERDES  core (redstone) */
        data = ((SERDESDIGITAL_MISC1_REFCLK_SEL_clk_125MHz
             << SERDESDIGITAL_MISC1_REFCLK_SEL_SHIFT) |
        SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
        (SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div20
         << SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT));
    mask = SERDESDIGITAL_MISC1_REFCLK_SEL_MASK |
           SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
           SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_MASK;
      } else {
    data = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK | 
           (SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div16_XENIA
        << SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT) |
        datar;
    mask = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
           SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_MASK |
           maskr;
      }
      SOC_IF_ERROR_RETURN(MODIFY_WC40_SERDESDIGITAL_MISC1r(unit,pc,data, mask));
    } else if (os == WCMOD_OS8) {
      SOC_IF_ERROR_RETURN (MODIFY_WC40_XGXSBLK0_XGXSCONTROLr(unit, pc,
               (XGXSBLK0_XGXSCONTROL_MODE_10G_IndLane_OS8 <<
                XGXSBLK0_XGXSCONTROL_MODE_10G_SHIFT),
               XGXSBLK0_XGXSCONTROL_MODE_10G_MASK));

      data = (SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK | 
              (SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div66
              << SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT) |
              datar);
      mask = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
             SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_MASK |
             maskr;

      SOC_IF_ERROR_RETURN(MODIFY_WC40_SERDESDIGITAL_MISC1r(unit,pc,data, mask));
    } else {
      if (pc->model_type == WCMOD_WL_A0 || pc->model_type == WCMOD_WC_C0) {
        SOC_IF_ERROR_RETURN(MODIFY_WC40_SERDESDIGITAL_MISC1r(unit, pc, 0x0,
                              SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK));
        /* QSGMII Mode for dependentl lanes is 6 not 5 */
      } else if ((pc->model_type == WCMOD_QS_A0) && (os == WCMOD_OS6)) {
        SOC_IF_ERROR_RETURN (MODIFY_WC40_XGXSBLK0_XGXSCONTROLr(unit, pc,
                                                               (XGXSBLK0_XGXSCONTROL_MODE_10G_IndLane_OS6 <<
                                                                XGXSBLK0_XGXSCONTROL_MODE_10G_SHIFT),
                                                               XGXSBLK0_XGXSCONTROL_MODE_10G_MASK));
        
      } else {
        printf("%-22s Error: Bad OSTYPE in independant mode:%d\n",FUNCTION_NAME(),os);
        return SOC_E_ERROR;
      }
    }
  } else {
    printf("%-22s Error: Bad combo/ind mode:%d\n",FUNCTION_NAME(),pc->port_type);
    return SOC_E_ERROR;
  }

  /* configure the PMA/PMD mux/demux */
  mask = 0x00ff;
  /* Mapping 16 bit value to 8 bit positions */
  if ((laneSwap & 0xffff) == 0x3210) {
    if (pc->verbosity > 0)
      printf("%-22s: Tx Lanes not swapped\n", FUNCTION_NAME());
  } else {
    laneValue =(((laneSwap & 0x3000) >> 6) | ((laneSwap & 0x0300) >> 4) |
                ((laneSwap & 0x0030) >> 2) | ((laneSwap & 0x0003) >> 0));
    SOC_IF_ERROR_RETURN
          (MODIFY_WC40_XGXSBLK8_TXLNSWAP1r(unit, pc, (laneValue & 0xff), mask));
    if (pc->verbosity > 0) printf("%-22s: Tx lanes swapped: %08d->%04d\n", FUNCTION_NAME(),
                                              (laneSwap & 0xffff), laneValue);
  }
  laneValue =(((laneSwap & 0x30000000) >> 22) | ((laneSwap & 0x03000000) >> 20) |
              ((laneSwap & 0x00300000) >> 18) | ((laneSwap & 0x00030000) >> 16));
  if ((laneSwap & 0xffff0000)== 0x32100000) {
    if (pc->verbosity > 0)
      printf("%-22s: Rx Lanes not swapped\n", FUNCTION_NAME());
  } else {
    SOC_IF_ERROR_RETURN
      (MODIFY_WC40_XGXSBLK8_RXLNSWAP1r(unit, pc, laneValue, mask));
    if (pc->verbosity > 0) printf("%-22s: Rx lanes swapped: %08d->%04d\n", FUNCTION_NAME(),
                                            (laneSwap&0xffff0000), laneValue);
  }

  /* VCO rate, set only for indep mode. Automatically done in combo mode. */
  if (pc->port_type == WCMOD_INDEPENDENT) {
    tmp_lane_select =  pc->lane_select; /* save. Next writes are broadcast */
    pc->lane_select = WCMOD_LANE_BCST;

    if (pc->refclk == 125) {
      data =((SERDESDIGITAL_MISC1_REFCLK_SEL_clk_125MHz
              << SERDESDIGITAL_MISC1_REFCLK_SEL_SHIFT) |
             SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
             (SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div20
              << SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT));
     mask = SERDESDIGITAL_MISC1_REFCLK_SEL_MASK |
            SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
            SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_MASK;
    } else {
      data =(SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
             (SERDESDIGITAL_MISC1_REFCLK_SEL_clk_156p25MHz
             << SERDESDIGITAL_MISC1_REFCLK_SEL_SHIFT));
      mask = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
             SERDESDIGITAL_MISC1_REFCLK_SEL_MASK;
    }

    /* Set to 1G mode to avoid conflicting default setting like R2 */
    /*
    SOC_IF_ERROR_RETURN(MODIFY_WC40_SERDESDIGITAL_MISC1r(unit, pc, 0x0,
                                         SERDESDIGITAL_MISC1_FORCE_SPEED_MASK));
    SOC_IF_ERROR_RETURN(MODIFY_WC40_DIGITAL4_MISC3r(unit, pc, 0x0,
                                           DIGITAL4_MISC3_FORCE_SPEED_B5_MASK));
    if (pc->model_type == WCMOD_XN) {
      SOC_IF_ERROR_RETURN(MODIFY_WC40_IEEE0BLK_MIICNTLr(unit, pc,
                                     IEEE0BLK_MIICNTL_MANUAL_SPEED1_MASK,
                                     (IEEE0BLK_MIICNTL_MANUAL_SPEED1_MASK |
                                      IEEE0BLK_MIICNTL_MANUAL_SPEED0_MASK)));
    } else {
      SOC_IF_ERROR_RETURN(MODIFY_WC40_COMBO_IEEE0_MIICNTLr(unit, pc,
                                     COMBO_IEEE0_MIICNTL_MANUAL_SPEED1_MASK,
                                     (COMBO_IEEE0_MIICNTL_MANUAL_SPEED1_MASK |
                                      COMBO_IEEE0_MIICNTL_MANUAL_SPEED0_MASK)));
    }
    */
    pc->lane_select = tmp_lane_select; /* restore. End of broadcast */
  }

  /* configure MAC interface: make a seperate process
  if ((pc->port_type==WCMOD_INDEPENDENT) &
      (  (spd_intf==WCMOD_SPD_R2_12000)
       || (spd_intf==WCMOD_SPD_10000_XFI)
       || (spd_intf==SCMOD_SPD_15750_HI_DXGXS)){
    mask = XGXSBLK6_XGXSX2CONTROL2_MAC_INF_RXCK_OVERRIDE_MASK;
    SOC_IF_ERROR_RETURN(MODIFY_WC40_XGXSBLK6_XGXSX2CONTROL2r(unit,pc,mask,mask);
    if (pc->verbosity > 0)
      printf("%-22s. mac_inf_rxck_override bit is set\n",FUNCTION_NAME());
  }

  if ((pc->port_type == WCMOD_INDEPENDENT) & (spd_intf == WCMOD_SPD_10000_XFI)) {
    mask = 1 << 13;
    SOC_IF_ERROR_RETURN(MODIFY_WC40_XGXSBLK6_XGXSX2CONTROL2r(unit,pc,mask,mask);
    if (pc->verbosity > 0)
      printf("%-22s txckout33_override bit is set\n",FUNCTION_NAME());
  }
  */
  
  /* load firmware */
  rv = wcmod_firmware_set (pc);
  pc->lane_num_ignore = 0;
  return rv;
} /* wcmod_set_port_mode(wcmod_st* pc) */

/*!
\brief Sets the 1G and 10G parallel detect mode

\param  pc handle to current warpcore context (#wcmod_st)

\returns The value #SOC_E_NONE upon successful completion

This function enables or disables parallel detection for a particular lane for
both 1G and 10G modes.  To control the parallel detect mode, set the
bit-encoded #wcmod_st::per_lane_control field as follows

The parallel detect mode is controlled by setting specific bits of
#wcmod_st::per_lane_control. Setting specific bit to 0 turns off the feature
and setting to 1 implies turns it on.

For lane 0, the 2 bits (0-1) are defined thus.
\li bit 0: controls 1g parallel detect
\li bit 1: controls 10g parallel detect

Bits associated with lane 1 (8-9), lane 2 (16-17) and lane 3 (24-25) have
similar definitions

This function modifies the following registers

\li SERDESDIGITAL_CONTROL1000X1 (0x8300)
\li SERDESDIGITAL_CONTROL1000X2 (0x8301)
\li XGXSBLK5_PARDET10GCONTROL   (0x8131)

<B>How to call:</B><br>
<p>Call this function directly or through the function #wcmod_tier1_selector() using the
case-sensitive string "PARALLEL_DETECT_CONTROL".</p>

*/
int wcmod_parallel_detect_control(wcmod_st* pc)    /* PARALLEL_DETECT_CONTROL */
{
  uint16 data16, mask16;
  int cntl = pc->per_lane_control;

  if (cntl & 0x1) {
    if (pc->verbosity > 0) printf("%-22s: 1G parallel_detect_enable\n",FUNCTION_NAME());
    SOC_IF_ERROR_RETURN(MODIFY_WC40_SERDESDIGITAL_CONTROL1000X2r(pc->unit, pc,
                SERDESDIGITAL_CONTROL1000X2_ENABLE_PARALLEL_DETECTION_MASK,
                SERDESDIGITAL_CONTROL1000X2_ENABLE_PARALLEL_DETECTION_MASK));
  } else {
    if (pc->verbosity > 0) printf("%-22s: 1G parallel_detect_disable\n",FUNCTION_NAME());
    data16 = SERDESDIGITAL_CONTROL1000X1_AUTODET_EN_ALIGN <<
                            SERDESDIGITAL_CONTROL1000X1_AUTODET_EN_SHIFT |
                            SERDESDIGITAL_CONTROL1000X1_FIBER_MODE_1000X_BITS;
    mask16 = SERDESDIGITAL_CONTROL1000X1_AUTODET_EN_MASK |
             SERDESDIGITAL_CONTROL1000X1_FIBER_MODE_1000X_MASK;
    SOC_IF_ERROR_RETURN(MODIFY_WC40_SERDESDIGITAL_CONTROL1000X1r(pc->unit, pc,
                                                               data16, mask16));
    SOC_IF_ERROR_RETURN(MODIFY_WC40_SERDESDIGITAL_CONTROL1000X1r(pc->unit, pc,
                SERDESDIGITAL_CONTROL1000X1_AUTODET_EN_ALIGN <<
                SERDESDIGITAL_CONTROL1000X1_AUTODET_EN_SHIFT,
                SERDESDIGITAL_CONTROL1000X1_AUTODET_EN_MASK));

    data16 = SERDESDIGITAL_CONTROL1000X2_DISABLE_FALSE_LINK_BITS <<
                SERDESDIGITAL_CONTROL1000X2_DISABLE_FALSE_LINK_SHIFT;

    data16 &= ~SERDESDIGITAL_CONTROL1000X2_ENABLE_PARALLEL_DETECTION_MASK;

    mask16= data16 | SERDESDIGITAL_CONTROL1000X2_ENABLE_PARALLEL_DETECTION_MASK;

    SOC_IF_ERROR_RETURN
      (MODIFY_WC40_SERDESDIGITAL_CONTROL1000X2r(pc->unit, pc, data16, mask16));
  }

  if (cntl & 0x2) {
    if (pc->verbosity > 0) printf("%-22s: 10G parallel_detect_enable\n",FUNCTION_NAME());
    SOC_IF_ERROR_RETURN(MODIFY_WC40_XGXSBLK5_PARDET10GCONTROLr(pc->unit, pc,
                XGXSBLK5_PARDET10GCONTROL_PARDET10G_EN_BITS,
                XGXSBLK5_PARDET10GCONTROL_PARDET10G_EN_MASK));

  } else {
    if (pc->verbosity > 0) printf("%-22s: 10G parallel_detect_disable\n",FUNCTION_NAME());

    SOC_IF_ERROR_RETURN(MODIFY_WC40_XGXSBLK5_PARDET10GCONTROLr(pc->unit, pc,
                XGXSBLK5_PARDET10GCONTROL_PARDET10G_EN_ALIGN,
                XGXSBLK5_PARDET10GCONTROL_PARDET10G_EN_MASK));
  }
  return SOC_E_NONE;
}

/*!
\brief Sets loopback mode from Tx to Rx at PCS/PMS parallel interface. (gloop).

\param  pc handle to current warpcore context (#wcmod_st)

\returns The value #SOC_E_NONE upon successful completion

This function sets the TX-to-RX digital loopback mode, which is set
independently for each lane at the PCS/PMS parallel interface.

The 1st, 2nd, 3rd and 4th byte of #wcmod_st::per_lane_control is associated
with lanes 0, 1, 2, and 3 respectively. The bits of each byte control their
lanes as follows.

\li 1:1 : Enable TX while in loopback.
\li 0:0 : Enable Loopback

Note that this function can program <B>multiple lanes simultaneously</B>.
As an example, to enable gloop on all lanes and enable TX on lane 0 only,
set #wcmod_st::per_lane_control to 0x01010103

This function modifies the following registers

\li XGXSBLK0_XGXSCONTROL (0x8000)
\li XGXSBLK1_LANECTRL2   (0x8017)
\li ANATXACONTROL0       (0x8061)
\li ANATXACONTROL1       (0x8071)
\li ANATXACONTROL2       (0x8081)
\li ANATXACONTROL3       (0x8091)

<B>How to call:</B><br>
<p>Call this function directly or through the function
#wcmod_tier1_selector() with the upper-case string
"TX_LOOPBACK_CONTROL".</p>
*/

int wcmod_tx_loopback_control(wcmod_st* pc)           /* TX_LOOPBACK_CONTROL  */
{
  int cntl;
  uint16 gloop_value, mdio_cont_en_value;

  cntl = pc->per_lane_control;

  gloop_value = 0;
  if (cntl & (0x1 << 0)) {
      gloop_value |= (0x1 << XGXSBLK1_LANECTRL2_GLOOP1G_SHIFT);
      if (pc->verbosity > 0) printf("%s Enabling Gloop(%x) for lane 0\n", FUNCTION_NAME(), gloop_value);
  } else {
      if (pc->verbosity > 0) printf("%s Disabling Gloop(%x) for lane 0\n", FUNCTION_NAME(), gloop_value);
  }
  if (cntl & (0x1 << 8)) {
      gloop_value |= (0x2 << XGXSBLK1_LANECTRL2_GLOOP1G_SHIFT);
      if (pc->verbosity > 0) printf("%s Enabling Gloop(%x) for lane 1\n", FUNCTION_NAME(), gloop_value);
  } else {
      if (pc->verbosity > 0) printf("%s Disabling Gloop(%x) for lane 1\n", FUNCTION_NAME(), gloop_value);
  }
  if (cntl & (0x1 << 16)) {
      gloop_value |= (0x4 << XGXSBLK1_LANECTRL2_GLOOP1G_SHIFT);
      if (pc->verbosity > 0) printf("%s Enabling Gloop(%x) for lane 2\n", FUNCTION_NAME(), gloop_value);
  } else {
      if (pc->verbosity > 0) printf("%s Disabling Gloop(%x) for lane 2\n", FUNCTION_NAME(), gloop_value);
  }
  if (cntl & (0x1 << 24)) {
      gloop_value |= (0x8 << XGXSBLK1_LANECTRL2_GLOOP1G_SHIFT);
      if (pc->verbosity > 0) printf("%s Enabling Gloop(%x) for lane 3\n", FUNCTION_NAME(), gloop_value);
  } else {
      if (pc->verbosity > 0) printf("%s Disabling Gloop(%x) for lane 3\n", FUNCTION_NAME(), gloop_value);
  }

  if ((pc->model_type) != WCMOD_QS_A0) {
  pc->lane_num_ignore = 1; /* all bits are in the same register */
  }

  mdio_cont_en_value = (gloop_value)?XGXSBLK0_XGXSCONTROL_MDIO_CONT_EN_MASK : 0;
  SOC_IF_ERROR_RETURN
    (MODIFY_WC40_XGXSBLK0_XGXSCONTROLr(pc->unit, pc, mdio_cont_en_value,
                       XGXSBLK0_XGXSCONTROL_MDIO_CONT_EN_MASK));
  SOC_IF_ERROR_RETURN
    (MODIFY_WC40_XGXSBLK1_LANECTRL2r(pc->unit, pc, gloop_value,
                     XGXSBLK1_LANECTRL2_GLOOP1G_MASK));
  /* gloop output enable lane 0 */
  if (cntl & (0x2 << 0))
    SOC_IF_ERROR_RETURN(MODIFY_WC40_TX0_ANATXACONTROL0r(pc->unit, pc,
                    TX0_ANATXACONTROL0_GLOOPOUTDIS_MASK,
                    TX0_ANATXACONTROL0_GLOOPOUTDIS_MASK));
  else
    SOC_IF_ERROR_RETURN(MODIFY_WC40_TX0_ANATXACONTROL0r(pc->unit, pc,
                    0, TX0_ANATXACONTROL0_GLOOPOUTDIS_MASK));
  /* gloop output enable lane 1 */
  if (cntl & (0x2 << 8))
    SOC_IF_ERROR_RETURN(MODIFY_WC40_TX1_ANATXACONTROL0r(pc->unit, pc,
                    TX1_ANATXACONTROL0_GLOOPOUTDIS_MASK,
                    TX1_ANATXACONTROL0_GLOOPOUTDIS_MASK));
  else
    SOC_IF_ERROR_RETURN(MODIFY_WC40_TX1_ANATXACONTROL0r(pc->unit, pc,
                    0, TX1_ANATXACONTROL0_GLOOPOUTDIS_MASK));

  /* gloop output enable lane 2 */
  if (cntl & (0x2 << 16))
    SOC_IF_ERROR_RETURN(MODIFY_WC40_TX2_ANATXACONTROL0r(pc->unit, pc,
                    TX2_ANATXACONTROL0_GLOOPOUTDIS_MASK,
                    TX2_ANATXACONTROL0_GLOOPOUTDIS_MASK));
  else
    SOC_IF_ERROR_RETURN(MODIFY_WC40_TX2_ANATXACONTROL0r(pc->unit, pc,
                    0, TX2_ANATXACONTROL0_GLOOPOUTDIS_MASK));
  /* gloop output enable lane 3 */
  if (cntl & (0x2 << 24))
    SOC_IF_ERROR_RETURN(MODIFY_WC40_TX3_ANATXACONTROL0r(pc->unit, pc,
                    TX3_ANATXACONTROL0_GLOOPOUTDIS_MASK,
                    TX3_ANATXACONTROL0_GLOOPOUTDIS_MASK));
  else
    SOC_IF_ERROR_RETURN(MODIFY_WC40_TX3_ANATXACONTROL0r(pc->unit, pc,
                    0, TX3_ANATXACONTROL0_GLOOPOUTDIS_MASK));

  if ((pc->model_type) != WCMOD_QS_A0) {
  pc->lane_num_ignore = 0 ;
  }
  return SOC_E_NONE;
}

/*!
\brief Set remote loopback mode for GMII, cl49, aggregate(XAUI) and R2 modes.

\param  pc handle to current warpcore context (#wcmod_st)

\returns The value #SOC_E_NONE upon successful completion

This function sets the remote loopback (RX-to-TX) mode for one lane at a time,
where the lane is indicated by the #wcmod_st::this_lane field. To enable remote
loopback, set the field #wcmod_st::en_dis to 1; to disable remote loopback, set
the field #wcmod_st::en_dis to 0.

Note that the speed field wcmod_st::spd_intf must be set prior to calling this
function because the loopback register and path are speed/interface dependent.

This function modifies the following registers:

\li TX66_CONTROL (0x83B0)
\li XGXSBLK0_XGXSCONTROL (0x8000)
\li SERDESDIGITAL_CONTROL1000X1 (0x8300)

The 1st, 2nd, 3rd and 4th byte of #wcmod_st::per_lane_control is associated
with lanes 0, 1, 2, and 3 respectively. The bits of each byte control their
lanes as follows.

\li 0:0 : Enable Loopback

<B>How to call:</B><br>
<p>Call this function directly or through the function
#wcmod_tier1_selector() with the upper-case string "RX_LOOPBACK_CONTROL".</p>
*/

int wcmod_rx_loopback_control(wcmod_st* pc)
{
  uint16 data;
  int cntl = (pc->per_lane_control & (0xff << pc->this_lane * 8)) >> pc->this_lane * 8;
  int mask = 0;
  int val = 0;
  int  lane_0 = 0, lane_1 = 0, lane_2 = 0, lane_3 = 0;

  wcmod_spd_intfc_set spd_intf = pc->spd_intf;

  if ((spd_intf == WCMOD_SPD_10000_XFI) || (spd_intf == WCMOD_SPD_10000_SFI) ||
      (spd_intf == WCMOD_SPD_11P5) || (spd_intf == WCMOD_SPD_12P5)) {
    data = cntl ? TX66_CONTROL_RLOOP_EN_MASK : 0;
    SOC_IF_ERROR_RETURN(MODIFY_WC40_TX66_CONTROLr(pc->unit, pc,
                                                 data,
                                                 TX66_CONTROL_RLOOP_EN_MASK));
  }else if((spd_intf == WCMOD_SPD_10000_HI) || (spd_intf == WCMOD_SPD_10000) ||
           (spd_intf == WCMOD_SPD_R2_12000) || (spd_intf == WCMOD_SPD_13000) ||
           (spd_intf == WCMOD_SPD_16000)   || (spd_intf == WCMOD_SPD_20000) ||
           (spd_intf == WCMOD_SPD_21000)   || (spd_intf == WCMOD_SPD_25455) ||
           (spd_intf == WCMOD_SPD_31500)   || (spd_intf == WCMOD_SPD_40G_KR4) ||
           (spd_intf == WCMOD_SPD_40G_CR4) ||
           (spd_intf == WCMOD_SPD_40G_XLAUI) || (spd_intf == WCMOD_SPD_42G_XLAUI) ||
           (spd_intf == WCMOD_SPD_15750_HI_DXGXS) || (spd_intf == WCMOD_SPD_40G) ||
           (spd_intf == WCMOD_SPD_20000_HI_DXGXS) || (spd_intf == WCMOD_SPD_42G) || 
           (spd_intf == WCMOD_SPD_10000_DXGXS) || (spd_intf == WCMOD_SPD_X2_10000) || 
           (spd_intf == WCMOD_SPD_X2_23000) || (spd_intf == WCMOD_SPD_48G) ||
           (spd_intf == WCMOD_SPD_44G)) {

    data = cntl ? XGXSBLK0_XGXSCONTROL_RLOOP_MASK : 0;
    MODIFY_WC40_XGXSBLK0_XGXSCONTROLr(pc->unit, pc,
                                             data,
                                             XGXSBLK0_XGXSCONTROL_RLOOP_MASK);
    mask = XGXSBLK2_UNICOREMODE10G_UNICOREMODE10GHIG_MASK | 
                XGXSBLK2_UNICOREMODE10G_UNICOREMODE10GCX4_MASK;
    if (cntl) {
        data = (XGXSBLK2_UNICOREMODE10G_UNICOREMODE10GHIG_XGXS_nLQ <<
            XGXSBLK2_UNICOREMODE10G_UNICOREMODE10GHIG_SHIFT) | 
            XGXSBLK2_UNICOREMODE10G_UNICOREMODE10GCX4_XGXS << 
            XGXSBLK2_UNICOREMODE10G_UNICOREMODE10GCX4_SHIFT;
    } else {
        data = (XGXSBLK2_UNICOREMODE10G_UNICOREMODE10GHIG_XGXS_nLQnCC << 
            XGXSBLK2_UNICOREMODE10G_UNICOREMODE10GHIG_SHIFT) | 
            (XGXSBLK2_UNICOREMODE10G_UNICOREMODE10GHIG_XGXG_nCC << 
            XGXSBLK2_UNICOREMODE10G_UNICOREMODE10GCX4_SHIFT);
    }
    MODIFY_WC40_XGXSBLK2_UNICOREMODE10Gr(pc->unit, pc, data, mask);
    mask = 0;

  }else if((spd_intf == WCMOD_SPD_1000_FIBER) || (spd_intf == WCMOD_SPD_10_SGMII)
         || (spd_intf == WCMOD_SPD_100_SGMII) || (spd_intf == WCMOD_SPD_1000_SGMII)){
    data = cntl ? SERDESDIGITAL_CONTROL1000X1_REMOTE_LOOPBACK_MASK : 0;
    MODIFY_WC40_SERDESDIGITAL_CONTROL1000X1r(pc->unit, pc,
                            data,
                            SERDESDIGITAL_CONTROL1000X1_REMOTE_LOOPBACK_MASK);
  }else if((spd_intf == WCMOD_SPD_PCSBYP_6P25G) || (spd_intf == WCMOD_SPD_PCSBYP_10P3125)
         || (spd_intf == WCMOD_SPD_PCSBYP_10P9375) || (spd_intf == WCMOD_SPD_PCSBYP_11P5)
         || (spd_intf == WCMOD_SPD_PCSBYP_12P5) || (spd_intf == WCMOD_SPD_PCSBYP_5P75G)
         || (spd_intf == WCMOD_SPD_PCSBYP_3P125G)){

    lane_0 = cntl & 0x01;
    lane_1 = cntl & 0x02;
    lane_2 = cntl & 0x04;
    lane_3 = cntl & 0x08;

    pc->lane_num_ignore = 1;

    if (lane_0) {
      val  |= (XGXSBLK1_LANECTRL1_TX1G_MODE_LN0_DWSDR_div1
                << XGXSBLK1_LANECTRL1_TX1G_MODE_LN0_SHIFT) |
            (XGXSBLK1_LANECTRL1_RX1G_MODE_LN0_DWSDR_div1
                << XGXSBLK1_LANECTRL1_RX1G_MODE_LN0_SHIFT);
      mask |= XGXSBLK1_LANECTRL1_TX1G_MODE_LN0_MASK |
            XGXSBLK1_LANECTRL1_RX1G_MODE_LN0_MASK;
    }
    if (lane_1) {
      val  |= (XGXSBLK1_LANECTRL1_TX1G_MODE_LN1_DWSDR_div1
                << XGXSBLK1_LANECTRL1_TX1G_MODE_LN1_SHIFT) |
            (XGXSBLK1_LANECTRL1_RX1G_MODE_LN1_DWSDR_div1
                << XGXSBLK1_LANECTRL1_RX1G_MODE_LN1_SHIFT);
      mask |= XGXSBLK1_LANECTRL1_TX1G_MODE_LN1_MASK |
            XGXSBLK1_LANECTRL1_RX1G_MODE_LN1_MASK;
    }
    if (lane_2) {
      val  |= (XGXSBLK1_LANECTRL1_TX1G_MODE_LN2_DWSDR_div1
                << XGXSBLK1_LANECTRL1_TX1G_MODE_LN2_SHIFT) |
            (XGXSBLK1_LANECTRL1_RX1G_MODE_LN2_DWSDR_div1
                << XGXSBLK1_LANECTRL1_RX1G_MODE_LN2_SHIFT);
      mask |= XGXSBLK1_LANECTRL1_TX1G_MODE_LN2_MASK |
            XGXSBLK1_LANECTRL1_RX1G_MODE_LN2_MASK;
    }
    if (lane_3) {
      val  |= (XGXSBLK1_LANECTRL1_TX1G_MODE_LN3_DWSDR_div1
                << XGXSBLK1_LANECTRL1_TX1G_MODE_LN3_SHIFT) |
            (XGXSBLK1_LANECTRL1_RX1G_MODE_LN3_DWSDR_div1
                << XGXSBLK1_LANECTRL1_RX1G_MODE_LN3_SHIFT);
      mask |= XGXSBLK1_LANECTRL1_TX1G_MODE_LN3_MASK |
            XGXSBLK1_LANECTRL1_RX1G_MODE_LN3_MASK;
    }

    MODIFY_WC40_XGXSBLK1_LANECTRL1r(pc->unit, pc, val, mask);

    val = (cntl & 0x0f) << XGXSBLK1_LANECTRL2_RLOOP1G_SHIFT;
    MODIFY_WC40_XGXSBLK1_LANECTRL2r(pc->unit, pc, val, val);

    pc->lane_num_ignore = 0;

  } else if (spd_intf == WCMOD_SPD_100G_CR10) {
       if (cntl) {
           MODIFY_WC40_XGXSBLK8_TX_PI_CONTROL1r(pc->unit, pc, XGXSBLK8_TX_PI_CONTROL1_TX_PI_EN_MASK, 
                                           XGXSBLK8_TX_PI_CONTROL1_TX_PI_EN_MASK);
       } else {
           MODIFY_WC40_XGXSBLK8_TX_PI_CONTROL1r(pc->unit, pc, 0, 
                                           XGXSBLK8_TX_PI_CONTROL1_TX_PI_EN_MASK);
       }

  } else {
      printf("%-22s: No loopback possible for speed:%d",FUNCTION_NAME(), spd_intf);
  }
  return(SOC_E_NONE);
}

/*!
\brief <B>Do not directly call this function.</B>  This is an internal
function, which sets the lane number for subsequent MDIO accesses.

\param  pc handle to current warpcore context (#wcmod_st)

\details
Each warpcore register is addressed by a 32-bit address.  Bits 16 and 17
indicate the particular lane with which the register is associated.  Some
registers have 1 instance for all lanes, while some registers have 1 instance
per lane.  In this later case, then the lane selection bits need to be set in
order to program a register.  This function facilitates programming of
lane-specific registers.  However, currently this function sets bits 18 through
31 to 0 in the process of setting the lane bits.

\returns The value #SOC_E_NONE upon successful completion
*/

int wcmod_aer_lane_select(wcmod_st* pc)         /* AER_LANE_SELECT */
{
  int AER_OFS_STRAP;
  int dxgxs=pc->dxgxs;

  AER_OFS_STRAP = 0;

  pc->lane_num_ignore = 1;
  if (dxgxs & 0x1) {
    /* dual lane mode selects lane_0 and lane_1 */
    WRITE_WC40_AERBLK_AERr(pc->unit, pc, WC_AER_BCST_OFS_STRAP + 1);
  } else if (dxgxs & 0x2) {
    /* dual lane mode selects lane_2 and lane_3 */
    WRITE_WC40_AERBLK_AERr(pc->unit, pc, WC_AER_BCST_OFS_STRAP + 2);
  } else if (pc->lane_select == WCMOD_LANE_BCST) {
    /* broadcast mode */
   /* WRITE_WC40_AERBLK_AERr(pc->unit, pc, WC_AER_BCST_OFS_STRAP); */
    WRITE_WC40_AERBLK_AERr(pc->unit, pc, 0x1f);
  } else {
    WRITE_WC40_AERBLK_AERr(pc->unit, pc, pc->this_lane + AER_OFS_STRAP);
  }
  pc->lane_num_ignore = 0;
  return SOC_E_NONE;
}

/*!
\brief Programs the speed mode for a particular lane

\param  pc handle to current warpcore context (#wcmod_st)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.

\details

This function sets the warpcore into a particular speed mode.  To set the
desired speed, set the following fields of #wcmod_st

\li #wcmod_st::lane_select to the lane number(s)
\li #wcmod_st::spd_intf to a value from the enumerated type #wcmod_spd_intfc_set
\li #wcmod_st::port_type to a type from the enumerated type #wcmod_port_type
\li #wcmod_st::os_type to a type from the enumerated type #wcmod_os_type

Note that legitimate combinations for these fields must be used; otherwise
ill-defined behavior may result.

This function must be called once for combo mode and once per lane for
indepedent lane mode. Only if all lanes are being set to the same
speed-interface can this function be called once for all lanes. Before calling
this function, the PLL sequencer must be disabled via the function
#wcmod_pll_sequencer_control().  After calling this function, the PLL sequencer
must be enabled via the same function.

To program multiple lanes in independent lane mode, it is recommended to
disable the PLL sequencer, call this function once for every lane being
programmed, and then enable the PLL sequencer.

This function modifies the following registers

\li SERDESDIGITAL_MISC1         (0x8308)
\li DIGITAL4_MISC3              (0x833C)
\li COMBO_IEEE0_MIICNTL         (0xFFE0)
\li SERDESDIGITAL_CONTROL1000X1 (0x8300)
\li XGXSBLK6_XGXSX2CONTROL2     (0x8141)
\li XGXSBLK1_LANECTRL1          (0x8016)
\li DSC2B0_DSC_MISC_CTRL0       (0x821E)


The following speeds are not implemented.
\li WCMOD_SPD_10000_DXGXS_SCR
\li WCMOD_SPD_15000
\li WCMOD_SPD_2000
\li WCMOD_SPD_20G_KR2
\li WCMOD_SPD_5000_SINGLE
\li WCMOD_SPD_6364_SINGLE

<B>How to call:</B><br>
<p>Call this function directly or through the function #wcmod_tier1_selector()
with the upper-case string "SET_SPD_INTF".</p>
*/

int wcmod_speed_intf_set(wcmod_st* pc)                  /* SPEED_INTF_SET */
{
  uint16   data16 = 0, mask16;
  uint16   speed_val, speed_mii, mask, this_lane, lane_num_ignore;
  uint16   b6, b5 ;  /* force_speed b5 and b6 */
  wcmod_spd_intfc_set spd_intf;

  spd_intf    = pc->spd_intf;
  this_lane   = pc->this_lane;
  speed_val   = 0;
  speed_mii   = 0;
  mask        =  SERDESDIGITAL_MISC1_FORCE_SPEED_MASK;
  b6          = 0x40 ;
  b5          = 0x20 ;

/* 1: Set ieee_blksel_val = miscControl1B[0] = 1'b1 (miscControl1_A == 16'h0001)
 * 1 not needed for 1000 fiber, but needed later on.
 * 2. Set force_speed[5:0] reg to desired value
 */

  /* this only works for combo mode right now. */
  if (pc->spd_intf > WCMOD_SPD_ILLEGAL) {
    printf("%-22s Error: Bad spd-intf: %d > WCMOD_SPD_ILLEGAL\n",
                                                        FUNCTION_NAME(), pc->spd_intf);
    return SOC_E_ERROR;
  } else {
     if (pc->verbosity > 0) printf("%-22s: %s[%d]\n", FUNCTION_NAME(), e2s_wcmod_spd_intfc_set[pc->spd_intf],
                                        e2n_wcmod_spd_intfc_set[pc->spd_intf]);
  }

  /* first disable pll force bit */
  SOC_IF_ERROR_RETURN(MODIFY_WC40_SERDESDIGITAL_MISC1r(pc->unit, pc, 0x0,
              SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK));   

  if (pc->accAddr != PMA_PMD_SPEED_ENFORCE) {
      SOC_IF_ERROR_RETURN(MODIFY_WC40_SERDESDIGITAL_MISC2r(pc->unit, pc, 0x0,
                  SERDESDIGITAL_MISC2_PMA_PMD_FORCED_SPEED_ENC_EN_MASK)); 
  }
  /* this is only for CD3 */
  SOC_IF_ERROR_RETURN(MODIFY_WC40_DIGITAL4_MISC3r(pc->unit, pc, 0,
                                           DIGITAL4_MISC3_LANEDISABLE_MASK));

  /* disable 100FX auto-detect */
  MODIFY_WC40_FX100_CONTROL1r(pc->unit,pc,0,
                              FX100_CONTROL1_AUTO_DETECT_FX_MODE_MASK |
                              FX100_CONTROL1_ENABLE_MASK);

  /* for warpLite clear force os enable bit */
  if ((pc->model_type == WCMOD_WL_A0) || (pc->model_type == WCMOD_WL_B0)) {
    SOC_IF_ERROR_RETURN(MODIFY_WC40_DIGITAL5_MISC7r(pc->unit, pc, 0, 
                                                    DIGITAL5_MISC7_FORCE_OSCDR_MODE_MASK)); 
  }

  if        (spd_intf == WCMOD_SPD_10_SGMII)   {
      speed_mii = 0; /*MII_CTRL_SS_10; */
  } else if (spd_intf == WCMOD_SPD_100_SGMII)  {
      speed_mii = IEEE0BLK_MIICNTL_MANUAL_SPEED0_BITS <<
          IEEE0BLK_MIICNTL_MANUAL_SPEED0_SHIFT; /* MII_CTRL_SS_100; */
  } else if (spd_intf == WCMOD_SPD_100_FX)     {
      speed_mii = 0; /* MII_CTRL_SS_10 */
  } else if (spd_intf == WCMOD_SPD_1000_SGMII) {
      speed_mii = IEEE0BLK_MIICNTL_MANUAL_SPEED1_BITS <<
          IEEE0BLK_MIICNTL_MANUAL_SPEED1_SHIFT; /* MII_CTRL_SS_1000; */
  } else if (spd_intf == WCMOD_SPD_1000_FIBER) {
      speed_mii = PMD_IEEE0BLK_PMD_IEEECONTROL1_SPEEDSELECTION1_MASK;
  } else if (spd_intf == WCMOD_SPD_2500) {
      speed_val = SERDESDIGITAL_MISC1_FORCE_SPEED_dr_2500BRCM_X1;
  } else if (spd_intf == WCMOD_SPD_5000) {
      speed_val = SERDESDIGITAL_MISC1_FORCE_SPEED_dr_5000BRCM_X4;
  } else if (spd_intf == WCMOD_SPD_6000) {
      speed_val = SERDESDIGITAL_MISC1_FORCE_SPEED_dr_6000BRCM_X4;
  } else if (spd_intf == WCMOD_SPD_10000_HI) {
      speed_val = SERDESDIGITAL_MISC1_FORCE_SPEED_dr_10GHiGig_X4;
  } else if (spd_intf == WCMOD_SPD_10000) {
       if (pc->model_type == WCMOD_XN) {
          speed_val = 0x14;
       } else {
          speed_val = SERDESDIGITAL_MISC1_FORCE_SPEED_dr_10GBASE_CX4;
       }
  } else if (spd_intf == WCMOD_SPD_12000_HI) {
      speed_val = SERDESDIGITAL_MISC1_FORCE_SPEED_dr_12GHiGig_X4;
  } else if (spd_intf == WCMOD_SPD_12500) {
      speed_val = SERDESDIGITAL_MISC1_FORCE_SPEED_dr_12p5GHiGig_X4;
  } else if (spd_intf == WCMOD_SPD_13000) {
      speed_val = SERDESDIGITAL_MISC1_FORCE_SPEED_dr_13GHiGig_X4;
  } else if (spd_intf == WCMOD_SPD_15000) {
      speed_val = SERDESDIGITAL_MISC1_FORCE_SPEED_dr_15GHiGig_X4;
  } else if (spd_intf == WCMOD_SPD_16000) {
      speed_val = SERDESDIGITAL_MISC1_FORCE_SPEED_dr_16GHiGig_X4;
  } else if (spd_intf == WCMOD_SPD_20000) {
      speed_val = SERDESDIGITAL_MISC1_FORCE_SPEED_dr_20GHiGig_X4;
  } else if (spd_intf == WCMOD_SPD_10000_DXGXS) {
      speed_val = b5 | SERDESDIGITAL_MISC1_FORCE_SPEED_dr_10G_DXGXS ;
  } else if (spd_intf == WCMOD_SPD_10000_DXGXS_SCR) {
      speed_val = b5 | SERDESDIGITAL_MISC1_FORCE_SPEED_dr_10G_DXGXS_SCR ;
  } else if (spd_intf == WCMOD_SPD_10500_HI_DXGXS) {
      speed_val = b5 | SERDESDIGITAL_MISC1_FORCE_SPEED_dr_10p5G_HiG_DXGXS ;
  } else if (spd_intf == WCMOD_SPD_10500_DXGXS) {
      speed_val = b5 | SERDESDIGITAL_MISC1_FORCE_SPEED_dr_10p5G_DXGXS;
  } else if (spd_intf == WCMOD_SPD_12773_HI_DXGXS) {
      speed_val = b5 | SERDESDIGITAL_MISC1_FORCE_SPEED_dr_12p773G_HiG_DXGXS ;
  } else if (spd_intf == WCMOD_SPD_12773_DXGXS) {
      speed_val = b5 | SERDESDIGITAL_MISC1_FORCE_SPEED_dr_12p773G_DXGXS ;
  } else if ((spd_intf == WCMOD_SPD_10000_XFI) || (spd_intf == WCMOD_SPD_11P5) || (spd_intf == WCMOD_SPD_12P5))  {
      speed_val = b5 | SERDESDIGITAL_MISC1_FORCE_SPEED_dr_10G_XFI ;
  } else if (spd_intf == WCMOD_SPD_10000_SFI) {
      speed_val = b5 | SERDESDIGITAL_MISC1_FORCE_SPEED_dr_10G_SFI ;
  } else if (spd_intf == WCMOD_SPD_10600_XFI_HG) {
      speed_val = b5 | SERDESDIGITAL_MISC1_FORCE_SPEED_dr_10G_XFI ;  
      /* use SERDESDIGITAL_MISC1_FORCE_SPEED_dr_10G_XFI instead */ 
  } else if (spd_intf == WCMOD_SPD_40G) {
      speed_val = b5 | SERDESDIGITAL_MISC1_FORCE_SPEED_dr_40G_X4 ;
  } else if (spd_intf == WCMOD_SPD_42G) {
      speed_val = b5 | SERDESDIGITAL_MISC1_FORCE_SPEED_dr_40G_X4 ;   
      /* use SERDESDIGITAL_MISC1_FORCE_SPEED_dr_40G_X4 instead */ 
  } else if (spd_intf == WCMOD_SPD_20G_DXGXS) {
      speed_val = b5 | SERDESDIGITAL_MISC1_FORCE_SPEED_dr_20G_DXGXS ;
  } else if (spd_intf == WCMOD_SPD_20G_DXGXS_SCR) {
      speed_val = b5 | SERDESDIGITAL_MISC1_FORCE_SPEED_dr_10G_DXGXS_SCR ;
  } else if (spd_intf == WCMOD_SPD_31500) {
      speed_val = b5 | SERDESDIGITAL_MISC1_FORCE_SPEED_dr_31p5G ;
  } else if (spd_intf == WCMOD_SPD_20000_SCR) {
      speed_val = b5 | SERDESDIGITAL_MISC1_FORCE_SPEED_dr_20G_SCR ;
  } else if (spd_intf == WCMOD_SPD_10000_HI_DXGXS_SCR) {
      speed_val = b5 | SERDESDIGITAL_MISC1_FORCE_SPEED_dr_10G_HiG_DXGXS_SCR ;
  } else if (spd_intf == WCMOD_SPD_R2_12000)  {
      speed_val = b5 | SERDESDIGITAL_MISC1_FORCE_SPEED_dr_12G_R2 ;
  } else if (spd_intf == WCMOD_SPD_X2_10000)  {
      speed_val = b5 | SERDESDIGITAL_MISC1_FORCE_SPEED_dr_10G_X2 ;
  } else if (spd_intf == WCMOD_SPD_X2_23000)  {
      speed_val = b5 | SERDESDIGITAL_MISC1_FORCE_SPEED_dr_10G_X2 ;
  } else if((spd_intf == WCMOD_SPD_40G_KR4)  || (spd_intf == WCMOD_SPD_42G_KR4)
          ||(spd_intf == WCMOD_SPD_40G_XLAUI) ||(spd_intf == WCMOD_SPD_42G_XLAUI)
           || (spd_intf == WCMOD_SPD_48G) || (spd_intf == WCMOD_SPD_44G)) {
      speed_val = b5 | SERDESDIGITAL_MISC1_FORCE_SPEED_dr_40G_KR4 ; 
      /* use SERDESDIGITAL_MISC1_FORCE_SPEED_dr_40G_KR4 for all of those */
  } else if (spd_intf == WCMOD_SPD_40G_CR4) {
      speed_val = b5 | SERDESDIGITAL_MISC1_FORCE_SPEED_dr_40G_CR4 ;
  } else if (spd_intf == WCMOD_SPD_10000_HI_DXGXS) {
      speed_val = SERDESDIGITAL_MISC1_FORCE_SPEED_dr_10G_HiG_DXGXS;
  } else if (spd_intf == WCMOD_SPD_15750_HI_DXGXS)  {
      speed_val = b5 | SERDESDIGITAL_MISC1_FORCE_SPEED_dr_15p75_HiG_DXGXS;
  } else if (spd_intf == WCMOD_SPD_21000) {
      speed_val = SERDESDIGITAL_MISC1_FORCE_SPEED_dr_21GHiGig_X4;
  } else if (spd_intf == WCMOD_SPD_25455) {
      speed_val = SERDESDIGITAL_MISC1_FORCE_SPEED_dr_25p45GHiGig_X4;
  } else if ((spd_intf == WCMOD_SPD_20000_HI_DXGXS)
          || (spd_intf == WCMOD_SPD_21G_HI_DXGXS)) {
      /* 21G is same as 20G, plldiv=70 */   /* 20G MLD */ 
      speed_val = b5 | SERDESDIGITAL_MISC1_FORCE_SPEED_dr_20G_HiG_DXGXS ; 
      /* use SERDESDIGITAL_MISC1_FORCE_SPEED_dr_20G_HiG_DXGXS for both */
  } else if ( (spd_intf == WCMOD_SPD_20G_KR2)) {
      speed_val = b5 | SERDESDIGITAL_MISC1_FORCE_SPEED_dr_20G_KR2 ;
  } else if ( (spd_intf == WCMOD_SPD_20G_CR2)) {
      speed_val = b6 | SERDESDIGITAL_MISC1_FORCE_SPEED_dr_20G_CR2 ;
  } else if (spd_intf == WCMOD_SPD_PCSBYP_3P125G)  {
    if (pc->model_type == WCMOD_WC_C0 || pc->model_type == WCMOD_WL_A0 || TRUE) {
        speed_val = b6 | SERDESDIGITAL_MISC1_FORCE_SPEED_dr_3p125G ;
    } else {
      printf("%-22s: Error: This speed allowed only for C0/WL model type %d\n", FUNCTION_NAME(), pc->model_type);
      return SOC_E_ERROR;
    }
  } else if (spd_intf == WCMOD_SPD_PCSBYP_6P25G)  {
    if (pc->model_type == WCMOD_WC_C0 || pc->model_type == WCMOD_WL_A0 || TRUE) {
      /* for canned speed, first disable pll_force_en and pma_pmd_forced_speed_enc_en */
      MODIFY_WC40_SERDESDIGITAL_MISC1r(pc->unit, pc, 0,
        SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK);
      MODIFY_WC40_SERDESDIGITAL_MISC2r(pc->unit, pc, 0, 
        SERDESDIGITAL_MISC2_PMA_PMD_FORCED_SPEED_ENC_EN_MASK);
      speed_val = b6 | SERDESDIGITAL_MISC1_FORCE_SPEED_dr_6p25G ;
    } else {
      printf("%-22s: Error: This speed allowed only for C0/WL\n", FUNCTION_NAME());
      return SOC_E_ERROR;
    }
  } else if (spd_intf == WCMOD_SPD_PCSBYP_10P3125)  {
    if (pc->model_type == WCMOD_WC_C0 || pc->model_type == WCMOD_WL_A0) {
      /* for canned speed, first disable pll_force_en and pma_pmd_forced_speed_enc_en */
      MODIFY_WC40_SERDESDIGITAL_MISC1r(pc->unit, pc, 0,
        SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK);
      MODIFY_WC40_SERDESDIGITAL_MISC2r(pc->unit, pc, 0, 
        SERDESDIGITAL_MISC2_PMA_PMD_FORCED_SPEED_ENC_EN_MASK);
      /* this set of writes added for 100G only.*/ /* PP */
      WRITE_WC40_XGXSBLK1_LANECTRL0r(pc->unit, pc, 0x0);
      WRITE_WC40_XGXSBLK1_LANECTRL2r(pc->unit, pc, 0x0);
      data16 = XGXSBLK8_TXLNSWAP1_FORCE_GB_BYPASS_EN_MASK;
      mask16 = XGXSBLK8_TXLNSWAP1_FORCE_GB_BYPASS_EN_MASK;
      MODIFY_WC40_XGXSBLK8_TXLNSWAP1r(pc->unit, pc, data16, mask16);
      data16 = (CL72_USERB0_CL72_MISC4_CONTROL_RX_WCLK_20_QUAD_MODE << 
                CL72_USERB0_CL72_MISC4_CONTROL_RX_WCLK_20_MODE_SEL_SHIFT) |
               (CL72_USERB0_CL72_MISC4_CONTROL_RX_WCLK_16_QUAD_MODE <<
                CL72_USERB0_CL72_MISC4_CONTROL_RX_WCLK_16_MODE_SEL_SHIFT) |
               (CL72_USERB0_CL72_MISC4_CONTROL_RX_WCLK33_QUAD_MODE <<
                CL72_USERB0_CL72_MISC4_CONTROL_RX_WCLK33_MODE_SEL_SHIFT);
      mask16 = CL72_USERB0_CL72_MISC4_CONTROL_RX_WCLK_20_MODE_SEL_MASK |
               CL72_USERB0_CL72_MISC4_CONTROL_RX_WCLK_16_MODE_SEL_MASK |
               CL72_USERB0_CL72_MISC4_CONTROL_RX_WCLK33_MODE_SEL_MASK;
      MODIFY_WC40_CL72_USERB0_CL72_MISC4_CONTROLr(pc->unit,pc,data16,mask16);
      data16 = XGXSBLK6_XGXSX2CONTROL2_100G_BIT_MASK;
      mask16 = XGXSBLK6_XGXSX2CONTROL2_100G_BIT_MASK;
      MODIFY_WC40_XGXSBLK6_XGXSX2CONTROL2r(pc->unit, pc, data16, mask16);
      speed_val = b6 | SERDESDIGITAL_MISC1_FORCE_SPEED_dr_10p3125G ;
    }
  } else if (spd_intf == WCMOD_SPD_PCSBYP_10P9375)  {
      if (pc->model_type == WCMOD_WC_C0 || pc->model_type == WCMOD_WL_A0 || TRUE) {
          /* for canned speed, first disable pll_force_en and pma_pmd_forced_speed_enc_en */
          MODIFY_WC40_SERDESDIGITAL_MISC1r(pc->unit, pc, 0,
            SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK);
          MODIFY_WC40_SERDESDIGITAL_MISC2r(pc->unit, pc, 0, 
            SERDESDIGITAL_MISC2_PMA_PMD_FORCED_SPEED_ENC_EN_MASK);
          speed_val = b6 | SERDESDIGITAL_MISC1_FORCE_SPEED_dr_10p9375G ;
      } else {
          printf("%-22s: Error: This speed allowed only for C0/WL\n", FUNCTION_NAME());
          return SOC_E_ERROR;
      }
  } else if ((spd_intf==WCMOD_SPD_PCSBYP_11P5) ||
             (spd_intf==WCMOD_SPD_PCSBYP_5P75G))  {
      if (pc->model_type == WCMOD_WC_C0 || pc->model_type == WCMOD_WL_A0 || TRUE) {
          /* for canned speed, first disable pll_force_en and pma_pmd_forced_speed_enc_en */
          MODIFY_WC40_SERDESDIGITAL_MISC1r(pc->unit, pc, 0,
            SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK);
          MODIFY_WC40_SERDESDIGITAL_MISC2r(pc->unit, pc, 0, 
            SERDESDIGITAL_MISC2_PMA_PMD_FORCED_SPEED_ENC_EN_MASK);
          speed_val = b6 | SERDESDIGITAL_MISC1_FORCE_SPEED_dr_11p5G ;   
          /* use SERDESDIGITAL_MISC1_FORCE_SPEED_dr_11p5G for both speed */
      } else {
          printf("%-22s: Error: This speed allowed only for C0/WL\n", FUNCTION_NAME());
          return SOC_E_ERROR;
      }
  } else if (spd_intf==WCMOD_SPD_PCSBYP_12P5) {
      if (pc->model_type == WCMOD_WC_C0 || pc->model_type == WCMOD_WL_A0 || TRUE) {
          /* for canned speed, first disable pll_force_en and pma_pmd_forced_speed_enc_en */
          MODIFY_WC40_SERDESDIGITAL_MISC1r(pc->unit, pc, 0,
            SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK);
          MODIFY_WC40_SERDESDIGITAL_MISC2r(pc->unit, pc, 0, 
            SERDESDIGITAL_MISC2_PMA_PMD_FORCED_SPEED_ENC_EN_MASK);
          speed_val = b6 | SERDESDIGITAL_MISC1_FORCE_SPEED_dr_12p5G ;
      } else {
          printf("%-22s: Error: This speed allowed only for C0/WL\n", FUNCTION_NAME());
          return SOC_E_ERROR;
      }
  } else if ((spd_intf==WCMOD_SPD_PCSBYP_11P5) ||
             (spd_intf==WCMOD_SPD_PCSBYP_5P75G))  {
      if (pc->model_type == WCMOD_WC_C0 || pc->model_type == WCMOD_WL_A0 || TRUE) {
          /* for canned speed, first disable pll_force_en and pma_pmd_forced_speed_enc_en */
          MODIFY_WC40_SERDESDIGITAL_MISC1r(pc->unit, pc, 0,
            SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK);
          MODIFY_WC40_SERDESDIGITAL_MISC2r(pc->unit, pc, 0, 
            SERDESDIGITAL_MISC2_PMA_PMD_FORCED_SPEED_ENC_EN_MASK);
          speed_val = b6 | SERDESDIGITAL_MISC1_FORCE_SPEED_dr_11p5G ;
          /* use SERDESDIGITAL_MISC1_FORCE_SPEED_dr_11p5G for both speed */
      } else {
       printf("%-22s: Error: This speed allowed only for C0/WL\n", FUNCTION_NAME());
       return SOC_E_ERROR;
      }
  } else if (spd_intf == WCMOD_SPD_CUSTOM_BYPASS){
      if (pc->model_type == WCMOD_WC_C0 || pc->model_type == WCMOD_WL_A0 || TRUE) {
          speed_val = b6 | SERDESDIGITAL_MISC1_FORCE_SPEED_dr_pcs_bypass ;
      } else {
          printf("%-22s: Error: This speed allowed only for C0/WL\n", FUNCTION_NAME());
          return SOC_E_ERROR;
      }
  }
  if (spd_intf == WCMOD_SPD_1000_FIBER) {
    data16 = SERDESDIGITAL_CONTROL1000X1_COMMA_DET_EN_MASK |
             SERDESDIGITAL_CONTROL1000X1_CRC_CHECKER_DISABLE_MASK |
             SERDESDIGITAL_CONTROL1000X1_FIBER_MODE_1000X_MASK;
    mask16 = SERDESDIGITAL_CONTROL1000X1_COMMA_DET_EN_MASK | 
             SERDESDIGITAL_CONTROL1000X1_CRC_CHECKER_DISABLE_MASK |
             SERDESDIGITAL_CONTROL1000X1_FIBER_MODE_1000X_MASK;
    MODIFY_WC40_SERDESDIGITAL_CONTROL1000X1r(pc->unit, pc, data16, mask16);
  } else if ((spd_intf==WCMOD_SPD_10_SGMII) ||
             (spd_intf == WCMOD_SPD_100_SGMII) ||
             (spd_intf == WCMOD_SPD_1000_SGMII)) {
    data16 = SERDESDIGITAL_CONTROL1000X1_COMMA_DET_EN_MASK |
             SERDESDIGITAL_CONTROL1000X1_CRC_CHECKER_DISABLE_MASK;
    mask16 = SERDESDIGITAL_CONTROL1000X1_COMMA_DET_EN_MASK | 
             SERDESDIGITAL_CONTROL1000X1_CRC_CHECKER_DISABLE_MASK;
    MODIFY_WC40_SERDESDIGITAL_CONTROL1000X1r(pc->unit, pc, data16, mask16);
    data16 = 0;
    mask16 = SERDESDIGITAL_CONTROL1000X1_FIBER_MODE_1000X_MASK;
    MODIFY_WC40_SERDESDIGITAL_CONTROL1000X1r(pc->unit, pc, data16, mask16);
  }
  if (spd_intf == WCMOD_SPD_100_FX) {

    /* disable 100FX auto-detect */
    MODIFY_WC40_FX100_CONTROL1r(pc->unit,pc,0,
                              FX100_CONTROL1_AUTO_DETECT_FX_MODE_MASK);

    /* disable 100FX idle detect */
    SOC_IF_ERROR_RETURN
            (MODIFY_WC40_FX100_CONTROL3r(pc->unit,pc,
                              FX100_CONTROL3_CORRELATOR_DISABLE_MASK,
                              FX100_CONTROL3_CORRELATOR_DISABLE_MASK));

    /* fiber mode 100fx, enable */
    MODIFY_WC40_FX100_CONTROL1r(pc->unit,pc,
                      FX100_CONTROL1_FAR_END_FAULT_EN_MASK |
                      FX100_CONTROL1_ENABLE_MASK,
                      FX100_CONTROL1_FAR_END_FAULT_EN_MASK |
                      FX100_CONTROL1_ENABLE_MASK);

    /* enable 100fx extended packet size */
    MODIFY_WC40_FX100_CONTROL2r(pc->unit,pc,
                    FX100_CONTROL2_EXTEND_PKT_SIZE_MASK,
                    FX100_CONTROL2_EXTEND_PKT_SIZE_MASK);
  }

  if (pc->port_type == WCMOD_COMBO) { /*************** COMBO *****************/
    SOC_IF_ERROR_RETURN(MODIFY_WC40_SERDESDIGITAL_MISC1r(pc->unit,pc,speed_val,
                                        SERDESDIGITAL_MISC1_FORCE_SPEED_MASK));

    if (pc->model_type != WCMOD_XN) {
    SOC_IF_ERROR_RETURN(MODIFY_WC40_DIGITAL4_MISC3r(pc->unit, pc,
                      (speed_val & 0x20)?  DIGITAL4_MISC3_FORCE_SPEED_B5_MASK:0,
                      DIGITAL4_MISC3_FORCE_SPEED_B5_MASK));
    }

    /* if (pc->model_type == WCMOD_XN) {
      SOC_IF_ERROR_RETURN(MODIFY_WC40_IEEE0BLK_MIICNTLr(pc->unit, pc, speed_mii,
                                  (IEEE0BLK_MIICNTL_MANUAL_SPEED1_MASK |
                                   IEEE0BLK_MIICNTL_MANUAL_SPEED0_MASK)));
    } else */{
     SOC_IF_ERROR_RETURN(MODIFY_WC40_COMBO_IEEE0_MIICNTLr(pc->unit,pc,speed_mii,
                                  (COMBO_IEEE0_MIICNTL_MANUAL_SPEED1_MASK |
                                   COMBO_IEEE0_MIICNTL_MANUAL_SPEED0_MASK)));
    }
    /* set sgmii mode */
    if ((spd_intf == WCMOD_SPD_10_SGMII) || (spd_intf == WCMOD_SPD_100_SGMII) ||
        (spd_intf == WCMOD_SPD_1000_SGMII)) {
      SOC_IF_ERROR_RETURN(MODIFY_WC40_SERDESDIGITAL_CONTROL1000X1r(pc->unit,pc,
                         0, SERDESDIGITAL_CONTROL1000X1_FIBER_MODE_1000X_MASK));
    }


    if (((spd_intf == WCMOD_SPD_1000_SGMII) || (spd_intf == WCMOD_SPD_1000_FIBER)) &&
        (pc->model_type != WCMOD_QS_A0)){
      if (pc->model_type == WCMOD_XN) {
    data16 = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_BITS <<
                 SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_SHIFT |
                 SERDESDIGITAL_MISC1_REFCLK_SEL_clk_156p25MHz <<
                 SERDESDIGITAL_MISC1_REFCLK_SEL_SHIFT |
                 SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div16_XENIA <<
                 SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT;
      } else {
    data16 = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_BITS <<
                 SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_SHIFT |
                 SERDESDIGITAL_MISC1_REFCLK_SEL_clk_156p25MHz <<
                 SERDESDIGITAL_MISC1_REFCLK_SEL_SHIFT |
                 SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div40 <<
                 SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT;
      }
      mask = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
             SERDESDIGITAL_MISC1_REFCLK_SEL_MASK |
             SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_MASK;

      SOC_IF_ERROR_RETURN(MODIFY_WC40_SERDESDIGITAL_MISC1r(pc->unit, pc,
                                                                data16, mask));
    } /* 1G SGMII or fiber */

    /*added the support for 2.5G QSGMII support */
    if (((spd_intf == WCMOD_SPD_2500) || (spd_intf == WCMOD_SPD_1000_FIBER)) && (pc->model_type == WCMOD_QS_A0)) {
	  int tmp_lane = pc->this_lane;
	  int i = 0;
      uint16 tmp_reg_val;

      /*first read the speed val on lane 0 */
      pc->this_lane = 0;
      SOC_IF_ERROR_RETURN(READ_WC40_SERDESDIGITAL_MISC1r(pc->unit, pc, &tmp_reg_val));
      if ((tmp_reg_val & SERDESDIGITAL_MISC1_FORCE_SPEED_MASK) != speed_val) {
	      for (i = 0; i < 2; i++) {
            pc->this_lane = i * 4;
            /* pc->lane_select = WCMOD_LANE_BCST; */ /* go to broadcast mode */
            /*SOC_IF_ERROR_RETURN(MODIFY_WC40_SERDESDIGITAL_MISC1r(pc->unit,pc,0,
                                        SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK)); */

            SOC_IF_ERROR_RETURN(MODIFY_WC40_SERDESDIGITAL_MISC1r(pc->unit,pc,speed_val,
                                        SERDESDIGITAL_MISC1_FORCE_SPEED_MASK));

            SOC_IF_ERROR_RETURN(MODIFY_WC40_COMBO_IEEE0_MIICNTLr(pc->unit,pc,speed_mii,
                                  (COMBO_IEEE0_MIICNTL_MANUAL_SPEED1_MASK |
                                   COMBO_IEEE0_MIICNTL_MANUAL_SPEED0_MASK)));
            /*disable the QSGMII mode for both tx/rx only lane 0 or lane 4 is valid for this write*/
            SOC_IF_ERROR_RETURN(MODIFY_WC40_XGXSBLK1_LANECTRL0r(pc->unit, pc, 0x0, 0xff00));
            if ((spd_intf == WCMOD_SPD_2500) || (spd_intf == WCMOD_SPD_1000_FIBER)) {
             /* enable the fiber mode also */
	            SOC_IF_ERROR_RETURN(MODIFY_WC40_SERDESDIGITAL_CONTROL1000X1r(pc->unit, pc, 0x1,
                                                                     SERDESDIGITAL_CONTROL1000X1_FIBER_MODE_1000X_MASK));
            }
          }
          /* restorre this lane  */
	      pc->this_lane =tmp_lane;
      } else {
         pc->this_lane =tmp_lane;
         SOC_IF_ERROR_RETURN(MODIFY_WC40_COMBO_IEEE0_MIICNTLr(pc->unit,pc,speed_mii,
                                              (COMBO_IEEE0_MIICNTL_MANUAL_SPEED1_MASK |
                                               COMBO_IEEE0_MIICNTL_MANUAL_SPEED0_MASK)));
      }

    }

    /*added the support for 10M/100M/1000M sgmii mode on QSGMII support */
    if (((spd_intf == WCMOD_SPD_10_SGMII) || (spd_intf == WCMOD_SPD_100_SGMII) ||
        (spd_intf == WCMOD_SPD_1000_SGMII)) && (pc->model_type == WCMOD_QS_A0)) {
	  int tmp_lane = pc->this_lane;
	  int i = 0;
      uint16 tmp_reg_val;

      /*first read the speed val on lane 0 */
      pc->this_lane = 0;
      SOC_IF_ERROR_RETURN(READ_WC40_SERDESDIGITAL_MISC1r(pc->unit, pc, &tmp_reg_val));
      if ((tmp_reg_val & SERDESDIGITAL_MISC1_FORCE_SPEED_MASK) != speed_val) {
          /* need change the speed id for both lane 0 and lane 4 */
	      for (i = 0; i < 2; i++) {
		    pc->this_lane = i * 4;
		    SOC_IF_ERROR_RETURN(MODIFY_WC40_SERDESDIGITAL_MISC1r(pc->unit,pc,speed_val,
                                            SERDESDIGITAL_MISC1_FORCE_SPEED_MASK));

          /*disable the QSGMII mode for both tx/rx only lane 0 or lane 4 is valid for this write*/
		    SOC_IF_ERROR_RETURN(MODIFY_WC40_XGXSBLK1_LANECTRL0r(pc->unit, pc, 0x0, 0xff00));

		    SOC_IF_ERROR_RETURN(MODIFY_WC40_COMBO_IEEE0_MIICNTLr(pc->unit,pc,speed_mii,
                                      (COMBO_IEEE0_MIICNTL_MANUAL_SPEED1_MASK |
                                       COMBO_IEEE0_MIICNTL_MANUAL_SPEED0_MASK)));
          }
      } else {
	      pc->this_lane = tmp_lane;
          SOC_IF_ERROR_RETURN(MODIFY_WC40_COMBO_IEEE0_MIICNTLr(pc->unit,pc,speed_mii,
                                      (COMBO_IEEE0_MIICNTL_MANUAL_SPEED1_MASK |
                                       COMBO_IEEE0_MIICNTL_MANUAL_SPEED0_MASK)));
      }
	  pc->this_lane = tmp_lane;
	  SOC_IF_ERROR_RETURN(MODIFY_WC40_SERDESDIGITAL_CONTROL1000X1r(pc->unit, pc, 0x0,
                                                         SERDESDIGITAL_CONTROL1000X1_FIBER_MODE_1000X_MASK));
    }

    /* Force pll mode = a for all lanes in XLAUI mode */
    if (   (spd_intf == WCMOD_SPD_42G_XLAUI) || (spd_intf == WCMOD_SPD_40G_KR4)
        || (spd_intf == WCMOD_SPD_40G_XLAUI) || (spd_intf == WCMOD_SPD_40G)) {
      lane_num_ignore = pc->lane_num_ignore; /* save and restore it below */
      pc->lane_num_ignore = 1;

      data16 = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
               SERDESDIGITAL_MISC1_REFCLK_SEL_clk_156p25MHz <<
                                        SERDESDIGITAL_MISC1_REFCLK_SEL_SHIFT |
               SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div66 <<
                                  SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT;

      mask = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
             SERDESDIGITAL_MISC1_REFCLK_SEL_MASK |
             SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_MASK;

      /*
       * COVERITY
       *
       * It is kept intentionally as a defensive default for future development.
       */
      /* coverity[end_of_scope] */
      SOC_IF_ERROR_RETURN(MODIFY_WC40_SERDESDIGITAL_MISC1r(pc->unit, pc,
                                                                data16, mask));
      pc->lane_num_ignore = lane_num_ignore; /* restore */
    }

    if ((spd_intf == WCMOD_SPD_40G_XLAUI) || (spd_intf == WCMOD_SPD_42G_XLAUI)) {
      data16 = UC_INFO_B1_FIRMWARE_MODE_LN3_MODE_XLAUI << UC_INFO_B1_FIRMWARE_MODE_LN3_MODE_SHIFT | 
               UC_INFO_B1_FIRMWARE_MODE_LN2_MODE_XLAUI << UC_INFO_B1_FIRMWARE_MODE_LN2_MODE_SHIFT | 
               UC_INFO_B1_FIRMWARE_MODE_LN1_MODE_XLAUI << UC_INFO_B1_FIRMWARE_MODE_LN1_MODE_SHIFT | 
               UC_INFO_B1_FIRMWARE_MODE_LN0_MODE_XLAUI;
      WRITE_WC40_UC_INFO_B1_FIRMWARE_MODEr(pc->unit, pc, data16);
    }

    /* set divider for 42G kr4 and 42G XLAUI */
    if ((spd_intf == WCMOD_SPD_42G_KR4) || (spd_intf == WCMOD_SPD_42G_XLAUI) ||
        (spd_intf == WCMOD_SPD_42G)) {
      data16 = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
               SERDESDIGITAL_MISC1_REFCLK_SEL_clk_156p25MHz <<
                                        SERDESDIGITAL_MISC1_REFCLK_SEL_SHIFT |
               SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div70 <<
                                  SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT;
      mask = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
             SERDESDIGITAL_MISC1_REFCLK_SEL_MASK |
             SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_MASK;

      SOC_IF_ERROR_RETURN (MODIFY_WC40_SERDESDIGITAL_MISC1r(pc->unit, pc,
                                                                data16, mask));
    }
    /* set divider for 48G XLAUI */
    if (spd_intf == WCMOD_SPD_48G) {
      data16 = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
               SERDESDIGITAL_MISC1_REFCLK_SEL_clk_156p25MHz <<
                                        SERDESDIGITAL_MISC1_REFCLK_SEL_SHIFT |
               SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div80 <<
                                  SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT;
      mask = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
             SERDESDIGITAL_MISC1_REFCLK_SEL_MASK |
             SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_MASK;

      SOC_IF_ERROR_RETURN (MODIFY_WC40_SERDESDIGITAL_MISC1r(pc->unit, pc,
                                                                data16, mask));
    }
    /* set divider for 44G XLAUI */
    if (spd_intf == WCMOD_SPD_44G) {
      data16 = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
               SERDESDIGITAL_MISC1_REFCLK_SEL_clk_156p25MHz <<
                                        SERDESDIGITAL_MISC1_REFCLK_SEL_SHIFT |
               SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div92 <<
                                  SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT;
      mask = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
             SERDESDIGITAL_MISC1_REFCLK_SEL_MASK |
             SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_MASK;

      SOC_IF_ERROR_RETURN (MODIFY_WC40_SERDESDIGITAL_MISC1r(pc->unit, pc,
                                                                data16, mask));
    }

    if (spd_intf == WCMOD_SPD_10600_XFI_HG) { /* PP, need for combo mode */
      data16 = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
               SERDESDIGITAL_MISC1_REFCLK_SEL_clk_156p25MHz <<
                                        SERDESDIGITAL_MISC1_REFCLK_SEL_SHIFT |
               SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div70 <<
                                  SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT;

      mask = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
             SERDESDIGITAL_MISC1_REFCLK_SEL_MASK |
             SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_MASK;

      SOC_IF_ERROR_RETURN (MODIFY_WC40_SERDESDIGITAL_MISC1r(pc->unit, pc,
                                                                 data16, mask));
    }

    /* in XFI mode set 40-bit interface */
    if ((spd_intf == WCMOD_SPD_10000_XFI) || (spd_intf == WCMOD_SPD_10600_XFI_HG) ||
       (spd_intf == WCMOD_SPD_10000_SFI) || (spd_intf == WCMOD_SPD_11P5) ||
       (spd_intf == WCMOD_SPD_12P5)) {
      MODIFY_WC40_DIGITAL4_MISC3r(pc->unit, pc,DIGITAL4_MISC3_IND_40BITIF_MASK,
                                               DIGITAL4_MISC3_IND_40BITIF_MASK);
    }

    /* When speed is speed_12000_hi, speed_25455, speed_31520, speed_40g the */
    /* txcko_div is set to 1'b0 (i.e. xgxsControl[0] = 1'b0) */

    if ((spd_intf == WCMOD_SPD_10000_HI) || (spd_intf == WCMOD_SPD_10000) ||
        (spd_intf == WCMOD_SPD_12000_HI) || (spd_intf == WCMOD_SPD_13000) ||
        (spd_intf == WCMOD_SPD_16000) || (spd_intf == WCMOD_SPD_20000)) {
      /* Xenia needs to have clock divided by 2 for 10G Xaui */
#ifdef INCLUDE_XGXS_WCMOD
      if ((spd_intf == WCMOD_SPD_10000) &&
          (getGenericModelType(pc->model_type) == WCMOD_XN)) {
        /* SOC_IF_ERROR_RETURN(MODIFY_WC40_XGXSBLK0_XGXSCONTROLr(pc->unit, pc,
                                          XGXSBLK0_XGXSCONTROL_TXCKO_DIV_MASK,
            XGXSBLK0_XGXSCONTROL_TXCKO_DIV_MASK)); */
      } else {
        SOC_IF_ERROR_RETURN(MODIFY_WC40_XGXSBLK0_XGXSCONTROLr(pc->unit, pc,0,
                                        XGXSBLK0_XGXSCONTROL_TXCKO_DIV_MASK));
      }
#endif
    }

    /*--------------------------------------------------------------------------
     * The following code configures the 64B/66B decoder when speed is forced
     * to speed_25455 or speed_12773_dxgxs or speed_31500 or speed_40g.
     *------------------------------------------------------------------------*/

    if ((spd_intf == WCMOD_SPD_25455) || (spd_intf == WCMOD_SPD_15750_HI_DXGXS) ||
        (spd_intf == WCMOD_SPD_40G)  || (spd_intf == WCMOD_SPD_20000_HI_DXGXS) ||
        (spd_intf == WCMOD_SPD_20G_DXGXS ) || (spd_intf == WCMOD_SPD_44G) ||
        (spd_intf == WCMOD_SPD_42G)  || (spd_intf == WCMOD_SPD_48G) ||
        (spd_intf == WCMOD_SPD_12773_HI_DXGXS) || (spd_intf == WCMOD_SPD_12773_DXGXS) || 
        (spd_intf == WCMOD_SPD_31500) || (spd_intf == WCMOD_SPD_21G_HI_DXGXS)) {

#ifdef _SDK_WCMOD_
      int i;
      /* temp work around for writing to all lanes */
      for (i=0; i<4; i++) {
        pc->this_lane = i;
        WRITE_WC40_RX66_SCW0r(pc->unit, pc, 0xE070);
        WRITE_WC40_RX66_SCW1r(pc->unit, pc, 0xC0D0);
        WRITE_WC40_RX66_SCW2r(pc->unit, pc, 0xA0B0);
        WRITE_WC40_RX66_SCW3r(pc->unit, pc, 0x8090);
        WRITE_WC40_RX66_SCW0_MASKr(pc->unit, pc, 0xF0F0);
        WRITE_WC40_RX66_SCW1_MASKr(pc->unit, pc, 0xF0F0);
        WRITE_WC40_RX66_SCW2_MASKr(pc->unit, pc, 0xF0F0);
        WRITE_WC40_RX66_SCW3_MASKr(pc->unit, pc, 0xF0F0);
      }
      pc->this_lane = this_lane ; 
#else

      SOC_IF_ERROR_RETURN
        (WRITE_WC40_AERBLK_AERr(pc->unit, pc, pc->aer_bcst_ofs_strap));

      WRITE_WC40_RX66_SCW0r(pc->unit, pc, 0xE070);
      WRITE_WC40_RX66_SCW1r(pc->unit, pc, 0xC0D0);
      WRITE_WC40_RX66_SCW2r(pc->unit, pc, 0xA0B0);
      WRITE_WC40_RX66_SCW3r(pc->unit, pc, 0x8090);
      WRITE_WC40_RX66_SCW0_MASKr(pc->unit, pc, 0xF0F0);
      WRITE_WC40_RX66_SCW1_MASKr(pc->unit, pc, 0xF0F0);
      WRITE_WC40_RX66_SCW2_MASKr(pc->unit, pc, 0xF0F0);
      WRITE_WC40_RX66_SCW3_MASKr(pc->unit, pc, 0xF0F0);
#endif

      SOC_IF_ERROR_RETURN (WRITE_WC40_AERBLK_AERr(pc->unit, pc, 0));
    }
    /* set BRCM 31G control */
    if (spd_intf == WCMOD_SPD_31500) {
#ifdef _SDK_WCMOD_
        int i;
        /* temp work around for writing to all lanes */
        for (i=0; i<4; i++) {
            pc->this_lane = i;
            SOC_IF_ERROR_RETURN
                  (MODIFY_WC40_DIGITAL5_MISC6r(pc->unit, pc, 
                           DIGITAL5_MISC6_USE_BRCM6466_31500_CYA_MASK, 
                           DIGITAL5_MISC6_USE_BRCM6466_31500_CYA_MASK));
        }
        pc->this_lane = this_lane ;  
#endif
    }
    /* set the fiber mode (bit 0) in CONTROL11000X1 reg. */
  } else if (pc->port_type == WCMOD_INDEPENDENT) { /******* INDEPENDENT *******/

#ifdef INCLUDE_XGXS_WCMOD
    if ( pc->model_type == WCMOD_XN &&  _wcmod_int_st.asymmetric_mode) {
      _wcmod_asymmetric_mode(pc);
    }

    /* clear the force pll mode for XENIA core */
    if ((pc->model_type == WCMOD_XN)) {
        /* first program the 0x8308 to 0x7110 */
        MODIFY_WC40_SERDESDIGITAL_MISC1r(pc->unit, pc, 0, 
                            SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK); 
        /* next disable the asymmetric bit in 0x8169 */
        MODIFY_WC40_XGXSBLK8_TXLNSWAP1r(pc->unit, pc, 0, 
                            XGXSBLK8_TXLNSWAP1_ASYMMETRIC_MODE_EN_MASK);
    }
#endif

    /*work around to fix Arad odd lane link flapping */
    if ((spd_intf == WCMOD_SPD_1000_SGMII) || (spd_intf == WCMOD_SPD_1000_FIBER) || 
          (spd_intf == WCMOD_SPD_2500)) {
        if (pc->model_type == WCMOD_WC_C0 || pc->model_type == WCMOD_WL_A0) {
          SOC_IF_ERROR_RETURN(MODIFY_WC40_DIGITAL4_MISC5r(pc->unit, pc,
                                                       (speed_val & 0xc0), 0xc0));
        }  
        SOC_IF_ERROR_RETURN(MODIFY_WC40_DIGITAL4_MISC3r(pc->unit, pc,
                          (speed_val & 0x20)?  DIGITAL4_MISC3_FORCE_SPEED_B5_MASK:0,
                           DIGITAL4_MISC3_FORCE_SPEED_B5_MASK)); 
        SOC_IF_ERROR_RETURN( MODIFY_WC40_SERDESDIGITAL_MISC1r(pc->unit, pc,
                                speed_val, SERDESDIGITAL_MISC1_FORCE_SPEED_MASK));
    } else {
        SOC_IF_ERROR_RETURN( MODIFY_WC40_SERDESDIGITAL_MISC1r(pc->unit, pc,
                                speed_val, SERDESDIGITAL_MISC1_FORCE_SPEED_MASK));

        SOC_IF_ERROR_RETURN(MODIFY_WC40_DIGITAL4_MISC3r(pc->unit, pc,
                          (speed_val & 0x20)?  DIGITAL4_MISC3_FORCE_SPEED_B5_MASK:0,
                           DIGITAL4_MISC3_FORCE_SPEED_B5_MASK)); 
        if (pc->model_type == WCMOD_WC_C0 || pc->model_type == WCMOD_WL_A0) {
          SOC_IF_ERROR_RETURN(MODIFY_WC40_DIGITAL4_MISC5r(pc->unit, pc,
                                                       (speed_val & 0xc0), 0xc0));
        } 
    }  

    /*added the support of HX4 QUAD SGMII 1G  mode */
    if (pc->model_type == WCMOD_QS) {
      /* disbale the fiber mode also */
      SOC_IF_ERROR_RETURN(MODIFY_WC40_SERDESDIGITAL_CONTROL1000X1r(pc->unit, pc, 0x0,
                                                             SERDESDIGITAL_CONTROL1000X1_FIBER_MODE_1000X_MASK));   
    }

    /*added the support of HX4 QSGMII 1G  mode */
    if ((spd_intf == WCMOD_SPD_1000_SGMII) && ((pc->model_type == WCMOD_QS_A0) ||(pc->model_type == WCMOD_QS_B0))) {
      /* disbale the fiber mode also */
      SOC_IF_ERROR_RETURN(MODIFY_WC40_SERDESDIGITAL_CONTROL1000X1r(pc->unit, pc, 0x0,
                                                             SERDESDIGITAL_CONTROL1000X1_FIBER_MODE_1000X_MASK));   
      SOC_IF_ERROR_RETURN(MODIFY_WC40_COMBO_IEEE0_MIICNTLr(pc->unit,pc,speed_mii,
                                  (COMBO_IEEE0_MIICNTL_MANUAL_SPEED1_MASK |
                                   COMBO_IEEE0_MIICNTL_MANUAL_SPEED0_MASK)));
    }

    /*QSGMII block has not analog, just PCS layer  */
    if (spd_intf == WCMOD_SPD_1G_QSGMII) {
      /* Speed added to hit each QSGMII since its register for broadcasting is different. */

      data16 = SERDESDIGITAL_CONTROL1000X1_FIBER_MODE_1000X_BITS  << SERDESDIGITAL_CONTROL1000X1_FIBER_MODE_1000X_ALIGN |
        SERDESDIGITAL_CONTROL1000X1_DISABLE_SIGNAL_DETECT_FILTER_BITS  << SERDESDIGITAL_CONTROL1000X1_DISABLE_SIGNAL_DETECT_FILTER_SHIFT |
        0 << SERDESDIGITAL_CONTROL1000X1_CRC_CHECKER_DISABLE_SHIFT;
      mask16 =  SERDESDIGITAL_CONTROL1000X1_DISABLE_SIGNAL_DETECT_FILTER_MASK | SERDESDIGITAL_CONTROL1000X1_DISABLE_SIGNAL_DETECT_FILTER_MASK |
        SERDESDIGITAL_CONTROL1000X1_CRC_CHECKER_DISABLE_MASK;      
      SOC_IF_ERROR_RETURN
         (MODIFY_WC40_SERDESDIGITAL_CONTROL1000X1r(pc->unit, pc, data16, mask16));

      data16 = IEEE0BLK_AUTONEGADV_FULL_DUPLEX_BITS << IEEE0BLK_AUTONEGADV_FULL_DUPLEX_SHIFT;
      mask16 = IEEE0BLK_AUTONEGADV_FULL_DUPLEX_MASK;  
      SOC_IF_ERROR_RETURN
        (MODIFY_WC40_IEEE0BLK_AUTONEGADVr(pc->unit, pc, data16, mask16));

      SOC_IF_ERROR_RETURN (MODIFY_WC40_XGXSBLK0_XGXSCONTROLr(pc->unit, pc,
                                                             (XGXSBLK0_XGXSCONTROL_MODE_10G_IndLane_OS6 <<
                                                              XGXSBLK0_XGXSCONTROL_MODE_10G_SHIFT),
                                                             XGXSBLK0_XGXSCONTROL_MODE_10G_MASK));

      SOC_IF_ERROR_RETURN(MODIFY_WC40_IEEE0BLK_MIICNTLr(pc->unit, pc, 
                                                        IEEE0BLK_MIICNTL_MANUAL_SPEED1_BITS <<  IEEE0BLK_MIICNTL_MANUAL_SPEED1_SHIFT |
                                                        IEEE0BLK_MIICNTL_FULL_DUPLEX_BITS  << IEEE0BLK_MIICNTL_FULL_DUPLEX_SHIFT |
                                                        IEEE0BLK_MIICNTL_AUTONEG_ENABLE_BITS << IEEE0BLK_MIICNTL_PWRDWN_SW_SHIFT |
                                                        0 << IEEE0BLK_MIICNTL_MANUAL_SPEED0_SHIFT ,
                                                        (IEEE0BLK_MIICNTL_MANUAL_SPEED1_MASK | IEEE0BLK_MIICNTL_FULL_DUPLEX_MASK |
                                                         IEEE0BLK_MIICNTL_AUTONEG_ENABLE_MASK | IEEE0BLK_MIICNTL_MANUAL_SPEED0_MASK)));
    }

    MODIFY_WC40_COMBO_IEEE0_MIICNTLr(pc->unit, pc, speed_mii,
                                (COMBO_IEEE0_MIICNTL_MANUAL_SPEED1_MASK |
                                  COMBO_IEEE0_MIICNTL_MANUAL_SPEED0_MASK));

    /* turn on cl-36 pcs (for xenia, may be implemented via straps in WC) */
    if ((spd_intf == WCMOD_SPD_1000_SGMII) || (spd_intf == WCMOD_SPD_1000_FIBER) ||
        (spd_intf == WCMOD_SPD_2500)) {
      SOC_IF_ERROR_RETURN(MODIFY_WC40_XGXSBLK1_LANECTRL0r(pc->unit, pc,
                 XGXSBLK1_LANECTRL0_CL36_PCS_EN_RX_MASK |
                 XGXSBLK1_LANECTRL0_CL36_PCS_EN_TX_MASK,
                 XGXSBLK1_LANECTRL0_CL36_PCS_EN_RX_MASK |
                 XGXSBLK1_LANECTRL0_CL36_PCS_EN_TX_MASK));

     /* set up rx mac clock frequency */
     /*data16 = 0;
     mask = 0;
     switch(pc->this_lane) {
     case 0:
       mask |=   XGXSBLK1_LANECTRL1_RX1G_MODE_LN0_MASK
           | XGXSBLK1_LANECTRL1_TX1G_MODE_LN0_MASK;
       data16 |= XGXSBLK1_LANECTRL1_RX1G_MODE_LN0_SWSDR_div1
               << XGXSBLK1_LANECTRL1_RX1G_MODE_LN0_SHIFT;
       break;
     case 1:
       mask |= XGXSBLK1_LANECTRL1_RX1G_MODE_LN1_MASK |
           XGXSBLK1_LANECTRL1_TX1G_MODE_LN1_MASK;
       data16 |= XGXSBLK1_LANECTRL1_RX1G_MODE_LN1_SWSDR_div1
                << XGXSBLK1_LANECTRL1_RX1G_MODE_LN1_SHIFT;    
       break;
     case 2:
       mask |= XGXSBLK1_LANECTRL1_RX1G_MODE_LN2_MASK
            | XGXSBLK1_LANECTRL1_TX1G_MODE_LN2_MASK;
       data16 |= XGXSBLK1_LANECTRL1_RX1G_MODE_LN2_SWSDR_div1
                << XGXSBLK1_LANECTRL1_RX1G_MODE_LN2_SHIFT;
       break;
     case 3:
       mask |= XGXSBLK1_LANECTRL1_RX1G_MODE_LN3_MASK
                | XGXSBLK1_LANECTRL1_TX1G_MODE_LN3_MASK;
       data16 |= XGXSBLK1_LANECTRL1_RX1G_MODE_LN3_SWSDR_div1
                << XGXSBLK1_LANECTRL1_RX1G_MODE_LN3_SHIFT;
       break;
     }
     MODIFY_WC40_XGXSBLK1_LANECTRL1r(pc->unit, pc, data16, mask);

     MODIFY_WC40_SERDESDIGITAL_MISC1r(pc->unit, pc,
                      SERDESDIGITAL_MISC1_FORCE_LN_MODE_MASK,
                      SERDESDIGITAL_MISC1_FORCE_LN_MODE_MASK);*/
    }
    if((pc->model_type == WCMOD_QS_A0) && ( pc->spd_intf == WCMOD_SPD_1G_QSGMII)) {
      MODIFY_WC40_SERDESDIGITAL_CONTROL1000X2r(pc->unit, pc, 0,
                          SERDESDIGITAL_CONTROL1000X2_FILTER_FORCE_LINK_MASK |
                          SERDESDIGITAL_CONTROL1000X2_DISABLE_FALSE_LINK_MASK);
      /* enable full duplex */
      MODIFY_WC40_COMBO_IEEE0_MIICNTLr(pc->unit, pc,
                        IEEE0BLK_MIICNTL_FULL_DUPLEX_MASK,
                        IEEE0BLK_MIICNTL_FULL_DUPLEX_MASK);
      /* disable lane disable */
      MODIFY_WC40_DIGITAL4_MISC3r(pc->unit, pc,0,
                                              DIGITAL4_MISC3_LANEDISABLE_MASK);
    }

    if (spd_intf == WCMOD_SPD_PCSBYP_5P75G) {
      /* SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div32 is no longer 32. It is 46.
       * By setting the clkmul to 46, the 20 bit interface is also setup */
      data16 = ((SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_BITS
              << SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_SHIFT) |
               (SERDESDIGITAL_MISC1_REFCLK_SEL_clk_125MHz
            << SERDESDIGITAL_MISC1_REFCLK_SEL_SHIFT) |
               (SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div32
            << SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT));

      mask = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
             SERDESDIGITAL_MISC1_REFCLK_SEL_MASK |
             SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_MASK;

      SOC_IF_ERROR_RETURN (MODIFY_WC40_SERDESDIGITAL_MISC1r(pc->unit, pc,
                                                                 data16, mask));
    }
    /* for 12.5G XFI with PCS, we need to force the pll divider */
    if (spd_intf == WCMOD_SPD_12P5) {
      data16 = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
               SERDESDIGITAL_MISC1_REFCLK_SEL_clk_156p25MHz <<
                                        SERDESDIGITAL_MISC1_REFCLK_SEL_SHIFT |
               SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div80 <<
                                  SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT;

      mask = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
             SERDESDIGITAL_MISC1_REFCLK_SEL_MASK |
             SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_MASK;

      SOC_IF_ERROR_RETURN (MODIFY_WC40_SERDESDIGITAL_MISC1r(pc->unit, pc,
                                                                 data16, mask));
      /* need to set the PLL rate */
      data16 = 28;
      SOC_IF_ERROR_RETURN(MODIFY_WC40_UC_INFO_B1_REGBr(pc->unit, pc, data16 <<
                      UC_INFO_B1_MISC_CTRL_SERDES_PLL_RATE_SHIFT, UC_INFO_B1_MISC_CTRL_SERDES_PLL_RATE_MASK));
    }

    /* for 11.5G XFI with PCS at 125M, we need to force the pll divider */
    if (spd_intf == WCMOD_SPD_11P5) {
      data16 = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
               SERDESDIGITAL_MISC1_REFCLK_SEL_clk_125MHz <<
                                        SERDESDIGITAL_MISC1_REFCLK_SEL_SHIFT |
               SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div92 <<
                                  SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT;

      mask = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
             SERDESDIGITAL_MISC1_REFCLK_SEL_MASK |
             SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_MASK;

      SOC_IF_ERROR_RETURN (MODIFY_WC40_SERDESDIGITAL_MISC1r(pc->unit, pc,
                                                                 data16, mask));
      /* need to set the PLL rate */
      data16 = 24;
      SOC_IF_ERROR_RETURN(MODIFY_WC40_UC_INFO_B1_REGBr(pc->unit, pc, data16 <<
                      UC_INFO_B1_MISC_CTRL_SERDES_PLL_RATE_SHIFT, UC_INFO_B1_MISC_CTRL_SERDES_PLL_RATE_MASK));

      /* use firmware mode OSDFE */
      data16 = UC_INFO_B1_FIRMWARE_MODE_LN0_MODE_LONG_CH_6G;
      data16  <<= (pc->this_lane * 4);
      mask16 = UC_INFO_B1_FIRMWARE_MODE_LN0_MODE_MASK << (pc->this_lane * 4);
      SOC_IF_ERROR_RETURN
          (MODIFY_WC40_UC_INFO_B1_FIRMWARE_MODEr(pc->unit,pc,data16,mask16));
    }

    if (spd_intf == WCMOD_SPD_10600_XFI_HG) {
      data16 = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
               SERDESDIGITAL_MISC1_REFCLK_SEL_clk_156p25MHz <<
                                        SERDESDIGITAL_MISC1_REFCLK_SEL_SHIFT |
               SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div70 <<
                                  SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT;

      mask = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
             SERDESDIGITAL_MISC1_REFCLK_SEL_MASK |
             SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_MASK;

      SOC_IF_ERROR_RETURN (MODIFY_WC40_SERDESDIGITAL_MISC1r(pc->unit, pc,
                                                                 data16, mask));
    } else if (spd_intf == WCMOD_SPD_15750_HI_DXGXS) {
      data16 = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
               (SERDESDIGITAL_MISC1_REFCLK_SEL_clk_156p25MHz <<
                                       SERDESDIGITAL_MISC1_REFCLK_SEL_SHIFT) |
               (SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div52 <<
                                SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT);
      mask = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
             SERDESDIGITAL_MISC1_REFCLK_SEL_MASK |
             SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_MASK;
      SOC_IF_ERROR_RETURN (MODIFY_WC40_SERDESDIGITAL_MISC1r(pc->unit, pc,
                                                                 data16, mask));
    } else if (spd_intf == WCMOD_SPD_21G_HI_DXGXS) {
      data16 = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
               (SERDESDIGITAL_MISC1_REFCLK_SEL_clk_156p25MHz <<
                                        SERDESDIGITAL_MISC1_REFCLK_SEL_SHIFT) |
               (SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div70 <<
                                  SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT);
      mask = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
             SERDESDIGITAL_MISC1_REFCLK_SEL_MASK |
             SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_MASK;

      SOC_IF_ERROR_RETURN (MODIFY_WC40_SERDESDIGITAL_MISC1r(pc->unit, pc,
                                                                 data16, mask));
    } else if (spd_intf == WCMOD_SPD_20G_DXGXS_SCR) { 
      data16 = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
               SERDESDIGITAL_MISC1_REFCLK_SEL_clk_156p25MHz <<
                                        SERDESDIGITAL_MISC1_REFCLK_SEL_SHIFT |
               SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div66 <<
                                  SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT;

      mask = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
             SERDESDIGITAL_MISC1_REFCLK_SEL_MASK |
             SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_MASK;

      SOC_IF_ERROR_RETURN (MODIFY_WC40_SERDESDIGITAL_MISC1r(pc->unit, pc,
                                                                 data16, mask));
    }

    if((spd_intf == WCMOD_SPD_2500)       || (spd_intf == WCMOD_SPD_10000_XFI)    ||
       (spd_intf == WCMOD_SPD_10000_SFI)  || (spd_intf == WCMOD_SPD_11P5) || (spd_intf == WCMOD_SPD_12P5) ||
       (spd_intf == WCMOD_SPD_R2_12000)   || (spd_intf == WCMOD_SPD_15750_HI_DXGXS) ||
       (spd_intf == WCMOD_SPD_10600_XFI_HG) || (spd_intf == WCMOD_SPD_21G_HI_DXGXS) ||
       (spd_intf == WCMOD_SPD_20G_DXGXS)  || (spd_intf == WCMOD_SPD_20000_HI_DXGXS)){
      MODIFY_WC40_SERDESDIGITAL_CONTROL1000X1r(pc->unit, pc, 
                             SERDESDIGITAL_CONTROL1000X1_FIBER_MODE_1000X_MASK,
                             SERDESDIGITAL_CONTROL1000X1_FIBER_MODE_1000X_MASK);
      /* set full duplex */
      MODIFY_WC40_COMBO_IEEE0_MIICNTLr(pc->unit, pc,
                        IEEE0BLK_MIICNTL_FULL_DUPLEX_MASK,
                        IEEE0BLK_MIICNTL_FULL_DUPLEX_MASK);

      mask = XGXSBLK6_XGXSX2CONTROL2_TXCKOUT33_OVERRIDE_MASK;
      MODIFY_WC40_XGXSBLK6_XGXSX2CONTROL2r(pc->unit, pc, mask, mask);
      if (pc->verbosity > 0) printf("%-22s: txckout33_override bit is set\n",FUNCTION_NAME());
    }

    /* 20G dual port mode PLL div and clk156p25 selected to ensure plldiv value  */
    if((spd_intf == WCMOD_SPD_20000_HI_DXGXS) || (spd_intf == WCMOD_SPD_20G_KR2) || 
       (spd_intf == WCMOD_SPD_20G_CR2) || (spd_intf == WCMOD_SPD_20G_DXGXS)) {
        
       data16 = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
          (SERDESDIGITAL_MISC1_REFCLK_SEL_clk_156p25MHz <<
           SERDESDIGITAL_MISC1_REFCLK_SEL_SHIFT) |
          (SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div66 <<
           SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT);
       mask = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
          SERDESDIGITAL_MISC1_REFCLK_SEL_MASK | 
          SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_MASK;
       
       SOC_IF_ERROR_RETURN (MODIFY_WC40_SERDESDIGITAL_MISC1r(pc->unit, pc,
                                                             data16, mask));
    }
    
    /*--------------------------------------------------------------------------
     * The following code configures the 64B/66B decoder when speed is forced
     * to speed_25455 or speed_12773_dxgxs or speed_31500 or speed_40g.
     *------------------------------------------------------------------------*/

    if ((spd_intf == WCMOD_SPD_25455) || (spd_intf == WCMOD_SPD_15750_HI_DXGXS) ||
        (spd_intf == WCMOD_SPD_40G)  || (spd_intf == WCMOD_SPD_20000_HI_DXGXS) ||
        (spd_intf == WCMOD_SPD_42G)  || (spd_intf == WCMOD_SPD_48G) || 
         (spd_intf == WCMOD_SPD_44G) || 
        (spd_intf == WCMOD_SPD_31500) || (spd_intf == WCMOD_SPD_21G_HI_DXGXS)) {
 
      WRITE_WC40_RX66_SCW0r(pc->unit, pc, 0xE070);
      WRITE_WC40_RX66_SCW1r(pc->unit, pc, 0xC0D0);
      WRITE_WC40_RX66_SCW2r(pc->unit, pc, 0xA0B0);
      WRITE_WC40_RX66_SCW3r(pc->unit, pc, 0x8090);
      WRITE_WC40_RX66_SCW0_MASKr(pc->unit, pc, 0xF0F0);
      WRITE_WC40_RX66_SCW1_MASKr(pc->unit, pc, 0xF0F0);
      WRITE_WC40_RX66_SCW2_MASKr(pc->unit, pc, 0xF0F0);
      WRITE_WC40_RX66_SCW3_MASKr(pc->unit, pc, 0xF0F0);
    }

    /* in XFI mode set 40-bit interface */
    if ((spd_intf == WCMOD_SPD_10000_XFI) || (spd_intf == WCMOD_SPD_10600_XFI_HG) ||
        (spd_intf == WCMOD_SPD_11P5) || (spd_intf == WCMOD_SPD_12P5) || 
        (spd_intf == WCMOD_SPD_10000_SFI)) {
      MODIFY_WC40_DIGITAL4_MISC3r(pc->unit, pc,DIGITAL4_MISC3_IND_40BITIF_MASK,
                                               DIGITAL4_MISC3_IND_40BITIF_MASK);
    }
    /* CONFIGURE MAC INTERFACE: MAKE A SEPERATE PROCESS */
    /* set mac_int_rxck_override */
    if ((pc->port_type == WCMOD_INDEPENDENT) && ((spd_intf == WCMOD_SPD_R2_12000) ||
        (spd_intf == WCMOD_SPD_10000_XFI) || (spd_intf == WCMOD_SPD_15750_HI_DXGXS) ||
        (spd_intf == WCMOD_SPD_10600_XFI_HG) || (spd_intf == WCMOD_SPD_10000_SFI) ||
        (spd_intf == WCMOD_SPD_11P5) || (spd_intf == WCMOD_SPD_12P5) ||
        (spd_intf == WCMOD_SPD_20000_HI_DXGXS) || (spd_intf == WCMOD_SPD_20G_DXGXS) ||
        (spd_intf == WCMOD_SPD_21G_HI_DXGXS))){
      mask = XGXSBLK6_XGXSX2CONTROL2_MAC_INF_RXCK_OVERRIDE_MASK;
      MODIFY_WC40_XGXSBLK6_XGXSX2CONTROL2r(pc->unit, pc, mask, mask);
      if (pc->verbosity > 0) 
        printf("%-22s: mac_inf_rxck_override bit is set\n",FUNCTION_NAME());
    }

    if ((pc->port_type == WCMOD_INDEPENDENT) & (spd_intf == WCMOD_SPD_R2_12000)) {
    }

    if ((pc->os_type == WCMOD_OS8) && (  (spd_intf == WCMOD_SPD_10000_XFI) ||
        (spd_intf == WCMOD_SPD_11P5) || (spd_intf == WCMOD_SPD_12P5) ||
        (spd_intf == WCMOD_SPD_10000_SFI) || (spd_intf == WCMOD_SPD_10600_XFI_HG))) {
      lane_num_ignore = pc->lane_num_ignore; /* save and restore it below */
      pc->lane_num_ignore = 1;

      switch(this_lane) {
        case 0:
     data16 = (XGXSBLK1_LANECTRL1_TX1G_MODE_LN0_DWSDR_div1
                                << XGXSBLK1_LANECTRL1_TX1G_MODE_LN0_SHIFT) |
            (XGXSBLK1_LANECTRL1_RX1G_MODE_LN0_DWSDR_div1
                                << XGXSBLK1_LANECTRL1_RX1G_MODE_LN0_SHIFT);
    
     MODIFY_WC40_XGXSBLK1_LANECTRL1r(pc->unit, pc, data16,
                     XGXSBLK1_LANECTRL1_TX1G_MODE_LN0_MASK |
                     XGXSBLK1_LANECTRL1_RX1G_MODE_LN0_MASK);
       break;
       case 1:
     data16 = (XGXSBLK1_LANECTRL1_TX1G_MODE_LN1_DWSDR_div1 
                                << XGXSBLK1_LANECTRL1_TX1G_MODE_LN1_SHIFT) |
              (XGXSBLK1_LANECTRL1_RX1G_MODE_LN1_DWSDR_div1 
                                << XGXSBLK1_LANECTRL1_RX1G_MODE_LN1_SHIFT);
    
     MODIFY_WC40_XGXSBLK1_LANECTRL1r(pc->unit, pc, data16,
                     XGXSBLK1_LANECTRL1_TX1G_MODE_LN1_MASK |
                     XGXSBLK1_LANECTRL1_RX1G_MODE_LN1_MASK);
       break;
       case 2:
     data16 = (XGXSBLK1_LANECTRL1_TX1G_MODE_LN2_DWSDR_div1 
                                << XGXSBLK1_LANECTRL1_TX1G_MODE_LN2_SHIFT) |
              (XGXSBLK1_LANECTRL1_RX1G_MODE_LN2_DWSDR_div1 
                                << XGXSBLK1_LANECTRL1_RX1G_MODE_LN2_SHIFT);
    
     MODIFY_WC40_XGXSBLK1_LANECTRL1r(pc->unit, pc, data16,
                     XGXSBLK1_LANECTRL1_TX1G_MODE_LN2_MASK |
                     XGXSBLK1_LANECTRL1_RX1G_MODE_LN2_MASK);
       break;
       case 3:
     data16 = (XGXSBLK1_LANECTRL1_TX1G_MODE_LN3_DWSDR_div1 
                                << XGXSBLK1_LANECTRL1_TX1G_MODE_LN3_SHIFT) |
              (XGXSBLK1_LANECTRL1_RX1G_MODE_LN3_DWSDR_div1 
                                << XGXSBLK1_LANECTRL1_RX1G_MODE_LN3_SHIFT);
    
     MODIFY_WC40_XGXSBLK1_LANECTRL1r(pc->unit, pc, data16,
                     XGXSBLK1_LANECTRL1_TX1G_MODE_LN3_MASK |
                     XGXSBLK1_LANECTRL1_RX1G_MODE_LN3_MASK);
     break;
       default:
         printf("%-22s: Error: invalid lane number %d\n",FUNCTION_NAME(),this_lane);
         return SOC_E_ERROR;
         break;
       }
       pc->lane_num_ignore = lane_num_ignore;
     }

     if ((pc->os_type == WCMOD_OS5) && ((spd_intf == WCMOD_SPD_10000_XFI) ||
         (spd_intf == WCMOD_SPD_10000_SFI) || (spd_intf == WCMOD_SPD_10600_XFI_HG))) {
       MODIFY_WC40_SERDESDIGITAL_MISC1r(pc->unit, pc,
                              (SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div66
                            << SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT) |
                               SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK,
                               SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_MASK |
                               SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK);
     }

     if( (spd_intf == WCMOD_SPD_PCSBYP_3P125G) ||
         (spd_intf == WCMOD_SPD_PCSBYP_6P25G) ||
         (spd_intf == WCMOD_SPD_PCSBYP_10P3125)) {
       lane_num_ignore = pc->lane_num_ignore; /* save and restore it below */
       pc->lane_num_ignore = 1;

       if (pc->model_type == WCMOD_WC_B0) {
         switch(this_lane) {
         case 0:
     if (spd_intf == WCMOD_SPD_PCSBYP_3P125G)
       data16 = (XGXSBLK1_LANECTRL1_TX1G_MODE_LN0_DWSDR_div2 
                                    << XGXSBLK1_LANECTRL1_TX1G_MODE_LN0_SHIFT) |
                (XGXSBLK1_LANECTRL1_RX1G_MODE_LN0_DWSDR_div2 
                                    << XGXSBLK1_LANECTRL1_RX1G_MODE_LN0_SHIFT);
     else if ((spd_intf == WCMOD_SPD_PCSBYP_6P25G)
               || (spd_intf == WCMOD_SPD_PCSBYP_10P3125))
       data16 = (XGXSBLK1_LANECTRL1_TX1G_MODE_LN0_DWSDR_div1 
                                << XGXSBLK1_LANECTRL1_TX1G_MODE_LN0_SHIFT) |
                (XGXSBLK1_LANECTRL1_RX1G_MODE_LN0_DWSDR_div1 
                                << XGXSBLK1_LANECTRL1_RX1G_MODE_LN0_SHIFT);
     else
       data16 = 0;

     MODIFY_WC40_XGXSBLK1_LANECTRL1r(pc->unit, pc, data16,
                     XGXSBLK1_LANECTRL1_TX1G_MODE_LN0_MASK |
                     XGXSBLK1_LANECTRL1_RX1G_MODE_LN0_MASK);
     break;
         case 1:
     if (spd_intf == WCMOD_SPD_PCSBYP_3P125G)
       data16 = (XGXSBLK1_LANECTRL1_TX1G_MODE_LN1_DWSDR_div2 
                                << XGXSBLK1_LANECTRL1_TX1G_MODE_LN1_SHIFT) |
                (XGXSBLK1_LANECTRL1_RX1G_MODE_LN1_DWSDR_div2 
                                << XGXSBLK1_LANECTRL1_RX1G_MODE_LN1_SHIFT);
     else if ((spd_intf == WCMOD_SPD_PCSBYP_6P25G)
               || (spd_intf == WCMOD_SPD_PCSBYP_10P3125))
       data16 = (XGXSBLK1_LANECTRL1_TX1G_MODE_LN1_DWSDR_div1 
                                << XGXSBLK1_LANECTRL1_TX1G_MODE_LN1_SHIFT) |
                (XGXSBLK1_LANECTRL1_RX1G_MODE_LN1_DWSDR_div1 
                                << XGXSBLK1_LANECTRL1_RX1G_MODE_LN1_SHIFT);
     else
       data16 = 0;
    
     MODIFY_WC40_XGXSBLK1_LANECTRL1r(pc->unit, pc, data16,
                     XGXSBLK1_LANECTRL1_TX1G_MODE_LN1_MASK |
                     XGXSBLK1_LANECTRL1_RX1G_MODE_LN1_MASK);
     break;
         case 2:
     if (spd_intf == WCMOD_SPD_PCSBYP_3P125G)
       data16 = (XGXSBLK1_LANECTRL1_TX1G_MODE_LN2_DWSDR_div2 
                                << XGXSBLK1_LANECTRL1_TX1G_MODE_LN2_SHIFT) |
                (XGXSBLK1_LANECTRL1_RX1G_MODE_LN2_DWSDR_div2 
                                << XGXSBLK1_LANECTRL1_RX1G_MODE_LN2_SHIFT);
     else if ((spd_intf == WCMOD_SPD_PCSBYP_6P25G)
               || (spd_intf == WCMOD_SPD_PCSBYP_10P3125))
       data16 = (XGXSBLK1_LANECTRL1_TX1G_MODE_LN2_DWSDR_div1 
                                << XGXSBLK1_LANECTRL1_TX1G_MODE_LN2_SHIFT) |
                (XGXSBLK1_LANECTRL1_RX1G_MODE_LN2_DWSDR_div1
                                << XGXSBLK1_LANECTRL1_RX1G_MODE_LN2_SHIFT);
     else
       data16 = 0;
       MODIFY_WC40_XGXSBLK1_LANECTRL1r(pc->unit, pc, data16,
                     XGXSBLK1_LANECTRL1_TX1G_MODE_LN2_MASK |
                     XGXSBLK1_LANECTRL1_RX1G_MODE_LN2_MASK);
     break;
         case 3:
     if (spd_intf == WCMOD_SPD_PCSBYP_3P125G)
       data16 = (XGXSBLK1_LANECTRL1_TX1G_MODE_LN3_DWSDR_div2 
                                << XGXSBLK1_LANECTRL1_TX1G_MODE_LN3_SHIFT) |
                (XGXSBLK1_LANECTRL1_RX1G_MODE_LN3_DWSDR_div2 
                                << XGXSBLK1_LANECTRL1_RX1G_MODE_LN3_SHIFT);
     else if ((spd_intf == WCMOD_SPD_PCSBYP_6P25G)
               || (spd_intf == WCMOD_SPD_PCSBYP_10P3125))
       data16 = (XGXSBLK1_LANECTRL1_TX1G_MODE_LN3_DWSDR_div1
                                << XGXSBLK1_LANECTRL1_TX1G_MODE_LN3_SHIFT) |
                (XGXSBLK1_LANECTRL1_RX1G_MODE_LN3_DWSDR_div1
                                << XGXSBLK1_LANECTRL1_RX1G_MODE_LN3_SHIFT);
     else
       data16 = 0;

     MODIFY_WC40_XGXSBLK1_LANECTRL1r(pc->unit, pc, data16,
                     XGXSBLK1_LANECTRL1_TX1G_MODE_LN3_MASK |
                     XGXSBLK1_LANECTRL1_RX1G_MODE_LN3_MASK);
     break;
         default:
           printf("%-22s: Error: Invalid lane %d\n", FUNCTION_NAME(), this_lane);
       return SOC_E_ERROR;
         } /* switch(this_lane) */
         pc->lane_num_ignore = 0;

         if (spd_intf == WCMOD_SPD_PCSBYP_3P125G) {
       SOC_IF_ERROR_RETURN(MODIFY_WC40_DSC2B0_DSC_MISC_CTRL0r(pc->unit, pc,
                              DSC2B0_DSC_MISC_CTRL0_CDRBR_SEL_FORCE_MASK,
                              DSC2B0_DSC_MISC_CTRL0_CDRBR_SEL_FORCE_MASK |
                              DSC2B0_DSC_MISC_CTRL0_CDRBR_SEL_FORCE_VAL_MASK));
         }

         MODIFY_WC40_SERDESDIGITAL_MISC1r(pc->unit, pc,
                                (SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div40
                               << SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT)
                              | SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK
                              | (SERDESDIGITAL_MISC1_FORCE_SPEED_dr_6363BRCM_X1
                               << SERDESDIGITAL_MISC1_FORCE_SPEED_SHIFT),
                              SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_MASK |
                              SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
                                      SERDESDIGITAL_MISC1_FORCE_SPEED_MASK);
      } /*endif B0 */
      pc->lane_num_ignore = lane_num_ignore;
    }

    /* put the workaround for 1G and  2.5G for XENIA core aymmetric mode */
     if ((pc->model_type == WCMOD_XN) && (pc->asymmetric_mode)) {
        if ((spd_intf == WCMOD_SPD_2500) || (spd_intf == WCMOD_SPD_1000_SGMII) || (spd_intf == WCMOD_SPD_1000_FIBER)) {
            /* first program the 0x8308 to 0x7110 */
            if (spd_intf == WCMOD_SPD_2500){
                WRITE_WC40_SERDESDIGITAL_MISC1r(pc->unit, pc, 0x7110);
            } else {
                WRITE_WC40_SERDESDIGITAL_MISC1r(pc->unit, pc, 0x7120);
            }
            /* next enable the asymmetric bit in 0x8169 */
            MODIFY_WC40_XGXSBLK8_TXLNSWAP1r(pc->unit, pc, 
                                XGXSBLK8_TXLNSWAP1_ASYMMETRIC_MODE_EN_MASK,
                                XGXSBLK8_TXLNSWAP1_ASYMMETRIC_MODE_EN_MASK);
            /*set the tx/rx clock control bit */
            if ((spd_intf == WCMOD_SPD_1000_SGMII) || (spd_intf == WCMOD_SPD_1000_FIBER)) {

                switch (pc->this_lane) {
                case 0: MODIFY_WC40_TX0_ANATXACONTROL1r(pc->unit, pc, 0x40, 0x40); 
                        MODIFY_WC40_RX0_ANARXACONTROL1r(pc->unit, pc, 0x40, 0x40); 
                        break;
                case 1: MODIFY_WC40_TX1_ANATXACONTROL1r(pc->unit, pc, 0x40, 0x40); 
                        MODIFY_WC40_RX1_ANARXACONTROL1r(pc->unit, pc, 0x40, 0x40); 
                        break;
                case 2: MODIFY_WC40_TX2_ANATXACONTROL1r(pc->unit, pc, 0x40, 0x40); 
                        MODIFY_WC40_RX2_ANARXACONTROL1r(pc->unit, pc, 0x40, 0x40); 
                        break;
                case 3: MODIFY_WC40_TX3_ANATXACONTROL1r(pc->unit, pc, 0x40, 0x40); 
                        MODIFY_WC40_RX3_ANARXACONTROL1r(pc->unit, pc, 0x40, 0x40); 
                        break;
                default: break;
                }
            } else {

                switch (pc->this_lane) {
                case 0: MODIFY_WC40_TX0_ANATXACONTROL1r(pc->unit, pc, 0x00, 0x40); 
                        MODIFY_WC40_RX0_ANARXACONTROL1r(pc->unit, pc, 0x00, 0x40); 
                        break;
                case 1: MODIFY_WC40_TX1_ANATXACONTROL1r(pc->unit, pc, 0x00, 0x40); 
                        MODIFY_WC40_RX1_ANARXACONTROL1r(pc->unit, pc, 0x00, 0x40); 
                        break;
                case 2: MODIFY_WC40_TX2_ANATXACONTROL1r(pc->unit, pc, 0x00, 0x40); 
                        MODIFY_WC40_RX2_ANARXACONTROL1r(pc->unit, pc, 0x00, 0x40); 
                        break;
                case 3: MODIFY_WC40_TX3_ANATXACONTROL1r(pc->unit, pc, 0x00, 0x40); 
                        MODIFY_WC40_RX3_ANARXACONTROL1r(pc->unit, pc, 0x00, 0x40); 
                        break;
                default: break;
                }
            }
            /* make sure os mode is set to 1 */
            WRITE_WC40_XGXSBLK1_LANECTRL1r(pc->unit, pc, 0xffff);
       }
    }
    if (spd_intf == WCMOD_SPD_X2_10000) {       
        if(125 == pc->refclk) {
            data16 = ((SERDESDIGITAL_MISC1_REFCLK_SEL_clk_125MHz << SERDESDIGITAL_MISC1_REFCLK_SEL_SHIFT) |
                      SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
                      (SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div50 << SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT));
            mask16 = SERDESDIGITAL_MISC1_REFCLK_SEL_MASK |
                     SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
                     SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_MASK;

            SOC_IF_ERROR_RETURN( MODIFY_WC40_SERDESDIGITAL_MISC1r(pc->unit, pc, data16, mask16));
        } else {
            data16 = ((SERDESDIGITAL_MISC1_REFCLK_SEL_clk_156p25MHz << SERDESDIGITAL_MISC1_REFCLK_SEL_SHIFT) |
                      SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
                      (SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div40 << SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT));
            mask16 = SERDESDIGITAL_MISC1_REFCLK_SEL_MASK |
                     SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
                     SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_MASK;
            SOC_IF_ERROR_RETURN( MODIFY_WC40_SERDESDIGITAL_MISC1r(pc->unit, pc, data16, mask16));
        }
    }
    if (spd_intf == WCMOD_SPD_X2_23000) { 
       if(125 == pc->refclk) {
          data16 = ((SERDESDIGITAL_MISC1_REFCLK_SEL_clk_125MHz << SERDESDIGITAL_MISC1_REFCLK_SEL_SHIFT) |
                    SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
                    (SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div92 << SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT));
          mask16 = SERDESDIGITAL_MISC1_REFCLK_SEL_MASK |
             SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
             SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_MASK;
          
          SOC_IF_ERROR_RETURN( MODIFY_WC40_SERDESDIGITAL_MISC1r(pc->unit, pc, data16, mask16));
       } else {
          /* no proper div = 73.6 */
          data16 = ((SERDESDIGITAL_MISC1_REFCLK_SEL_clk_156p25MHz << SERDESDIGITAL_MISC1_REFCLK_SEL_SHIFT) |
                    SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
                    (SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div70 << SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT));
          mask16 = SERDESDIGITAL_MISC1_REFCLK_SEL_MASK |
             SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
             SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_MASK;
          SOC_IF_ERROR_RETURN( MODIFY_WC40_SERDESDIGITAL_MISC1r(pc->unit, pc, data16, mask16));
       }
     
       data16 = DIGITAL4_MISC4_SCR_EN_OVER_MASK ;
       mask16 = DIGITAL4_MISC4_SCR_EN_OVER_MASK ;
       SOC_IF_ERROR_RETURN(MODIFY_WC40_DIGITAL4_MISC4r(pc->unit, pc, data16, mask16));
       
       data16 = DIGITAL4_MISC3_SCR_EN_PER_LANE_MASK ;
       mask16 = DIGITAL4_MISC3_SCR_EN_PER_LANE_MASK ;
       SOC_IF_ERROR_RETURN(MODIFY_WC40_DIGITAL4_MISC3r(pc->unit, pc, data16, mask16));
    }
   if ((spd_intf == WCMOD_SPD_10000_HI_DXGXS)     || (spd_intf == WCMOD_SPD_10000_DXGXS) ||
        (spd_intf == WCMOD_SPD_10000_HI_DXGXS_SCR) || (spd_intf == WCMOD_SPD_10000_DXGXS_SCR) ) {
       data16 = ((SERDESDIGITAL_MISC1_REFCLK_SEL_clk_156p25MHz << SERDESDIGITAL_MISC1_REFCLK_SEL_SHIFT) |
                 SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
                 (SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div40 << SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT));
       mask16 = SERDESDIGITAL_MISC1_REFCLK_SEL_MASK |
          SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
          SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_MASK;

       SOC_IF_ERROR_RETURN( MODIFY_WC40_SERDESDIGITAL_MISC1r(pc->unit, pc, data16, mask16));
    }
    if ((spd_intf == WCMOD_SPD_12773_DXGXS) || (spd_intf==WCMOD_SPD_12773_HI_DXGXS)||
        (spd_intf == WCMOD_SPD_10500_DXGXS) || (spd_intf==WCMOD_SPD_10500_HI_DXGXS) ) {
       data16 = ((SERDESDIGITAL_MISC1_REFCLK_SEL_clk_156p25MHz << SERDESDIGITAL_MISC1_REFCLK_SEL_SHIFT) |
                 SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
                 (SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div42 << SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT));
       mask16 = SERDESDIGITAL_MISC1_REFCLK_SEL_MASK |
          SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
          SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_MASK;

       SOC_IF_ERROR_RETURN( MODIFY_WC40_SERDESDIGITAL_MISC1r(pc->unit, pc, data16, mask16));
    } 
                                
    if (spd_intf == WCMOD_SPD_PCSBYP_5G) {
        if (pc->model_type == WCMOD_WC_C0 || pc->model_type == WCMOD_WL_A0) {
            /* first program PLL divider 0x8308 */
            SOC_IF_ERROR_RETURN( WRITE_WC40_SERDESDIGITAL_MISC1r(pc->unit, pc, 0x790a));
            SOC_IF_ERROR_RETURN( WRITE_WC40_DIGITAL4_MISC5r(pc->unit, pc, 0x40));
            SOC_IF_ERROR_RETURN( MODIFY_WC40_DIGITAL5_MISC7r(pc->unit, pc, 0x600, 0x600));
        }
    }

    /* needs to update the pll rate for  for canned pcs bypass mode with 0*/
    if ((spd_intf == WCMOD_SPD_PCSBYP_12P5) ||
         (spd_intf == WCMOD_SPD_PCSBYP_11P5) ||
         (spd_intf == WCMOD_SPD_PCSBYP_10P9375) ||
         (spd_intf == WCMOD_SPD_PCSBYP_10P3125) ||
         (spd_intf == WCMOD_SPD_PCSBYP_6P25G) ||
         (spd_intf == WCMOD_SPD_PCSBYP_5P75G)) {
        SOC_IF_ERROR_RETURN(MODIFY_WC40_UC_INFO_B1_REGBr(pc->unit, pc, 0 <<
                            UC_INFO_B1_MISC_CTRL_SERDES_PLL_RATE_SHIFT, UC_INFO_B1_MISC_CTRL_SERDES_PLL_RATE_MASK));
    }

    if (spd_intf == WCMOD_SPD_CUSTOM_BYPASS) {
        if (pc->model_type == WCMOD_WC_C0 || pc->model_type == WCMOD_WL_A0) {
            /* first program PLL divider 0x8308 */
            switch (pc->pll_divider) {
                case 46:
                    data16 = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
                             SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div32 << SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT;
                    break;
                case 72:
                    data16 = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
                             SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div36 << SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT;
                    break;
                case 40:
                    data16 = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
                             SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div40 << SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT;
                    break;
                case 42:
                    data16 = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
                             SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div42 << SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT;
                    break;
                case 48:
                    data16 = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
                             SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div48 << SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT;
                    break;
                case 50:
                    data16 = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
                             SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div50 << SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT;
                    break;
                case 52:
                    data16 = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
                             SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div52 << SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT;
                    break;
                case 54:
                    data16 = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
                             SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div54 << SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT;
                    break;
                case 60:
                    data16 = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
                             SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div60 << SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT;
                    break;
                case 64:
                    data16 = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
                             SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div64 << SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT;
                    break;
                case 66:
                    data16 = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
                             SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div66 << SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT;
                    break;
                case 68:
                    data16 = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
                             SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div68 << SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT;
                    break;
                case 70:
                    data16 = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
                             SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div70 << SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT;
                    break;
                case 80:
                    data16 = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
                             SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div80 << SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT;
                    break;
                case 92:
                    data16 = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
                             SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div92 << SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT;
                    break;
                case 100:
                    data16 = SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
                             SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_div100 << SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT;
                    break;
                default:
                    break;
            }
            SOC_IF_ERROR_RETURN( MODIFY_WC40_SERDESDIGITAL_MISC1r(pc->unit, pc, data16,
                                SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_MASK |
                                SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK));
            /* next program the os mode  0x8349 */
            SOC_IF_ERROR_RETURN(MODIFY_WC40_DIGITAL5_MISC7r(pc->unit, pc, DIGITAL5_MISC7_FORCE_OSCDR_MODE_MASK |
                                                            pc->oversample_mode << DIGITAL5_MISC7_FORCE_OSCDR_MODE_VAL_SHIFT, 
                                                            DIGITAL5_MISC7_FORCE_OSCDR_MODE_MASK | 
                                                            DIGITAL5_MISC7_FORCE_OSCDR_MODE_VAL_MASK));
            /* next inform micro controller the vco speed */
            data16 = (pc->vco_freq - 5750) / 250 + 1;
            SOC_IF_ERROR_RETURN(MODIFY_WC40_UC_INFO_B1_REGBr(pc->unit, pc, data16 <<
                                UC_INFO_B1_MISC_CTRL_SERDES_PLL_RATE_SHIFT, UC_INFO_B1_MISC_CTRL_SERDES_PLL_RATE_MASK));
        } 
    }
  } /* pc->port_type == WCMOD_INDEPENDENT */
  return SOC_E_NONE;
} /* Wcmod_speed_intf_set(wcmod_st* pc) */

int wcmod_speed_intf_control(wcmod_st* pc)              /* SPEED_INTF_CONTROL */
{
  return wcmod_speed_intf_set(pc);
}

/*!
\brief Hold/Release/Read receiver sequencer (CDR) for a particular lane

\param  pc handle to current warpcore context (#wcmod_st)

\returns The value #SOC_E_NONE upon successful completion.

This function is used to enable or disable the clock data recovery (CDR)
sequencer for a particular lane by using bit 0 of #wcmod_st::per_lane_control. 

\li set to 0x1 to enable
\li Set to 0x0 disable CDR on selected lane.

This function is used to read the clock data recovery (CDR) sequencer state
for a particular lane by using bit 4 of #wcmod_st::per_lane_control. 

\li set to 0x1? to read the status and store it within #wcmod_st::accData.

This function reads/modifies the following registers:

\li DSC2B0_DSC_MISC_CTRL0 (0x821E)

<B>How to call:</B><br>
<p>Call this function directly or through the function
#wcmod_tier1_selector() with the upper-case string "RX_SEQ_CONTROL".</p>
*/

int wcmod_rx_seq_control(wcmod_st* pc)         /* START_RX_SEQ or STOP_RX_SEQ */
{
  uint16 data, prevStat;

  int cntl = pc->per_lane_control;

  if (cntl & 0x10) {
    if        (pc->this_lane == 0) { /*************** LANE 0 *****************/
      SOC_IF_ERROR_RETURN(READ_WC40_RX0_ANARXCONTROLr(pc->unit,pc,&prevStat));

      /* enable sync status reporting in the status reg.*/
      SOC_IF_ERROR_RETURN (MODIFY_WC40_RX0_ANARXCONTROLr(pc->unit, pc,
                                     RX0_ANARXCONTROL_STATUS_SEL_sigdetStatus,
                                     RX0_ANARXCONTROL_STATUS_SEL_MASK));
      SOC_IF_ERROR_RETURN(READ_WC40_RX0_ANARXSTATUSr(pc->unit, pc, &data));
      if (data & RX0_ANARXSTATUS_SIGDET_STATUS_RXSEQDONE_MASK) {
        pc->accData = 1;
      } else {
        pc->accData = 0;
      }
      /* restore original contents for AnaRxStatus */
      SOC_IF_ERROR_RETURN (MODIFY_WC40_RX0_ANARXCONTROLr(pc->unit, pc, prevStat,
                                       RX0_ANARXCONTROL_STATUS_SEL_MASK));
    } else if (pc->this_lane == 1) { /*************** LANE 1 *****************/
      SOC_IF_ERROR_RETURN(READ_WC40_RX1_ANARXCONTROLr(pc->unit,pc,&prevStat));

      /* enable sync status reporting in the status reg.*/
      SOC_IF_ERROR_RETURN (MODIFY_WC40_RX1_ANARXCONTROLr(pc->unit, pc,
                                     RX1_ANARXCONTROL_STATUS_SEL_sigdetStatus,
                                     RX1_ANARXCONTROL_STATUS_SEL_MASK));
      SOC_IF_ERROR_RETURN(READ_WC40_RX1_ANARXSTATUSr(pc->unit, pc, &data));
      if (data & RX1_ANARXSTATUS_SIGDET_STATUS_RXSEQDONE_MASK) {
        pc->accData = 1;
      } else {
        pc->accData = 0;
      }
      /* restore original contents for AnaRxStatus */
      SOC_IF_ERROR_RETURN (MODIFY_WC40_RX1_ANARXCONTROLr(pc->unit, pc, prevStat,
                                       RX1_ANARXCONTROL_STATUS_SEL_MASK));
    } else if (pc->this_lane == 2) { /*************** LANE 2 *****************/
      SOC_IF_ERROR_RETURN(READ_WC40_RX2_ANARXCONTROLr(pc->unit,pc,&prevStat));

      /* enable sync status reporting in the status reg.*/
      SOC_IF_ERROR_RETURN (MODIFY_WC40_RX2_ANARXCONTROLr(pc->unit, pc,
                                     RX2_ANARXCONTROL_STATUS_SEL_sigdetStatus,
                                     RX2_ANARXCONTROL_STATUS_SEL_MASK));
      SOC_IF_ERROR_RETURN(READ_WC40_RX2_ANARXSTATUSr(pc->unit, pc, &data));
      if (data & RX2_ANARXSTATUS_SIGDET_STATUS_RXSEQDONE_MASK) {
        pc->accData = 1;
      } else {
        pc->accData = 0;
      }
      /* restore original contents for AnaRxStatus */
      SOC_IF_ERROR_RETURN (MODIFY_WC40_RX2_ANARXCONTROLr(pc->unit, pc, prevStat,
                                       RX2_ANARXCONTROL_STATUS_SEL_MASK));
    } else if (pc->this_lane == 3) { /*************** LANE 3 *****************/
      SOC_IF_ERROR_RETURN(READ_WC40_RX3_ANARXCONTROLr(pc->unit,pc,&prevStat));

      /* enable sync status reporting in the status reg.*/
      SOC_IF_ERROR_RETURN (MODIFY_WC40_RX3_ANARXCONTROLr(pc->unit, pc,
                                     RX3_ANARXCONTROL_STATUS_SEL_sigdetStatus,
                                     RX3_ANARXCONTROL_STATUS_SEL_MASK));
      SOC_IF_ERROR_RETURN(READ_WC40_RX3_ANARXSTATUSr(pc->unit, pc, &data));
      if (data & RX3_ANARXSTATUS_SIGDET_STATUS_RXSEQDONE_MASK) {
        pc->accData = 1;
      } else {
        pc->accData = 0;
      }
      /* restore original contents for AnaRxStatus */
      SOC_IF_ERROR_RETURN (MODIFY_WC40_RX3_ANARXCONTROLr(pc->unit, pc, prevStat,
                                       RX3_ANARXCONTROL_STATUS_SEL_MASK));
    }
  } else if (cntl & 0x1) { 
    SOC_IF_ERROR_RETURN (MODIFY_WC40_DSC2B0_DSC_MISC_CTRL0r(pc->unit, pc, 0,
                                        DSC2B0_DSC_MISC_CTRL0_RXSEQSTART_MASK));
  } else if (cntl == 0x0) {
    SOC_IF_ERROR_RETURN (MODIFY_WC40_DSC2B0_DSC_MISC_CTRL0r(pc->unit, pc,
                                        DSC2B0_DSC_MISC_CTRL0_RXSEQSTART_MASK,
                                        DSC2B0_DSC_MISC_CTRL0_RXSEQSTART_MASK));
  } else {
    printf("%-22s: Error: invalid per_lane_control %d\n",FUNCTION_NAME(), cntl);
    return SOC_E_ERROR;
  }
  return SOC_E_NONE;
}

/*!
\brief Performs soft reset of selected lanes.

\param  pc handle to current warpcore context (#wcmod_st)

\returns The value #SOC_E_NONE upon successful completion.

\details

This function performs soft reset on select lane(s) via #wcmod_st::lane_select.
This is not a hard (IO based) reset. Softresets reset all PCS registers to
default. In combo mode, #wcmod_st::lane_select is ignored (considered 0).

In independent mode, the reset bits is self clearing. In combo mode reset is
done by setting IEEECONTROL1 bit, and <b>not</b> by resetting all 4 lanes.

The 1st, 2nd, 3rd and 4th byte of #wcmod_st::per_lane_control is associated
with lanes 0, 1, 2, and 3 respectively. The bits of each byte control their
lanes as follows.

\li 1:1 : soft reset applied to RX
\li 0:0 : soft reset applied to TX

To apply softreset to all 4 lanes, you would set #wcmod_st::per_lane_control to
0x03030303

This function adds a 2 microsecond delay for the reset to take effect.

This function modifies the following registers:

\li XGXSBLK0_MMDSELECT (0x800D)
\li IEEE0BLK_MIICNTL   (0x0000)

<B>How to call:</B><br>
<p>Call this function directly or through the function
#wcmod_tier1_selector() with the upper-case string "SET_SOFT_RESET".</p>
*/

int wcmod_soft_reset(wcmod_st* pc)                      /* SET_SOFT_RESET */
{
  uint16 data16, mask16;
  int cntl;

  cntl = (pc->per_lane_control & (0xff << pc->this_lane * 8)) >> pc->this_lane * 8;
  data16 = 0;
  mask16 = 0;

  if (cntl & 0x80) {
      data16 = (cntl & 0x01) << DIGITAL5_MISC6_RESET_TX_ASIC_SHIFT;
      mask16 = DIGITAL5_MISC6_RESET_TX_ASIC_MASK;
  }
  if (cntl & 0x40) {
      data16 |= ((cntl & 0x02) >> 1) << DIGITAL5_MISC6_RESET_RX_ASIC_SHIFT;
      mask16 |= DIGITAL5_MISC6_RESET_RX_ASIC_MASK;
  }
  if ((cntl & 0x80) | (cntl & 0x40)) {
    if (pc->verbosity > 0) printf("%s soft reset. Lane:%d %0d cntl:%x\n", FUNCTION_NAME(),pc->this_lane, pc->lane_select, cntl);

    SOC_IF_ERROR_RETURN(MODIFY_WC40_DIGITAL5_MISC6r(pc->unit,pc,data16,mask16));
  }

  return SOC_E_NONE;
}

/*!
\brief Read the 16 bit rev. id value etc.

\param  pc handle to current warpcore context (#wcmod_st)

\returns The value #SOC_E_NONE upon successful completion.  The revision id is
stored within #wcmod_st::accData.

\details
This  fucntion reads the revid register that contains core number, revision
number and stores the 16-bit value in the field #wcmod_st::accData.

<B>How to call:</B><br>
<p>Call this function directly or through the function
#wcmod_tier1_selector() with the upper-case string "REVID_READ".</p>
*/
int wcmod_revid_read(wcmod_st* pc)              /* REVID_READ */
{
  uint16 data;
  SOC_IF_ERROR_RETURN (READ_WC40_SERDESID_SERDESID0r(pc->unit,pc,&data));
  pc->accData = data;
  return SOC_E_NONE;
}

/*!
\brief Control per lane clause 72 auto tuning function  (training patterns)

\param  pc handle to current warpcore context (#wcmod_st)

\returns The value #SOC_E_NONE upon successful completion

\details

This function enables/disables clause-72 auto-tuning per lane.  It is used
in 40G_KR4, 40G_CR4, 10G_KR modes etc.
Lane is selected via #wcmod_st::lane_select.
This Function is controlled by #wcmod_st::per_lane_control as follows.

\li 0Xxxxxxxx1 : Enable  CL72 hardware block
\li 0Xxxxxxxx0 : Disable CL72 hardware block
\li 0Xxxxxxx10 : Enable  CL72 without AN in Micro (auto-CL72)
\li 0Xxxxxxx00 : Disable CL72 without AN in Micro (auto-CL72)

Note that auto-CL72 also enables CL72 hardware block. (No point in disabling
the hardware and enabling the micro).

This function modifies the following registers:

\li PMD_IEEE9BLK_TENGBASE_KR_PMD_CONTROL_REGISTER_150 (0x0800_0096)
\li CL72_USER_B0_CL72_MISC1_CONTROL                   (0x82E3)

<B>How to call:</B><br>
<p>Call this function directly or through the function
#wcmod_tier1_selector() with the upper-case string
"CLAUSE_72_CONTROL".</p>
*/

int wcmod_clause72_control(wcmod_st* pc)                /* CLAUSE_72_CONTROL */
{
  int blkCntl, uCntl;
  uint16 mask, data;
  int timeout=0;
  int lane_select ;
  const int UCODE_CMDREAD_TIMEOUT = 100;
  /*wcmod_spd_intfc_set spd_intf = pc->spd_intf; */

  blkCntl = pc->per_lane_control & 0x1;
  uCntl  = pc->per_lane_control & 0x10;


  /* first check if 6 canned speed */
  /*if ((pc->spd_intf == WCMOD_SPD_PCSBYP_10P3125) || (pc->spd_intf == WCMOD_SPD_PCSBYP_10P9375)
      || (pc->spd_intf == WCMOD_SPD_PCSBYP_11P5) || (pc->spd_intf == WCMOD_SPD_PCSBYP_12P5)
      || (pc->spd_intf == WCMOD_SPD_CUSTOM_BYPASS) || (pc->spd_intf == WCMOD_SPD_PCSBYP_6P25G))*/ {
    /* if enable, then set the firmware_mode to be cl72_woAN */
    /* Wait if it is not ready to take a cmd. */
    do {
      SOC_IF_ERROR_RETURN(READ_WC40_DSC1B0_UC_CTRLr(pc->unit, pc, &data));
    } while (!(data & DSC1B0_UC_CTRL_READY_FOR_CMD_MASK)
            && (++timeout < UCODE_CMDREAD_TIMEOUT));

    /* clear error bit */
    SOC_IF_ERROR_RETURN(MODIFY_WC40_DSC1B0_UC_CTRLr(pc->unit, pc, 0x0,
                                          (DSC1B0_UC_CTRL_ERROR_FOUND_MASK)));

    mask = (UC_INFO_B1_FIRMWARE_MODE_LN0_MODE_MASK       << (pc->this_lane * 4));
    data = (UC_INFO_B1_FIRMWARE_MODE_LN0_MODE_CL72_NO_AN << (pc->this_lane * 4));
    if (blkCntl || uCntl) {
         MODIFY_WC40_UC_INFO_B1_FIRMWARE_MODEr(pc->unit, pc, data, mask);
    } else {
        /* restore the original firmware mode */
        data = (pc->firmware_mode) << (pc->this_lane * 4) ; 
        MODIFY_WC40_UC_INFO_B1_FIRMWARE_MODEr(pc->unit, pc, data, mask);
    }
  }

  lane_select = pc->lane_select ;
  if( pc->port_type == WCMOD_COMBO ) {
      /* for combo mode, do BCST */
      pc->lane_select = WCMOD_LANE_BCST ;
  } else {
      /* for dual port mode, do dual bcst as well */
      if( pc->dual_type ) {
          /* may not be needed */
      }
  }

  /* Block Control. Turn it on if requested */
  if (blkCntl || uCntl) {
    MODIFY_WC40_PMD_IEEE9BLK_TENGBASE_KR_PMD_CONTROL_REGISTER_150r(pc->unit, pc,
       PMD_IEEE9BLK_TENGBASE_KR_PMD_CONTROL_REGISTER_150_TRAINING_ENABLE_MASK,
       PMD_IEEE9BLK_TENGBASE_KR_PMD_CONTROL_REGISTER_150_TRAINING_ENABLE_MASK);
    data = CL72_USERB0_CL72_MISC2_CONTROL_DIS_MAX_WAIT_TIMER_FORCEVAL_MASK | 
           CL72_USERB0_CL72_MISC2_CONTROL_DIS_MAX_WAIT_TIMER_FORCE_MASK; 
    MODIFY_WC40_CL72_USERB0_CL72_MISC2_CONTROLr(pc->unit,pc,data, data);
    /* disable TXFIR force bit */
    MODIFY_WC40_CL72_USERB0_CL72_TX_FIR_TAP_REGISTERr(pc->unit, pc, 0, 
        CL72_USERB0_CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_FORCE_MASK);
  } else if (!blkCntl && !uCntl) {
    MODIFY_WC40_PMD_IEEE9BLK_TENGBASE_KR_PMD_CONTROL_REGISTER_150r(pc->unit,pc,0,
        PMD_IEEE9BLK_TENGBASE_KR_PMD_CONTROL_REGISTER_150_TRAINING_ENABLE_MASK);
  }

  mask = CL72_USERB0_CL72_MISC1_CONTROL_LINK_CONTROL_FORCE_MASK |
           CL72_USERB0_CL72_MISC1_CONTROL_LINK_CONTROL_FORCEVAL_MASK;

  if (blkCntl || uCntl) {
    MODIFY_WC40_CL72_USERB0_CL72_MISC1_CONTROLr(pc->unit, pc, mask, mask);
  } else if (!blkCntl && !uCntl) {
    MODIFY_WC40_CL72_USERB0_CL72_MISC1_CONTROLr(pc->unit, pc, 0, mask);
  }

  /* wait for cmd to be read by micro. i.e it is ready to take next cmd */
  timeout = 0;
  do {
    SOC_IF_ERROR_RETURN(READ_WC40_DSC1B0_UC_CTRLr(pc->unit, pc, &data));
  } while (!(data & DSC1B0_UC_CTRL_READY_FOR_CMD_MASK)
            && (++timeout < UCODE_CMDREAD_TIMEOUT));
  
  /* check for error */
  SOC_IF_ERROR_RETURN(READ_WC40_DSC1B0_UC_CTRLr(pc->unit, pc, &data));
  if (data & DSC1B0_UC_CTRL_ERROR_FOUND_MASK) {
    printf("%-22s: Micro reported error\n", FUNCTION_NAME());
  }

  pc->lane_select = lane_select ;
 
#ifdef _SDK_WCMOD_
    sal_usleep(1000);  /* 1ms */
#endif

  return SOC_E_NONE;
}

/*!
\brief Check PRBS sync status

\param  pc handle to current warpcore context (#wcmod_st)

\returns #SOC_E_NONE if prbs lock with no errors; #SOC_E_TIMEOUT for a timeout,
meaning that either the prbs did not lock, or that prbs had errors, or both

\details
This function reads from one of  the RX[n]_ANARXSTATUS register (where n
indicates the lane number) until the value reads 0x8000, meaning that prbs has
locked and there are no prbs errors, OR until there is a timeout, which is set
inside of the function nominally as 10 reads.  This function only looks for a
lock for the lane indicated in the lane_select field of the wcmod_st data
struct.

The errors are stored in #wcmod_st::accData, if any.

Set ANARXCONTROL[n] register to select prbs status to show up in ANARXSTATUS[n].

Read ANARXSTATUS[n] until prbslock and no errors are indicated OR until timeout.

\li RX0_ANARXCONTROL (0x80B1)
\li RX1_ANARXCONTROL (0x80C1)
\li RX2_ANARXCONTROL (0x80D2)
\li RX3_ANARXCONTROL (0x80E3)

\li RX0_ANARXSTATUS (0x80B0)
\li RX1_ANARX STATUS (0x80C0)
\li RX2_ANARX STATUS (0x80D0)
\li RX3_ANARX STATUS (0x80E0)


<B>How to call:</B><br>
<p>Call this function directly or through the function #wcmod_tier1_selector()
with the upper-case string "PRBS_CHECK".</p>

*/
int wcmod_prbs_check(wcmod_st* pc)
{
  uint16 data, old_status;

  /* Lane 0 */
  if (pc->this_lane == 0) {
    SOC_IF_ERROR_RETURN(READ_WC40_RX0_ANARXCONTROLr(pc->unit, pc, &old_status));

    /* ensure AnaRxStatus reads the values for PRBS */
    SOC_IF_ERROR_RETURN (MODIFY_WC40_RX0_ANARXCONTROLr(pc->unit, pc,
                   RX0_ANARXCONTROL_STATUS_SEL_prbsStatus,
                   RX0_ANARXCONTROL_STATUS_SEL_MASK));

    SOC_IF_ERROR_RETURN(READ_WC40_RX0_ANARXSTATUSr(pc->unit, pc, &data));

    /* check for errors and also lost of lock */
    if (data & RX0_ANARXSTATUS_PRBS_STATUS_PRBS_LOCK_MASK) {
      /* next check the sticky bit for the lost of lock */
      if (data & RX0_ANARXSTATUS_PRBS_STATUS_PRBS_STKY_MASK) {
       pc->accData = -2;
       if (pc->verbosity >0 ) printf("%-22s: PRBS lost lock %04x\n", FUNCTION_NAME(), data);
      } else {
        pc->accData = (data & RX0_ANARXSTATUS_PRBS_STATUS_PRBS_ERRORS_MASK) >> 1;
        if (pc->verbosity >0 ) printf("%-22s: Error bits set in PRBS: %04x\n", FUNCTION_NAME(), data);
      }
    } else {
      pc->accData = -1;
       if (pc->verbosity >0 ) printf("%-22s: PRBS not locked %04x\n", FUNCTION_NAME(), data);

    }

    /* write the original contents for AnaRxStatus */
    SOC_IF_ERROR_RETURN (MODIFY_WC40_RX0_ANARXCONTROLr(pc->unit, pc, old_status,
                     RX0_ANARXCONTROL_STATUS_SEL_MASK));

    return SOC_E_NONE;
  } else if (pc->this_lane == 1) {
  /* check if we are using 2 or 4 lanes per port */
    SOC_IF_ERROR_RETURN(READ_WC40_RX1_ANARXCONTROLr(pc->unit, pc, &old_status));

    /* ensure AnaRxStatus reads the values for PRBS */
    SOC_IF_ERROR_RETURN (MODIFY_WC40_RX1_ANARXCONTROLr(pc->unit, pc,
                     RX1_ANARXCONTROL_STATUS_SEL_prbsStatus,
                     RX1_ANARXCONTROL_STATUS_SEL_MASK));

    SOC_IF_ERROR_RETURN(READ_WC40_RX1_ANARXSTATUSr(pc->unit, pc, &data));

    /* check for errors. */
    if (data & RX1_ANARXSTATUS_PRBS_STATUS_PRBS_LOCK_MASK) {
      /* next check the sticky bit for the lost of lock */
      if (data & RX1_ANARXSTATUS_PRBS_STATUS_PRBS_STKY_MASK) {
       pc->accData = -2;
       if (pc->verbosity >0 ) printf("%-22s: PRBS lost lock %04x\n", FUNCTION_NAME(), data);
      } else {
        pc->accData = (data & RX1_ANARXSTATUS_PRBS_STATUS_PRBS_ERRORS_MASK) >> 1;
        if (pc->verbosity >0 ) printf("%-22s: Error bits set in PRBS: %04x\n", FUNCTION_NAME(), data);
      }
    } else {
      pc->accData = -1;
       if (pc->verbosity >0 ) printf("%-22s: PRBS not locked %04x\n", FUNCTION_NAME(), data);

    }

    /* write the original contents for AnaRxStatus */
    SOC_IF_ERROR_RETURN (MODIFY_WC40_RX1_ANARXCONTROLr(pc->unit, pc, old_status,
                     RX1_ANARXCONTROL_STATUS_SEL_MASK));

    return SOC_E_NONE;
  } else if (pc->this_lane == 2) {
  /* check if we are using 4 lanes per port */
    SOC_IF_ERROR_RETURN(READ_WC40_RX2_ANARXCONTROLr(pc->unit, pc, &old_status));

    /* ensure AnaRxStatus reads the values for PRBS */
    SOC_IF_ERROR_RETURN(MODIFY_WC40_RX2_ANARXCONTROLr(pc->unit, pc,
                     RX2_ANARXCONTROL_STATUS_SEL_prbsStatus,
                     RX2_ANARXCONTROL_STATUS_SEL_MASK));

    SOC_IF_ERROR_RETURN(READ_WC40_RX2_ANARXSTATUSr(pc->unit, pc, &data));

    /* check for errors. */
    if (data & RX2_ANARXSTATUS_PRBS_STATUS_PRBS_LOCK_MASK) {
      /* next check the sticky bit for the lost of lock */
      if (data & RX2_ANARXSTATUS_PRBS_STATUS_PRBS_STKY_MASK) {
       pc->accData = -2;
       if (pc->verbosity >0 ) printf("%-22s: PRBS lost lock %04x\n", FUNCTION_NAME(), data);
      } else {
        pc->accData = (data & RX2_ANARXSTATUS_PRBS_STATUS_PRBS_ERRORS_MASK) >> 1;
        if (pc->verbosity >0 ) printf("%-22s: Error bits set in PRBS: %04x\n", FUNCTION_NAME(), data);
      }
    } else {
      pc->accData = -1;
       if (pc->verbosity >0 ) printf("%-22s: PRBS not locked %04x\n", FUNCTION_NAME(), data);
    }
    /* write the original contents for AnaRxStatus */
    SOC_IF_ERROR_RETURN (MODIFY_WC40_RX2_ANARXCONTROLr(pc->unit, pc,old_status,
                     RX2_ANARXCONTROL_STATUS_SEL_MASK));
    return SOC_E_NONE;
  } else if (pc->this_lane == 3) {
  /* check if we are using 4 lanes per port */
    SOC_IF_ERROR_RETURN(READ_WC40_RX3_ANARXCONTROLr(pc->unit, pc, &old_status));

    /* ensure AnaRxStatus reads the values for PRBS */
    SOC_IF_ERROR_RETURN(MODIFY_WC40_RX3_ANARXCONTROLr(pc->unit, pc,
                     RX3_ANARXCONTROL_STATUS_SEL_prbsStatus,
                     RX3_ANARXCONTROL_STATUS_SEL_MASK));

    SOC_IF_ERROR_RETURN(READ_WC40_RX3_ANARXSTATUSr(pc->unit, pc, &data));

    /* check for errors. */
    if (data & RX3_ANARXSTATUS_PRBS_STATUS_PRBS_LOCK_MASK) {
      /* next check the sticky bit for the lost of lock */
      if (data & RX3_ANARXSTATUS_PRBS_STATUS_PRBS_STKY_MASK) {
       pc->accData = -2;
       if (pc->verbosity >0 ) printf("%-22s: PRBS lost lock %04x\n", FUNCTION_NAME(), data);
      } else {
        pc->accData = (data & RX3_ANARXSTATUS_PRBS_STATUS_PRBS_ERRORS_MASK) >> 1;
        if (pc->verbosity >0 ) printf("%-22s: Error bits set in PRBS: %04x\n", FUNCTION_NAME(), data);
      }
    } else {
      pc->accData = -1;
       if (pc->verbosity >0 ) printf("%-22s: PRBS not locked %04x\n", FUNCTION_NAME(), data);
    }

    /* write the original contents for AnaRxStatus */
    SOC_IF_ERROR_RETURN (MODIFY_WC40_RX3_ANARXCONTROLr(pc->unit, pc,old_status,
                     RX3_ANARXCONTROL_STATUS_SEL_MASK));
    return SOC_E_NONE;
  } else {
    printf("%-22s: Error: Bad lane: %d\n",FUNCTION_NAME(),pc->this_lane);
    return SOC_E_ERROR;
  }
  return SOC_E_NONE;
}

/*!
\brief program the prbs settings

\param  pc handle to current warpcore context (#wcmod_st)

\returns The vluae #SOC_E_NONE upon successful completion

Some side effects of this are.

<ol>
<li> Disable clause-36 PCS for RX and TX on all lanes
<li> Disable comma detect
<li> Disable 8b10b encoding decoding
</ol>

PRBS settings are associated with the XgxsBlk1_lanePrbs register located at
0x8019. To change the prbs settings, set the #wcmod_st::lane_select field to the
desired lane number(s) and set the #wcmod_st::per_lane_control field to the
desired lane number. Only the prbs settings for the lane number as indicated by
the #wcmod_st::lane_select field are updated;  prbs settings for the other lanes
are not affected by a call to this function.

The #wcmod_st::per_lane_control field is interpreted as 16-bit value as follows

<table cellspacing=0>

<tr><td colspan=3><B>'per_lane_control' bit-mappings</B></td></tr>
<tr><td><B>Lane</B></td> <td><B>Bit(s)</B></td> <td><B>Purpose</B></td> </tr>
<tr><td><B>3</B></td><td>15   </td><td>enable(1) or disable(0) prbs</td> </tr>
<tr><td>        </td><td>14   </td><td>invert(1) or do-not-invert(0) prbs polynomail</td> </tr>
<tr><td>        </td><td>13:12</td><td>select polynomial order. PRBS7(0),PRBS15(1),PRBS23(2), or PRBS31(3)</td> </tr>
<tr><td><B>2</B></td><td>11   </td><td>enable(1) or disable(0) prbs</td> </tr>
<tr><td>        </td><td>10   </td><td>invert(1) or do-not-invert(0) prbs polynomail</td> </tr>
<tr><td>        </td><td>9:8  </td><td>select polynomial order. PRBS7(0), PRBS15(1), PRBS23(2), or PRBS31(3)</td> </tr>
<tr><td><B>1</B></td><td>7    </td><td>enable(1) or disable(0) prbs</td> </tr>
<tr><td>        </td><td>6    </td><td>invert(1) or do-not-invert(0) prbs polynomail</td> </tr>
<tr><td>        </td><td>5:4  </td><td>select polynomial order. PRBS7(0), PRBS15(1), PRBS23(2), or PRBS31(3)</td> </tr>
<tr><td><B>0</B></td><td>3    </td><td>enable(1) or disable(0) prbs</td> </tr>
<tr><td>        </td><td>2    </td><td>invert(1) or do-not-invert(0) prbs polynomail</td> </tr>
<tr><td>        </td><td>1:0  </td><td>select polynomial order. PRBS7(0), PRBS15(1), PRBS23(2), or PRBS31(3)</td> </tr>

</table>


The 1st, 2nd, 3rd and 4th byte of #wcmod_st::per_lane_control is associated
with lanes 0, 1, 2, and 3 respectively. The bits of each byte control their
lanes as follows.

\li 3:3 : enable(1) or disable(0) prbs
\li 2:2 : invert (1) or do-not-invert(0) prbs polynomail
\li 0:1 : select PRBS polynomial. 7(0x0), 15(0x1), 23(0x2), 31(0x3)

Calls to this function disables clause-36 PCS for RX and TX on all lanes
(XgxsBlk1_laneCtrl0 at 0x8015 bits 0 through 7 are zeroed).  Bits 2 and 3 of
XgxsBlk0_xgxsControl at 0x8000 are disabled as well -- this turns off 8B/10B
encoding and comma-detect. Moreover 1G comma detect and 1G 8B/10B encoding are
disabled by this function call for all lanes (XgxsBlk1_laneCtrl2 at 0x8017 has
bits 8 through 15 zeroed).


<B>How to call:</B><br>
<p>Call this function directly or through the function #wcmod_tier1_selector()
with the upper-case string "PRBS_CONTROL".</p>

*/
int wcmod_prbs_control(wcmod_st* pc)                /* PRBS_CONTROL */
{
  uint16 data16, mask16, mask;
  uint16 lane_ctrl0_mask = 0;
  uint16 lane_ctrl1_mask = 0;
  uint16 lane_ctrl2_mask = 0;

  mask = XGXSBLK1_LANEPRBS_PRBS_EN0_MASK | XGXSBLK1_LANEPRBS_PRBS_EN1_MASK |
         XGXSBLK1_LANEPRBS_PRBS_EN2_MASK | XGXSBLK1_LANEPRBS_PRBS_EN3_MASK;
  mask16 = (0xf << (pc->this_lane * 4));
  data16 = ((pc->per_lane_control & (0x0f << pc->this_lane * 8))
                                                        >> pc->this_lane * 4);
  if (pc->verbosity > 0) printf("%-22s data:%x mask:%x\n", FUNCTION_NAME(), data16, mask16);
  pc->lane_num_ignore = 1;

  SOC_IF_ERROR_RETURN (MODIFY_WC40_XGXSBLK0_XGXSCONTROLr(pc->unit, pc, 0,
                       XGXSBLK0_XGXSCONTROL_CDET_EN_MASK |
                       XGXSBLK0_XGXSCONTROL_EDEN_MASK));      /* per core */

  /* This function is called per lane by upper layer*/
  lane_ctrl0_mask  = ((1 << XGXSBLK1_LANECTRL0_CL36_PCS_EN_RX_SHIFT) << (pc->this_lane)) |
                     ((1 << XGXSBLK1_LANECTRL0_CL36_PCS_EN_TX_SHIFT) << (pc->this_lane));
  lane_ctrl2_mask  = ((1 << XGXSBLK1_LANECTRL2_CDET_EN1G_SHIFT) << (pc->this_lane)) |
                     ((1 << XGXSBLK1_LANECTRL2_EDEN1G_SHIFT) << (pc->this_lane));

  lane_ctrl1_mask = (XGXSBLK1_LANECTRL1_TX1G_MODE_LN0_MASK | XGXSBLK1_LANECTRL1_RX1G_MODE_LN0_MASK)
                      << ((pc->this_lane) * 2);

  /* for XFI external loopback prbs, reg 0x8016 needs to be set to a value of 0xffff */
  if (data16 & mask) {
      /* 4 bits, one bit per lane */
      SOC_IF_ERROR_RETURN (MODIFY_WC40_XGXSBLK1_LANECTRL0r(pc->unit, pc, 0, lane_ctrl0_mask));
      SOC_IF_ERROR_RETURN (MODIFY_WC40_XGXSBLK1_LANECTRL2r(pc->unit, pc, 0, lane_ctrl2_mask));
      SOC_IF_ERROR_RETURN (MODIFY_WC40_XGXSBLK1_LANECTRL1r(pc->unit, pc,
                           (XGXSBLK1_LANECTRL1_TX1G_MODE_LN0_DWSDR_div1 |
                           (XGXSBLK1_LANECTRL1_RX1G_MODE_LN0_DWSDR_div1 << XGXSBLK1_LANECTRL1_RX1G_MODE_LN0_SHIFT))
                           << ((pc->this_lane) * 2), lane_ctrl1_mask));

      /*for the warpcore 8.25 and 3.3 os mode, need to enable cya bit */
        if (pc->model_type == WCMOD_WC_C0) {
            uint16 data_tmp = 0;
            /*first check speed less than 2.5G */
            switch (pc->this_lane) {
            case 0:
                SOC_IF_ERROR_RETURN
                     (READ_WC40_GP2_REG_GP2_2r(pc->unit, pc, &data_tmp));
                data_tmp &= 0x3f;
                break;
            case 1:
                SOC_IF_ERROR_RETURN
                     (READ_WC40_GP2_REG_GP2_2r(pc->unit, pc, &data_tmp));
                data_tmp &= 0x3f00;
                data_tmp >>= 8;
                break;
            case 2:
                SOC_IF_ERROR_RETURN
                     (READ_WC40_GP2_REG_GP2_3r(pc->unit, pc, &data_tmp));
                data_tmp &= 0x3f;
                break;
            case 3:
                SOC_IF_ERROR_RETURN
                     (READ_WC40_GP2_REG_GP2_3r(pc->unit, pc, &data_tmp));
                data_tmp &= 0x3f00;
                data_tmp >>= 8;
                break;
            default:
                break;
            }
            if (data_tmp <= 0x3) {
            /*need to  enable cya bit */
            
                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_DIGITAL5_MISC6r(pc->unit, pc,
                                                  DIGITAL5_MISC6_RX_OS8_OS3_ALT_BITS_CYA_MASK,
                                                  DIGITAL5_MISC6_RX_OS8_OS3_ALT_BITS_CYA_MASK));
                SOC_IF_ERROR_RETURN 
                    (MODIFY_WC40_XGXSBLK1_LANECTRL1r(pc->unit, pc, 0x0, lane_ctrl1_mask));
            }
        }                                                      
  } else {

     /* 4 bits, one bit per lane */
    SOC_IF_ERROR_RETURN (MODIFY_WC40_XGXSBLK1_LANECTRL1r(pc->unit, pc,
                           (XGXSBLK1_LANECTRL1_TX1G_MODE_LN0_SWSDR_div1 |
                           (XGXSBLK1_LANECTRL1_RX1G_MODE_LN0_SWSDR_div1 << XGXSBLK1_LANECTRL1_RX1G_MODE_LN0_SHIFT))
                           << ((pc->this_lane) * 2), lane_ctrl1_mask));

    /*for the warpcore 8.25 and 3.3 os mode, need to enable cya bit */
        if (pc->model_type == WCMOD_WC_C0) {
            uint16 data_tmp = 0;;
            /*first check speed less than 2.5G */
            switch (pc->this_lane) {
            case 0:
                SOC_IF_ERROR_RETURN
                     (READ_WC40_GP2_REG_GP2_2r(pc->unit, pc, &data_tmp));
                data_tmp &= 0x3f;
                break;
            case 1:
                SOC_IF_ERROR_RETURN
                     (READ_WC40_GP2_REG_GP2_2r(pc->unit, pc, &data_tmp));
                data_tmp &= 0x3f00;
                data_tmp >>= 8;
                break;
            case 2:
                SOC_IF_ERROR_RETURN
                     (READ_WC40_GP2_REG_GP2_3r(pc->unit, pc, &data_tmp));
                data_tmp &= 0x3f;
                break;
            case 3:
                SOC_IF_ERROR_RETURN
                     (READ_WC40_GP2_REG_GP2_3r(pc->unit, pc, &data_tmp));
                data_tmp &= 0x3f00;
                data_tmp >>= 8;
                break;
            default:
                break;
            }
            if (data_tmp <= 0x3) {
            /*need to  enable cya bit */
            
                SOC_IF_ERROR_RETURN
                    (MODIFY_WC40_DIGITAL5_MISC6r(pc->unit, pc, 0,
                                                  DIGITAL5_MISC6_RX_OS8_OS3_ALT_BITS_CYA_MASK));
            }
        }                                              
  }

  SOC_IF_ERROR_RETURN (MODIFY_WC40_XGXSBLK1_LANEPRBSr(pc->unit, pc,
                                                      data16, mask16));  /* 4 bits per lane */
  return SOC_E_NONE;
}

/*!
\brief This is the decoupled PRBS control Tier1

\param  pc handle to current warpcore context (#wcmod_st)

\returns #SOC_E_NONE upon successful completion

\details

Set TX and RX to different PRBS polys. Also supports PRBS9 and PRBS11. Uses
different set of registers from #wcmod_prbs_control.

The 1st, 2nd, 3rd and 4th byte of #wcmod_st::per_lane_control is associated
with lanes 0, 1, 2, and 3 respectively.

\li 7:7 : Set TX parameters
\li 6:6 : Set RX parameters
\li 5:5 : PRBS decouple enable or disable
\li 4:4 : PRBS enable or disable
\li 3:3 : PRBS inversion
\li 0:2 : PRBS order

<B>How to call:</B><br>
<p>Call this function directly or through the function #wcmod_tier1_selector() using the
case-sensitive string "PRBS_DECOUPLE_CONTROL".</p>

*/
int wcmod_prbs_decouple_control(wcmod_st* pc)        /* PRBS_DECOUPLE_CONTROL */
{
  uint16 data, mask;
  int tx_lane_0,rx_lane_0,prbs_order,  prbv_inv,  prbs_en,  prbs_decouple_en;
  int tx_lane_1,rx_lane_1,prbs_order_1,prbv_inv_1,prbs_en_1,prbs_decouple_en_1;
  int tx_lane_2,rx_lane_2,prbs_order_2,prbv_inv_2,prbs_en_2,prbs_decouple_en_2;
  int tx_lane_3,rx_lane_3,prbs_order_3,prbv_inv_3,prbs_en_3,prbs_decouple_en_3;
  int cntl, cntl_1, cntl_2, cntl_3;

  cntl = pc->per_lane_control;

  pc->lane_num_ignore = 1;
  /*disable the cdet/eden1G is prbs is enabled*/
  MODIFY_WC40_XGXSBLK1_LANECTRL2r(pc->unit, pc, 0,
                                  XGXSBLK1_LANECTRL2_CDET_EN1G_MASK |
                                  XGXSBLK1_LANECTRL2_EDEN1G_MASK);
  pc->lane_num_ignore = 0;

  tx_lane_0          = cntl & 0x80;
  rx_lane_0          = cntl & 0x40;
  prbs_decouple_en   = cntl & 0x20;
  prbs_en            = cntl & 0x10;
  prbv_inv           = cntl & 0x08;
  prbs_order         = cntl & 0x07;

  cntl_1 = (cntl & 0xff00) >> 8;
  tx_lane_1          = cntl_1 & 0x80;
  rx_lane_1          = cntl_1 & 0x40;
  prbs_decouple_en_1 = cntl_1 & 0x20;
  prbs_en_1          = cntl_1 & 0x10;
  prbv_inv_1         = cntl_1 & 0x08;
  prbs_order_1       = cntl_1 & 0x07;

  cntl_2 = (cntl & 0xff0000) >> 16;
  tx_lane_2          = cntl_2 & 0x80;
  rx_lane_2          = cntl_2 & 0x40;
  prbs_decouple_en_2 = cntl_2 & 0x20;
  prbs_en_2          = cntl_2 & 0x10;
  prbv_inv_2         = cntl_2 & 0x08;
  prbs_order_2       = cntl_2 & 0x07;

  cntl_3 = (cntl & 0xff000000) >> 24;
  tx_lane_3          = cntl_3 & 0x80;
  rx_lane_3          = cntl_3 & 0x40;
  prbs_decouple_en_3 = cntl_3 & 0x20;
  prbs_en_3          = cntl_3 & 0x10;
  prbv_inv_3         = cntl_3 & 0x08;
  prbs_order_3       = cntl_3 & 0x07;

  data=((prbs_order & 0x04) >> 2) << TX0_ANATXACONTROL5_TX_PRBS_ORDER_2_SHIFT |
         (prbs_decouple_en >> 5) << TX0_ANATXACONTROL5_TX_PRBS_DECOUPLE_EN_SHIFT |
         (prbs_en >> 4) << TX0_ANATXACONTROL5_TX_PRBS_EN_DECOUPLE_SHIFT |
         (prbv_inv >> 3) << TX0_ANATXACONTROL5_TX_PRBS_INV_DECOUPLE_SHIFT |
         (prbs_order & 0x03) << TX0_ANATXACONTROL5_TX_PRBS_ORDER_DECOUPLE_SHIFT;

  mask = TX0_ANATXACONTROL5_TX_PRBS_ORDER_2_MASK
        | TX0_ANATXACONTROL5_TX_PRBS_DECOUPLE_EN_MASK
        | TX0_ANATXACONTROL5_TX_PRBS_EN_DECOUPLE_MASK
        | TX0_ANATXACONTROL5_TX_PRBS_INV_DECOUPLE_MASK
        | TX0_ANATXACONTROL5_TX_PRBS_ORDER_DECOUPLE_MASK;

  if (tx_lane_0) {
    MODIFY_WC40_TX0_ANATXACONTROL5r(pc->unit, pc, data, mask);
  }

  data=((prbs_order & 4) >> 2) << RX0_ANARXCONTROL2_1G_PRBS_ORDER_RX_R_2_SHIFT |
        (prbs_decouple_en >> 5) << RX0_ANARXCONTROL2_1G_PRBS_SEL_RX_R_SHIFT |
        (prbs_en >> 4) << RX0_ANARXCONTROL2_1G_PRBS_EN_RX_R_SHIFT |
        (prbv_inv >> 3) << RX0_ANARXCONTROL2_1G_PRBS_INV_RX_R_SHIFT |
        (prbs_order & 3) << RX0_ANARXCONTROL2_1G_PRBS_ORDER_RX_R_SHIFT;

  mask =  RX0_ANARXCONTROL2_1G_PRBS_ORDER_RX_R_2_MASK
        | RX0_ANARXCONTROL2_1G_PRBS_SEL_RX_R_MASK
        | RX0_ANARXCONTROL2_1G_PRBS_EN_RX_R_MASK
        | RX0_ANARXCONTROL2_1G_PRBS_INV_RX_R_MASK
        | RX0_ANARXCONTROL2_1G_PRBS_ORDER_RX_R_MASK;

  if (rx_lane_0) {
    MODIFY_WC40_RX0_ANARXCONTROL2_1Gr(pc->unit, pc, data, mask);
  }

  data=((prbs_order_1 & 4) >> 2) << TX1_ANATXACONTROL5_TX_PRBS_ORDER_2_SHIFT |
        (prbs_decouple_en_1 >> 5) << TX1_ANATXACONTROL5_TX_PRBS_DECOUPLE_EN_SHIFT |
        (prbs_en_1 >> 4) << TX1_ANATXACONTROL5_TX_PRBS_EN_DECOUPLE_SHIFT |
        (prbv_inv_1 >> 3) << TX1_ANATXACONTROL5_TX_PRBS_INV_DECOUPLE_SHIFT |
        (prbs_order_1 & 3) << TX1_ANATXACONTROL5_TX_PRBS_ORDER_DECOUPLE_SHIFT;

  mask = TX1_ANATXACONTROL5_TX_PRBS_ORDER_2_MASK
       | TX1_ANATXACONTROL5_TX_PRBS_DECOUPLE_EN_MASK
       | TX1_ANATXACONTROL5_TX_PRBS_EN_DECOUPLE_MASK
       | TX1_ANATXACONTROL5_TX_PRBS_INV_DECOUPLE_MASK
       | TX1_ANATXACONTROL5_TX_PRBS_ORDER_DECOUPLE_MASK;

  if (tx_lane_1) {
    MODIFY_WC40_TX1_ANATXACONTROL5r(pc->unit, pc, data, mask);
  }

  data=((prbs_order_1 & 4) >> 2) << RX1_ANARXCONTROL2_1G_PRBS_ORDER_RX_R_2_SHIFT |
        (prbs_decouple_en_1 >> 5) << RX1_ANARXCONTROL2_1G_PRBS_SEL_RX_R_SHIFT |
        (prbs_en_1 >> 4) << RX1_ANARXCONTROL2_1G_PRBS_EN_RX_R_SHIFT |
        (prbv_inv_1 >> 3) << RX1_ANARXCONTROL2_1G_PRBS_INV_RX_R_SHIFT |
        (prbs_order_1 & 3) << RX1_ANARXCONTROL2_1G_PRBS_ORDER_RX_R_SHIFT;

  mask = RX1_ANARXCONTROL2_1G_PRBS_ORDER_RX_R_2_MASK
        | RX1_ANARXCONTROL2_1G_PRBS_SEL_RX_R_MASK
        | RX1_ANARXCONTROL2_1G_PRBS_EN_RX_R_MASK
        | RX1_ANARXCONTROL2_1G_PRBS_INV_RX_R_MASK
        | RX1_ANARXCONTROL2_1G_PRBS_ORDER_RX_R_MASK;

  if (rx_lane_1) {
    MODIFY_WC40_RX1_ANARXCONTROL2_1Gr(pc->unit, pc, data, mask);
  }

  data=((prbs_order_2 & 4) >> 2) << TX2_ANATXACONTROL5_TX_PRBS_ORDER_2_SHIFT |
        (prbs_decouple_en_2 >> 5) << TX2_ANATXACONTROL5_TX_PRBS_DECOUPLE_EN_SHIFT |
        (prbs_en_2 >> 4) << TX2_ANATXACONTROL5_TX_PRBS_EN_DECOUPLE_SHIFT |
        (prbv_inv_2 >> 3) << TX2_ANATXACONTROL5_TX_PRBS_INV_DECOUPLE_SHIFT |
        (prbs_order_2 & 3) << TX2_ANATXACONTROL5_TX_PRBS_ORDER_DECOUPLE_SHIFT;

  mask = TX2_ANATXACONTROL5_TX_PRBS_ORDER_2_MASK 
       | TX2_ANATXACONTROL5_TX_PRBS_DECOUPLE_EN_MASK 
       | TX2_ANATXACONTROL5_TX_PRBS_EN_DECOUPLE_MASK 
       | TX2_ANATXACONTROL5_TX_PRBS_INV_DECOUPLE_MASK 
       | TX2_ANATXACONTROL5_TX_PRBS_ORDER_DECOUPLE_MASK;

  if (tx_lane_2) {
    MODIFY_WC40_TX2_ANATXACONTROL5r(pc->unit, pc, data, mask);
  }

  data=((prbs_order_2 & 4) >> 2) << RX2_ANARXCONTROL2_1G_PRBS_ORDER_RX_R_2_SHIFT |
         (prbs_decouple_en_2 >> 5) << RX2_ANARXCONTROL2_1G_PRBS_SEL_RX_R_SHIFT |
         (prbs_en_2 >> 4) << RX2_ANARXCONTROL2_1G_PRBS_EN_RX_R_SHIFT |
         (prbv_inv_2 >> 3) << RX2_ANARXCONTROL2_1G_PRBS_INV_RX_R_SHIFT |
         (prbs_order_2 & 3) << RX2_ANARXCONTROL2_1G_PRBS_ORDER_RX_R_SHIFT;

  mask = RX2_ANARXCONTROL2_1G_PRBS_ORDER_RX_R_2_MASK 
       | RX2_ANARXCONTROL2_1G_PRBS_SEL_RX_R_MASK 
       | RX2_ANARXCONTROL2_1G_PRBS_EN_RX_R_MASK 
       | RX2_ANARXCONTROL2_1G_PRBS_INV_RX_R_MASK 
       | RX2_ANARXCONTROL2_1G_PRBS_ORDER_RX_R_MASK;

  if (rx_lane_2) {
    MODIFY_WC40_RX2_ANARXCONTROL2_1Gr(pc->unit, pc, data, mask);
  }

  data=((prbs_order_3 & 4) >> 2) << TX3_ANATXACONTROL5_TX_PRBS_ORDER_2_SHIFT |
        (prbs_decouple_en_3 >> 5) << TX3_ANATXACONTROL5_TX_PRBS_DECOUPLE_EN_SHIFT |
        (prbs_en_3 >> 4) << TX3_ANATXACONTROL5_TX_PRBS_EN_DECOUPLE_SHIFT |
        (prbv_inv_3 >> 3) << TX3_ANATXACONTROL5_TX_PRBS_INV_DECOUPLE_SHIFT |
        (prbs_order_3 & 3) << TX3_ANATXACONTROL5_TX_PRBS_ORDER_DECOUPLE_SHIFT;

  mask = TX3_ANATXACONTROL5_TX_PRBS_ORDER_2_MASK 
       | TX3_ANATXACONTROL5_TX_PRBS_DECOUPLE_EN_MASK 
       | TX3_ANATXACONTROL5_TX_PRBS_EN_DECOUPLE_MASK 
       | TX3_ANATXACONTROL5_TX_PRBS_INV_DECOUPLE_MASK 
       | TX3_ANATXACONTROL5_TX_PRBS_ORDER_DECOUPLE_MASK;

  if (tx_lane_3) {
    MODIFY_WC40_TX3_ANATXACONTROL5r(pc->unit, pc, data, mask);
  }

  data=((prbs_order_3 & 4) >> 2) << RX3_ANARXCONTROL2_1G_PRBS_ORDER_RX_R_2_SHIFT |
        (prbs_decouple_en_3 >> 5) << RX3_ANARXCONTROL2_1G_PRBS_SEL_RX_R_SHIFT |
        (prbs_en_3 >> 4) << RX3_ANARXCONTROL2_1G_PRBS_EN_RX_R_SHIFT |
        (prbv_inv_3 >> 3) << RX3_ANARXCONTROL2_1G_PRBS_INV_RX_R_SHIFT |
        (prbs_order_3 & 3) << RX3_ANARXCONTROL2_1G_PRBS_ORDER_RX_R_SHIFT;

  mask = RX3_ANARXCONTROL2_1G_PRBS_ORDER_RX_R_2_MASK 
       | RX3_ANARXCONTROL2_1G_PRBS_SEL_RX_R_MASK 
       | RX3_ANARXCONTROL2_1G_PRBS_EN_RX_R_MASK 
       | RX3_ANARXCONTROL2_1G_PRBS_INV_RX_R_MASK 
       | RX3_ANARXCONTROL2_1G_PRBS_ORDER_RX_R_MASK;

  if (rx_lane_3) {
    MODIFY_WC40_RX3_ANARXCONTROL2_1Gr(pc->unit, pc, data, mask);
  }
  return SOC_E_NONE;
}

/*!
\brief Sets CJPAT/CRPAT parameters for a particular lane

\param  pc handle to current warpcore context (#wcmod_st)

\returns The value #SOC_E_NONE upon successful completion

This function enables either a CJPAT or CRPAT for a particular lane as
indicated by #wcmod_st::this_lane. The speed mode of the lane must be chosen
from the enumerated type #wcmod_spd_intfc_set and set in the
#wcmod_st::spd_intf field.  To enable CJPAT, set #wcmod_st::per_lane_control to
the value #USE_CJPAT; to enable CRPAT, set #wcmod_st::per_lane_control to the
value #USE_CRPAT.

This function modifies the following registers:

\li XGXSBLK0_XGXSCONTROL (0x8000)
\li DIGITAL4_MISC5       (0x833E)
\li LNRXBERT_RXBERTCTRL  (0x83E0)
\li RXBERT_RXBERTCTRL    (0x8030)

<B>How to call:</B><br>
<p>Call this function directly or through the function #wcmod_tier1_selector()
using the upper case string "CJPAT_CRPAT_CONTROL".</p>
*/

int wcmod_cjpat_crpat_control(wcmod_st* pc)     /* CJPAT_CRPAT_CONTROL  */
{
  uint16      data16, mask16;

  wcmod_spd_intfc_set spd_intf = pc->spd_intf;
  int cntl = pc->per_lane_control;

  /* Enable pgen_en, pcmp_en */
  if ((cntl & (USE_CJPAT | USE_CRPAT)) &&
      ((spd_intf == WCMOD_SPD_10000_HI) || (spd_intf == WCMOD_SPD_10000)         ||
      (spd_intf == WCMOD_SPD_12000_HI)   || (spd_intf == WCMOD_SPD_13000)         ||
      (spd_intf == WCMOD_SPD_16000)      || (spd_intf == WCMOD_SPD_20000)         ||
      (spd_intf == WCMOD_SPD_21000)      || (spd_intf == WCMOD_SPD_25455)         ||
      (spd_intf == WCMOD_SPD_31500)      || (spd_intf == WCMOD_SPD_15750_HI_DXGXS) ||
      (spd_intf == WCMOD_SPD_40G_XLAUI)  || (spd_intf == WCMOD_SPD_42G_XLAUI)     ||
      (spd_intf == WCMOD_SPD_R2_12000)   || (spd_intf == WCMOD_SPD_20000_HI_DXGXS) ||
      (spd_intf == WCMOD_SPD_40G_KR4)    || (spd_intf == WCMOD_SPD_21G_HI_DXGXS))) {
    data16 = XGXSBLK0_XGXSCONTROL_PGEN_EN_BITS
                << XGXSBLK0_XGXSCONTROL_PGEN_EN_SHIFT
            | XGXSBLK0_XGXSCONTROL_PCMP_EN_BITS
                << XGXSBLK0_XGXSCONTROL_PCMP_EN_SHIFT;
    mask16 =  XGXSBLK0_XGXSCONTROL_PGEN_EN_MASK
            | XGXSBLK0_XGXSCONTROL_PCMP_EN_MASK;

    MODIFY_WC40_XGXSBLK0_XGXSCONTROLr(pc->unit, pc, data16, mask16);
  } else {
    /*  mask16 = XGXSBLK0_XGXSCONTROL_PGEN_EN_MASK | */
    /*              XGXSBLK0_XGXSCONTROL_PCMP_EN_MASK; */
    /*  MODIFY_WC40_XGXSBLK0_XGXSCONTROLr(pc->unit, pc, 0, mask16); */
  }

  /* enable tgen_data_sel and rchk_en (receive checker CRC enable) */
  if ((cntl & (USE_CJPAT|USE_CRPAT)) && (  (spd_intf == WCMOD_SPD_10000_XFI) ||
      (spd_intf == WCMOD_SPD_11P5) || (spd_intf == WCMOD_SPD_12P5) ||
      (spd_intf == WCMOD_SPD_10000_SFI) || (spd_intf == WCMOD_SPD_10600_XFI_HG))){
    WRITE_WC40_DIGITAL4_MISC5r(pc->unit, pc, 0x2808);
    WRITE_WC40_LNRXBERT_RXBERTCTRLr(pc->unit, pc, 0x0200);
  }
  /* Enable crcChk_en, rxStartSel */
  if ((cntl & (USE_CJPAT | USE_CRPAT)) && ((spd_intf == WCMOD_SPD_10000_HI) ||
       (spd_intf == WCMOD_SPD_10000) || (spd_intf == WCMOD_SPD_12000_HI) ||
       (spd_intf == WCMOD_SPD_13000) || (spd_intf == WCMOD_SPD_16000) ||
       (spd_intf == WCMOD_SPD_20000) || (spd_intf == WCMOD_SPD_21000) ||
       (spd_intf == WCMOD_SPD_25455) || (spd_intf == WCMOD_SPD_31500) ||
       (spd_intf == WCMOD_SPD_40G_KR4) || (spd_intf == WCMOD_SPD_15750_HI_DXGXS) ||
       (spd_intf == WCMOD_SPD_20000_HI_DXGXS) ||
       (spd_intf == WCMOD_SPD_21G_HI_DXGXS) ||
       (spd_intf == WCMOD_SPD_40G_XLAUI) || (spd_intf == WCMOD_SPD_42G_XLAUI) ||
       (spd_intf == WCMOD_SPD_R2_12000))) {

    data16 = RXBERT_RXBERTCTRL_CRCCHK_EN_MASK | 
             RXBERT_RXBERTCTRL_COUNTRST_DIS_MASK;
    mask16 = RXBERT_RXBERTCTRL_CRCCHK_EN_MASK
                | RXBERT_RXBERTCTRL_COUNTRST_DIS_MASK;

   MODIFY_WC40_RXBERT_RXBERTCTRLr(pc->unit, pc, data16, mask16);
  } else {
 /*   mask16 = RXBERT_RXBERTCTRL_CRCCHK_EN_MASK | */
 /*               RXBERT_RXBERTCTRL_COUNTRST_DIS_MASK; */
 /*   MODIFY_WC40_RXBERT_RXBERTCTRLr(pc->unit, pc, 0, mask16); */
  }

  /* Enable CJPAT */
  if ((cntl & USE_CJPAT) && ((spd_intf == WCMOD_SPD_10000_HI) ||
        (spd_intf == WCMOD_SPD_10000) || (spd_intf == WCMOD_SPD_12000_HI) ||
        (spd_intf == WCMOD_SPD_13000) || (spd_intf == WCMOD_SPD_16000) ||
        (spd_intf == WCMOD_SPD_20000) || (spd_intf == WCMOD_SPD_21000) ||
        (spd_intf == WCMOD_SPD_25455) || (spd_intf == WCMOD_SPD_31500) ||
        (spd_intf == WCMOD_SPD_40G_KR4) || (spd_intf == WCMOD_SPD_15750_HI_DXGXS) ||
        (spd_intf == WCMOD_SPD_20000_HI_DXGXS) ||
        (spd_intf == WCMOD_SPD_21G_HI_DXGXS) ||
        (spd_intf == WCMOD_SPD_40G_XLAUI) || (spd_intf == WCMOD_SPD_42G_XLAUI) ||
        (spd_intf == WCMOD_SPD_R2_12000))) {
    data16 = TXBERT_TXBERTCTRL_CJPAT_EN_MASK | 
                    TXBERT_TXBERTCTRL_COUNTRST_DIS_MASK;
    mask16 = TXBERT_TXBERTCTRL_CJPAT_EN_MASK |
                    TXBERT_TXBERTCTRL_COUNTRST_DIS_MASK;
    MODIFY_WC40_TXBERT_TXBERTCTRLr(pc->unit, pc, data16, mask16);
  } else {
    /*mask16 = TXBERT_TXBERTCTRL_CJPAT_EN_MASK
            | TXBERT_TXBERTCTRL_COUNTRST_DIS_MASK;
      MODIFY_WC40_TXBERT_TXBERTCTRLr(pc->unit, pc, 0, mask16); */
  }

  if ((cntl & USE_CJPAT) && ((  spd_intf == WCMOD_SPD_10000_XFI) ||
      (spd_intf == WCMOD_SPD_11P5) || (spd_intf == WCMOD_SPD_12P5) ||
      (spd_intf == WCMOD_SPD_10000_SFI)  || (spd_intf == WCMOD_SPD_10600_XFI_HG))) {
    WRITE_WC40_LNTXBERT_TXBERTCTRLr(pc->unit, pc, 0x0042);
  }

  /* Enable CRPAT */
  if ((cntl & USE_CRPAT) && ((spd_intf == WCMOD_SPD_10000_HI) ||
        (spd_intf == WCMOD_SPD_10000) || (spd_intf == WCMOD_SPD_12000_HI) ||
        (spd_intf == WCMOD_SPD_13000) || (spd_intf == WCMOD_SPD_16000) ||
        (spd_intf == WCMOD_SPD_20000) || (spd_intf == WCMOD_SPD_21000) ||
        (spd_intf == WCMOD_SPD_25455) || (spd_intf == WCMOD_SPD_31500) ||
        (spd_intf == WCMOD_SPD_40G_KR4) || (spd_intf == WCMOD_SPD_15750_HI_DXGXS) ||
        (spd_intf == WCMOD_SPD_20000_HI_DXGXS) || (spd_intf == WCMOD_SPD_21G_HI_DXGXS) ||
        (spd_intf == WCMOD_SPD_40G_XLAUI) || (spd_intf == WCMOD_SPD_42G_XLAUI) ||
        (spd_intf == WCMOD_SPD_R2_12000))) {
    data16 = TXBERT_TXBERTCTRL_CRPAT_EN_MASK |
                TXBERT_TXBERTCTRL_COUNTRST_DIS_MASK;
    mask16 = TXBERT_TXBERTCTRL_CRPAT_EN_MASK |
                TXBERT_TXBERTCTRL_COUNTRST_DIS_MASK;
    MODIFY_WC40_TXBERT_TXBERTCTRLr(pc->unit, pc, data16, mask16);
  } else {
    /*  mask16 = TXBERT_TXBERTCTRL_CRPAT_EN_MASK 
           | TXBERT_TXBERTCTRL_COUNTRST_DIS_MASK;
   MODIFY_WC40_TXBERT_TXBERTCTRLr(pc->unit, pc, 0, mask16); */
  }
   if ((cntl & USE_CRPAT) && (  (spd_intf == WCMOD_SPD_10000_XFI) ||
       (spd_intf == WCMOD_SPD_11P5) || (spd_intf == WCMOD_SPD_12P5) ||
       (spd_intf == WCMOD_SPD_10000_SFI) || (spd_intf == WCMOD_SPD_10600_XFI_HG))) {
     WRITE_WC40_LNTXBERT_TXBERTCTRLr(pc->unit, pc, 0x0020);
   }
  return(SOC_E_NONE);
}

/*!
\brief Checks CJPAT/CRPAT parameters for a particular lane

\param  pc handle to current warpcore context (#wcmod_st)

\returns The value #SOC_E_NONE upon successful completion

This function checks for CJPAT or CRPAT for a particular lane as indicated by
#wcmod_st::this_lane. The speed mode of the lane must be chosen from the
enumerated type #wcmod_spd_intfc_set and set in the #wcmod_st::spd_intf field.
CJPAT/CRPAT must first enabled using #wcmod_cjpat_crpat_control(). Packet
transmission is next enabled using #wcmod_tx_bert_control() . Packet
transmission is disabled using #wcmod_tx_bert_control() again. The duration of
packet transmission is controlled externally and is the time duration between
the two function calls above.

This function compares the CJPAT/CRPAT TX and RX counters and prints the
results.  Byte count, IPG count and Packet count can all be compared by setting
bits #wcmod_st::per_lane_control 0x100, 0x10, and 0x1 . Currently only packet
count is supported.

This function reads the following registers:

\li TXBERT_TXBERTPACKETUr (0x8029) Upper 16b of count of Transmitted packets
\li TXBERT_TXBERTPACKETLr (0x802a) Lower 16b of count of Transmitted packets
\li RXBERT_RXBERTPACKETUr (0x8039) Upper 16b of count of Recieved    packets
\li RXBERT_RXBERTPACKETLr (0x803a) Lower 16b of count of Recieved    packets

<B>How to call:</B><br>
<p>Call this function directly or through the function #wcmod_tier1_selector()
using the upper case string "CJPAT_CRPAT_CHECK".</p>
*/

int wcmod_cjpat_crpat_check(wcmod_st* pc)     /* CJPAT_CRPAT_CHECK */
{
  uint16        rdValueTx, rdValueRx;

  wcmod_spd_intfc_set spd_intf = pc->spd_intf;
  int cntl = pc->per_lane_control;

  if (cntl) {
    if ((spd_intf == WCMOD_SPD_10000) ||    (spd_intf == WCMOD_SPD_40G_XLAUI) ||
        (spd_intf == WCMOD_SPD_42G_XLAUI) || (spd_intf == WCMOD_SPD_R2_12000)) {
      READ_WC40_RXBERT_RXBERTPACKETUr(pc->unit, pc, &rdValueRx);
      READ_WC40_TXBERT_TXBERTPACKETUr(pc->unit, pc, &rdValueTx);

      if (pc->verbosity > 0) printf("%-22s: Upper Pkt Cnt:Rx=%d Tx=%d\n",FUNCTION_NAME(),rdValueRx,rdValueTx);

      READ_WC40_RXBERT_RXBERTPACKETLr(pc->unit, pc, &rdValueRx);
      READ_WC40_TXBERT_TXBERTPACKETLr(pc->unit, pc, &rdValueTx);

      if (pc->verbosity > 0) printf("%-22s: Lower Pkt Cnt:Rx=%d Tx=%d\n",FUNCTION_NAME(),rdValueRx,rdValueTx);

      READ_WC40_RXBERT_RXBERTPKTERRr(pc->unit, pc, &rdValueRx);
      if (pc->verbosity > 0) printf("%-22s: Rx Packet Error Cnt (bad crc):%d \n",FUNCTION_NAME(),rdValueRx);

   /* } else if (pc->port_type == WCMOD_INDEPENDENT) { */
    } else {
      READ_WC40_LNRXBERT_RXBERTPACKETUr(pc->unit, pc, &rdValueRx);
      READ_WC40_LNTXBERT_TXBERTPACKETUr(pc->unit, pc, &rdValueTx);
      if (pc->verbosity > 0) printf("%-22s: Upper Pkt Cnt:Rx=%d Tx=%d\n",FUNCTION_NAME(),rdValueRx,rdValueTx);

      READ_WC40_LNRXBERT_RXBERTPACKETLr(pc->unit, pc, &rdValueRx);
      READ_WC40_LNTXBERT_TXBERTPACKETLr(pc->unit, pc, &rdValueTx);
      if (pc->verbosity > 0) printf("%-22s: Lower Pkt Cnt:Rx=%d Tx=%d\n",FUNCTION_NAME(),rdValueRx,rdValueTx);

      READ_WC40_LNRXBERT_RXBERTPKTERRr(pc->unit, pc, &rdValueRx);
      if (pc->verbosity > 0) printf("%-22s: Rx Packet Error Cnt (bad crc):%d \n",FUNCTION_NAME(),rdValueRx);
    }
  }
  return(SOC_E_NONE);
}
/*!
\brief  Enables or disables the bit error rate testing for a particular lane.

\param  pc handle to current warpcore context (#wcmod_st)

\returns The value #SOC_E_NONE upon successful completion.

\details

This function enables or disables bit-error-rate testing (BERT)
for a particular lane as indicated by #wcmod_st::this_lane from data
generated by PRBS, CJPAT, or CRPAT.  This function must be used in
conjunction with #wcmod_prbs_control() or #wcmod_cjpat_crpat_control().

Using PRBS without this function results in traffic that is
continuous, but not packetized; using PRBS with this function
results in traffic that is packetized.

BERT is enabled for a particular lane by setting #wcmod_st::en_dis to 1,
and disabled by setting the same field to 0. The speed mode from
the enumerated set #wcmod_spd_intfc_set must be indicated in the field
#wcmod_st::spd_intf.

For combo mode, #wcmod_st::lane_select must be set to 0.

This function modifies the following registers:

\li TXBERT_TXBERTCTRL (0x83D0)

<B>How to call:</B><br>
<p>Call this function directly or through the function #wcmod_tier1_selector()
with the upper-case string "TX_BERT_CONTROL".</p>

*/
int wcmod_tx_bert_control(wcmod_st* pc)         /* TX_BERT_CONTROL  */
{
  int cntl = pc->per_lane_control;
  wcmod_spd_intfc_set spd_intf = pc->spd_intf;

  if ((cntl) && ((spd_intf == WCMOD_SPD_10000) ||
      (spd_intf == WCMOD_SPD_40G_XLAUI) || (spd_intf == WCMOD_SPD_42G_XLAUI) ||
      (spd_intf == WCMOD_SPD_R2_12000))) {
    MODIFY_WC40_TXBERT_TXBERTCTRLr(pc->unit, pc, TXBERT_TXBERTCTRL_PKT_EN_MASK,
                                                 TXBERT_TXBERTCTRL_PKT_EN_MASK);
  } else if ((cntl == 0) && ((spd_intf == WCMOD_SPD_10000) ||
      (spd_intf == WCMOD_SPD_40G_XLAUI) || (spd_intf == WCMOD_SPD_42G_XLAUI) ||
      (spd_intf == WCMOD_SPD_R2_12000))) {
    MODIFY_WC40_TXBERT_TXBERTCTRLr(pc->unit,pc,0,TXBERT_TXBERTCTRL_PKT_EN_MASK);
  } else {
   /* do nothing */
  }

 if (cntl && (  (spd_intf == WCMOD_SPD_10000_XFI) ||
              (spd_intf == WCMOD_SPD_11P5) || (spd_intf == WCMOD_SPD_12P5) ||
              (spd_intf == WCMOD_SPD_10000_SFI) ||
              (spd_intf == WCMOD_SPD_10600_XFI_HG))) {
   MODIFY_WC40_LNTXBERT_TXBERTCTRLr(pc->unit,pc,
                                        LNTXBERT_TXBERTCTRL_PKT_EN_MASK,
                                        LNTXBERT_TXBERTCTRL_PKT_EN_MASK);
 } else if ((cntl == 0) &&(  (spd_intf == WCMOD_SPD_10000_XFI) || (spd_intf == WCMOD_SPD_10000_SFI) ||
            (spd_intf == WCMOD_SPD_11P5) || (spd_intf == WCMOD_SPD_12P5))) {
   MODIFY_WC40_LNTXBERT_TXBERTCTRLr(pc->unit, pc, 0,
                                            LNTXBERT_TXBERTCTRL_PKT_EN_MASK);
 } else {
 /* do nothing */
 }
  return(SOC_E_NONE);
}

/*!
\brief sets both the RX and TX lane swap values for all lanes simultaneously.

\param  pc handle to current warpcore context (#wcmod_st)

\returns The value #SOC_E_NONE upon successful completion

This function sets the RX and TX lane swap values for all lanes simultaneously.

The warpcore has several two sets of lane swap registers per lane.  This function uses the
lane swap registers closest to the pads.  For TX, the lane swap occurs after all other
analog/digital functionality.  For RX, the lane swap occurs prior to all other
analog/digital functionality.

It is not advisable to swap lanes without going through reset.

How the swap is done is indicated in the #wcmod_st::per_lane_control field;
bits 0 through 15 represent the swap settings for RX while bits 16
through 31 represent the swap settings for TX.

Each 4 bit chunk within both the RX and TX sections represent the swap settings
for a particular lane as follows:

Bits lane assignment

\li 31:28  TX lane 3
\li 27:24  TX lane 2
\li 23:20  TX lane 1
\li 19:16  TX lane 0

\li 15:12  RX lane 3
\li 11:8   RX lane 2
\li 7:4    RX lane 1
\li 3:0    RX lane 0

Each 4-bit value may be either 0, 1, 2 or 3, with the value indicating the lane
mapping.  By default, the equivalent per_lane_control settings are 0x3210_3210,
indicating that there is no re-routing of traffic.  As an example, to swap lanes
0 and 1 for both RX and TX, the per_lane_control should be set to 0x3201_3201.
To swap lanes 0 and 1, and also lanes 2 and 3 for both RX and TX, the
per_lane_control should be set to 0x2301_2301.

This function modifies the following registers:

\li XGXSBLK8_RXLNSWAP1 (0x816B)
\li XGXSBLK8_TXLNSWAP1 (0x8169)

<B>How to call:</B><br>
<p>Call this function directly or through the function "wcmod_tier1_selector"
with the upper-case string "LANE_SWAP".</p>

*/
int wcmod_lane_swap(wcmod_st *pc)
{
  unsigned int map, shift, rx_map, tx_map, n, mask;

  pc->lane_num_ignore = 0;

  map = (unsigned int)pc->per_lane_control;
  rx_map = 0;
  tx_map = 0;
  /* 3 2 1 0 needs to map to 3 2 1 0 for RX,  for TX */
  for(n = 0, shift = 0; n < 4; ++n, shift += 2) {
    mask = 0x3 << shift;
    rx_map |= (map >> shift) & mask;
    tx_map |= (map >> (shift + 16)) & mask;
  }
  SOC_IF_ERROR_RETURN (MODIFY_WC40_XGXSBLK8_RXLNSWAP1r(pc->unit, pc, rx_map,
                     XGXSBLK8_RXLNSWAP1_RX3_LNSWAP_SEL_MASK |
                     XGXSBLK8_RXLNSWAP1_RX2_LNSWAP_SEL_MASK |
                     XGXSBLK8_RXLNSWAP1_RX1_LNSWAP_SEL_MASK |
                     XGXSBLK8_RXLNSWAP1_RX0_LNSWAP_SEL_MASK));
  SOC_IF_ERROR_RETURN (MODIFY_WC40_XGXSBLK8_TXLNSWAP1r(pc->unit, pc, tx_map,
                     XGXSBLK8_TXLNSWAP1_TX3_LNSWAP_SEL_MASK |
                     XGXSBLK8_TXLNSWAP1_TX2_LNSWAP_SEL_MASK |
                     XGXSBLK8_TXLNSWAP1_TX1_LNSWAP_SEL_MASK |
                     XGXSBLK8_TXLNSWAP1_TX0_LNSWAP_SEL_MASK));
  return SOC_E_NONE;
}

/*!
\brief Set polarity for all lanes on the RX and TX simultaneously.

\param  pc handle to current warpcore context (#wcmod_st)

\returns The value #SOC_E_NONE upon successful completion.

\details
The polarity is updated with the settings as indicated in the
#wcmod_st::per_lane_control field. The bit assignments are as follows

Bits

\li 7   TX polarity lane 7
\li 6   TX polarity lane 6
\li 5   TX polarity lane 5
\li 4   TX polarity lane 4

\li 3   RX polarity lane 3
\li 2   RX polarity lane 2
\li 1   RX polarity lane 1
\li 0   RX polarity lane 0

Update the registers below

\li RX0_ANARXCONTROLPCI  (0x80BA)
\li RX1_ANARXCONTROLPCI  (0x80CA)
\li RX2_ANARXCONTROLPCI  (0x80DA)
\li RX3_ANARXCONTROLPCI  (0x80EA)

\li TX0_ANATXACONTROL0 (0x8061)
\li TX1_ANATXACONTROL0 (0x8071)
\li TX2_ANATXACONTROL0 (0x8081)
\li TX3_ANATXACONTROL0 (0x8091)

<B>How to call:</B><br>
<p>Call this function directly or through the function #wcmod_tier1_selector()
using the upper case string "SET_POLARITY".</p>
*/

#define MAX_LANES 4

int wcmod_tx_rx_polarity(wcmod_st *pc)
{
  uint16  data ;  int ix ;
    int lane, is_tx, enable;
    int reg_on[MAX_LANES], offset_val[MAX_LANES] ;
    /* turn the per_lane_control value into an array */
    enable =  pc->per_lane_control & 0x1;
    is_tx  = (pc->per_lane_control & 0x2)>>0x1;
    lane   = (pc->per_lane_control & ~0x3)>>0x2;

    if( enable == 0 ) return SOC_E_NONE ;

    for(ix=0; ix <MAX_LANES; ix++) { reg_on[ix]=0 ; }  /* init reg_on[] which is indexed by register_ix*/
    for(ix=0; ix <MAX_LANES; ix++) { offset_val[ix] = 0 ; }
    /* offset_val[] is indexed by register_ix and the offset pointer for lane (polarity value) */
    if(!is_tx) {  /* RX */
        if( pc->port_type == WCMOD_COMBO ) {
            /* RX lane 0 */
            pc->rx_pol = lane & 0xf ;
            reg_on[0] = 1 ; offset_val[0] = 0 ;
            reg_on[1] = 1 ; offset_val[1] = 1 ;
            reg_on[2] = 1 ; offset_val[2] = 2 ;
            reg_on[3] = 1 ; offset_val[3] = 3 ;
        } else {
            if( pc->dual_type ) {
                /* x2 port */
                pc->rx_pol = lane & 0x3 ;
                
                if( pc->this_lane ) { /* lane 2, lane 3 */
                    reg_on[2] = 1 ; offset_val[2] = 0 ;
                    reg_on[3] = 1 ; offset_val[3] = 1 ;
                } else {
                    reg_on[0] = 1 ; offset_val[0] = 0 ;
                    reg_on[1] = 1 ; offset_val[1] = 1 ;
                }
            } else {
                /* x1 port */
                pc->rx_pol = lane & 0x1 ;
                switch(pc->this_lane % 4) {
                    case 0:  reg_on[0] = 1 ; offset_val[0] = 0 ; break ;
                    case 1:  reg_on[1] = 1 ; offset_val[1] = 0 ; break ;
                    case 2:  reg_on[2] = 1 ; offset_val[2] = 0 ; break ;
                    case 3:  reg_on[3] = 1 ; offset_val[3] = 0 ; break ;
                    default: break ;
                }
            }
        }
        
        if( reg_on[0] ) { /* RX lane 0 */
            SOC_IF_ERROR_RETURN (READ_WC40_RX0_ANARXCONTROLPCIr(pc->unit, pc, &data)) ;
            if(((lane >>offset_val[0])&0x1) != ((data & RX0_ANARXCONTROLPCI_RX_POLARITY_R_MASK)
                                                 >>RX0_ANARXCONTROLPCI_RX_POLARITY_R_SHIFT)) {
                data ^= RX0_ANARXCONTROLPCI_RX_POLARITY_R_MASK ;
                data |= RX0_ANARXCONTROLPCI_RX_POLARITY_FORCE_SM_MASK ;
                SOC_IF_ERROR_RETURN (WRITE_WC40_RX0_ANARXCONTROLPCIr(pc->unit, pc, data)) ;
            }
        }
        if(reg_on[1])   { /* RX lane 1 */
            SOC_IF_ERROR_RETURN (READ_WC40_RX1_ANARXCONTROLPCIr(pc->unit, pc, &data)) ;
            if(((lane >>offset_val[1])&0x1) != ((data & RX1_ANARXCONTROLPCI_RX_POLARITY_R_MASK)
                                                 >>RX1_ANARXCONTROLPCI_RX_POLARITY_R_SHIFT)) {
                data ^= RX1_ANARXCONTROLPCI_RX_POLARITY_R_MASK ;
                data |= RX1_ANARXCONTROLPCI_RX_POLARITY_FORCE_SM_MASK ;
              SOC_IF_ERROR_RETURN (WRITE_WC40_RX1_ANARXCONTROLPCIr(pc->unit, pc, data)) ;
            }
        }
        if(reg_on[2]) { /* RX lane 2 */
            SOC_IF_ERROR_RETURN (READ_WC40_RX2_ANARXCONTROLPCIr(pc->unit, pc, &data)) ;
            if(((lane >>offset_val[2])&0x1)  != ((data & RX2_ANARXCONTROLPCI_RX_POLARITY_R_MASK)
                                                 >>RX2_ANARXCONTROLPCI_RX_POLARITY_R_SHIFT)) {
                data ^= RX2_ANARXCONTROLPCI_RX_POLARITY_R_MASK ;
                data |= RX2_ANARXCONTROLPCI_RX_POLARITY_FORCE_SM_MASK ;
                SOC_IF_ERROR_RETURN (WRITE_WC40_RX2_ANARXCONTROLPCIr(pc->unit, pc, data)) ;
            }
        }
        if(reg_on[3]){    /* RX lane 3 */
            SOC_IF_ERROR_RETURN (READ_WC40_RX3_ANARXCONTROLPCIr(pc->unit, pc, &data)) ;
            if(((lane >>offset_val[3])&0x1) != ((data & RX3_ANARXCONTROLPCI_RX_POLARITY_R_MASK)
                                                 >>RX3_ANARXCONTROLPCI_RX_POLARITY_R_SHIFT)) {
                data ^= RX3_ANARXCONTROLPCI_RX_POLARITY_R_MASK ;
                data |= RX3_ANARXCONTROLPCI_RX_POLARITY_FORCE_SM_MASK ;
                SOC_IF_ERROR_RETURN (WRITE_WC40_RX3_ANARXCONTROLPCIr(pc->unit, pc, data)) ;
            }
        }
    } else {
        if( pc->port_type == WCMOD_COMBO ) {
            /* TX lane 0 */
            pc->tx_pol = lane & 0xf ;
            reg_on[0] = 1 ; offset_val[0] = 0 ;
            reg_on[1] = 1 ; offset_val[1] = 1 ;
            reg_on[2] = 1 ; offset_val[2] = 2 ;
            reg_on[3] = 1 ; offset_val[3] = 3 ;
        } else {
            if( pc->dual_type ) {
                /* x2 port */
                pc->tx_pol = lane & 0x3 ;
                if( pc->this_lane ) { /* lane 2, lane 3 */
                    reg_on[2] = 1 ; offset_val[2] = 0 ;
                    reg_on[3] = 1 ; offset_val[3] = 1 ;
                } else {
                    reg_on[0] = 1 ; offset_val[0] = 0 ;
                    reg_on[1] = 1 ; offset_val[1] = 1 ;
                }
            } else {
                /* x1 port */
                pc->tx_pol = lane & 0x1 ;
                
                switch(pc->this_lane % 4) {
                    case 0:  reg_on[0] = 1 ; offset_val[0] = 0 ; break ;
                    case 1:  reg_on[1] = 1 ; offset_val[1] = 0 ; break ;
                    case 2:  reg_on[2] = 1 ; offset_val[2] = 0 ; break ;
                    case 3:  reg_on[3] = 1 ; offset_val[3] = 0 ; break ;
                    default: break ;
                }
            }
        }
        if( reg_on[0]) { /* TX lane 0 */
            SOC_IF_ERROR_RETURN (READ_WC40_TX0_ANATXACONTROL0r(pc->unit, pc, &data)) ;
            if( ((lane>>offset_val[0])&0x1) != ((data & TX0_ANATXACONTROL0_TXPOL_FLIP_MASK)
                                                     >> TX0_ANATXACONTROL0_TXPOL_FLIP_SHIFT)) {

                data ^= TX0_ANATXACONTROL0_TXPOL_FLIP_MASK ;  
                SOC_IF_ERROR_RETURN (WRITE_WC40_TX0_ANATXACONTROL0r(pc->unit, pc, data)) ;
            }
        }                    
        if( reg_on[1]) { /* TX lane 0 */
            SOC_IF_ERROR_RETURN (READ_WC40_TX1_ANATXACONTROL0r(pc->unit, pc, &data)) ;
            if( ((lane>>offset_val[1])&0x1) != ((data & TX1_ANATXACONTROL0_TXPOL_FLIP_MASK)
                                                     >> TX1_ANATXACONTROL0_TXPOL_FLIP_SHIFT)) {

                data ^= TX1_ANATXACONTROL0_TXPOL_FLIP_MASK ;  
                SOC_IF_ERROR_RETURN (WRITE_WC40_TX1_ANATXACONTROL0r(pc->unit, pc, data)) ;
            }
        }                
        if( reg_on[2]) { /* TX lane 0 */
            SOC_IF_ERROR_RETURN (READ_WC40_TX2_ANATXACONTROL0r(pc->unit, pc, &data)) ;
            if( ((lane>>offset_val[2])&0x1) != ((data & TX2_ANATXACONTROL0_TXPOL_FLIP_MASK)
                                                     >> TX2_ANATXACONTROL0_TXPOL_FLIP_SHIFT)) {

                data ^= TX2_ANATXACONTROL0_TXPOL_FLIP_MASK ;  
                SOC_IF_ERROR_RETURN (WRITE_WC40_TX2_ANATXACONTROL0r(pc->unit, pc, data)) ;
            }
        }                
        if( reg_on[3]) { /* TX lane 0 */
            SOC_IF_ERROR_RETURN (READ_WC40_TX3_ANATXACONTROL0r(pc->unit, pc, &data)) ;
            if( ((lane>>offset_val[3])&0x1) != ((data & TX3_ANATXACONTROL0_TXPOL_FLIP_MASK)
                                                     >> TX3_ANATXACONTROL0_TXPOL_FLIP_SHIFT)) {

                data ^= TX3_ANATXACONTROL0_TXPOL_FLIP_MASK ;  
                SOC_IF_ERROR_RETURN (WRITE_WC40_TX3_ANATXACONTROL0r(pc->unit, pc, data)) ;
            }
        }          
    }
  
    return SOC_E_NONE;
}

/*!
\brief Program a data pattern to be transmitted out on a particular lane

\param  pc handle to current warpcore context (#wcmod_st)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.

\details

This function sets a pattern to be repeatedly transmitted for a particular lane.
In some speed modes this pattern is 20 bits, and in other modes it is 10 bits.
The pattern is transmitted LSB first.

\li For 10G XFI, pcs bypass 6.25G, pcs bypass 3.25G, the pattern is 20 bits.
\li For combo 1G mode, the pattern is 10 bits.

To set the data pattern set the first two bytes and the last two bytes of
#wcmod_st::per_lane_control to the lower and upper 10 pattern bits respectively.

Example: Upper=0x3ff and Lower=0x3aa, per_lane_control = 0x03ff03aa.

For C0 and WL, the pattern generator can also generate 256 bits. This is
accomplished with different elements in #wcmod_st

\li Set #wcmod_st::per_lane_control_x = 1;
\li Set #wcmod_st::per_lane_control_1 through #wcmod_st::per_lane_control_8
with desired 256 pattern.

Note that you have to always load the full 256 bits. Partial loading is not
implemented.

This function modifies the following registers:

\li TX0_ANATXACONTROL0 (0x8061)
\li TX0_ANATXMDATA0    (0x8062)
\li TX0_ANATXMDATA1    (0x8063)

\li TX1_ANATXACONTROL0 (0x8071)
\li TX1_ANATXMDATA0    (0x8072)
\li TX1_ANATXMDATA1    (0x8073)

\li TX2_ANATXACONTROL0 (0x8081)
\li TX2_ANATXMDATA0    (0x8082)
\li TX2_ANATXMDATA1    (0x8083)

\li TX3_ANATXACONTROL0 (0x8091)
\li TX3_ANATXMDATA0    (0x8092)
\li TX3_ANATXMDATA1    (0x8093)

<B>How to call:</B><br>
<p>Call this function directly or through the function
#wcmod_tier1_selector() with the upper-case string "PROG_DATA".</p>
 */
int wcmod_prog_data(wcmod_st *pc)
{
  int lane, enable;
  unsigned int dataLo, dataHi;
  uint16 p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15;

  lane = pc->this_lane;

  if (pc->per_lane_control_x) {
    if (pc->model_type == WCMOD_WC_C0 || pc->model_type == WCMOD_WL_A0) {
    } else {
      if (pc->verbosity > 0) printf("%-22s: Can't enable extended control unless WL or C0:%s",
                                 FUNCTION_NAME(),e2s_wcmod_model_type[pc->model_type]);
      return SOC_E_ERROR;
    }
    p0 = pc->per_lane_control_1 & 0xffff;
    p1 =(pc->per_lane_control_1 & 0xffff0000)>>16;
    p2 = pc->per_lane_control_2 & 0xffff;
    p3 =(pc->per_lane_control_2 & 0xffff0000)>>16;
    p4 = pc->per_lane_control_3 & 0xffff;
    p5 =(pc->per_lane_control_3 & 0xffff0000)>>16;
    p6 = pc->per_lane_control_4 & 0xffff;
    p7 =(pc->per_lane_control_4 & 0xffff0000)>>16;
    p8 = pc->per_lane_control_5 & 0xffff;
    p9 =(pc->per_lane_control_5 & 0xffff0000)>>16;
    p10= pc->per_lane_control_6 & 0xffff;
    p11=(pc->per_lane_control_6 & 0xffff0000)>>16;
    p12= pc->per_lane_control_7 & 0xffff;
    p13=(pc->per_lane_control_7 & 0xffff0000)>>16;
    p14= pc->per_lane_control_8 & 0xffff;
    p15=(pc->per_lane_control_8 & 0xffff0000)>>16;

    WRITE_WC40_PATGEN256_0_REG0r(pc->unit, pc, p0);
    WRITE_WC40_PATGEN256_0_REG1r(pc->unit, pc, p1);
    WRITE_WC40_PATGEN256_0_REG2r(pc->unit, pc, p2);
    WRITE_WC40_PATGEN256_0_REG3r(pc->unit, pc, p3);
    WRITE_WC40_PATGEN256_0_REG4r(pc->unit, pc, p4);
    WRITE_WC40_PATGEN256_0_REG5r(pc->unit, pc, p5);
    WRITE_WC40_PATGEN256_0_REG6r(pc->unit, pc, p6);
    WRITE_WC40_PATGEN256_0_REG7r(pc->unit, pc, p7);
    WRITE_WC40_PATGEN256_0_REG8r(pc->unit, pc, p8);
    WRITE_WC40_PATGEN256_0_REG9r(pc->unit, pc, p9);
    WRITE_WC40_PATGEN256_0_REG10r(pc->unit, pc, p10);
    WRITE_WC40_PATGEN256_0_REG11r(pc->unit, pc, p11);
    WRITE_WC40_PATGEN256_0_REG12r(pc->unit, pc, p12);
    WRITE_WC40_PATGEN256_0_REG13r(pc->unit, pc, p13);
    WRITE_WC40_PATGEN256_0_REG14r(pc->unit, pc, p14);

    WRITE_WC40_PATGEN256_1_REG15r(pc->unit, pc, p15);
    WRITE_WC40_PATGEN256_1_CTRLr (pc->unit, pc, pc->per_lane_control);

    return SOC_E_NONE;
  } else {
  dataLo = pc->per_lane_control & TX0_ANATXMDATA0_TXMDIOTSTDATAL_MASK;
  dataHi =(pc->per_lane_control >> 16) & TX0_ANATXMDATA1_TXMDIOTSTDATAH_MASK;
  enable =( pc->per_lane_control) & 0x80000000;

  if (lane == 0) {
    SOC_IF_ERROR_RETURN (MODIFY_WC40_TX0_ANATXACONTROL0r(pc->unit, pc,
                       enable ? TX0_ANATXACONTROL0_TX_MDATA_EN_MASK : 0,
                       TX0_ANATXACONTROL0_TX_MDATA_EN_MASK));
    SOC_IF_ERROR_RETURN (MODIFY_WC40_TX0_ANATXMDATA0r(pc->unit, pc,
                                dataLo,TX0_ANATXMDATA0_TXMDIOTSTDATAL_MASK));
    SOC_IF_ERROR_RETURN (MODIFY_WC40_TX0_ANATXMDATA1r(pc->unit, pc,
                dataHi,TX0_ANATXMDATA1_TXMDIOTSTDATAH_MASK));
  } else if (lane == 1) {
    SOC_IF_ERROR_RETURN (MODIFY_WC40_TX1_ANATXACONTROL0r(pc->unit, pc,
                       enable ? TX0_ANATXACONTROL0_TX_MDATA_EN_MASK : 0,
                       TX1_ANATXACONTROL0_TX_MDATA_EN_MASK));
    SOC_IF_ERROR_RETURN (MODIFY_WC40_TX1_ANATXMDATA0r(pc->unit, pc,
                dataLo,TX1_ANATXMDATA0_TXMDIOTSTDATAL_MASK));
    SOC_IF_ERROR_RETURN (MODIFY_WC40_TX1_ANATXMDATA1r(pc->unit, pc,
                dataHi,TX1_ANATXMDATA1_TXMDIOTSTDATAH_MASK));
  } else if (lane == 2) {
    SOC_IF_ERROR_RETURN (MODIFY_WC40_TX2_ANATXACONTROL0r(pc->unit, pc,
                       enable ? TX0_ANATXACONTROL0_TX_MDATA_EN_MASK : 0,
                       TX2_ANATXACONTROL0_TX_MDATA_EN_MASK));
    SOC_IF_ERROR_RETURN (MODIFY_WC40_TX2_ANATXMDATA0r(pc->unit, pc,
                dataLo,TX2_ANATXMDATA0_TXMDIOTSTDATAL_MASK));
    SOC_IF_ERROR_RETURN (MODIFY_WC40_TX2_ANATXMDATA1r(pc->unit, pc,
                dataHi,TX2_ANATXMDATA1_TXMDIOTSTDATAH_MASK));
  } else if (lane == 3) {
    SOC_IF_ERROR_RETURN (MODIFY_WC40_TX3_ANATXACONTROL0r(pc->unit, pc,
                       enable ? TX0_ANATXACONTROL0_TX_MDATA_EN_MASK : 0,
                       TX3_ANATXACONTROL0_TX_MDATA_EN_MASK));
    SOC_IF_ERROR_RETURN (MODIFY_WC40_TX3_ANATXMDATA0r(pc->unit, pc,
                dataLo,TX3_ANATXMDATA0_TXMDIOTSTDATAL_MASK));
    SOC_IF_ERROR_RETURN (MODIFY_WC40_TX3_ANATXMDATA1r(pc->unit, pc,
                dataHi,TX3_ANATXMDATA1_TXMDIOTSTDATAH_MASK));
  } else {
    printf("%-22s: Bad Lane:%d \n",FUNCTION_NAME(),lane);
    return SOC_E_ERROR;
  }
  return SOC_E_NONE;
  }
  return SOC_E_ERROR;
}

/*!
\brief Apply reset to complete core (not per lane).

\param  pc handle to current warpcore context (#wcmod_st)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.

\details
This reset is applied at the very beginning of the WarpCore initialization to
reset the complete core. To configure to any new operating speed, it is
advisable to start with core reset. This resets all the MDIO registers and
State Machines. This is a Self Clearing bit.

<B>How to call:</B><br>
<p>Call this function directly or through the function #wcmod_tier1_selector()
with the upper-case string "CORE_RESET".</p>

*/
int wcmod_core_reset(wcmod_st* pc)
{
  uint16      data16;

  /* select multi mmd, 0x400f*/
  data16 = (XGXSBLK0_MMDSELECT_DEVCL22_EN_MASK  |
            XGXSBLK0_MMDSELECT_DEVDEVAD_EN_MASK |
            XGXSBLK0_MMDSELECT_DEVPMD_EN_MASK   |
            XGXSBLK0_MMDSELECT_DEVAN_EN_MASK    |
            XGXSBLK0_MMDSELECT_MULTIMMDS_EN_MASK  );
  SOC_IF_ERROR_RETURN (WRITE_WC40_XGXSBLK0_MMDSELECTr(pc->unit, pc, data16));

  SOC_IF_ERROR_RETURN (MODIFY_WC40_XGXSBLK2_LANERESETr(pc->unit, pc,
                                          XGXSBLK2_LANERESET_RESET_MDIO_MASK |
                                          XGXSBLK2_LANERESET_RESET_TX_MASK |
                                          XGXSBLK2_LANERESET_RESET_RX_MASK,
                                          XGXSBLK2_LANERESET_RESET_MDIO_MASK |
                                          XGXSBLK2_LANERESET_RESET_TX_MASK |
                                          XGXSBLK2_LANERESET_RESET_RX_MASK));
  sal_usleep(10);

  SOC_IF_ERROR_RETURN (WRITE_WC40_XGXSBLK0_MMDSELECTr(pc->unit, pc, data16));
  return SOC_E_NONE;
}

/*!
\brief load 8051 microcode.  (one microcontroller per warpcore).

\param  pc handle to current warpcore context (a.k.a wcmod struct)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.

\details

<B>This function is not implemented. It is unlikely to be implemented since the
platform usually has more efficient ways of loading firmware.</B>

<B>How to call:</B><br>
<p>Call this function directly or through the function
#wcmod_tier1_selector() with the upper-case string "FIRMWARE_SET".</p>
*/
int wcmod_firmware_set(wcmod_st* pc)
{
 
  return SOC_E_NONE;
}

int wcmod_tx_reset(wcmod_st* pc)                      /* TX_RESET */
{
  uint16 data16, mask16;
  int cntl;

  cntl = (pc->per_lane_control & (0xff << pc->this_lane * 8)) >> pc->this_lane * 8;
  data16 = 0;
  mask16 = 0;

  if (cntl) {
      data16 = 1 << DIGITAL5_MISC6_RESET_TX_ASIC_SHIFT;
      mask16 = DIGITAL5_MISC6_RESET_TX_ASIC_MASK;
  } else {
      data16 = 0;
      mask16 |= DIGITAL5_MISC6_RESET_TX_ASIC_MASK;
  }
  if (cntl) {
    if (pc->verbosity > 0) printf("%s Tx reset. Lane:%d sel=%0x cntl:%x\n", FUNCTION_NAME(),pc->this_lane, pc->lane_select, cntl);
  } 
  SOC_IF_ERROR_RETURN(MODIFY_WC40_DIGITAL5_MISC6r(pc->unit,pc,data16,mask16));
  

  return SOC_E_NONE;
}

int wcmod_rx_reset(wcmod_st* pc)                      /* RX_RESET */
{
  uint16 data16, mask16;
  int cntl;

  cntl = (pc->per_lane_control & (0xff << pc->this_lane * 8)) >> pc->this_lane * 8;
  data16 = 0;
  mask16 = 0;

  if (cntl) {
      data16 = 1 << DIGITAL5_MISC6_RESET_RX_ASIC_SHIFT;
      mask16 = DIGITAL5_MISC6_RESET_RX_ASIC_MASK;
  } else {
      data16 = 0;
      mask16 |= DIGITAL5_MISC6_RESET_RX_ASIC_MASK;
  }
  if (cntl) {
    if (pc->verbosity > 0) printf("%s Rx reset. Lane:%d sel=%x cntl:%x\n", FUNCTION_NAME(),pc->this_lane, pc->lane_select, cntl);
  } 
  SOC_IF_ERROR_RETURN(MODIFY_WC40_DIGITAL5_MISC6r(pc->unit,pc,data16,mask16));
  
  return SOC_E_NONE;
}


int wcmod_firmware_mode_set(wcmod_st* pc)
{
    uint16 tempData, data16 =0 , mask16;
    int timeout, rv, link;

    link = pc->accAddr;
    /* first need to stop the uC properly */
    /* check if uc is active or not */
    SOC_IF_ERROR_RETURN(READ_WC40_UC_INFO_B1_VERSIONr(pc->unit, pc, &data16));
    SOC_IF_ERROR_RETURN(READ_WC40_UC_INFO_B1_CRCr(pc->unit, pc, &tempData));
    if ((data16 != 0 ) && (tempData != 0) && link)  {
        /* wait for cmd to be read by micro. i.e it is ready to take next cmd */
        timeout = 0;
        do {
            SOC_IF_ERROR_RETURN(READ_WC40_DSC1B0_UC_CTRLr(pc->unit, pc, &data16));
        } while (!(data16 & DSC1B0_UC_CTRL_READY_FOR_CMD_MASK)
                && (++timeout <2000));

        /* check for error */
        SOC_IF_ERROR_RETURN(READ_WC40_DSC1B0_UC_CTRLr(pc->unit, pc, &data16));
        if (data16 & DSC1B0_UC_CTRL_ERROR_FOUND_MASK) {
            printf("firmware mode change %-22s: Micro reported error\n", FUNCTION_NAME());
        }
        /* next issue command to stop Uc gracefully */
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_DSC1B0_UC_CTRLr(pc->unit, pc, 0x0, DSC1B0_UC_CTRL_READY_FOR_CMD_MASK));
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_DSC1B0_UC_CTRLr(pc->unit, pc, 0x0, DSC1B0_UC_CTRL_ERROR_FOUND_MASK));
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_DSC1B0_UC_CTRLr(pc->unit, pc, 0x0, DSC1B0_UC_CTRL_CMD_INFO_MASK));

        data16 = (0x1 << DSC1B0_UC_CTRL_GP_UC_REQ_SHIFT) |
              (0x0 << DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT);
        mask16  = (DSC1B0_UC_CTRL_GP_UC_REQ_MASK  |
                 DSC1B0_UC_CTRL_SUPPLEMENT_INFO_MASK);
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_DSC1B0_UC_CTRLr(pc->unit, pc, data16, mask16));

        sal_usleep(1000);

        /* wait for uC ready for command:  bit7=1    */
        rv = wcmod_regbit_set_wait_check(pc, DSC1B0_UC_CTRLr,
                 DSC1B0_UC_CTRL_READY_FOR_CMD_MASK, 1, WC40_PLL_WAIT*1000);

        data16 = pc->accData;
        pc->accData = ((pc->accData) & DSC1B0_UC_CTRL_ERROR_FOUND_MASK) >> DSC1B0_UC_CTRL_ERROR_FOUND_SHIFT;

        if ((rv <0) || (pc->accData != 0)) {
           printf("firmware mode change : uController not ready pass 1!: u=%d\n", pc->unit);
        }
    }

    switch (pc->per_lane_control) {
    case WCMOD_DEFAULT:  
        data16 = UC_INFO_B1_FIRMWARE_MODE_LN0_MODE_DEFAULT;
        break;
    case WCMOD_SFP_OPT_SR4:  
        data16 = UC_INFO_B1_FIRMWARE_MODE_LN0_MODE_SFP_OPT_LR;
        break;
    case WCMOD_SFP_DAC:  
        data16 = UC_INFO_B1_FIRMWARE_MODE_LN0_MODE_SFP_DAC;
        break;
    case WCMOD_XLAUI:  
        data16 = UC_INFO_B1_FIRMWARE_MODE_LN0_MODE_XLAUI;
        break;
    case WCMOD_FORCE_OSDFE:  
        data16 = UC_INFO_B1_FIRMWARE_MODE_LN0_MODE_LONG_CH_6G;
        break;
    case WCMOD_FORCE_BRDFE:  
        data16 = UC_INFO_B1_FIRMWARE_MODE_LN0_MODE_BRDFE;
        break;
    case WCMOD_SW_CL72:  
        data16 = UC_INFO_B1_FIRMWARE_MODE_LN0_MODE_SW_3_TAP_TXFIR_TUNING;
        break;
    case WCMOD_CL72_woAN:  
        data16 = UC_INFO_B1_FIRMWARE_MODE_LN0_MODE_CL72_NO_AN;
        break;
    default:
        break;
    }
    
    data16  <<= (pc->this_lane * 4);
    mask16 = UC_INFO_B1_FIRMWARE_MODE_LN0_MODE_MASK << (pc->this_lane * 4);
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_UC_INFO_B1_FIRMWARE_MODEr(pc->unit,pc,data16,mask16));

    /* check if uc is active or not */
    SOC_IF_ERROR_RETURN(READ_WC40_UC_INFO_B1_VERSIONr(pc->unit, pc, &data16));
    SOC_IF_ERROR_RETURN(READ_WC40_UC_INFO_B1_CRCr(pc->unit, pc, &tempData));
    if ((data16 != 0 ) && (tempData != 0) && link)  {
        /* wait for cmd to be read by micro. i.e it is ready to take next cmd */
        timeout = 0;
        do {
            SOC_IF_ERROR_RETURN(READ_WC40_DSC1B0_UC_CTRLr(pc->unit, pc, &data16));
        } while (!(data16 & DSC1B0_UC_CTRL_READY_FOR_CMD_MASK)
                && (++timeout <2000));

        /* check for error */
        SOC_IF_ERROR_RETURN(READ_WC40_DSC1B0_UC_CTRLr(pc->unit, pc, &data16));
        if (data16 & DSC1B0_UC_CTRL_ERROR_FOUND_MASK) {
            printf("%-22s: Micro reported error\n", FUNCTION_NAME());
        }
        /* next issue command to resume Uc gracefully */
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_DSC1B0_UC_CTRLr(pc->unit, pc, 0x0, DSC1B0_UC_CTRL_READY_FOR_CMD_MASK));
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_DSC1B0_UC_CTRLr(pc->unit, pc, 0x0, DSC1B0_UC_CTRL_ERROR_FOUND_MASK));
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_DSC1B0_UC_CTRLr(pc->unit, pc, 0x0, DSC1B0_UC_CTRL_CMD_INFO_MASK));

        data16 = (0x1 << DSC1B0_UC_CTRL_GP_UC_REQ_SHIFT) |
              (0x3 << DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT);
        mask16  = (DSC1B0_UC_CTRL_GP_UC_REQ_MASK  |
                 DSC1B0_UC_CTRL_SUPPLEMENT_INFO_MASK);
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_DSC1B0_UC_CTRLr(pc->unit, pc, data16, mask16));

        sal_usleep(1000);

        /* wait for uC ready for command:  bit7=1    */
        rv = wcmod_regbit_set_wait_check(pc, DSC1B0_UC_CTRLr,
                 DSC1B0_UC_CTRL_READY_FOR_CMD_MASK, 1, WC40_PLL_WAIT*1000);

        data16 = pc->accData;
        pc->accData = ((pc->accData) & DSC1B0_UC_CTRL_ERROR_FOUND_MASK) >> DSC1B0_UC_CTRL_ERROR_FOUND_SHIFT;

        if ((rv <0) || (pc->accData != 0)) {
           printf("DSC dump : uController not ready pass 1!: u=%d\n", pc->unit);
        }
    }
 
    return SOC_E_NONE;
}

/*!
\brief Controls BRCM64B66B encoder/decoder in 25.45G, 30G/40G_BRCM,
12.73G_dxgxs, 20G_dxgxs, 15.75G_dxgxs.

\param  pc handle to current warpcore context (a.k.a wcmod struct)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.

\details
This function will take eight 16bit values for scw and scw_mask for each lane.

<B>Currently, this function is not implemented</B>

<B>How to call:</B><br>
<p>Call this function directly or through the function
#wcmod_tier1_selector() with the upper-case string "BRCM64B66B_CONTROL".</p>
*/
int wcmod_BRCM64B66B_control(wcmod_st* pc)
{
  return SOC_E_NONE;
}

/*!
\brief enables/disables FEC function.

\param  pc handle to current warpcore context (a.k.a wcmod struct)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.

\details
FEC is controlled by fields in #wcmod_st::per_lane_control. The bit assignments
0:7 for lane 0 are below. Use 8:15, 16:23, 24:31 for  lanes 1 through 3.

\li 3-8 Unused
\li 2   FEC error enable override
\li 1   FEC enable override
\li 0   FEC enable

<B>How to call:</B><br>
<p>Call this function directly or through the function
#wcmod_tier1_selector() with the upper-case string "FEC_CONTROL".</p>
*/

int wcmod_FEC_control(wcmod_st* pc)
{
  int cntl = (pc->per_lane_control & (0xff << pc->this_lane * 8)) >> pc->this_lane * 8;
  uint16      data16, mask16b1, mask16b2, mask16b3;
  mask16b1 = (1 << (GP2_REG_GP2_E_FEC_ENABLE_SHIFT +  pc->this_lane));
  mask16b2 = (CL74_USERB0_UFECCONTROL3_FEC_ENABLE_OVR_VAL_MASK |
              CL74_USERB0_UFECCONTROL3_FEC_ENABLE_OVR_MASK);
  mask16b3 = (CL74_USERB0_UFECCONTROL3_FEC_ERROR_EN_OVR_VAL_MASK    |
              CL74_USERB0_UFECCONTROL3_FEC_ERROR_EN_OVR_MASK    );

  if (cntl & 0x1) {
    /* this is an aggregated register. But we cannot use aggregation */
    /* optimizatin here since the other registers are not aggregate. */
    data16 = mask16b1;
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_CL74_USERB0_UFECCONTROL3r(pc->unit,pc,data16,mask16b1));
  } else if (!(cntl & 0x1)) {
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_CL74_USERB0_UFECCONTROL3r(pc->unit,pc,0x0000,mask16b1));
  } else {
  }
  if (cntl & 0x2) {
    data16 = mask16b2;
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_CL74_USERB0_UFECCONTROL3r(pc->unit,pc,data16,mask16b2));
  } else if (!(cntl & 0x2)) {
        (MODIFY_WC40_CL74_USERB0_UFECCONTROL3r(pc->unit,pc,0x0000,mask16b2));
  } else {
  }
  if (cntl & 0x4) {
    data16 = mask16b2;
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_CL74_USERB0_UFECCONTROL3r(pc->unit,pc,data16,mask16b3));
  } else if (!(cntl & 0x2)) {
        (MODIFY_WC40_CL74_USERB0_UFECCONTROL3r(pc->unit,pc,0x0000,mask16b3));
  } else {
  }
  return SOC_E_NONE;
}

/*!
\brief sets Idriver, Ipredriver, and post2_coef values of transmitter.

\param  pc handle to current warpcore context (a.k.a wcmod struct)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.

\details
Idriver (4 bits) and pdriver (4 bits) controls programmable TX amplitude.
idriver and pdriver are provided in fields in #wcmod_st::per_lane_control.
This function has to be called once per lane. (i.e. lane_select must be one hot)

\li first byte  Ipredriver
\li second byte Idriver
\li third byte  post2_coef
\li fourth byte control (bits 1, 2, and 4 choose idriver

<B>How to call:</B><br>
<p>Call this function directly or through the function
#wcmod_tier1_selector() with the upper-case string
"TX_AMP_CONTROL".</p>
*/
int wcmod_tx_amp_control(wcmod_st* pc)
{
  uint16      data16 = 0;
  int cntl = pc->per_lane_control;
  int  idrv, ipredrv, p2_coef, mask_cntl = 0;
      ipredrv    = cntl & 0xff;
      idrv    = (cntl & 0xff00) >> 8;
      p2_coef = (cntl & 0xff0000) >> 16;

  if (pc->verbosity > 0) printf("%-22s: ipredrv/ipdrv/post2:%x/%x/%x\n",FUNCTION_NAME(),ipredrv,idrv,p2_coef);

  if (cntl & 0x1000000) { /* set Ipredriver */
  mask_cntl |= TXB_TX_DRIVER_IPREDRIVER_MASK;
  data16 |= ((ipredrv << TXB_TX_DRIVER_IPREDRIVER_SHIFT)
                               & TXB_TX_DRIVER_IPREDRIVER_MASK);
  }
  if (cntl & 0x2000000) { /* set Idriver */
  mask_cntl |= TXB_TX_DRIVER_IDRIVER_MASK;
  data16 |= ((idrv << TXB_TX_DRIVER_IDRIVER_SHIFT)
                               & TXB_TX_DRIVER_IDRIVER_MASK);
  }
  if (cntl & 0x4000000) { /* set post2_coeff */
  mask_cntl |= TXB_TX_DRIVER_POST2_COEFF_MASK;
  data16 |= ((p2_coef << TXB_TX_DRIVER_POST2_COEFF_SHIFT)
                               & TXB_TX_DRIVER_POST2_COEFF_MASK);
  }

  /* added the support for qsgmii where only driver current is supported */
  if (pc->model_type == WCMOD_QS_A0) {
      if (cntl & 0x2000000) { /* set Idriver */
          uint16 tmp_lane = 0;

          tmp_lane = pc->this_lane;
          pc->this_lane = (tmp_lane / 4) * 4;
          SOC_IF_ERROR_RETURN
              (MODIFY_WC40_TX0_TX_DRIVERr(pc->unit, pc, idrv << 6, 0x3f << 6));
          pc->this_lane = tmp_lane;
      }
  } else {
      if (cntl & 0x7000000) {    /* was using || */
          if ( pc->verbosity > 0) printf("%-22s: lane=%0d sel=%x cntl=%x ipredrv/ipdrv/post2:%x/%x/%x\n",
                                             FUNCTION_NAME(), pc->this_lane, pc->lane_select, cntl, ipredrv,idrv,p2_coef);
          if (pc->port_type == WCMOD_COMBO) {
              SOC_IF_ERROR_RETURN
                  (MODIFY_WC40_TX0_TX_DRIVERr(pc->unit, pc, data16, mask_cntl));
              SOC_IF_ERROR_RETURN
                  (MODIFY_WC40_TX1_TX_DRIVERr(pc->unit, pc, data16, mask_cntl));
              SOC_IF_ERROR_RETURN
                  (MODIFY_WC40_TX2_TX_DRIVERr(pc->unit, pc, data16, mask_cntl));
              SOC_IF_ERROR_RETURN
                  (MODIFY_WC40_TX3_TX_DRIVERr(pc->unit, pc, data16, mask_cntl));
          } else if  (pc->dxgxs) {
              if (pc->dxgxs == 1) {
                  SOC_IF_ERROR_RETURN
                      (MODIFY_WC40_TX0_TX_DRIVERr(pc->unit, pc, data16, mask_cntl));
                  SOC_IF_ERROR_RETURN
                      (MODIFY_WC40_TX1_TX_DRIVERr(pc->unit, pc, data16, mask_cntl));
              } else if (pc->dxgxs == 2) {
                  SOC_IF_ERROR_RETURN
                      (MODIFY_WC40_TX2_TX_DRIVERr(pc->unit, pc, data16, mask_cntl));
                  SOC_IF_ERROR_RETURN
                      (MODIFY_WC40_TX3_TX_DRIVERr(pc->unit, pc, data16, mask_cntl));
              }
          } else if  ((pc->this_lane == 0) && (pc->port_type != WCMOD_COMBO)) {
              SOC_IF_ERROR_RETURN
                  (MODIFY_WC40_TX0_TX_DRIVERr(pc->unit, pc, data16, mask_cntl));
          } else if (pc->this_lane == 1) {
              SOC_IF_ERROR_RETURN
                  (MODIFY_WC40_TX1_TX_DRIVERr(pc->unit, pc, data16, mask_cntl));
          } else if (pc->this_lane == 2) {
              SOC_IF_ERROR_RETURN
                  (MODIFY_WC40_TX2_TX_DRIVERr(pc->unit, pc, data16, mask_cntl));
          } else if (pc->this_lane == 3) {
              SOC_IF_ERROR_RETURN
                  (MODIFY_WC40_TX3_TX_DRIVERr(pc->unit, pc, data16, mask_cntl));
          } else {
              printf("%s Error: Bad lanenum:%d\n", FUNCTION_NAME(), pc->this_lane);
              return SOC_E_ERROR;
          }
      }
  }
  return SOC_E_NONE;
}

/*!
\brief sets p1 slicers for the receive path.

\param  pc handle to current warpcore context (a.k.a wcmod struct)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.

\details

\li first byte  is p1  even control (6 bits)
\li second byte is p1  odd  control (6 bits)
\li third byte is ignored
\li fourth byte is set to 1 to enable even, 2 to enable odd
\li first and second byte (12 bits relevent) contain slicer value.

<B>How to call:</B><br>
<p>Call this function directly or through the function
#wcmod_tier1_selector() with the upper-case string
"RX_P1_SLICER_CONTROL".</p>
*/

int wcmod_rx_p1_slicer_control(wcmod_st* pc)
{
  int cntl = pc->per_lane_control;
  int val =0, mask = 0;
  int odd = (cntl & 0xff00) >> 8;
  int even = (cntl & 0xff);
  int enableEven = (cntl & 0x01000000);
  int enableOdd  = (cntl & 0x02000000);

  if (pc->verbosity > 0) printf("%-22s: odd:%x even:%x\n",FUNCTION_NAME(), odd,even);
  if (enableEven) {
    val |= ((even << DSC2B0_DSC_ANA_CTRL0_P1_EVN_CTRL_SHIFT)  |
             DSC2B0_DSC_ANA_CTRL0_FORCE_P1_EVN_CTRL_MASK);
    mask =   DSC2B0_DSC_ANA_CTRL0_P1_EVN_CTRL_MASK  |
             DSC2B0_DSC_ANA_CTRL0_FORCE_P1_EVN_CTRL_MASK;
    if (pc->verbosity > 0) printf("%-22s:p1_even_val:%x p1_even_mask:%x\n",FUNCTION_NAME(), val,mask);
  } else {
    val |=  0;
    mask|=  DSC2B0_DSC_ANA_CTRL0_FORCE_P1_EVN_CTRL_MASK;
  }

  if (enableOdd) {
    val |= ((odd << DSC2B0_DSC_ANA_CTRL0_P1_ODD_CTRL_SHIFT) |
             DSC2B0_DSC_ANA_CTRL0_FORCE_P1_ODD_CTRL_MASK);
    mask |=   DSC2B0_DSC_ANA_CTRL0_P1_ODD_CTRL_MASK  |
             DSC2B0_DSC_ANA_CTRL0_FORCE_P1_ODD_CTRL_MASK;
    if (pc->verbosity > 0) printf("%-22s:p1_even_odd_val:%x p1_even_odd_mask:%x\n",FUNCTION_NAME(), val,mask);
  } else {
    mask|=  DSC2B0_DSC_ANA_CTRL0_FORCE_P1_ODD_CTRL_MASK;
  }

  if (pc->verbosity > 0) printf("%-22s:p1_even_odd_val:%x p1_even_odd_mask:%x\n",FUNCTION_NAME(), val,mask);
  MODIFY_WC40_DSC2B0_DSC_ANA_CTRL0r(pc->unit,pc,
                    val | DSC2B0_DSC_ANA_CTRL0_FORCE_P1_CTRL_STROBE_MASK,
                    mask| DSC2B0_DSC_ANA_CTRL0_FORCE_P1_CTRL_STROBE_MASK);
  return SOC_E_NONE;
}

/*!
\brief sets m1 slicers for the receive path.

\param  pc handle to current warpcore context (a.k.a wcmod struct)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.

\details

\li first byte  is m1  even control (6 bits)
\li second byte is m1  odd  control (6 bits)
\li third byte is ignored
\li fourth byte is set to 1 to enable even, 2 to enable odd

\li first and second byte (12 bits relevent) contain slicer value.

<B>How to call:</B><br>
<p>Call this function directly or through the function
#wcmod_tier1_selector() with the upper-case string
"RX_M1_SLICER_CONTROL".</p>
*/

int wcmod_rx_m1_slicer_control(wcmod_st* pc)
{
  int cntl = pc->per_lane_control;
  int val =0, mask = 0;
  int odd = (cntl & 0xff00) >> 8;
  int even = (cntl & 0xff);
  int enableEven = (cntl & 0x01000000);
  int enableOdd  = (cntl & 0x02000000);

  if (pc->verbosity > 0) printf("%-22s: odd:%x even:%x\n",FUNCTION_NAME(), odd,even);
  if (enableEven) {
    val |= ((even << DSC2B0_DSC_ANA_CTRL2_M1_EVN_CTRL_SHIFT) |
             DSC2B0_DSC_ANA_CTRL2_FORCE_M1_EVN_CTRL_MASK);
    mask =   DSC2B0_DSC_ANA_CTRL2_M1_EVN_CTRL_MASK  |
             DSC2B0_DSC_ANA_CTRL2_FORCE_M1_EVN_CTRL_MASK;
  } else {
    val |=  0;
    mask|=  DSC2B0_DSC_ANA_CTRL2_FORCE_M1_EVN_CTRL_MASK;
  }

  if (enableOdd) {
    val |= ((odd << DSC2B0_DSC_ANA_CTRL2_M1_ODD_CTRL_SHIFT) |
             DSC2B0_DSC_ANA_CTRL2_FORCE_M1_ODD_CTRL_MASK);
    mask |=   DSC2B0_DSC_ANA_CTRL2_M1_ODD_CTRL_MASK  |
             DSC2B0_DSC_ANA_CTRL2_FORCE_M1_ODD_CTRL_MASK;
  } else {
    mask|=  DSC2B0_DSC_ANA_CTRL2_FORCE_M1_ODD_CTRL_MASK;
  }

  MODIFY_WC40_DSC2B0_DSC_ANA_CTRL2r(pc->unit,pc,
                    val | DSC2B0_DSC_ANA_CTRL2_FORCE_M1_CTRL_STROBE_MASK,
                    mask| DSC2B0_DSC_ANA_CTRL2_FORCE_M1_CTRL_STROBE_MASK);
  return SOC_E_NONE;
}

/*!
\brief sets d slicers for the receive path.

\param  pc handle to current warpcore context (a.k.a wcmod struct)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.

\details

\li first byte  is d  even control (6 bits)
\li second byte is d  odd  control (6 bits)
\li third byte is ignored
\li fourth byte is set to 1 to enable even, 2 to enable odd

\li first and second byte (12 bits relevent) contain slicer value.

<B>How to call:</B><br>
<p>Call this function directly or through the function
#wcmod_tier1_selector() with the upper-case string
"RX_D_SLICER_CONTROL".</p>
*/

int wcmod_rx_d_slicer_control(wcmod_st* pc)
{
  int cntl = pc->per_lane_control;
  int val =0, mask = 0;
  int odd = (cntl & 0xff00) >> 8;
  int even = (cntl & 0xff);
  int enableEven = (cntl & 0x01000000);
  int enableOdd  = (cntl & 0x02000000);

  if (pc->verbosity > 0) printf("%-22s: odd:%x even:%x\n",FUNCTION_NAME(), odd,even);
  if (enableEven) {
    val |= ((even << DSC2B0_DSC_ANA_CTRL1_D_EVN_CTRL_SHIFT) |
             DSC2B0_DSC_ANA_CTRL1_FORCE_D_EVN_CTRL_MASK);
    mask =   DSC2B0_DSC_ANA_CTRL1_D_EVN_CTRL_MASK  |
             DSC2B0_DSC_ANA_CTRL1_FORCE_D_EVN_CTRL_MASK;
  } else {
    val |=  0;
    mask|=  DSC2B0_DSC_ANA_CTRL1_FORCE_D_EVN_CTRL_MASK;
  }

  if (enableOdd) {
    val |= ((odd << DSC2B0_DSC_ANA_CTRL1_D_ODD_CTRL_SHIFT) |
             DSC2B0_DSC_ANA_CTRL1_FORCE_D_ODD_CTRL_MASK);
    mask |=   DSC2B0_DSC_ANA_CTRL1_D_ODD_CTRL_MASK  |
             DSC2B0_DSC_ANA_CTRL1_FORCE_D_ODD_CTRL_MASK;
  } else {
    mask|=  DSC2B0_DSC_ANA_CTRL1_FORCE_D_ODD_CTRL_MASK;
  }

  MODIFY_WC40_DSC2B0_DSC_ANA_CTRL1r(pc->unit,pc,
                    val | DSC2B0_DSC_ANA_CTRL1_FORCE_D_CTRL_STROBE_MASK,
                    mask| DSC2B0_DSC_ANA_CTRL1_FORCE_D_CTRL_STROBE_MASK);
  return SOC_E_NONE;
}

/*!
\brief Controls TX pre, main and post tap values.

\param  pc handle to current warpcore context (a.k.a wcmod struct)

\returns nothing

\details
This function is used to override the TX pre, main, and post tap values.

\li main_tap_adjust_force   : Override enable to control main_tap_adjust_en
\li main_tap_adjust_forceval: override value of main_tap_adjust_en
\li main_tap_adjust_en      : When set and in in non-Cl72 modes, adjust main_tap
as follows: main_tap = MAX(main_tap_limit - post_tap - pre_tap, main_tap).

NOTE: main_tap_limit defaults to 6'd63
Reset value is 1.

Only one lane can be modified at a time.  Input to this functions is in
#wcmod_st::per_lane_control as follows.

\li 5:0   Pre tap value (7:6   Not used)
\li 24:24 Enable Pre Tap Override.
\li 13:8  Main tap value (15:14 Not used)
\li 25:25 Enable Main Tap Override.
\li 22:16 Post Tap value (23:22 Not used)
\li 26:26 Enable Post Tap Override.

\li 27:27 Release the force enable and undo override capability.

To override pre tap with say 0x1a' set per_lane_control to 0x0100001a
To override pre tap and post tap with 0x1a' set per_lane_control to 0x03001a1a

<B>How to call:</B><br>
<p>Call this function directly or through the function
#wcmod_tier1_selector() with the upper-case string "TX_TAP_CONTROL".</p>
*/
int wcmod_tx_tap_control(wcmod_st* pc)
{
  int cntl = pc->per_lane_control;
  int val = 0, mask = 0, mask_cntl = 0;
  uint16 tmp_lane, post, main_tap;
  
  post = 0;
  main_tap = 0;  

  if (pc->model_type == WCMOD_QS_A0) {
    tmp_lane = pc->this_lane;
    pc->this_lane = (pc->this_lane / 4) * 4;
  } else {
    tmp_lane = pc->this_lane;
  }

  if ((pc->model_type != WCMOD_QS_A0) && (tmp_lane >= 4)) {
    printf ("%s FATAL: Internal. Bad lane:%d\n", FUNCTION_NAME(), tmp_lane);
    return SOC_E_ERROR;
  }

  if (pc->model_type == WCMOD_XN) {
      val = cntl & 0xf;
      switch(pc->this_lane) {
        case 0:    /* 0x8065 */
            MODIFY_WC40_TX0_ANATXACONTROL1r(pc->unit, pc,  val << 11, 0x7800);
            break;
        case 1:    /* 0x8075 */
            MODIFY_WC40_TX1_ANATXACONTROL1r(pc->unit, pc,  val << 11, 0x7800);
            break;
        case 2:    /* 0x8085 */
            MODIFY_WC40_TX2_ANATXACONTROL1r(pc->unit, pc,  val << 11, 0x7800);
            break;
        case 3:    /* 0x8095 */
            MODIFY_WC40_TX3_ANATXACONTROL1r(pc->unit, pc,  val << 11, 0x7800);
            break;
        default:
            break;
      }
      pc->this_lane = tmp_lane;
  } else if (pc->model_type == WCMOD_QS_A0) {
      if (cntl & 0x2000000) { /* set main override */
        mask = CL72_USERB0_CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_MAIN_MASK >>
               CL72_USERB0_CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_MAIN_SHIFT ;
        main_tap = (((cntl & 0xff00)>>8) & mask) ;
      }
      if (cntl & 0x4000000) { /* set post override */
        mask = CL72_USERB0_CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_POST_MASK >>
               CL72_USERB0_CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_POST_SHIFT;
        post = (((cntl & 0xff0000)>>16) & mask);
      }

      MODIFY_WC40_TX0_ANATXACONTROL2r(pc->unit, pc,  main_tap << 10, 0x7c00);    /* 0x8066 */
      MODIFY_WC40_TX0_ANATXACONTROL2r(pc->unit, pc, (post & 0x1) << 15, 0x8000); 
      MODIFY_WC40_TX0_TX_DRIVERr(pc->unit, pc, (post & 0x1e) >> 1, 0xf);         /* 0x8067 */

      pc->this_lane = tmp_lane;
  } else {
      if (cntl & 0x8000000) {
        MODIFY_WC40_CL72_USERB0_CL72_TX_FIR_TAP_REGISTERr(pc->unit, pc,
                0, CL72_USERB0_CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_FORCE_MASK );
        return SOC_E_NONE;
      }
      if (cntl & 0x1000000) { /* set pre override */
        mask_cntl |= CL72_USERB0_CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_PRE_MASK;
        mask = CL72_USERB0_CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_PRE_MASK >>
               CL72_USERB0_CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_PRE_SHIFT;
        val = ((cntl & 0xff)  & mask)
               << CL72_USERB0_CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_PRE_SHIFT;

      }
      if (cntl & 0x2000000) { /* set main override */
        mask_cntl |= CL72_USERB0_CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_MAIN_MASK;
        mask = CL72_USERB0_CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_MAIN_MASK >>
               CL72_USERB0_CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_MAIN_SHIFT ;
        val |= (((cntl & 0xff00)>>8) & mask)
                << CL72_USERB0_CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_MAIN_SHIFT;
      }

      if (cntl & 0x4000000) { /* set post override */
        mask_cntl |= CL72_USERB0_CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_POST_MASK;
        mask = CL72_USERB0_CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_POST_MASK >>
               CL72_USERB0_CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_POST_SHIFT;
        val |= (((cntl & 0xff0000)>>16) & mask)
                << CL72_USERB0_CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_POST_SHIFT;
      }

      if (cntl & 0x7000000) {  /* was using || */
        MODIFY_WC40_CL72_USERB0_CL72_TX_FIR_TAP_REGISTERr(pc->unit, pc,
          (val | CL72_USERB0_CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_FORCE_MASK),
          (mask_cntl | CL72_USERB0_CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_FORCE_MASK));
      }
  }
  return SOC_E_NONE;
}


/*!
\brief Controls transmit phase interpolator

\param  pc handle to current warpcore context (a.k.a wcmod struct)

\returns nothing

\details
This function can be used to force the tansmit interpolator with any of the
four receive interpolator.  The core has only one Tx interpolator. Therefore
any selection of phase will be used for all four Tx lanes.

\li tx_pi_en: Enables Rx-phase interpolator to be used for Tx-phase interpolator
\li tx_pi_rx_lane_sel Selects Rx-phase interpolator

This function modifies the following registers:

\li XGXSBLK8_TX_PI_CONTROL1 (0x8164[0]) for tx_pi_en
\li XGXSBLK8_TX_PI_CONTROL2 (0x8165[1:0]) for tx_pi_rx_lane_sel

Reset value is 1.

\li First byte sets the Rx lane number. Bit0 of 4th byte controls tx_pi_en bit.

<B>How to call:</B><br>
<p>Call this function directly or through the function
#wcmod_tier1_selector() with the upper-case string
"TX_PI_CONTROL".</p>
*/

int wcmod_tx_pi_control(wcmod_st* pc)
{
  int cntl = pc->per_lane_control;

  if (cntl & 0x1) {
    MODIFY_WC40_XGXSBLK8_TX_PI_CONTROL1r(pc->unit, pc,
                            XGXSBLK8_TX_PI_CONTROL1_TX_PI_EN_MASK,
                            XGXSBLK8_TX_PI_CONTROL1_TX_PI_EN_MASK);
    /* TX PI Frequency Override Enable BIT[0]
       Frequency Override value BIT[15:1] 
     */
    WRITE_WC40_XGXSBLK8_TX_PI_CONTROL3r(pc->unit, pc, cntl);
  } else {
    MODIFY_WC40_XGXSBLK8_TX_PI_CONTROL1r(pc->unit, pc, 0,
                            XGXSBLK8_TX_PI_CONTROL1_TX_PI_EN_MASK);
    /* TX PI Frequency Override Enable BIT[0]
       Frequency Override value BIT[15:1] 
     */
    WRITE_WC40_XGXSBLK8_TX_PI_CONTROL3r(pc->unit, pc, 0);
  }
  return SOC_E_NONE;
}

/*!
\brief Controls receiver peaking filter value.

\param  pc handle to current warpcore context (a.k.a #wcmod_st)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.

\details

This function is used to modify the peaking filter settings on a per lane
basis.  To change the peaking filter settings, set #wcmod_st::lane_select to
the desired lane selection setting and then modify #wcmod_st::per_lane_control,
which is a bit-wise ORed, value as desired.

The peaking filter amplitude can be adjusted with the lower four bits of
#wcmod_st::per_lane_control.  However, in order to effect a change in the
peaking filter amplitude, the desired value must be ORed with the constant
#CHANGE_RX_PF_VALUES.  As an example to set the peaking filter amplitude to
0xA, set #wcmod_st::per_lane_control to

#wcmod_st::per_lane_control = 0xA | #CHANGE_RX_PF_VALUES

By simultaneously bit-wise ORing other constants into
#wcmod_st::per_lane_control, other peaking filter programming settings may be
altered, including the RX peaking filter threshold, whether the RX M1 threshold
should be on/off, and whether the peaking filter bandwidth should be an
extended or normal range.

Permissible values for the RX peaking filter threshold are

\li #RX_THRESH_125_MV
\li #RX_THRESH_150_MV
\li #RX_THRESH_175_MV
\li #RX_THRESH_200_MV

To enable/disable M1 theshold zero, bit-wise OR in one of the following values

\li #RX_M1_THRESH_ZERO_ON
\li #RX_M1_THRESH_ZERO_OFF

To select extended peaking filter bandwidth, bit-wise OR in the following value
\li #RX_PF_HIZ_ON

To select a normal peaking filter bandwidth, bit-wise OR in the following value
\li #RX_PF_HIZ_OFF

If RX peaking filter voltage threshold, M1 threshold zero, or bandwidth values
are not specified, then the respective settings are not changed by this
function.

As an example of function usage, if it desired to set the peaking filter
amplitude to 0x6 with a threshold voltage of 150 mV with an extended bandwidth
(while the M1 threshold zero settings remain unchanged), set
#wcmod_st::per_lane_control as follows

#wcmod_st::per_lane_control = 0x6 |
                              #CHANGE_RX_PF_VALUES |
                              #RX_THRESH_150_MV |
                              #RX_PF_HIZ_ON

\li RX_PF_VAL_MASK        0xF
\li CHANGE_RX_PF_VALUES   0x10
\li RX_THRESH_125_MV      0x20
\li RX_THRESH_150_MV      0x40
\li RX_THRESH_175_MV      0x80
\li RX_THRESH_200_MV      0x100
\li RX_M1_THRESH_ZERO_ON  0x200
\li RX_M1_THRESH_ZERO_OFF 0x400
\li RX_PF_HIZ_ON          0x800
\li RX_PF_HIZ_OFF         0x1000

<B>How to call:</B><br>
<p>Call this function directly or through the function #wcmod_tier1_selector()
with the upper-case string "RX_PF_CONTROL".</p>
*/
int wcmod_rx_PF_control(wcmod_st* pc)
{
    int cntl = pc->per_lane_control;
    int mask = 0;
    int val = 0;
    int supl_info = 1; /* 1 means uc will not touch PF */
    int data, rv;
    int supl_info_mask = 0xfe00;
    uint16 tmp_data;

    /* first check which tap is frozen */
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DSC1B0_UC_CTRLr(pc->unit, pc, 0x0, DSC1B0_UC_CTRL_READY_FOR_CMD_MASK));
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DSC1B0_UC_CTRLr(pc->unit, pc, 0x0, DSC1B0_UC_CTRL_ERROR_FOUND_MASK));
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DSC1B0_UC_CTRLr(pc->unit, pc, 0x0, DSC1B0_UC_CTRL_CMD_INFO_MASK));

    val = (0x4 << DSC1B0_UC_CTRL_GP_UC_REQ_SHIFT) |
          (0x80 << DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT);
    mask  = (DSC1B0_UC_CTRL_GP_UC_REQ_MASK  |
             DSC1B0_UC_CTRL_SUPPLEMENT_INFO_MASK);
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DSC1B0_UC_CTRLr(pc->unit, pc, val, mask));

    /* wait for uC ready for command:  bit7=1    */
    rv = wcmod_regbit_set_wait_check(pc, DSC1B0_UC_CTRLr,
             DSC1B0_UC_CTRL_READY_FOR_CMD_MASK, 1, WC40_PLL_WAIT*1000);

    data = pc->accData;
    pc->accData = ((pc->accData) & DSC1B0_UC_CTRL_ERROR_FOUND_MASK) >> DSC1B0_UC_CTRL_ERROR_FOUND_SHIFT;

    if ((rv <0) || (pc->accData != 0)) {
       printf("RX_PEAK_FILTER : uController not ready pass 1!: u=%d\n", pc->unit);
       return (SOC_E_FAIL);
    }

    data &= DSC1B0_UC_CTRL_SUPPLEMENT_INFO_MASK & supl_info_mask;

    if (cntl & RX_UC_CONTROL) { /* this bit is common to all RX parms */
        /*need to check if DSc done set or not */
        switch (pc->this_lane) {
        case 0: 
            SOC_IF_ERROR_RETURN(READ_WC40_UC_INFO_B0_STATE_DEBUG_LN0r(pc->unit, pc, &tmp_data));
            if (!(tmp_data & UC_INFO_B0_STATE_DEBUG_LN0_DSC_DONE_MASK)) { 
                printf("RX_PEAK_FILTER : DSC done bit is not set!: u=%d\n", pc->unit);
                return (SOC_E_FAIL);
            }
            break;
        case 1: 
            SOC_IF_ERROR_RETURN(READ_WC40_UC_INFO_B0_STATE_DEBUG_LN1r(pc->unit, pc, &tmp_data));
            if (!(tmp_data & UC_INFO_B0_STATE_DEBUG_LN1_DSC_DONE_MASK)) { 
                printf("RX_PEAK_FILTER : DSC done bit is not set!: u=%d\n", pc->unit);
                return (SOC_E_FAIL);
            }
            break;
        case 2: 
            SOC_IF_ERROR_RETURN(READ_WC40_UC_INFO_B0_STATE_DEBUG_LN2r(pc->unit, pc, &tmp_data));
            if (!(tmp_data & UC_INFO_B0_STATE_DEBUG_LN2_DSC_DONE_MASK)) { 
                printf("RX_PEAK_FILTER : DSC done bit is not set!: u=%d\n", pc->unit);
                return (SOC_E_FAIL);
            }
            break;
        case 3: 
            SOC_IF_ERROR_RETURN(READ_WC40_UC_INFO_B0_STATE_DEBUG_LN3r(pc->unit, pc, &tmp_data));
            if (!(tmp_data & UC_INFO_B0_STATE_DEBUG_LN3_DSC_DONE_MASK)) { 
                printf("RX_PEAK_FILTER : DSC done bit is not set!: u=%d\n", pc->unit);
                return (SOC_E_FAIL);
            }
            break;
        default:
            break;
        }
        /* first thing to enable diagonstic */
        SOC_IF_ERROR_RETURN(MODIFY_WC40_DSC1B0_DSC_DIAG_CTRL0r(pc->unit, pc, 
                 DSC1B0_DSC_DIAG_CTRL0_DIAGNOSTICS_EN_MASK,
                 DSC1B0_DSC_DIAG_CTRL0_DIAGNOSTICS_EN_MASK));
        /* 4 is the command code for PF/VGA/DFE control  */
        val = (0x4 << DSC1B0_UC_CTRL_GP_UC_REQ_SHIFT) | data |
              (supl_info << DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT);
        mask  = (DSC1B0_UC_CTRL_GP_UC_REQ_MASK  |
                 DSC1B0_UC_CTRL_SUPPLEMENT_INFO_MASK);
        SOC_IF_ERROR_RETURN (MODIFY_WC40_DSC1B0_UC_CTRLr(pc->unit, pc, val, mask));

        /* wait for uC ready for command:  bit7=1    */
        rv = wcmod_regbit_set_wait_check(pc, DSC1B0_UC_CTRLr,
                 DSC1B0_UC_CTRL_READY_FOR_CMD_MASK, 1, WC40_PLL_WAIT*1000);

        data = pc->accData;
        pc->accData = ((pc->accData) & DSC1B0_UC_CTRL_ERROR_FOUND_MASK) >> DSC1B0_UC_CTRL_ERROR_FOUND_SHIFT;

        if ((rv <0) || (pc->accData != 0)) {
           printf("RX_PEAK_FILTER : uController not ready pass 1!: u=%d\n", pc->unit);
        }
        val = 0;
        mask = 0;

        /* Change the RX peaking filter values + turn on value forcing */
        if (cntl & CHANGE_RX_PF_VALUES) {
        val  |= (cntl & RX_PF_VAL_MASK) | CHANGE_RX_PF_VALUES;
        mask |= DSC2B0_DSC_ANA_CTRL3_RX_PF_CTRL_MASK |
          DSC2B0_DSC_ANA_CTRL3_FORCE_RX_PF_CTRL_MASK;
        } else {
        return SOC_E_NONE;
        }

        /* Set the RX threshold voltage */
        if (cntl & RX_THRESH_125_MV) {
        val  |= 0x0 << DSC2B0_DSC_ANA_CTRL3_RX_THRESH_SEL_SHIFT;
        mask |= DSC2B0_DSC_ANA_CTRL3_RX_THRESH_SEL_MASK;
        } else if (cntl & RX_THRESH_150_MV) {
        val  |= 0x1 << DSC2B0_DSC_ANA_CTRL3_RX_THRESH_SEL_SHIFT;
        mask |= DSC2B0_DSC_ANA_CTRL3_RX_THRESH_SEL_MASK;
        } else if (cntl & RX_THRESH_175_MV) {
        val  |= 0x2 << DSC2B0_DSC_ANA_CTRL3_RX_THRESH_SEL_SHIFT;
        mask |= DSC2B0_DSC_ANA_CTRL3_RX_THRESH_SEL_MASK;
        } else if (cntl & RX_THRESH_200_MV) {
        val  |= 0x3 << DSC2B0_DSC_ANA_CTRL3_RX_THRESH_SEL_SHIFT;
        mask |= DSC2B0_DSC_ANA_CTRL3_RX_THRESH_SEL_MASK;
        }

        /* Turn on/off the M1 threshold zero + turn on zero forcing */
        if (cntl & RX_M1_THRESH_ZERO_ON) {
        val  |= DSC2B0_DSC_ANA_CTRL3_RX_M1_THRESH_ZERO_MASK;
        mask |= DSC2B0_DSC_ANA_CTRL3_RX_M1_THRESH_ZERO_MASK |
          DSC2B0_DSC_ANA_CTRL3_FORCE_RX_M1_THRESH_ZERO_MASK;
        } else if (cntl & RX_M1_THRESH_ZERO_OFF) {
        mask |= DSC2B0_DSC_ANA_CTRL3_RX_M1_THRESH_ZERO_MASK |
          DSC2B0_DSC_ANA_CTRL3_FORCE_RX_M1_THRESH_ZERO_MASK;
        }

        /* Set the peaking filter BW (RX_PF_HIZ_ON) to an extended range or
         to a normal range (RX_PF_HIZ_OFF) */
        if (cntl & RX_PF_HIZ_ON) {
        val  |= DSC2B0_DSC_ANA_CTRL3_RX_PF_HIZ_MASK;
        mask |= DSC2B0_DSC_ANA_CTRL3_RX_PF_HIZ_MASK;
        } else if (cntl & RX_PF_HIZ_OFF) {
        mask |= DSC2B0_DSC_ANA_CTRL3_RX_PF_HIZ_MASK;
        }

        MODIFY_WC40_DSC2B0_DSC_ANA_CTRL3r(pc->unit, pc, val, mask);
        /* last thing to disable diagonstic */
        SOC_IF_ERROR_RETURN(MODIFY_WC40_DSC1B0_DSC_DIAG_CTRL0r(pc->unit, pc, 0, 
                 DSC1B0_DSC_DIAG_CTRL0_DIAGNOSTICS_EN_MASK));
    } else {
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_DSC1B0_UC_CTRLr(pc->unit, pc, 0x0, DSC1B0_UC_CTRL_READY_FOR_CMD_MASK));
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_DSC1B0_UC_CTRLr(pc->unit, pc, 0x0, DSC1B0_UC_CTRL_ERROR_FOUND_MASK));
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_DSC1B0_UC_CTRLr(pc->unit, pc, 0x0, DSC1B0_UC_CTRL_CMD_INFO_MASK));

        val = (0x4 << DSC1B0_UC_CTRL_GP_UC_REQ_SHIFT) | data |
              /* 1 means uc will not touch PF */
              (0 << DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT);
        mask  = (DSC1B0_UC_CTRL_GP_UC_REQ_MASK  |
                 /* use only 1 bit of the mask */
                (supl_info << DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT));
        MODIFY_WC40_DSC1B0_UC_CTRLr(pc->unit, pc, val, mask);
        /* wait for uC ready for command:  bit7=1    */
        rv = wcmod_regbit_set_wait_check(pc, DSC1B0_UC_CTRLr,
                 DSC1B0_UC_CTRL_READY_FOR_CMD_MASK, 1, WC40_PLL_WAIT*1000);

        data = pc->accData;
        pc->accData = ((pc->accData) & DSC1B0_UC_CTRL_ERROR_FOUND_MASK) >> DSC1B0_UC_CTRL_ERROR_FOUND_SHIFT;

        if ((rv <0) || (pc->accData != 0)) {
           printf("RX_PEAK_FILTER : uController not ready pass 1!: u=%d\n", pc->unit);
        }

    }

    pc->lane_num_ignore = 0;

    return SOC_E_NONE;
}

/*
<B>How to call:</B><br>
<p>Call this function directly or through the function #wcmod_tier1_selector()
with the upper-case string "RX_LOW_FREQ_PF_CONTROL".</p>
*/
int wcmod_rx_low_freq_PF_control(wcmod_st* pc)
{
    int cntl = pc->per_lane_control;
    int mask = 0;
    int val = 0;
    int supl_info = 1; /* 1 means uc will not touch PF */
    int data, rv;
    int supl_info_mask = 0xfe00;
    uint16 tmp_data;

    /* first check which tap is frozen */
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DSC1B0_UC_CTRLr(pc->unit, pc, 0x0, DSC1B0_UC_CTRL_READY_FOR_CMD_MASK));
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DSC1B0_UC_CTRLr(pc->unit, pc, 0x0, DSC1B0_UC_CTRL_ERROR_FOUND_MASK));
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DSC1B0_UC_CTRLr(pc->unit, pc, 0x0, DSC1B0_UC_CTRL_CMD_INFO_MASK));

    val = (0x4 << DSC1B0_UC_CTRL_GP_UC_REQ_SHIFT) |
          (0x80 << DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT);
    mask  = (DSC1B0_UC_CTRL_GP_UC_REQ_MASK  |
             DSC1B0_UC_CTRL_SUPPLEMENT_INFO_MASK);
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DSC1B0_UC_CTRLr(pc->unit, pc,val, mask));

    /* wait for uC ready for command:  bit7=1    */
    rv = wcmod_regbit_set_wait_check(pc, DSC1B0_UC_CTRLr,
             DSC1B0_UC_CTRL_READY_FOR_CMD_MASK, 1, WC40_PLL_WAIT*1000);

    data = pc->accData;
    pc->accData = ((pc->accData) & DSC1B0_UC_CTRL_ERROR_FOUND_MASK) >> DSC1B0_UC_CTRL_ERROR_FOUND_SHIFT;

    if ((rv <0) || (pc->accData != 0)) {
       printf("RX_LOW_FREQ_PF : uController not ready pass 1!: u=%d\n", pc->unit);
       return (SOC_E_FAIL);
    }

    data &= DSC1B0_UC_CTRL_SUPPLEMENT_INFO_MASK & supl_info_mask; 

    if (cntl & RX_UC_CONTROL) { /* this bit is common to all RX parms */
        /*need to check if DSc done set or not */
        switch (pc->this_lane) {
        case 0: 
            SOC_IF_ERROR_RETURN(READ_WC40_UC_INFO_B0_STATE_DEBUG_LN0r(pc->unit, pc, &tmp_data));
            if (!(tmp_data & UC_INFO_B0_STATE_DEBUG_LN0_DSC_DONE_MASK)) { 
                printf("RX_LOW_FREQ_PF : DSC done bit is not set!: u=%d\n", pc->unit);
                return (SOC_E_FAIL);
            }
            break;
        case 1: 
            SOC_IF_ERROR_RETURN(READ_WC40_UC_INFO_B0_STATE_DEBUG_LN1r(pc->unit, pc, &tmp_data));
            if (!(tmp_data & UC_INFO_B0_STATE_DEBUG_LN1_DSC_DONE_MASK)) { 
                printf("RX_LOW_FREQ_PF : DSC done bit is not set!: u=%d\n", pc->unit);
                return (SOC_E_FAIL);
            }
            break;
        case 2: 
            SOC_IF_ERROR_RETURN(READ_WC40_UC_INFO_B0_STATE_DEBUG_LN2r(pc->unit, pc, &tmp_data));
            if (!(tmp_data & UC_INFO_B0_STATE_DEBUG_LN2_DSC_DONE_MASK)) { 
                printf("RX_LOW_FREQ_PF : DSC done bit is not set!: u=%d\n", pc->unit);
                return (SOC_E_FAIL);
            }
            break;
        case 3: 
            SOC_IF_ERROR_RETURN(READ_WC40_UC_INFO_B0_STATE_DEBUG_LN3r(pc->unit, pc, &tmp_data));
            if (!(tmp_data & UC_INFO_B0_STATE_DEBUG_LN3_DSC_DONE_MASK)) { 
                printf("RX_LOW_FREQ_PF : DSC done bit is not set!: u=%d\n", pc->unit);
                return (SOC_E_FAIL);
            }
            break;
        default:
            break;
        }
        /* first thing to enable diagonstic */
        SOC_IF_ERROR_RETURN(MODIFY_WC40_DSC1B0_DSC_DIAG_CTRL0r(pc->unit, pc, 
                 DSC1B0_DSC_DIAG_CTRL0_DIAGNOSTICS_EN_MASK,
                 DSC1B0_DSC_DIAG_CTRL0_DIAGNOSTICS_EN_MASK));
        /* 4 is the command code for PF/VGA/DFE control  */
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_DSC1B0_UC_CTRLr(pc->unit, pc, 0x0, DSC1B0_UC_CTRL_READY_FOR_CMD_MASK));
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_DSC1B0_UC_CTRLr(pc->unit, pc, 0x0, DSC1B0_UC_CTRL_ERROR_FOUND_MASK));
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_DSC1B0_UC_CTRLr(pc->unit, pc, 0x0, DSC1B0_UC_CTRL_CMD_INFO_MASK));

        val = (0x4 << DSC1B0_UC_CTRL_GP_UC_REQ_SHIFT) | data |
              (supl_info << DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT);
        mask  = (DSC1B0_UC_CTRL_GP_UC_REQ_MASK  |
                 DSC1B0_UC_CTRL_SUPPLEMENT_INFO_MASK);
        SOC_IF_ERROR_RETURN (MODIFY_WC40_DSC1B0_UC_CTRLr(pc->unit, pc, val, mask));

        /* wait for uC ready for command:  bit7=1    */
        rv = wcmod_regbit_set_wait_check(pc, DSC1B0_UC_CTRLr,
                 DSC1B0_UC_CTRL_READY_FOR_CMD_MASK, 1, WC40_PLL_WAIT*1000);

        data = pc->accData;
        pc->accData = ((pc->accData) & DSC1B0_UC_CTRL_ERROR_FOUND_MASK) >> DSC1B0_UC_CTRL_ERROR_FOUND_SHIFT;

        if ((rv <0) || (pc->accData != 0)) {
           printf("RX_LOW_FREQ_PF : uController not ready pass 1!: u=%d\n", pc->unit);
        }
        val = 0;
        mask = 0;
        /* Change the RX low freq peaking filter values */
        if (cntl & CHANGE_RX_LOW_FREQ_PF_VALUES) {
        val  |= (cntl & RX_PF_VAL_MASK) << DSC2B0_DSC_ANA_CTRL4_RX_PF2_LOWP_CTRL_SHIFT;
        mask |= DSC2B0_DSC_ANA_CTRL4_RX_PF2_LOWP_CTRL_MASK;
        MODIFY_WC40_DSC2B0_DSC_ANA_CTRL4r(pc->unit, pc, val, mask);
        val = (cntl & RX_PF_VAL_MASK) << DSC2B0_DSC_ANA_CTRL4_RX_PF2_ALLP_CTRL_SHIFT;
        mask = DSC2B0_DSC_ANA_CTRL4_RX_PF2_ALLP_CTRL_MASK;
        MODIFY_WC40_DSC2B0_DSC_ANA_CTRL4r(pc->unit, pc, val, mask);
        } else {
        return SOC_E_NONE;
        }
        /* last thing to disable diagonstic */
        SOC_IF_ERROR_RETURN(MODIFY_WC40_DSC1B0_DSC_DIAG_CTRL0r(pc->unit, pc, 0, 
                 DSC1B0_DSC_DIAG_CTRL0_DIAGNOSTICS_EN_MASK));
    } else {
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_DSC1B0_UC_CTRLr(pc->unit, pc, 0x0, DSC1B0_UC_CTRL_READY_FOR_CMD_MASK));
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_DSC1B0_UC_CTRLr(pc->unit, pc, 0x0, DSC1B0_UC_CTRL_ERROR_FOUND_MASK));
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_DSC1B0_UC_CTRLr(pc->unit, pc, 0x0, DSC1B0_UC_CTRL_CMD_INFO_MASK));

        val = (0x4 << DSC1B0_UC_CTRL_GP_UC_REQ_SHIFT) | data |
              /* 1 means uc will not touch PF */
              (0 << DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT);
        mask  = (DSC1B0_UC_CTRL_GP_UC_REQ_MASK  |
                 /* use only 1 bit of the mask */
                (supl_info << DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT));
        MODIFY_WC40_DSC1B0_UC_CTRLr(pc->unit, pc, val, mask);
        /* wait for uC ready for command:  bit7=1    */
        rv = wcmod_regbit_set_wait_check(pc, DSC1B0_UC_CTRLr,
                 DSC1B0_UC_CTRL_READY_FOR_CMD_MASK, 1, WC40_PLL_WAIT*1000);

        data = pc->accData;
        pc->accData = ((pc->accData) & DSC1B0_UC_CTRL_ERROR_FOUND_MASK) >> DSC1B0_UC_CTRL_ERROR_FOUND_SHIFT;

        if ((rv <0) || (pc->accData != 0)) {
           printf("RX_LOW_FREQ_PF : uController not ready pass 1!: u=%d\n", pc->unit);
        }
    }

    pc->lane_num_ignore = 0;

    return SOC_E_NONE;
}

/*!
\brief An internal function for setting the DFE and VGA values

\param  pc handle to current warpcore context (a.k.a #wcmod_st)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.

\details
This function sets the DFE (tap_number ranges from 1 through 5 inclusive) or
VGA (tap_number = 0) tap settings.  Other functions wrap this one.
The actual value to be written is provided in #wcmod_st::per_lane_control

\li 5:0   Value to override.
\li 31:31 UC Control

*/
static int _wcmod_rx_DFE_VGA_control(wcmod_st* pc, int tap_number)
{
    int supl_info = 0, val, mask, supl_info_mask = 0;
    int tap_sel, tap_val;
    uint16 data;
    int rv;

    tap_sel = ((tap_number << DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_TAPSEL_SHIFT)
                    & DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_TAPSEL_MASK);
    tap_val = ((pc->per_lane_control << DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_VAL_SHIFT)
                    & DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_VAL_MASK);

    if      (tap_number == 0) { supl_info_mask = 0xfd00;}
    else if (tap_number == 1) { supl_info_mask = 0xfb00;}
    else if (tap_number == 2) { supl_info_mask = 0xf700;}
    else if (tap_number == 3) { supl_info_mask = 0xef00;}
    else if (tap_number == 4) { supl_info_mask = 0xdf00;}
    else if (tap_number == 5) { supl_info_mask = 0xbf00;}


    /* first check which tap is frozen */
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DSC1B0_UC_CTRLr(pc->unit, pc, 0x0, DSC1B0_UC_CTRL_READY_FOR_CMD_MASK));
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DSC1B0_UC_CTRLr(pc->unit, pc, 0x0, DSC1B0_UC_CTRL_ERROR_FOUND_MASK));
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DSC1B0_UC_CTRLr(pc->unit, pc, 0x0, DSC1B0_UC_CTRL_CMD_INFO_MASK));

    val = (0x4 << DSC1B0_UC_CTRL_GP_UC_REQ_SHIFT) |
          (0x80 << DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT);
    mask  = (DSC1B0_UC_CTRL_GP_UC_REQ_MASK  |
             DSC1B0_UC_CTRL_SUPPLEMENT_INFO_MASK);
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_DSC1B0_UC_CTRLr(pc->unit, pc, val, mask));

    /* wait for uC ready for command:  bit7=1    */
    rv = wcmod_regbit_set_wait_check(pc, DSC1B0_UC_CTRLr,
             DSC1B0_UC_CTRL_READY_FOR_CMD_MASK, 1, WC40_PLL_WAIT*1000);

    data = pc->accData;
    pc->accData = ((pc->accData) & DSC1B0_UC_CTRL_ERROR_FOUND_MASK) >> DSC1B0_UC_CTRL_ERROR_FOUND_SHIFT;

    if ((rv <0) || (pc->accData != 0)) {
       printf("RX_TAPS : uController not ready pass 1!: u=%d\n", pc->unit);
       return (SOC_E_FAIL);
    }

    data &= DSC1B0_UC_CTRL_SUPPLEMENT_INFO_MASK & supl_info_mask; 

    if (pc->per_lane_control  & RX_UC_CONTROL) {
        /*need to check if DSc done set or not */
        switch (pc->this_lane) {
        case 0: 
            SOC_IF_ERROR_RETURN(READ_WC40_UC_INFO_B0_STATE_DEBUG_LN0r(pc->unit, pc, &data));
            if (!(data & UC_INFO_B0_STATE_DEBUG_LN0_DSC_DONE_MASK)) { 
                printf("RX_TAPS : DSC done bit is not set!: u=%d\n", pc->unit);
                return (SOC_E_FAIL);
            }
            break;
        case 1: 
            SOC_IF_ERROR_RETURN(READ_WC40_UC_INFO_B0_STATE_DEBUG_LN1r(pc->unit, pc, &data));
            if (!(data & UC_INFO_B0_STATE_DEBUG_LN1_DSC_DONE_MASK)) { 
                printf("RX_TAPS : DSC done bit is not set!: u=%d\n", pc->unit);
                return (SOC_E_FAIL);
            }
            break;
        case 2: 
            SOC_IF_ERROR_RETURN(READ_WC40_UC_INFO_B0_STATE_DEBUG_LN2r(pc->unit, pc, &data));
            if (!(data & UC_INFO_B0_STATE_DEBUG_LN2_DSC_DONE_MASK)) { 
                printf("RX_TAPS : DSC done bit is not set!: u=%d\n", pc->unit);
                return (SOC_E_FAIL);
            }
            break;
        case 3: 
            SOC_IF_ERROR_RETURN(READ_WC40_UC_INFO_B0_STATE_DEBUG_LN3r(pc->unit, pc, &data));
            if (!(data & UC_INFO_B0_STATE_DEBUG_LN3_DSC_DONE_MASK)) { 
                printf("RX_TAPS : DSC done bit is not set!: u=%d\n", pc->unit);
                return (SOC_E_FAIL);
            }
            break;
        default:
            break;
        }
        /* first thing to enable diagonstic */
        SOC_IF_ERROR_RETURN(MODIFY_WC40_DSC1B0_DSC_DIAG_CTRL0r(pc->unit, pc, 
                 DSC1B0_DSC_DIAG_CTRL0_DIAGNOSTICS_EN_MASK,
                 DSC1B0_DSC_DIAG_CTRL0_DIAGNOSTICS_EN_MASK));
        /* take away  control from ucode */
        /* 4 is the command code for PF/VGA/DFE control  */
        if      (tap_number == 0) { supl_info = 0x2;}
        else if (tap_number == 1) { supl_info = 0x4;}
        else if (tap_number == 2) { supl_info = 0x8;}
        else if (tap_number == 3) { supl_info = 0x10;}
        else if (tap_number == 4) { supl_info = 0x20;}
        else if (tap_number == 5) { supl_info = 0x40;}

        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_DSC1B0_UC_CTRLr(pc->unit, pc, 0x0, DSC1B0_UC_CTRL_READY_FOR_CMD_MASK));
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_DSC1B0_UC_CTRLr(pc->unit, pc, 0x0, DSC1B0_UC_CTRL_ERROR_FOUND_MASK));
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_DSC1B0_UC_CTRLr(pc->unit, pc, 0x0, DSC1B0_UC_CTRL_CMD_INFO_MASK));
       
        val = (0x4 << DSC1B0_UC_CTRL_GP_UC_REQ_SHIFT) | data |
              (supl_info << DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT);
        mask  = (DSC1B0_UC_CTRL_GP_UC_REQ_MASK  |
                 DSC1B0_UC_CTRL_SUPPLEMENT_INFO_MASK);

        SOC_IF_ERROR_RETURN (MODIFY_WC40_DSC1B0_UC_CTRLr(pc->unit, pc, val, mask));

        /* wait for uC ready for command:  bit7=1    */
        rv = wcmod_regbit_set_wait_check(pc, DSC1B0_UC_CTRLr,
                 DSC1B0_UC_CTRL_READY_FOR_CMD_MASK, 1, WC40_PLL_WAIT*1000);

        data = pc->accData;
        pc->accData = ((pc->accData) & DSC1B0_UC_CTRL_ERROR_FOUND_MASK) >> DSC1B0_UC_CTRL_ERROR_FOUND_SHIFT;

        if ((rv <0) || (pc->accData != 0)) {
           printf("RX_TAPS : uController not ready pass 1!: u=%d\n", pc->unit);
        }

        if (tap_sel == 0) { /* this means VGA */
        MODIFY_WC40_DSC2B0_ACQ_SM_CTRL1r(pc->unit, pc,
                0xc, DSC2B0_ACQ_SM_CTRL1_VGA_FRZVAL_MASK|
                     DSC2B0_ACQ_SM_CTRL1_VGA_FRCFRZ_MASK);
        } else if (tap_number >= 1 && tap_number <= 5) {
        MODIFY_WC40_DSC2B0_ACQ_SM_CTRL1r(pc->unit, pc,
                (1 << (DSC2B0_ACQ_SM_CTRL1_DFE_FRZVAL_SHIFT + tap_number - 1) |
                     DSC2B0_ACQ_SM_CTRL1_DFE_FRCFRZ_MASK),
                     (1 << (DSC2B0_ACQ_SM_CTRL1_DFE_FRZVAL_SHIFT  + tap_number-1) |
                     DSC2B0_ACQ_SM_CTRL1_DFE_FRCFRZ_MASK));
        }
        MODIFY_WC40_DSC1B0_DFE_VGA_CTRL2r(pc->unit, pc,
                    tap_val | tap_sel,
                    DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_TAPSEL_MASK |
                    DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_VAL_MASK);

        /* enable write enable */
        MODIFY_WC40_DSC1B0_DFE_VGA_CTRL2r(pc->unit, pc,
                DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_EN_MASK,
                DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_EN_MASK);
        /* disable write enable */
        MODIFY_WC40_DSC1B0_DFE_VGA_CTRL2r(pc->unit, pc, 0,
                            DSC1B0_DFE_VGA_CTRL2_DFE_VGA_WRITE_EN_MASK);
        /*set the steady bypass for VGA overwrite*/
        if (tap_sel == 0) { /* this means VGA */
            SOC_IF_ERROR_RETURN(MODIFY_WC40_UC_INFO_B1_TUNING_STATE_BYPASSr(pc->unit, pc,
                     UC_INFO_B1_TUNING_STATE_BYPASS_BYPASS_STEADY_STATE_TUNING_MASK,
                     UC_INFO_B1_TUNING_STATE_BYPASS_BYPASS_STEADY_STATE_TUNING_MASK));
        }
        /* last thing to disable diagonstic */
        SOC_IF_ERROR_RETURN(MODIFY_WC40_DSC1B0_DSC_DIAG_CTRL0r(pc->unit, pc, 0, 
                 DSC1B0_DSC_DIAG_CTRL0_DIAGNOSTICS_EN_MASK));
    } else {
        if (tap_sel == 0) { /* this means VGA */
            SOC_IF_ERROR_RETURN(MODIFY_WC40_UC_INFO_B1_TUNING_STATE_BYPASSr(pc->unit, pc, 0,
                     UC_INFO_B1_TUNING_STATE_BYPASS_BYPASS_STEADY_STATE_TUNING_MASK));
        }
        /* give control back to ucode */
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_DSC1B0_UC_CTRLr(pc->unit, pc, 0x0, DSC1B0_UC_CTRL_READY_FOR_CMD_MASK));
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_DSC1B0_UC_CTRLr(pc->unit, pc, 0x0, DSC1B0_UC_CTRL_ERROR_FOUND_MASK));
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_DSC1B0_UC_CTRLr(pc->unit, pc, 0x0, DSC1B0_UC_CTRL_CMD_INFO_MASK));

        val = (0x4 << DSC1B0_UC_CTRL_GP_UC_REQ_SHIFT)| data;  
              /* 1 means uc will not touch PF */
           /*   (0 << DSC1B0_UC_CTRL_SUPPLEMENT_INFO_SHIFT);  */
        mask  = DSC1B0_UC_CTRL_GP_UC_REQ_MASK  |
                DSC1B0_UC_CTRL_SUPPLEMENT_INFO_MASK;
        MODIFY_WC40_DSC1B0_UC_CTRLr(pc->unit, pc, val, mask);
        /* wait for uC ready for command:  bit7=1    */
        rv = wcmod_regbit_set_wait_check(pc, DSC1B0_UC_CTRLr,
                 DSC1B0_UC_CTRL_READY_FOR_CMD_MASK, 1, WC40_PLL_WAIT*1000);

        data = pc->accData;
        pc->accData = ((pc->accData) & DSC1B0_UC_CTRL_ERROR_FOUND_MASK) >> DSC1B0_UC_CTRL_ERROR_FOUND_SHIFT;

        if ((rv <0) || (pc->accData != 0)) {
           printf("RX_VGA : uController not ready pass 1!: u=%d\n", pc->unit);
        }
    }

    if (pc->verbosity > 0) printf("%s tap_sel:0x%x tap_val:0x%x\n", FUNCTION_NAME(), tap_sel, tap_val);


    return SOC_E_NONE;
}

/*!
\brief Override Rx DFE tap1 value.

\param  pc handle to current warpcore context (a.k.a #wcmod_st)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.

\details

This function is to force receiver DFE tap1 of any lane to any desired value.

To change the 5-bit DFE tap1, set lower 5-bits of #wcmod_st::per_lane_control.

<B>How to call:</B><br>
<p>Call this function directly or through the function #wcmod_tier1_selector()
with the upper-case string "RX_DFE_TAP1_CONTROL".</p>
*/
int wcmod_rx_DFE_tap1_control(wcmod_st* pc)
{
  return _wcmod_rx_DFE_VGA_control(pc, 1);
}

/*!
\brief Control Rx DFE tap2 value.

\param  pc handle to current warpcore context (a.k.a #wcmod_st)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.

This function is to force receiver DFE tap2 of any lane to any desired value.

To change the DFE tap2 value, set lower 5-bits of #wcmod_st::per_lane_control.

<B>How to call:</B><br>
<p>Call this function directly or through the function #wcmod_tier1_selector()
with the upper-case string "RX_DFE_TAP2_CONTROL".</p>
*/
int wcmod_rx_DFE_tap2_control(wcmod_st* pc)
{
  return _wcmod_rx_DFE_VGA_control(pc, 2);
}

/*!
\brief Override Rx DFE tap3 value.

\param  pc handle to current warpcore context (a.k.a wcmod struct)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.

This function is to force receiver DFE tap3 of any lane to any desired value.

To change the DFE tap3 value, set lower 5-bits of #wcmod_st::per_lane_control.


<B>How to call:</B><br>
<p>Call this function directly or through the function #wcmod_tier1_selector()
with the upper-case string "RX_DFE_TAP3_CONTROL".</p>
*/
int wcmod_rx_DFE_tap3_control(wcmod_st* pc)
{
  return _wcmod_rx_DFE_VGA_control(pc, 3);
}

/*!
\brief Override Rx DFE tap4 value.

\param  pc handle to current warpcore context (a.k.a #wcmod_st)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.

This function is to force receiver DFE tap4 of any lane to any desired value.

To change the DFE tap4 value, set lower 5-bits of #wcmod_st::per_lane_control.


\details
This function is to force receiver DFE tap4 to any desired value. Each lane can
be accessed through AER access.

<B>How to call:</B><br>
<p>Call this function directly or through the function #wcmod_tier1_selector()
with the upper-case string "RX_DFE_TAP4_CONTROL".</p>
*/
int wcmod_rx_DFE_tap4_control(wcmod_st* pc)
{
  return _wcmod_rx_DFE_VGA_control(pc, 4);
}

/*!
\brief Override Rx DFE tap5 value.

\param  pc handle to current warpcore context (a.k.a #wcmod_st)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.

This function is to force receiver DFE tap5 of any lane to any desired value.

To change the DFE tap5 value, set lower 5-bits of #wcmod_st::per_lane_control.

\details
This function is to force receiver DFE tap5 to any desired value. Each lane can
be accessed through AER access.

<B>How to call:</B><br>
<p>Call this function directly or through the function #wcmod_tier1_selector()
with the upper-case string "RX_DFE_TAP5_CONTROL".</p>
*/
int wcmod_rx_DFE_tap5_control(wcmod_st* pc)
{
  return _wcmod_rx_DFE_VGA_control(pc, 5);
}


/*!
\brief Force Rx VGA value.

\param  pc handle to current warpcore context (a.k.a wcmod struct)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.

\details

This function is to force receiver VGA of any lane to any desired value.

To change 5-bit VGA value, set the lower 5-bits of #wcmod_st::per_lane_control.

<B>How to call:</B><br>
<p>Call this function directly or through the function #wcmod_tier1_selector()
with the upper-case string "SET_RX_VGA_OVERRIDE".</p>
*/
int wcmod_rx_VGA_control(wcmod_st* pc)
{
  return _wcmod_rx_DFE_VGA_control(pc, 0);
}

/*!
\brief is used to power down transmitter or receiver per lane basis.

\param  pc handle to current warpcore context (a.k.a #wcmod_st)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.

\details

This function is used to power on/off the TX and RX lanes in one swoop.  To
change the power down state for TX or RX, bit-wise OR one of the following

\li #CHANGE_TX0_POWERED_STATE
\li #CHANGE_TX1_POWERED_STATE
\li #CHANGE_TX2_POWERED_STATE
\li #CHANGE_TX3_POWERED_STATE

<br>

\li #CHANGE_RX0_POWERED_STATE
\li #CHANGE_RX1_POWERED_STATE
\li #CHANGE_RX2_POWERED_STATE
\li #CHANGE_RX3_POWERED_STATE

into #wcmod_st::per_lane_control.  These values serve as masks for whether the
power on/off for a particular lane should be changed.

To turn off the power for the selected TX/RX lane, bit-wise OR one of these

\li #POWER_DOWN_TX0
\li #POWER_DOWN_TX1
\li #POWER_DOWN_TX2
\li #POWER_DOWN_TX3

<br>

\li #POWER_DOWN_RX0
\li #POWER_DOWN_RX1
\li #POWER_DOWN_RX2
\li #POWER_DOWN_RX3

into #wcmod_st::per_lane_control.

As an example, to turn off the TX power on lane 0, turn on the TX power on lane1,
leave the RX power states along with the TX power on lanes 2 and 3 unaffected, set

#wcmod_st::per_lane_control = #CHANGE_TX0_POWERED_STATE |
                              #POWER_DOWN_TX0 |
                              #CHANGE_TX1_POWERED_STATE;

<B>How to call:</B><br>
<p>Call this function directly or through the function #wcmod_tier1_selector()
with the upper-case string "POWER_CONTROL".</p>
*/
int wcmod_power_control(wcmod_st* pc)
{
  int cntl = pc->per_lane_control;
  int power_down_val = 0;
  int mask = 0;
  int tmp_lane;
  uint16 ln_ctrl;

  tmp_lane = pc->this_lane;

  /* if the core is qsgmii core */ 
  if (((pc->model_type) == WCMOD_QS_A0) && !(pc->port_type == WCMOD_COMBO)) {
      int power_down = 0;
      if (cntl & CHANGE_TX0_POWERED_STATE){
        if (cntl & POWER_DOWN_TX0)
        power_down = 1;
      }
      if (cntl & CHANGE_TX1_POWERED_STATE){
        if (cntl & POWER_DOWN_TX1)
        power_down = 1;
      }
      if (cntl & CHANGE_TX2_POWERED_STATE){
        if (cntl & POWER_DOWN_TX2)
        power_down = 1;
      }
      if (cntl & CHANGE_TX3_POWERED_STATE){
        if (cntl & POWER_DOWN_TX3)
        power_down = 1;
      }
      MODIFY_WC40_COMBO_IEEE0_MIICNTLr(pc->unit, pc,
                                   power_down << IEEE0BLK_MIICNTL_PWRDWN_SW_SHIFT,
                                   IEEE0BLK_MIICNTL_PWRDWN_SW_MASK);
  } else {
  if (cntl & CHANGE_TX0_POWERED_STATE){
    if (cntl & POWER_DOWN_TX0)
      power_down_val |= 0x1 << (XGXSBLK1_LANECTRL3_PWRDN_TX_SHIFT);
    mask |= 0x1 << (XGXSBLK1_LANECTRL3_PWRDN_TX_SHIFT);
  }
  if (cntl & CHANGE_TX1_POWERED_STATE){
    if (cntl & POWER_DOWN_TX1)
      power_down_val |= 0x1 << (XGXSBLK1_LANECTRL3_PWRDN_TX_SHIFT + 1);
    mask |= 0x1 << (XGXSBLK1_LANECTRL3_PWRDN_TX_SHIFT + 1);
  }
  if (cntl & CHANGE_TX2_POWERED_STATE){
    if (cntl & POWER_DOWN_TX2)
      power_down_val |= 0x1 << (XGXSBLK1_LANECTRL3_PWRDN_TX_SHIFT + 2);
    mask |= 0x1 << (XGXSBLK1_LANECTRL3_PWRDN_TX_SHIFT + 2);
  }
  if (cntl & CHANGE_TX3_POWERED_STATE){
    if (cntl & POWER_DOWN_TX3)
      power_down_val |= 0x1 << (XGXSBLK1_LANECTRL3_PWRDN_TX_SHIFT + 3);
    mask |= 0x1 << (XGXSBLK1_LANECTRL3_PWRDN_TX_SHIFT + 3);
  }

  if (cntl & CHANGE_RX0_POWERED_STATE){
    if (cntl & POWER_DOWN_RX0)
      power_down_val |= 0x1 << (XGXSBLK1_LANECTRL3_PWRDN_RX_SHIFT);
    mask |= 0x1 << (XGXSBLK1_LANECTRL3_PWRDN_RX_SHIFT);
  }
  if (cntl & CHANGE_RX1_POWERED_STATE){
    if (cntl & POWER_DOWN_RX1)
      power_down_val |= 0x1 << (XGXSBLK1_LANECTRL3_PWRDN_RX_SHIFT + 1);
    mask |= 0x1 << (XGXSBLK1_LANECTRL3_PWRDN_RX_SHIFT + 1);
  }
  if (cntl & CHANGE_RX2_POWERED_STATE){
    if (cntl & POWER_DOWN_RX2)
      power_down_val |= 0x1 << (XGXSBLK1_LANECTRL3_PWRDN_RX_SHIFT + 2);
    mask |= 0x1 << (XGXSBLK1_LANECTRL3_PWRDN_RX_SHIFT + 2);
  }
  if (cntl & CHANGE_RX3_POWERED_STATE){
    if (cntl & POWER_DOWN_RX3)
      power_down_val |= 0x1 << (XGXSBLK1_LANECTRL3_PWRDN_RX_SHIFT + 3);
    mask |= 0x1 << (XGXSBLK1_LANECTRL3_PWRDN_RX_SHIFT + 3);
  }

  /*add the tweak for the QSGMII 8 lane core */
  if ( tmp_lane >= 4) {
    pc->this_lane = 0x4;
  } else {
      pc->lane_num_ignore = 1;
      
      SOC_IF_ERROR_RETURN
          (READ_WC40_XGXSBLK1_LANECTRL3r(pc->unit, pc, &ln_ctrl));
      
      ln_ctrl &= ~mask;
      ln_ctrl |= power_down_val;

      if (ln_ctrl & 0xff) {
          /* any lane disabled, need to force PWRDWN */
          power_down_val |= 0x1 << (XGXSBLK1_LANECTRL3_PWRDWN_FORCE_SHIFT);
      } 
      mask |= XGXSBLK1_LANECTRL3_PWRDWN_FORCE_MASK;
  }  
  MODIFY_WC40_XGXSBLK1_LANECTRL3r(pc->unit, pc, power_down_val, mask);

  pc->lane_num_ignore = 0;
  pc->this_lane = tmp_lane;
  }

  return SOC_E_NONE;
}

/*!
\brief sets the lanes in Full Duplex/ half duplex mode.

\param  pc handle to current warpcore context (a.k.a wcmod struct)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.

\details
This is bit encoded function. Bit 3 to 0 is for lane 3 to 0. High sets full
duplex. Low is for half duplex.

<B>Currently, this function is not implemented</B>

<B>How to call:</B><br>
<p>Call this function directly or through the function #wcmod_tier1_selector()
with the upper-case string "DUPLEX_CONTROL".</p>
*/
int wcmod_duplex_control(wcmod_st* pc)
{
  return SOC_E_NONE;
}

/*!
\brief get critical link statistics.

\param  pc handle to current warpcore context (a.k.a wcmod struct)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.

\details
This function updates the warpcore context struct and also displays the
following parameters.

\li 1. speed
\li 2. interface type
\li 3. half-duplex/duplex info.
\li 4. more info...

<B>this function moved to wcmod_diagnostics.</B>

<B>How to call:</B><br>
<p>Don't Call this function . Use wcmod_diag instead.</p>
*/
int wcmod_port_stat_display(wcmod_st* pc)
{
   wcmod_spd_intfc_set spd_intf = pc->spd_intf;
  uint16 rdValue;

  READ_WC40_XGXSBLK0_XGXSSTATUSr(pc->unit, pc, &rdValue);
  if (pc->verbosity > 0) printf("%-22s: xgxsStatus (0x8001) bit11: txpll_lock 0x%x\n",FUNCTION_NAME(), rdValue);
  if (rdValue & XGXSBLK0_XGXSSTATUS_TXPLL_LOCK_MASK) {
    if (pc->verbosity > 0) printf("%-22s: txpll_lock up\n", FUNCTION_NAME());
  } else {
    if (pc->verbosity > 0) printf("%-22s: txpll_lock down\n", FUNCTION_NAME());
  }

  if ((spd_intf == WCMOD_SPD_10000_HI) || (spd_intf == WCMOD_SPD_10000) ||
        (spd_intf == WCMOD_SPD_12000_HI) || (spd_intf == WCMOD_SPD_13000) ||
        (spd_intf == WCMOD_SPD_16000) || (spd_intf == WCMOD_SPD_20000) ||
        (spd_intf == WCMOD_SPD_21000) || (spd_intf == WCMOD_SPD_25455)) {
    READ_WC40_XGXSBLK5_XGXSSTATUS4r(pc->unit, pc, &rdValue);
    if (pc->verbosity > 0) printf("%-22s: xgxsStatus(813C.13): link, bit5-0: actual_speed_ln0 0x%x\n",
                                                            FUNCTION_NAME(),rdValue);
    if (rdValue & XGXSBLK5_XGXSSTATUS4_LINK10G_MASK) {
      if (pc->verbosity > 0) printf("%-22s: 10G link up\n", FUNCTION_NAME());
    } else {
      if (pc->verbosity > 0) printf("%-22s: 10G link down\n", FUNCTION_NAME());
    }

  } else if (spd_intf == WCMOD_SPD_1000_FIBER) {
    READ_WC40_GP2_REG_GP2_1r(pc->unit, pc, &rdValue);
    if (pc->verbosity > 0) printf("%-22s: GP2_1:81d1.15-12:link_KR, bit11-8: link_status:0x%x\n",
                                                        FUNCTION_NAME(),rdValue);
    if (rdValue & GP2_REG_GP2_1_LINK_STATUS_MASK) {
      if (pc->verbosity > 0) printf("%-22s Per port GP2_1 link up\n",FUNCTION_NAME());
    } else {
      if (pc->verbosity > 0) printf("%-22s Per port GP2_1 link down\n",FUNCTION_NAME());
    }
  }
  else {
  /* no op */
  }
  return SOC_E_NONE;
}

/*!
\brief Control scrambler parameters of each lane.

\param  pc handle to current warpcore context (a.k.a #wcmod_st)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.

This function may be used to enable/disable the scrambler for both 8B/10B
and 64B/66B encoding.  To set the encoding in 8B/10B mode, ensure that the
#wcmod_st::port_type is set to either #WCMOD_COMBO or #WCMOD_INDEPENDENT and
then set #wcmod_st::per_lane_control to either

\li #SCRAMBLER_ON_8B_10B
\li #SCRAMBLER_OFF_8B_10B


TX 64B/66B scrambler settings is set bit-wise ORing the following values into
#wcmod_st::per_lane_control

\li #BYPASS_SCRAMBLER_64B_66B
\li #SCRAMBLE_ENTIRE_64B_66B
\li #SCRAMBLE_PAYLOAD_ONLY_64B_66B

and various RX 64B/66B scrambler settings is set by using the following fields

\li #BYPASS_DESCRAMBLER_64B_66B
\li #DESCRAMBLE_ENTIRE_64B_66B
\li #DESCRAMBLE_PAYLOAD_ONLY_64B_66B

<B>How to call:</B><br>
<p>Call this function directly or through the function #wcmod_tier1_selector()
with the upper-case string "SCRAMBLER_CONTROL".</p>
*/
int wcmod_scrambler_control (wcmod_st* pc)
{
  int cntl = pc->per_lane_control;

  /* 8b/10B Scrambler control in combo mode */

  if ((cntl & SCRAMBLER_ON_8B_10B) && (pc->port_type == WCMOD_COMBO)) {
    pc->lane_num_ignore = 0;

    MODIFY_WC40_XGXSBLK0_XGMIIRCONTROLr(pc->unit, pc,
                XGXSBLK0_XGMIIRCONTROL_SCR_EN_4LANE_MASK,
                XGXSBLK0_XGMIIRCONTROL_SCR_EN_4LANE_MASK);

  } else if ((cntl & SCRAMBLER_OFF_8B_10B) && (pc->port_type == WCMOD_COMBO)) {
      pc->lane_num_ignore = 1;
      MODIFY_WC40_XGXSBLK0_XGMIIRCONTROLr(pc->unit, pc,
                0,
                XGXSBLK0_XGMIIRCONTROL_SCR_EN_4LANE_MASK);
  }
  pc->lane_num_ignore = 0;
  /* 8b/10B Scrambler control in independent mode */
  if ((cntl & SCRAMBLER_ON_8B_10B) && (pc->port_type == WCMOD_INDEPENDENT)) {
    MODIFY_WC40_DIGITAL4_MISC3r(pc->unit, pc,
                DIGITAL4_MISC3_SCR_EN_PER_LANE_MASK,
                DIGITAL4_MISC3_SCR_EN_PER_LANE_MASK);
  } else if ((cntl & SCRAMBLER_OFF_8B_10B) && (pc->port_type == WCMOD_INDEPENDENT)) {
    MODIFY_WC40_DIGITAL4_MISC3r(pc->unit, pc, 0,
                DIGITAL4_MISC3_SCR_EN_PER_LANE_MASK);
  }

  /* TX 64/66 scrambler control */
  if (cntl & BYPASS_SCRAMBLER_64B_66B) {
    MODIFY_WC40_TX66_CONTROLr(pc->unit, pc,
                  0x0 << TX66_CONTROL_SCRAMBLERCONTROL_SHIFT,
                  TX66_CONTROL_SCRAMBLERCONTROL_MASK);
  } else if (cntl & SCRAMBLE_ENTIRE_64B_66B) {
    MODIFY_WC40_TX66_CONTROLr(pc->unit, pc,
                  0x1 << TX66_CONTROL_SCRAMBLERCONTROL_SHIFT,
                  TX66_CONTROL_SCRAMBLERCONTROL_MASK);
  } else if (cntl & SCRAMBLE_PAYLOAD_ONLY_64B_66B) {
    MODIFY_WC40_TX66_CONTROLr(pc->unit, pc,
                  0x2 << TX66_CONTROL_SCRAMBLERCONTROL_SHIFT,
                  TX66_CONTROL_SCRAMBLERCONTROL_MASK);
  }

  /* RX 64/66 descrambler control */

  if (cntl & BYPASS_DESCRAMBLER_64B_66B) {
    MODIFY_WC40_RX66_CONTROLr(pc->unit, pc,
                  0x0 << RX66_CONTROL_DESCRAMBLERCONTROL_SHIFT,
                  RX66_CONTROL_DESCRAMBLERCONTROL_MASK);
  } else if (cntl & DESCRAMBLE_ENTIRE_64B_66B) {
     MODIFY_WC40_RX66_CONTROLr(pc->unit, pc,
                  0x1 << RX66_CONTROL_DESCRAMBLERCONTROL_SHIFT,
                  RX66_CONTROL_DESCRAMBLERCONTROL_MASK);
  } else if (cntl & DESCRAMBLE_PAYLOAD_ONLY_64B_66B) {
    MODIFY_WC40_RX66_CONTROLr(pc->unit, pc,
                  0x2 << RX66_CONTROL_DESCRAMBLERCONTROL_SHIFT,
                  RX66_CONTROL_DESCRAMBLERCONTROL_MASK);
  }
  return SOC_E_NONE;
}

/*!
\brief Set Remote loopback set in PCS bypass mode including SFP+ remote timing loop.

\param  pc handle to current warpcore context (a.k.a wcmod struct)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.

\details
Set PCS bypass mode remote loopback. This is a bit encoded function. Bit 3 to 0
is for lane 3 to lane 0. If PCS bypass remote loopback is set, both Local and
Remote cores need to be in the same clock domain or Tx phase interpolator need
to be set by corresponding Rx lane. There is only one Tx phase interpolator in
the design.

\li XGXSBLK1_LANECTRL2 (0x8017[7:4]) bit 7 to 4 for lanes 3 to 0 respectively.
\li XGXSBLK1_LANECTRL1 (0x8016) [1:0/7:6/9:8/15:14]tx_lane_0/3, rx_lane0/3.
<B>Currently, this function is not implemented</B>

<B>How to call:</B><br>
<p>Call this function directly or through the function #wcmod_tier1_selector()
with the upper-case string "RX_LOOPBACK_PCS_BYPASS".</p>

*/

int wcmod_rx_loopback_pcs_bypass(wcmod_st* pc)
{
  int cntl = pc->per_lane_control;
  int mask = 0;
  int val = 0;
  int  lane_0 = 0, lane_1 = 0, lane_2 = 0, lane_3 = 0;
  wcmod_spd_intfc_set spd_intf = pc->spd_intf;

  lane_0 = cntl & 0x01;
  lane_1 = cntl & 0x02;
  lane_2 = cntl & 0x04;
  lane_3 = cntl & 0x08;

  pc->lane_num_ignore = 1;

  /* enable the tx_interpreter */
  if (cntl) {
      MODIFY_WC40_XGXSBLK8_TX_PI_CONTROL1r(pc->unit, pc, XGXSBLK8_TX_PI_CONTROL1_TX_PI_EN_MASK, 
                                           XGXSBLK8_TX_PI_CONTROL1_TX_PI_EN_MASK);
  } else {
      MODIFY_WC40_XGXSBLK8_TX_PI_CONTROL1r(pc->unit, pc, 0, 
                                           XGXSBLK8_TX_PI_CONTROL1_TX_PI_EN_MASK);
  }
   
  /* need to turn of clock compensation if enabled */
  if (cntl) {
    MODIFY_WC40_RX66_CONTROLr(pc->unit, pc, 0, RX66_CONTROL_CC_EN_MASK);
  } else if ((spd_intf == WCMOD_SPD_10000_XFI) || (spd_intf == WCMOD_SPD_10000_SFI) ||
          (spd_intf == WCMOD_SPD_11P5) || (spd_intf == WCMOD_SPD_12P5) ||
          (spd_intf == WCMOD_SPD_1000_FIBER) || (spd_intf == WCMOD_SPD_1000_SGMII) ||
          (spd_intf == WCMOD_SPD_10_SGMII) || (spd_intf == WCMOD_SPD_100_SGMII)) {
    MODIFY_WC40_RX66_CONTROLr(pc->unit, pc, RX66_CONTROL_CC_EN_MASK, RX66_CONTROL_CC_EN_MASK);
  }
  if (spd_intf != WCMOD_SPD_100G_CR10) {
      if (lane_0) {
          MODIFY_WC40_XGXSBLK8_TX_PI_CONTROL2r(pc->unit, pc, 0, XGXSBLK8_TX_PI_CONTROL2_TX_PI_RX_LANE_SEL_MASK);
      }
      if (lane_1) {
          MODIFY_WC40_XGXSBLK8_TX_PI_CONTROL2r(pc->unit, pc, 1, XGXSBLK8_TX_PI_CONTROL2_TX_PI_RX_LANE_SEL_MASK);
      }
      if (lane_2) {
          MODIFY_WC40_XGXSBLK8_TX_PI_CONTROL2r(pc->unit, pc, 2, XGXSBLK8_TX_PI_CONTROL2_TX_PI_RX_LANE_SEL_MASK);
      }
      if (lane_3) {
          MODIFY_WC40_XGXSBLK8_TX_PI_CONTROL2r(pc->unit, pc, 3, XGXSBLK8_TX_PI_CONTROL2_TX_PI_RX_LANE_SEL_MASK);
      }
  }

   val = (cntl & 0x0f) << XGXSBLK1_LANECTRL2_RLOOP1G_SHIFT;
   mask = val ? val : XGXSBLK1_LANECTRL2_RLOOP1G_MASK; 
   MODIFY_WC40_XGXSBLK1_LANECTRL2r(pc->unit, pc, val, mask);

  pc->lane_num_ignore = 0;

  return SOC_E_NONE;
}

/*!
\brief set the test packet size and IPG for each lane.

\param  pc handle to current warpcore context (a.k.a wcmod struct)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.

\details

<B>Currently, this function is not implemented</B>

<B>How to call:</B><br>
<p>Call this function directly or through the function #wcmod_tier1_selector()
with the upper-case string "PACKET_SIZE_IPG_SET ".</p>

*/
int wcmod_packet_size_IPG_set (wcmod_st* pc)
{
  return SOC_E_NONE;
}

/*!
\brief check per lane receive packet count.

\param  pc handle to current warpcore context (a.k.a wcmod struct)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.

<B>Currently, this function is not implemented</B>

<B>How to call:</B><br>
<p>Call this function directly or through the function
#wcmod_tier1_selector() with the upper-case string
"TX_RX_PACKETS_CHECK".</p>
*/
int wcmod_tx_rx_packets_check(wcmod_st* pc)
{
  return SOC_E_NONE;
}


/*!
\brief Wait for an event to happen.

\param  pc handle to current warpcore context (a.k.a wcmod struct)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.

\details
Currently it can only wait for a specific register bit to toggle. This bit is
specified in the 'yet to be defined' context field.

<B>Currently, this function is deprecated. Will not be implemented</B>

*/
int wcmod_wait_bit(wcmod_st* pc)
{
  return SOC_E_NONE;
}

/*!
\brief Wait for specific time in micro-seconds.

\param  pc handle to current warpcore context (a.k.a wcmod struct)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.

The time unit is independent on the platform.

<B>Currently, this function is deprecated. Will not be implemented</B>

<B>How to call:</B><br>
<p>Call this function directly or through the function #wcmod_tier1_selector()
with the upper-case string "WAIT_TIME".</p>

*/

int wcmod_wait_time(wcmod_st* pc)
{
  return SOC_E_NONE;
}

/*!
\brief will set WarpCore in EEE pass thru mode.

\param  pc handle to current warpcore context (a.k.a wcmod struct)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.

\details
Place holder at this time.

<B>Currently, this function is not implemented</B>

<B>How to call:</B><br>
<p>Call this function directly or through the function #wcmod_tier1_selector()
with the upper-case string "EEE_PASS_THRU_SET".</p>
*/
int wcmod_EEE_pass_thru_set(wcmod_st* pc)
{
  int cntl = pc->per_lane_control;

  if (cntl) {
    /*enable LPI pass thru */
    MODIFY_WC40_XGXSBLK7_EEECONTROLr(pc->unit, pc, 
                                     XGXSBLK7_EEECONTROL_LPI_EN_RX_MASK |
                                     XGXSBLK7_EEECONTROL_LPI_EN_TX_MASK,
                                     XGXSBLK7_EEECONTROL_LPI_EN_RX_MASK |
                                     XGXSBLK7_EEECONTROL_LPI_EN_TX_MASK);
    MODIFY_WC40_DIGITAL4_MISC5r(pc->unit, pc, 
                                DIGITAL4_MISC5_LPI_EN_RX_MASK |
                                DIGITAL4_MISC5_LPI_EN_TX_MASK,
                                DIGITAL4_MISC5_LPI_EN_RX_MASK |
                                DIGITAL4_MISC5_LPI_EN_TX_MASK);
  } else {
    /*disable LPI pass thru */
    MODIFY_WC40_XGXSBLK7_EEECONTROLr(pc->unit, pc, 0,
                                     XGXSBLK7_EEECONTROL_LPI_EN_RX_MASK |
                                     XGXSBLK7_EEECONTROL_LPI_EN_TX_MASK);
    MODIFY_WC40_DIGITAL4_MISC5r(pc->unit, pc, 0,
                                DIGITAL4_MISC5_LPI_EN_RX_MASK |
                                DIGITAL4_MISC5_LPI_EN_TX_MASK);
  }
  return SOC_E_NONE;
}

/*!
\brief EEE parameters set (full featured)

\param  pc handle to current warpcore context (a.k.a wcmod struct)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.


\details
place holder at this time.

<B>Currently, this function is not implemented</B>

<B>How to call:</B><br>
<p>Call this function directly or through the function #wcmod_tier1_selector()
with the upper-case string "EEE_SET".</p>
*/
int wcmod_EEE_set(wcmod_st* pc)
{
  return SOC_E_NONE;
}

/*!
\brief Carrier extension set

\param  pc handle to current warpcore context (a.k.a wcmod struct)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.

\details
Only for Xenia. 

<B>Currently, this function is not implemented</B>

<B>How to call:</B><br>
<p>Call this function directly or through the function #wcmod_tier1_selector()
with the upper-case string "CARRIER_EXTENSION_SET".</p>
*/
int wcmod_carrier_extension_set(wcmod_st *ws)
{
  if (ws->per_lane_control == WCMOD_DISABLE) {
    MODIFY_WC40_SERDESDIGITAL_CONTROL1000X2r(ws->unit, ws,
        SERDESDIGITAL_CONTROL1000X2_DISABLE_CARRIER_EXTEND_MASK,
        SERDESDIGITAL_CONTROL1000X2_DISABLE_CARRIER_EXTEND_MASK);
  } else {
    MODIFY_WC40_SERDESDIGITAL_CONTROL1000X2r(ws->unit, ws,
        0,
        SERDESDIGITAL_CONTROL1000X2_DISABLE_CARRIER_EXTEND_MASK);
  }
  return SOC_E_NONE;
}

/*!
\brief Set reference clock

\param  pc handle to current warpcore context (a.k.a wcmod struct)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.


\details
The reference clock is set to inform the micro. The value written into the
register is interpreted by the micro as thus.

    refclk = regVal * 2 + 100;

    Since the refclk field is 7 bits, the range is 100 - 228

<B>How to call:</B><br>
<p>Call this function directly or through the function #wcmod_tier1_selector()
with the upper-case string "REFCLK_SET".</p>
*/
int wcmod_refclk_set(wcmod_st* pc)
{
  int refclkEncode, cntl;

  cntl  = pc->per_lane_control;
  refclkEncode = (int) ((cntl-100) / 2);

  SOC_IF_ERROR_RETURN(MODIFY_WC40_UC_INFO_B1_REGBr(pc->unit, pc, refclkEncode,
                      UC_INFO_B1_REGB_REFCLK_FREQ_MASK));

  return SOC_E_NONE;
  
}

/*!
\brief Set asymmetric mode.

\param  pc handle to current warpcore context (a.k.a wcmod struct)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.


\details

Set asymmetric mode for Xenia. This will allow 1G and 2.5G simultanesously

<B>How to call:</B><br>
<p>Call this function directly or through the function #wcmod_tier1_selector()
with the upper-case string "ASYMMETRIC_MODE_SET".</p>
*/

int wcmod_asymmetric_mode_set(wcmod_st* pc)
{
  return SOC_E_NONE;
}

int _wcmod_asymmetric_mode(wcmod_st* pc)
{
  int _1g_or_2p5g;
  uint16 data, data2;

  SOC_IF_ERROR_RETURN(MODIFY_WC40_XGXSBLK8_TXLNSWAP1r(pc->unit, pc,
                XGXSBLK8_TXLNSWAP1_ASYMMETRIC_MODE_EN_MASK,
                XGXSBLK8_TXLNSWAP1_ASYMMETRIC_MODE_EN_MASK));

  if        (pc->spd_intf == WCMOD_SPD_1000_FIBER) {
    _1g_or_2p5g = 1;
  } else if (pc->spd_intf == WCMOD_SPD_2500) {
    _1g_or_2p5g = 0;
  } else {
    printf("Error: %s Cannot call _asymmetric mode with this speed", FUNCTION_NAME());
    return SOC_E_ERROR;
  }
  data  = _1g_or_2p5g ? TX3_ANATXACONTROL1_ASYM_MODE_MASK : 0;
  data2 = _1g_or_2p5g ? RX0_ANARXACONTROL1_SIG_PWRDN_MASK : 0;
  switch (pc->this_lane) {
    case 0:
      SOC_IF_ERROR_RETURN(MODIFY_WC40_TX0_ANATXACONTROL1r(pc->unit,pc,data,
                      TX0_ANATXACONTROL1_ASYM_MODE_MASK));
      SOC_IF_ERROR_RETURN(MODIFY_WC40_RX0_ANARXACONTROL1r(pc->unit,pc,data2,
                      RX0_ANARXACONTROL1_SIG_PWRDN_MASK));
      break;
    case 1:
      SOC_IF_ERROR_RETURN(MODIFY_WC40_TX1_ANATXACONTROL1r(pc->unit,pc,data,
                      TX0_ANATXACONTROL1_ASYM_MODE_MASK));
      SOC_IF_ERROR_RETURN(MODIFY_WC40_RX1_ANARXACONTROL1r(pc->unit,pc,data2,
                      RX0_ANARXACONTROL1_SIG_PWRDN_MASK));
      break;
    case 2:
      SOC_IF_ERROR_RETURN(MODIFY_WC40_TX2_ANATXACONTROL1r(pc->unit,pc,data,
                      TX0_ANATXACONTROL1_ASYM_MODE_MASK));
      SOC_IF_ERROR_RETURN(MODIFY_WC40_RX2_ANARXACONTROL1r(pc->unit,pc,data2,
                      RX0_ANARXACONTROL1_SIG_PWRDN_MASK));
      break;
    case 3:
      SOC_IF_ERROR_RETURN(MODIFY_WC40_TX3_ANATXACONTROL1r(pc->unit,pc,data,
                      TX0_ANATXACONTROL1_ASYM_MODE_MASK));
      SOC_IF_ERROR_RETURN(MODIFY_WC40_RX3_ANARXACONTROL1r(pc->unit,pc,data2,
                      RX0_ANARXACONTROL1_SIG_PWRDN_MASK));
      break;
    default:
      break;
  }
  return SOC_E_NONE;
}
