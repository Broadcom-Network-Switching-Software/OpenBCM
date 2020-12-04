/*----------------------------------------------------------------------
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Broadcom Corporation
 * Proprietary and Confidential information
 * All rights reserved
 * This source file is the property of Broadcom Corporation, and
 * may not be copied or distributed in any isomorphic form without the
 * prior written consent of Broadcom Corporation.
 *---------------------------------------------------------------------
 * File       : tscmod_cfg_seq.c
 * Description: c functions implementing Tier1s for TSCMod Serdes Driver
 *---------------------------------------------------------------------*/
#include <sal/types.h>
#include <sal/core/spl.h>
#include <sal/core/thread.h>
#include <shared/bsl.h>

#include "tscmod_main.h"
#include "tscmod.h"
#include "tscmod_defines.h"
#ifdef _SDK_TSCMOD_
#include "tscmod_phyreg.h"
#endif

#include "phydefs.h"

#if defined(INCLUDE_XGXS_TSCMOD)
int tscmod_pll_lock_wait(tscmod_st* pc);
extern int tscmod_firmware_set (tscmod_st *pc);
extern int _tscmod_asymmetric_mode(tscmod_st* pc);
extern tscmod_int_st _tscmod_int_st;
extern int tscmod_reg_uc_sync_error_debug(uint32 unit, tscmod_st *pc) ;
extern int tscmod_reg_set_porxy_blk_addr(uint32 unit, tscmod_st *pc) ;
extern int tscmod_is_warm_boot(uint32 unit, tscmod_st *pc) ;

int tscmod_credit_control(tscmod_st* pc);
int tscmod_tx_lane_control(tscmod_st* pc);
int tscmod_rx_lane_control(tscmod_st* pc);
int tscmod_power_control(tscmod_st* pc);
int tscmod_credit_set(tscmod_st* pc);
int tscmod_prbs_credit_set(tscmod_st* pc) ;
int tscmod_lf_rf_control(tscmod_st* pc) ;
int tscmod_uc_sync_cmd(tscmod_st* pc) ;
int tscmod_uc_sync_mask_wr_cmd(tscmod_st* pc) ;
int tscmod_mask_wr_cmd(tscmod_st* pc, uint32 addr, uint16 data, uint16 mask) ;
static int tscmod_uc_rx_lane_control(tscmod_st* pc, int lane, int value) ;

/* int tscmod_print_tscmod_st(tscmod_st*)           :: PRINT_TSCMOD_ST        */
/* int tscmod_100fx_control (tscmod_st*)            :: 100FX_CONTROL          */
/* int tscmod_master_slave_control(tscmod_st* pc)   :: MASTER_SLAVE_CONTROL   */
/* int tscmod_tx_lane_disable (tscmod_st*)          :: TX_LANE_DISABLE        */
/* int tscmod_pcs_bypass_ctl(tscmod_st*)            :: PCS_BYPASS_CTL         */
/* int tscmod_autoneg_control(tscmod_st*)           :: AUTONEG_CONTROL        */
/* int tscmod_pll_sequencer_control(tscmod_st*)     :: PLL_SEQUENCER_CONTROL  */
/* int tscmod_pll_lock_wait(pc);                    :: PLL_LOCK_WAIT          */
/* int tscmod_set_aer(tscmod_st*)                   :: SET_AER                */
/* int tscmod_set_port_mode(tscmod_st*)             :: SET_PORT_MODE          */
/* int tscmod_set_an_port_mode(tscmod_st*)          :: SET_AN_PORT_MODE       */
/* int tscmod_parallel_detect_control(tscmod_st*)   :: PARALLEL_DETECT_CONTROL*/
/* int tscmod_tx_loopback_control(tscmod_st*)       :: TX_LOOPBACK_CONTROL    */
/* int tscmod_aer_lane_select(tscmod_st*)           :: AER_LANE_SELECT        */
/* int tscmod_set_spd_intf(tscmod_st*)              :: SET_SPD_INTF           */
/* int tscmod_rx_seq_control(tscmod_st*)            :: RX_SEQ_CONTROL         */
/* int tscmod_soft_reset(tscmod_st*)                :: SOFT_RESET             */
/* int tscmod_revid_read(tscmod_st*)                :: REVID_READ             */
/* int tscmod_clause72_control(tscmod_st*)          :: CLAUSE72_CONTROL       */
/* int tscmod_prbs_check(tscmod_st*)                :: PRBS_CHECK             */
/* int tscmod_prbs_control(tscmod_st*)              :: PRBS_CONTROL           */
/* int tscmod_prbs_mode(tscmod_st*)                 :: PRBS_MODE              */
/* int tscmod_prbs_seed_a1a0(tscmod_st*)            :: PRBS_SEED_A1A0         */
/* int tscmod_prbs_seed_a3a2(tscmod_st*)            :: PRBS_SEED_A3A2         */
/* int tscmod_prbs_seed_load_control(tscmod_st*)    :: PRBS_SEED_LOAD_CONTROL */
/* int tscmod_prbs_decouple_control(tscmod_st*)     :: PRBS_DECOUPLE_CONTROL  */
/* int tscmod_cjpat_crpat_control(tscmod_st*)       :: CJPAT_CRPAT_CONTROL    */
/* int tscmod_cjpat_crpat_check  (tscmod_st*)       :: CJPAT_CRPAT_CHECK      */
/* int tscmod_tx_bert_control(tscmod_st*)           :: TX_BERT_CONTROL        */
/* int tscmod_lane_swap(tscmod_st *pc)              :: LANE_SWAP              */
/* int tscmod_tx_rx_polarity(tscmod_st *pc)         :: TX_RX_POLARITY         */
/* int tscmod_prog_data(tscmod_st *pc)              :: PROG_DATA              */
/* int tscmod_core_reset(tscmod_st*)                :: CORE_RESET             */
/* int tscmod_firmware_set (tscmod_st*)             :: FIRMWARE_SET           */
/* int tscmod_BRCM64B66B_control(tscmod_st*)        :: BRCM64B66B_CONTROL     */
/* int tscmod_FEC_control(tscmod_st*)               :: FEC_CONTROL            */
/* int tscmod_tx_amp_control(tscmod_st*)            :: TX_AMP_CONTROL         */
/* int tscmod_tx_pi_control(tscmod_st*)             :: TX_PI_CONTROL          */
/* int tscmod_rx_p1_slicer_control(tscmod_st*)      :: RX_P1_SLICER_CONTROL   */
/* int tscmod_rx_m1_slicer_control(tscmod_st*)      :: RX_M1_SLICER_CONTROL   */
/* int tscmod_rx_d_slicer_control(tscmod_st*)       :: RX_Z_SLICER_CONTROL    */
/* int tscmod_tx_main_tap_control(tscmod_st* pc)    :: TX_MAIN_TAP_CONTROL    */
/* int tscmod_tx_pre_tap_control(tscmod_st* pc)     :: TX_PRE_TAP_CONTROL     */
/* int tscmod_tx_post_tap_control(tscmod_st* pc)    :: TX_POST_TAP_CONTROL    */
/* int tscmod_tx_tap_control(tscmod_st*)            :: TX_TAP_CONTROL         */
/* int tscmod_rx_PF_control(tscmod_st*)             :: RX_PF_CONTROL          */
/* int tscmod_rx_DFE_tap1_control(tscmod_st*)       :: RX_DFE_TAP1_CONTROL    */
/* int tscmod_rx_DFE_tap2_control(tscmod_st*)       :: RX_DFE_TAP2_CONTROL    */
/* int tscmod_rx_DFE_tap3_control(tscmod_st*)       :: RX_DFE_TAP3_CONTROL    */
/* int tscmod_rx_DFE_tap4_control(tscmod_st*)       :: RX_DFE_TAP4_CONTROL    */
/* int tscmod_rx_DFE_tap5_control(tscmod_st*)       :: RX_DFE_TAP5_CONTROL    */
/* int tscmod_rx_VGA_control(tscmod_st*)            :: RX_VGA_CONTROL         */
/* int tscmod_power_control(tscmod_st*)             :: POWER_CONTROL          */
/* int tscmod_duplex_control(tscmod_st*)            :: DUPLEX_CONTROL         */
/* int tscmod_port_stat_display(tscmod_st*)         :: PORT_STAT_DISPLAY      */
/* int tscmod_autoneg_set(tscmod_st*)               :: AUTONEG_SET            */
/* int tscmod_scrambler_control(tscmod_st*)         :: SCRAMBLER_CONTROL      */
/* int tscmod_rx_loopback_control(tscmod_st*)       :: RX_LOOPBACK_CONTROL    */
/* int tscmod_rx_loopback_pcs_bypass(tscmod_st*)    :: RX_LOOPBACK_PCS_BYPASS */
/* int tscmod_packet_size_IPG_set(tscmod_st*)       :: PACKET_SIZE_IPG_SET    */
/* int tscmod_tx_rx_packets_check(tscmod_st*)       :: TX_RX_PACKETS_CHECK    */
/* int tscmod_refclk_set(tscmod_st*)                :: REFCLK_SET 	      */
/* int tscmod_asymmetric_mode_set(tscmod_st*)       :: ASYMMETRIC_MODE_SET    */
/* int tscmod_EEE_pass_thru_set(tscmod_st*)         :: EEE_PASS_THRU_SET      */
/* int tscmod_EEE_set(tscmod_st*)                   :: EEE_SET                */
/* int tscmod_reg_read(tscmod_st*)                  :: REG_READ               */
/* int tscmod_reg_write(tscmod_st*)                 :: REG_WRITE              */
/* int tscmod_wait_bit(tscmod_st*)                  :: WAIT_BIT               */
/* int tscmod_wait_time(tscmod_st*)                 :: WAIT_TIME              */
/* int tscmod_credit_set(tscmod_st*)                :: CREDIT_SET             */
/* int tscmod_credit_control(tscmod_st*)            :: CREDIT_CONTROL         */
/* int tscmod_encode_set(tscmod_st*)                :: ENCODE_SET             */
/* int tscmod_tx_lane_control(tscmod_st*)           :: TX_LANE_CONTROL        */
/* int tscmod_rx_lane_control(tscmod_st*)           :: RX_LANE_CONTROL        */
/* int tscmod_decode_set(tscmod_st*)                :: DECODE_SET             */
/* int tscmod_rx_fast_lock_control(tscmod_st*)      :: RX_FAST_LOCK_CONTROL   */
/* int tscmod_signal_detect_fec_sel(tscmod_st*)     :: SIGNAL_DETECT_FEC_SEL  */
/* int tscmod_lf_rf_control(tscmod_st*)             :: LF_RF_CONTROL          */
/* int tscmod_higig2_control(tscmod_st*)            :: HIGIG2_CONTROL         */
/* int tscmod_afe_reset_analog(tscmod_st*)          :: AFE_RESET_ANALOG       */
/* int tscmod_afe_rxseq_start_control(tscmod_st*)   :: AFE_RXSEQ_START_CONTROL*/
/* int tscmod_afe_speed_up_dsc_vga(tscmod_st*)      :: AFE_SPEED_UP_DSC_VGA   */
/* int tscmod_wait_pmd_lock(tscmod_st*)             :: WAIT_PMD_LOCK          */
/* int tscmod_uc_sync_cmd(tscmod_st*)               :: UC_SYNC_CMD            */

/*!
\brief This function waits for the TX-PLL PLL_LOCK bit.  The register is one copy
       at Lane0.

\param  pc handle to current TSC context (#tscmod_st)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.
*/

int tscmod_pll_lock_wait(tscmod_st* pc)
{
  int rv;

   rv = tscmod_regbit_set_wait_check(pc,
                 0x9010,
                 ANAPLL_STATUS_PLLSEQPASS_MASK, 1, (TSC40_PLL_WAIT*200));

   if (rv == SOC_E_TIMEOUT) {
          printf("%-22s: Error. p=%0d Timeout PLL lock:\n", FUNCTION_NAME(), pc->port);
          return (SOC_E_TIMEOUT); }

  return SOC_E_NONE;
}

/*!
\brief This function waits for the the RX-PMD to lock.

\param  pc handle to current TSC context (#tscmod_st)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.

\details  This function has a time out limit (TSC40_PLL_WAIT*50) micro-second in driver.
or TSC40_PLL_WAIT/20 pulling count in simualtion, if per_lane_control is set.  Otherwise,
if per_lane_control==0, it reads back PMD_LIVE_STATUS and put it on accData.
*/

int tscmod_wait_pmd_lock(tscmod_st* pc)
{
   int rv;   uint16 data ;

  if(pc->per_lane_control) {
     rv = tscmod_regbit_set_wait_check(pc,
             TSC40_RX_X4_STATUS1_PMA_PMD_LIVE_STATUSr,
             RX_X4_STATUS1_PMA_PMD_LIVE_STATUS_PMD_LOCK_MASK,1,(TSC40_PLL_WAIT*100));

     if (rv == SOC_E_TIMEOUT) {
        printf("%-22s: Error. p=%0d Timeout TXPMD lock:\n", FUNCTION_NAME(), pc->port);
        pc->accData =0 ;
        return (SOC_E_NONE);
     } else {
        pc->accData =1 ;
     }
  } else {
     SOC_IF_ERROR_RETURN
        (READ_WC40_RX_X4_STATUS1_PMA_PMD_LIVE_STATUSr(pc->unit,pc,&data));
     if (data & RX_X4_STATUS1_PMA_PMD_LIVE_STATUS_PMD_LOCK_MASK) {
        pc->accData =1 ;
     } else {
        pc->accData =0 ;
     }
  }
  return (SOC_E_NONE);
}

/*!
\brief Print TSC programmers context.

\param  pc handle to current TSC context (#tscmod_st)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.

\detalis This prints out the fields of the #tscmod_st struct. Currently it does
not print all fields. The user TSC context (void pointer attached) cannot
be printed since this function does not know the fields.
*/

int tscmod_print_tscmod_st(tscmod_st* pc)
{
  /* fix this */
  return SOC_E_ERROR;
}

/*!
\brief Squelch TX lane output.

\param  pc handle to current TSC context (#tscmod_st)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.


\details This function disables transmission on a specific lane. No reset is
required to restart the transmission. Lane control is done through
#tscmod_st::per_lane_control. Set bits 0, 8, 16, or 24 to  <B>0 to disable
transmission on lanes 0, 1, 2, or 3,</B>. Set the bits to <B>1 to enable
transmission.


<B>How to call:</B><br>
<p>Call this function directly or through the function
#tscmod_tier1_selector() with the upper-case string "TX_LANE_DISABLE".</p>
*/

int tscmod_tx_lane_disable (tscmod_st* pc)
{
  /* fix this */
  return SOC_E_ERROR;
}

/*!
\brief controls 100fx registers to enable/disable autodetect of 100fx

\param  pc handle to current TSC context (#tscmod_st)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.


\details Set Bits 0, 8, 16 and/or 24 of #tscmod_st::per_lane_control to <B>0 to
disable</B> 100FX auto-detect, and <B>1 to enable</B>.

This function modifies the following registers

<B>How to call:</B><br>
<p>Call this function directly or through the function
#tscmod_tier1_selector() with the upper-case string "100FX_CONTROL".</p>
*/
int tscmod_100fx_control (tscmod_st* pc)
{
  /* fix this */
  return SOC_E_ERROR;
}

/*!
\brief Controls Master Slave settings for 100/120/150G setups.

\param  pc handle to current TSC context (#tscmod_st)

\returns The value #SOC_E_NONE upon successful completion of the function.

\details  In 100/120/150G mode, three TSCs work in tandem, one as a master and
the other two as slaves. The master can control the slaves via straps. Software
overrides exist too. It is possible to bring up 100G and up completely in
software. The TSCs are always in combo mode.

Use the ::tscmod_st::per_lane_control field of #tscmod_st as follows.
This is in combo mode, set only the first byte.
\li First bit:    0 -> Do nothing.
                  1 -> wakeup slave.

<B>How to call:</B><br>
<p>This function may be called directly or through the function
#tscmod_tier1_selector() using case-sensitive string "SLAVE_WAKEUP_CONTROL".</p>
*/
int tscmod_slave_wakeup_control (tscmod_st* pc)
{
  /* fix this */
  return SOC_E_ERROR;
}

/*!
\brief Wake up the slave in 100G mode. Slaves need kickstart from the straps
connected to the master, or via a software override. It is possible to bring
100G up completely in software.

\param  pc handle to current TSC context (#tscmod_st)

\returns The value #SOC_E_NONE upon successful completion of the function.

\details Set the ::tscmod_st::per_lane_control field of #tscmod_st as follows.
 This is always in combo mode, so you only set the first byte.
\li First nibble: 0 -> disabled. In the other  cases below 0 is always active.
                  1 -> TSC is master, 4 lanes active (0,1,2,3)
                  2 -> TSC is master, 3 lanes active (0,1,2  )
                  3 -> TSC is master, 2 lanes active (0,1    )
                  4 -> TSC is slave,  4 lanes active (0,1,2,3)
                  5 -> TSC is slave,  3 lanes active (0,1,2  )
                  6 -> TSC is slave,  2 lanes active (0,1    )

Other parameters like speed are set in the set_sped intfc.

<B>How to call:</B><br>
<p>This function may be called directly or through the function
#tscmod_tier1_selector() using case-sensitive string "MASTER_SLAVE_CONTROL".</p>
*/
int tscmod_master_slave_control (tscmod_st* pc)
{
  /* fix this */
  return SOC_E_ERROR;
}

/*!
\brief controls PCS Bypass Control function.

\param  pc handle to current TSC context (#tscmod_st)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.


\details Set the ::tscmod_st::per_lane_control field of #tscmod_st to <B>0 to
disable</B> PCS  <B>1 to enable</B>.

This function modifies the following registers

<B>How to call:</B><br>
<p>Call this function directly or through the function
#tscmod_tier1_selector() with the upper-case string "PCS_BYPASS_CTL".</p>
*/

int tscmod_pcs_bypass_ctl (tscmod_st* pc)
{
  /* fix this */
  return SOC_E_ERROR;
}

/*!
\brief Controls the setting/resetting of autoneg advertisement registers.

\param  pc handle to current TSC context (#tscmod_st)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.

\details
This function modifies the following registers:

    AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BASE_ABILITIES_0 (0xc186)

    tsc->per_lane_control=0x001   writing cl73/cl37 ability.
    tsc->per_lane_control=0x400   TSCMOD_AN_SET_RF_DISABLE
                                  disable remote falut
    tsc->per_lane_control=0x100   TSCMOD_AN_SET_SGMII_SPEED
                                  SGMII speed mode. [1:0] 10=1000M; 01=100M; 00=10M
                                                    [4] 1 for master

                                                    (TBD)
    tsc->per_lane_control=0x200   TSCMOD_AN_SET_SGMII_MASTER
                                  sgmii_master_mode [0] 1=master.

    tsc->per_lane_control=0x1000  TSCMOD_AN_SET_HG_MODE

    tsc->per_lane_control=0x2000  TSCMOD_AN_SET_FEC_MODE

    tsc->per_lane_control=0x4000  TSCMOD_AN_SET_CL72_MODE


<B>How to call:</B><br>
<p>Call this function directly or through the function
#tscmod_tier1_selector() with the upper-case string "AUTONEG_SET".</p>
*/

int tscmod_autoneg_set(tscmod_st* pc)               /* AUTONEG_SET */
{

   uint16 data, mask, cl37_pause ;   int sgmii_speed, sgmii_master ;
   data = 0 ;
   mask = 0x0;
   cl37_pause = 0 ;  sgmii_speed = 0 ;

   if(pc->per_lane_control & TSCMOD_AN_SET_RF_DISABLE ) { /* 0x10 */
      data = DIGITAL_CONTROL1000X2_DISABLE_REMOTE_FAULT_REPORTING_MASK ;
      mask = DIGITAL_CONTROL1000X2_DISABLE_REMOTE_FAULT_REPORTING_MASK ;
      SOC_IF_ERROR_RETURN
         (MODIFY_WC40_DIGITAL_CONTROL1000X2r(pc->unit, pc, data, mask)) ;
      return SOC_E_NONE;
   }

   if(pc->per_lane_control & TSCMOD_AN_SET_SGMII_SPEED) {
      sgmii_speed = pc->per_lane_control & 0x3 ;
      sgmii_master= (pc->per_lane_control>>4) & 0x1 ;
      data = sgmii_speed << AN_X4_ABILITIES_LOCAL_DEVICE_CL37_BASE_ABILITIES_SGMII_SPEED_SHIFT |
             sgmii_master<< AN_X4_ABILITIES_LOCAL_DEVICE_CL37_BASE_ABILITIES_SGMII_MASTER_MODE_SHIFT;
      mask = AN_X4_ABILITIES_LOCAL_DEVICE_CL37_BASE_ABILITIES_SGMII_SPEED_MASK|
             AN_X4_ABILITIES_LOCAL_DEVICE_CL37_BASE_ABILITIES_SGMII_MASTER_MODE_MASK ;
      /* 0xc181 */
      SOC_IF_ERROR_RETURN
         (MODIFY_WC40_AN_X4_ABILITIES_LOCAL_DEVICE_CL37_BASE_ABILITIESr(pc->unit, pc, data, mask));
      return SOC_E_NONE;
   }

   if(pc->per_lane_control & TSCMOD_AN_SET_CL37_ATTR ){
      mask = AN_X4_ABILITIES_LOCAL_DEVICE_OVER1G_ABILITIES_1_HG2_MASK |
             AN_X4_ABILITIES_LOCAL_DEVICE_OVER1G_ABILITIES_1_FEC_MASK |
             AN_X4_ABILITIES_LOCAL_DEVICE_OVER1G_ABILITIES_1_CL72_MASK ;
      data  = (pc->per_lane_control & TSCMOD_AN_SET_CL72_MODE)? AN_X4_ABILITIES_LOCAL_DEVICE_OVER1G_ABILITIES_1_CL72_MASK:0 ;
      data |= (pc->per_lane_control & TSCMOD_AN_SET_FEC_MODE) ? AN_X4_ABILITIES_LOCAL_DEVICE_OVER1G_ABILITIES_1_FEC_MASK:0  ;
      data |= (pc->per_lane_control & TSCMOD_AN_SET_HG_MODE)  ? AN_X4_ABILITIES_LOCAL_DEVICE_OVER1G_ABILITIES_1_HG2_MASK:0  ;
      SOC_IF_ERROR_RETURN (MODIFY_WC40_AN_X4_ABILITIES_LOCAL_DEVICE_OVER1G_ABILITIES_1r(pc->unit, pc, data, mask));
      return SOC_E_NONE;
   }

   if(pc->per_lane_control & TSCMOD_AN_SET_CL73_FEC_OFF) {
      data = 0 ; mask = AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BASE_ABILITIES_0_FEC_MASK ;
      SOC_IF_ERROR_RETURN
         (MODIFY_WC40_AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BASE_ABILITIES_0r(pc->unit, pc, data, mask));
      return SOC_E_NONE;
   }

   if(pc->per_lane_control & TSCMOD_AN_SET_CL73_FEC_ON) {
      data = 3 << AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BASE_ABILITIES_0_FEC_SHIFT ;
      mask = AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BASE_ABILITIES_0_FEC_MASK ;
      SOC_IF_ERROR_RETURN
         (MODIFY_WC40_AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BASE_ABILITIES_0r(pc->unit, pc, data, mask));
      return SOC_E_NONE;
   }

   /* for 10G pdetect */
   if(pc->per_lane_control & TSCMOD_AN_SET_CL48_SYNC) {
      data = (pc->per_lane_control & 0x1) ? RX_X4_CONTROL0_DECODE_CONTROL_0_CL48_SYNCACQ_EN_MASK : 0 ;
      mask = RX_X4_CONTROL0_DECODE_CONTROL_0_CL48_SYNCACQ_EN_MASK ;
      SOC_IF_ERROR_RETURN    /* 0xc134 */
        (MODIFY_WC40_RX_X4_CONTROL0_DECODE_CONTROL_0r(pc->unit, pc, data, mask));
      return SOC_E_NONE;
   }

   /* CRTSC-723 */
   if(pc->per_lane_control & TSCMOD_AN_SET_LK_FAIL_INHIBIT_TIMER_NO_CL72){
      data = (pc->per_lane_control & 0x1) ? 0x8236 :
         ((pc->an_ctl & TSCMOD_AN_CTL_LINK_FAIL_NOCL72_WAIT_TIMER)?0xbb8:0x1a10) ;
      SOC_IF_ERROR_RETURN
         (WRITE_WC40_AN_X1_TIMERS_LINK_FAIL_INHIBIT_TIMER_NOT_CL72r
          (pc->unit, pc, data));
      return SOC_E_NONE;
   }

   if( (pc->an_tech_ability >> TSCMOD_ABILITY_1G_KX ) & 1 ) {
      data |= ( 1 << AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BASE_ABILITIES_0_BASE_1000BASE_KX_SHIFT) ;
   }
   if( (pc->an_tech_ability >> TSCMOD_ABILITY_10G_KX4 ) & 1 ) {
      data |= ( 1 << AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BASE_ABILITIES_0_BASE_10GBASE_KX4_SHIFT) ;
   }
   if( (pc->an_tech_ability >> TSCMOD_ABILITY_10G_KR ) & 1 ) {
      data |= ( 1 << AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BASE_ABILITIES_0_BASE_10GBASE_KR_SHIFT) ;
   }
   if( (pc->an_tech_ability >> TSCMOD_ABILITY_40G_KR4 ) & 1 ) {
      data |= ( 1 << AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BASE_ABILITIES_0_BASE_40GBASE_KR4_SHIFT) ;
   }
   if( (pc->an_tech_ability >> TSCMOD_ABILITY_40G_CR4 ) & 1 ) {
      data |= ( 1 << AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BASE_ABILITIES_0_BASE_40GBASE_CR4_SHIFT) ;
   }
   if( (pc->an_tech_ability >> TSCMOD_ABILITY_100G_CR10 ) & 1 ) {
      data |= ( 1 << AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BASE_ABILITIES_0_BASE_100GBASE_CR10_SHIFT) ;
   }
   if(pc->an_tech_ability & TSCMOD_ABILITY_SYMM_PAUSE )
      data |= (0x1 << AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BASE_ABILITIES_0_CL73_PAUSE_SHIFT) ;
   if(pc->an_tech_ability & TSCMOD_ABILITY_ASYM_PAUSE )
      data |= (0x2 << AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BASE_ABILITIES_0_CL73_PAUSE_SHIFT) ;

   if(pc->an_fec) {
      data |= 0x3 << AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BASE_ABILITIES_0_FEC_SHIFT ;  /* FEC on */
   } else {
      data &= ~AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BASE_ABILITIES_0_FEC_MASK ;
   }

   /* 0xc186 */
   SOC_IF_ERROR_RETURN
      (WRITE_WC40_AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BASE_ABILITIES_0r(pc->unit, pc, data));

  data = 0x0 ;
  mask = 0x0;
  if( (pc->an_tech_ability >> TSCMOD_ABILITY_20G_KR2 ) & 1 ) {
     data |= AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BAM_ABILITIES_BAM_20GBASE_KR2_MASK ;
     data |= AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BAM_ABILITIES_HPAM_20GKR2_MASK ;
  }
  if( (pc->an_tech_ability >> TSCMOD_ABILITY_20G_CR2 ) & 1 ) {
     data |= AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BAM_ABILITIES_BAM_20GBASE_CR2_MASK ;
  }
  mask = AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BAM_ABILITIES_BAM_20GBASE_KR2_MASK |
         AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BAM_ABILITIES_BAM_20GBASE_CR2_MASK |
         AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BAM_ABILITIES_HPAM_20GKR2_MASK ;
  /* set cl73_bam_code set 0xc187 */
  SOC_IF_ERROR_RETURN (MODIFY_WC40_AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BAM_ABILITIESr(pc->unit, pc, data, mask));

  data = 0x0 ;
  mask = 0x0;
  if((pc->an_bam37_ability >> TSCMOD_BAM37ABL_2P5G)&1)
     data |= (1<<AN_X4_ABILITIES_LOCAL_DEVICE_OVER1G_ABILITIES_0_BAM_2P5GBASE_X_SHIFT);
  if((pc->an_bam37_ability >> TSCMOD_BAM37ABL_5G_X4)&1)
     data |= (1<<AN_X4_ABILITIES_LOCAL_DEVICE_OVER1G_ABILITIES_0_BAM_5GBASE_X4_SHIFT);
  if((pc->an_bam37_ability >> TSCMOD_BAM37ABL_6G_X4)&1)
     data |= (1<<AN_X4_ABILITIES_LOCAL_DEVICE_OVER1G_ABILITIES_0_BAM_6GBASE_X4_SHIFT);
  if((pc->an_bam37_ability >> TSCMOD_BAM37ABL_10G_HIGIG)&1)
     data |= (1<<AN_X4_ABILITIES_LOCAL_DEVICE_OVER1G_ABILITIES_0_BAM_10GBASE_X4_SHIFT);
  if((pc->an_bam37_ability >> TSCMOD_BAM37ABL_10G_CX4)&1)
     data |= (1<<AN_X4_ABILITIES_LOCAL_DEVICE_OVER1G_ABILITIES_0_BAM_10GBASE_X4_CX4_SHIFT);
  if((pc->an_bam37_ability >> TSCMOD_BAM37ABL_10G_DXGXS)&1)
     data |= (1<<AN_X4_ABILITIES_LOCAL_DEVICE_OVER1G_ABILITIES_0_BAM_10GBASE_X2_SHIFT);
  if((pc->an_bam37_ability >> TSCMOD_BAM37ABL_10G_X2_CX4)&1)
     data |= (1<<AN_X4_ABILITIES_LOCAL_DEVICE_OVER1G_ABILITIES_0_BAM_10GBASE_X2_CX4_SHIFT);
  if((pc->an_bam37_ability >> TSCMOD_BAM37ABL_10P5G_DXGXS)&1)
     data |= (1<<AN_X4_ABILITIES_LOCAL_DEVICE_OVER1G_ABILITIES_0_BAM_10P5GBASE_X2_SHIFT);
  if((pc->an_bam37_ability >> TSCMOD_BAM37ABL_12G_X4)&1)
     data |= (1<<AN_X4_ABILITIES_LOCAL_DEVICE_OVER1G_ABILITIES_0_BAM_12GBASE_X4_SHIFT);
  if((pc->an_bam37_ability >> TSCMOD_BAM37ABL_12P5_X4)&1)
     data |= (1<<AN_X4_ABILITIES_LOCAL_DEVICE_OVER1G_ABILITIES_0_BAM_12P5GBASE_X4_SHIFT);
  if((pc->an_bam37_ability >> TSCMOD_BAM37ABL_12P7_DXGXS)&1)
     data |= (1<<AN_X4_ABILITIES_LOCAL_DEVICE_OVER1G_ABILITIES_0_BAM_12P7GBASE_X2_SHIFT);

  /* 0xc184 BAM37 */
  SOC_IF_ERROR_RETURN (WRITE_WC40_AN_X4_ABILITIES_LOCAL_DEVICE_OVER1G_ABILITIES_0r(pc->unit, pc, data));

  data = 0x0 ;
  mask = 0x0;
  if((pc->an_bam37_ability >> TSCMOD_BAM37ABL_13G_X4)&1)
     data |= (1<<AN_X4_ABILITIES_LOCAL_DEVICE_OVER1G_ABILITIES_1_BAM_13GBASE_X4_SHIFT) ;
  if((pc->an_bam37_ability >> TSCMOD_BAM37ABL_15G_X4)&1)
     data |= (1<<AN_X4_ABILITIES_LOCAL_DEVICE_OVER1G_ABILITIES_1_BAM_15GBASE_X4_SHIFT);
  if((pc->an_bam37_ability >> TSCMOD_BAM37ABL_15P75G_R2)&1)
     data |= (1<<AN_X4_ABILITIES_LOCAL_DEVICE_OVER1G_ABILITIES_1_BAM_15P75GBASE_X2_SHIFT);
  if((pc->an_bam37_ability >> TSCMOD_BAM37ABL_16G_X4)&1)
     data |= (1<<AN_X4_ABILITIES_LOCAL_DEVICE_OVER1G_ABILITIES_1_BAM_16GBASE_X4_SHIFT);
  if((pc->an_bam37_ability >> TSCMOD_BAM37ABL_20G_X4_CX4)&1)
     data |= (1<<AN_X4_ABILITIES_LOCAL_DEVICE_OVER1G_ABILITIES_1_BAM_20GBASE_X4_CX4_SHIFT);
  if((pc->an_bam37_ability >> TSCMOD_BAM37ABL_20G_X4)&1)
     data |= (1<<AN_X4_ABILITIES_LOCAL_DEVICE_OVER1G_ABILITIES_1_BAM_20GBASE_X4_SHIFT);
  if((pc->an_bam37_ability >> TSCMOD_BAM37ABL_20G_X2)&1)
     data |= (1<<AN_X4_ABILITIES_LOCAL_DEVICE_OVER1G_ABILITIES_1_BAM_20GBASE_X2_SHIFT);
  if((pc->an_bam37_ability >> TSCMOD_BAM37ABL_20G_X2_CX4)&1)
     data |= (1<<AN_X4_ABILITIES_LOCAL_DEVICE_OVER1G_ABILITIES_1_BAM_20GBASE_X2_CX4_SHIFT);
  if((pc->an_bam37_ability >> TSCMOD_BAM37ABL_21G_X4)&1)
     data |= (1<<AN_X4_ABILITIES_LOCAL_DEVICE_OVER1G_ABILITIES_1_BAM_21GBASE_X4_SHIFT);
  if((pc->an_bam37_ability >> TSCMOD_BAM37ABL_25P455G)&1)
     data |= (1<<AN_X4_ABILITIES_LOCAL_DEVICE_OVER1G_ABILITIES_1_BAM_25P455GBASE_X4_SHIFT);
  if((pc->an_bam37_ability >> TSCMOD_BAM37ABL_31P5G)&1)
     data |= (1<<AN_X4_ABILITIES_LOCAL_DEVICE_OVER1G_ABILITIES_1_BAM_31P5GBASE_X4_SHIFT);
  if((pc->an_bam37_ability >> TSCMOD_BAM37ABL_32P7G)&1)
     data |= (1<<AN_X4_ABILITIES_LOCAL_DEVICE_OVER1G_ABILITIES_1_BAM_32P7GBASE_X4_SHIFT);
  if((pc->an_bam37_ability >> TSCMOD_BAM37ABL_40G)&1)
     data |= (1<<AN_X4_ABILITIES_LOCAL_DEVICE_OVER1G_ABILITIES_1_BAM_40GBASE_X4_SHIFT);

  mask = 0x1fff ; /* all ability */
  /* 0xc183 BAM37 */
  SOC_IF_ERROR_RETURN (MODIFY_WC40_AN_X4_ABILITIES_LOCAL_DEVICE_OVER1G_ABILITIES_1r(pc->unit, pc, data, mask));


  /* 0xc181 */
  cl37_pause |= (pc->an_tech_ability & TSCMOD_ABILITY_SYMM_PAUSE)? 0x1 : 0  ;
  cl37_pause |= (pc->an_tech_ability & TSCMOD_ABILITY_ASYM_PAUSE)? 0x2 : 0  ;
  data = cl37_pause << AN_X4_ABILITIES_LOCAL_DEVICE_CL37_BASE_ABILITIES_CL37_PAUSE_SHIFT ;
  mask = AN_X4_ABILITIES_LOCAL_DEVICE_CL37_BASE_ABILITIES_CL37_PAUSE_MASK ;
  SOC_IF_ERROR_RETURN
     (MODIFY_WC40_AN_X4_ABILITIES_LOCAL_DEVICE_CL37_BASE_ABILITIESr(pc->unit, pc, data, mask));

  /* 0x9115 CL72 cap bits disabled: no need once CL72 control is deployed.
  SOC_IF_ERROR_RETURN (WRITE_WC40_CL72_SHARED_CL72_MISC3_CONTROLr(pc->unit, pc, 0));
  */

  return SOC_E_NONE;
}


/*!
\brief To get autoneg advertisement registers.

\param  pc handle to current TSC context (#tscmod_st)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.

\details
This function modifies the following registers:

    tsc->per_lane_control=0xa  TSCMOD_AN_GET_CL73_ABIL reading cl73 ability
    tsc->per_lane_control=0xb  TSCMOD_AN_GET_CL37_ABIL reading cl37 ability
    tsc->per_lane_control=0xc  TSCMOD_AN_GET_MISC_ABIL reading other ability

    tsc->per_lane_control=0xa8 TSCMOD_AN_GET_LP_CL73_ABIL  reading LP cl73 ability
    tsc->per_lane_control=0xb8 TSCMOD_AN_GET_LP_CL37_ABIL  reading LP cl37 ability
    tsc->per_lane_control=0xc8 TSCMOD_AN_GET_LP_MISC_ABIL  reading LP other ability


<B>How to call:</B><br>
<p>Call this function directly or through the function
#tscmod_tier1_selector() with the upper-case string "AUTONEG_GET".</p>
*/

int tscmod_autoneg_get(tscmod_st* pc)               /* AUTONEG_GET */
{

   uint16 data; uint32 ability ; uint32 user_code ;

   data = 0 ; ability = 0 ;  user_code = 0 ;
   if(pc->per_lane_control==TSCMOD_AN_GET_CL73_ABIL) {   /* reading cl73 */
      /* 0xc186 */
      SOC_IF_ERROR_RETURN
         (READ_WC40_AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BASE_ABILITIES_0r(pc->unit, pc, &data));
      if(data&AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BASE_ABILITIES_0_BASE_1000BASE_KX_MASK)
         ability |=(1<<TSCMOD_ABILITY_1G_KX) ;
      if(data&AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BASE_ABILITIES_0_BASE_10GBASE_KX4_MASK)
         ability |=(1<<TSCMOD_ABILITY_10G_KX4) ;
      if(data&AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BASE_ABILITIES_0_BASE_10GBASE_KR_MASK)
         ability |=(1<<TSCMOD_ABILITY_10G_KR) ;
      if(data&AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BASE_ABILITIES_0_BASE_40GBASE_KR4_MASK)
         ability |=(1<<TSCMOD_ABILITY_40G_KR4);
      if(data&AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BASE_ABILITIES_0_BASE_40GBASE_CR4_MASK)
         ability |=(1<<TSCMOD_ABILITY_40G_CR4);
      if(data&AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BASE_ABILITIES_0_BASE_100GBASE_CR10_MASK)
         ability |=(1<<TSCMOD_ABILITY_100G_CR10) ;

      data = 0 ;
      /* set cl73_bam_code set 0xc187 */
      SOC_IF_ERROR_RETURN
         (READ_WC40_AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BAM_ABILITIESr(pc->unit, pc, &data));

      if(data&AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BAM_ABILITIES_BAM_20GBASE_KR2_MASK)
         ability |=(1<<TSCMOD_ABILITY_20G_KR2) ;
      if(data&AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BAM_ABILITIES_BAM_20GBASE_CR2_MASK)
         ability |=(1<<TSCMOD_ABILITY_20G_CR2) ;

   }
   else if(pc->per_lane_control==TSCMOD_AN_GET_CL37_ABIL) {  /* reading cl37 */
      /* 0xc184 BAM37 */
      SOC_IF_ERROR_RETURN
         (READ_WC40_AN_X4_ABILITIES_LOCAL_DEVICE_OVER1G_ABILITIES_0r(pc->unit, pc, &data));
      if(data&AN_X4_ABILITIES_LOCAL_DEVICE_OVER1G_ABILITIES_0_BAM_2P5GBASE_X_MASK)
         ability |=(1<<TSCMOD_BAM37ABL_2P5G) ;
      if(data&AN_X4_ABILITIES_LOCAL_DEVICE_OVER1G_ABILITIES_0_BAM_5GBASE_X4_MASK)
         ability |=(1<<TSCMOD_BAM37ABL_5G_X4) ;
      if(data&AN_X4_ABILITIES_LOCAL_DEVICE_OVER1G_ABILITIES_0_BAM_6GBASE_X4_MASK)
         ability |=(1<<TSCMOD_BAM37ABL_6G_X4) ;
      if(data&AN_X4_ABILITIES_LOCAL_DEVICE_OVER1G_ABILITIES_0_BAM_10GBASE_X4_MASK)
         ability |=(1<<TSCMOD_BAM37ABL_10G_HIGIG) ;
      if(data&AN_X4_ABILITIES_LOCAL_DEVICE_OVER1G_ABILITIES_0_BAM_10GBASE_X4_CX4_MASK)
         ability |=(1<<TSCMOD_BAM37ABL_10G_CX4) ;
      if(data&AN_X4_ABILITIES_LOCAL_DEVICE_OVER1G_ABILITIES_0_BAM_10GBASE_X2_MASK)
         ability |=(1<<TSCMOD_BAM37ABL_10G_DXGXS) ;
      if(data&AN_X4_ABILITIES_LOCAL_DEVICE_OVER1G_ABILITIES_0_BAM_10GBASE_X2_CX4_MASK)
         ability |=(1<<TSCMOD_BAM37ABL_10G_X2_CX4);
      if(data&AN_X4_ABILITIES_LOCAL_DEVICE_OVER1G_ABILITIES_0_BAM_10P5GBASE_X2_MASK)
         ability |=(1<<TSCMOD_BAM37ABL_10P5G_DXGXS) ;
      if(data&AN_X4_ABILITIES_LOCAL_DEVICE_OVER1G_ABILITIES_0_BAM_12GBASE_X4_MASK)
         ability |=(1<<TSCMOD_BAM37ABL_12G_X4);
      if(data&AN_X4_ABILITIES_LOCAL_DEVICE_OVER1G_ABILITIES_0_BAM_12P5GBASE_X4_MASK)
         ability |=(1<<TSCMOD_BAM37ABL_12P5_X4);
      if(data&AN_X4_ABILITIES_LOCAL_DEVICE_OVER1G_ABILITIES_0_BAM_12P7GBASE_X2_MASK)
         ability |=(1<<TSCMOD_BAM37ABL_12P7_DXGXS);

      data = 0 ;
      /* 0xc183 BAM37 */
      SOC_IF_ERROR_RETURN
         (READ_WC40_AN_X4_ABILITIES_LOCAL_DEVICE_OVER1G_ABILITIES_1r(pc->unit, pc, &data));
      if(data&AN_X4_ABILITIES_LOCAL_DEVICE_OVER1G_ABILITIES_1_BAM_13GBASE_X4_MASK)
         ability |=(1<<TSCMOD_BAM37ABL_13G_X4) ;
      if(data&AN_X4_ABILITIES_LOCAL_DEVICE_OVER1G_ABILITIES_1_BAM_15GBASE_X4_MASK)
         ability |=(1<<TSCMOD_BAM37ABL_15G_X4) ;
      if(data&AN_X4_ABILITIES_LOCAL_DEVICE_OVER1G_ABILITIES_1_BAM_15P75GBASE_X2_MASK)
         ability |=(1<<TSCMOD_BAM37ABL_15P75G_R2) ;
      if(data&AN_X4_ABILITIES_LOCAL_DEVICE_OVER1G_ABILITIES_1_BAM_16GBASE_X4_MASK)
         ability |=(1<<TSCMOD_BAM37ABL_16G_X4);
      if(data&AN_X4_ABILITIES_LOCAL_DEVICE_OVER1G_ABILITIES_1_BAM_20GBASE_X4_CX4_MASK)
         ability |=(1<<TSCMOD_BAM37ABL_20G_X4_CX4);
      if(data&AN_X4_ABILITIES_LOCAL_DEVICE_OVER1G_ABILITIES_1_BAM_20GBASE_X4_MASK)
         ability |=(1<<TSCMOD_BAM37ABL_20G_X4);
      if(data&AN_X4_ABILITIES_LOCAL_DEVICE_OVER1G_ABILITIES_1_BAM_20GBASE_X2_MASK)
         ability |=(1<<TSCMOD_BAM37ABL_20G_X2);
      if(data&AN_X4_ABILITIES_LOCAL_DEVICE_OVER1G_ABILITIES_1_BAM_20GBASE_X2_CX4_MASK)
         ability |=(1<<TSCMOD_BAM37ABL_20G_X2_CX4);
      if(data&AN_X4_ABILITIES_LOCAL_DEVICE_OVER1G_ABILITIES_1_BAM_21GBASE_X4_MASK)
         ability |=(1<<TSCMOD_BAM37ABL_21G_X4);
      if(data&AN_X4_ABILITIES_LOCAL_DEVICE_OVER1G_ABILITIES_1_BAM_25P455GBASE_X4_MASK)
         ability |=(1<<TSCMOD_BAM37ABL_25P455G);
      if(data&AN_X4_ABILITIES_LOCAL_DEVICE_OVER1G_ABILITIES_1_BAM_31P5GBASE_X4_MASK)
         ability |=(1<<TSCMOD_BAM37ABL_31P5G) ;
      if(data&AN_X4_ABILITIES_LOCAL_DEVICE_OVER1G_ABILITIES_1_BAM_32P7GBASE_X4_MASK)
         ability |=(1<<TSCMOD_BAM37ABL_32P7G);
      if(data&AN_X4_ABILITIES_LOCAL_DEVICE_OVER1G_ABILITIES_1_BAM_40GBASE_X4_MASK)
         ability |=(1<<TSCMOD_BAM37ABL_40G) ;

   }

   else if(pc->per_lane_control==TSCMOD_AN_GET_MISC_ABIL) {
      /* make sure CL73 and CL37 pasue setting is the same */
      ability = 0 ;
      SOC_IF_ERROR_RETURN
      (READ_WC40_AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BASE_ABILITIES_0r(pc->unit, pc, &data));

      if(data & (0x1 << AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BASE_ABILITIES_0_CL73_PAUSE_SHIFT))
         ability |= TSCMOD_SYMM_PAUSE ;

      if(data & (0x2 << AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BASE_ABILITIES_0_CL73_PAUSE_SHIFT))
         ability |= TSCMOD_ASYM_PAUSE ;

   }

   else if(pc->per_lane_control==TSCMOD_AN_GET_LP_CL73_ABIL) {   /* reading LP cl73 */
      /* 0xc199 */
      SOC_IF_ERROR_RETURN
         (READ_WC40_AN_X4_HW_LP_PAGES_LP_BASE_PAGE2r(pc->unit, pc, &data));
      if(data&(1<<10)) ability |=(1<<TSCMOD_ABILITY_100G_CR10) ;
      if(data&(1<<9))  ability |=(1<<TSCMOD_ABILITY_40G_CR4);
      if(data&(1<<8))  ability |=(1<<TSCMOD_ABILITY_40G_KR4);
      if(data&(1<<7))  ability |=(1<<TSCMOD_ABILITY_10G_KR);
      if(data&(1<<6))  ability |=(1<<TSCMOD_ABILITY_10G_KX4) ;
      if(data&(1<<5))  ability |=(1<<TSCMOD_ABILITY_1G_KX) ;

      data = 0 ;  /*0xc192 */
      SOC_IF_ERROR_RETURN
         (READ_WC40_AN_X4_HW_LP_PAGES_LP_MP5_UP3r(pc->unit, pc, &data));
      user_code = (data & 0x1ff) << 11 ; 

      data = 0 ;
      /* 0xc193 */
      SOC_IF_ERROR_RETURN
         (READ_WC40_AN_X4_HW_LP_PAGES_LP_MP5_UP4r(pc->unit, pc, &data));
      if(data&(1<<1)) {
         ability |=(1<<TSCMOD_ABILITY_20G_KR2) ;
      } else {
         user_code |= (data & 0x7ff) ; 
         if(user_code == 0xabe20) 
            ability |=(1<<TSCMOD_ABILITY_20G_KR2) ;
      }
      if(data&(1<<0)) ability |=(1<<TSCMOD_ABILITY_20G_CR2) ;
   }
   else if(pc->per_lane_control==TSCMOD_AN_GET_LP_CL37_ABIL) {   /* reading LP cl37 */
      /* 0xc194 */
      SOC_IF_ERROR_RETURN
         (READ_WC40_AN_X4_HW_LP_PAGES_LP_MP1024_UP1r(pc->unit, pc, &data));
      if(data&(1<<0)) ability |=(1<<TSCMOD_BAM37ABL_2P5G) ;
      if(data&(1<<1)) ability |=(1<<TSCMOD_BAM37ABL_5G_X4) ;
      if(data&(1<<2)) ability |=(1<<TSCMOD_BAM37ABL_6G_X4) ;
      if(data&(1<<3)) ability |=(1<<TSCMOD_BAM37ABL_10G_HIGIG) ;
      if(data&(1<<4)) ability |=(1<<TSCMOD_BAM37ABL_10G_CX4) ;
      if(data&(1<<5)) ability |=(1<<TSCMOD_BAM37ABL_12G_X4);
      if(data&(1<<6)) ability |=(1<<TSCMOD_BAM37ABL_12P5_X4);
      if(data&(1<<7)) ability |=(1<<TSCMOD_BAM37ABL_13G_X4) ;
      if(data&(1<<8)) ability |=(1<<TSCMOD_BAM37ABL_15G_X4) ;
      if(data&(1<<9)) ability |=(1<<TSCMOD_BAM37ABL_16G_X4);
      if(data&(1<<10))ability |=(1<<TSCMOD_BAM37ABL_20G_X4_CX4);

      data = 0 ;
      /* 0xc196 */
      SOC_IF_ERROR_RETURN
         (READ_WC40_AN_X4_HW_LP_PAGES_LP_MP1024_UP3r(pc->unit, pc, &data));
      if(data&(1<<5)) ability |=(1<<TSCMOD_BAM37ABL_40G) ;
      if(data&(1<<6)) ability |=(1<<TSCMOD_BAM37ABL_32P7G);
      if(data&(1<<7)) ability |=(1<<TSCMOD_BAM37ABL_31P5G) ;
      if(data&(1<<8)) ability |=(1<<TSCMOD_BAM37ABL_25P455G);
      if(data&(1<<9)) ability |=(1<<TSCMOD_BAM37ABL_21G_X4);

      data = 0 ;
      /* 0xc197 */
      SOC_IF_ERROR_RETURN
         (READ_WC40_AN_X4_HW_LP_PAGES_LP_MP1024_UP4r(pc->unit, pc, &data));
      if(data&(1<<0)) ability |=(1<<TSCMOD_BAM37ABL_20G_X4);
      if(data&(1<<1)) ability |=(1<<TSCMOD_BAM37ABL_10G_DXGXS) ;
      if(data&(1<<2)) ability |=(1<<TSCMOD_BAM37ABL_10G_X2_CX4);
      if(data&(1<<3)) ability |=(1<<TSCMOD_BAM37ABL_10P5G_DXGXS) ;
      if(data&(1<<4)) ability |=(1<<TSCMOD_BAM37ABL_12P7_DXGXS);
      if(data&(1<<5)) ability |=(1<<TSCMOD_BAM37ABL_20G_X2);
      if(data&(1<<6)) ability |=(1<<TSCMOD_BAM37ABL_20G_X2_CX4);
      if(data&(1<<7)) ability |=(1<<TSCMOD_BAM37ABL_15P75G_R2) ;
      if(data&(1<<8)) ability |=(1<<TSCMOD_BAM37ABL_10G_CX4) ;  /* 10G_CX1 ? */
      /* 1G ? */
   }
   else if(pc->per_lane_control==TSCMOD_AN_GET_LP_MISC_ABIL) {   /* reading misc */
      /* 0xc198 */
      SOC_IF_ERROR_RETURN
         (READ_WC40_AN_X4_HW_LP_PAGES_LP_BASE_PAGE1r(pc->unit, pc, &data));
      if((pc->an_type == TSCMOD_CL73)||(pc->an_type == TSCMOD_CL73_BAM)) {
         ability = (data >>10) & 0x3  ;
      } else if(pc->an_type ==TSCMOD_CL37_SGMII) {
         ability = 0 ;
      } else {
         ability = (data >>7) & 0x3  ;
      }

   }
   else if(pc->per_lane_control==TSCMOD_AN_GET_LP_SGMII_ABIL) {
      /* 0xc198 */
      SOC_IF_ERROR_RETURN
         (READ_WC40_AN_X4_HW_LP_PAGES_LP_BASE_PAGE1r(pc->unit, pc, &data));
      ability = (data>>10) & 0x3 ;
   }

   pc->accData = ability ;
   return SOC_E_NONE;
}



/*!
\brief Controls cl37 aneg set/reset user pages 4, 3, 2 & 1 advertisement regs.

\param  pc handle to current TSC context (#tscmod_st)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.

This function modifies the following registers:

<B>How to call:</B><br>
<p>Call this function directly or through the function
#tscmod_tier1_selector() with the upper-case string "AUTONEG_PAGE_SET".</p>
*/

int tscmod_autoneg_page_set(tscmod_st* pc)               /* AUTONEG_PAGE_SET */
{
  /* fix this */
  return SOC_E_ERROR;
}

/*!
\brief Controls the setting/resetting of autoneg ability and enabling/disabling

\param  pc handle to current TSC context (#tscmod_st)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.


This function programs auto-negotiation (AN) modes for the TSC. It can
enable/disable clause37/clause73/BAM autonegotiation capabilities. Call this
function once for combo mode and once per lane in independent lane mode.

The autonegotiation mode is controlled by setting specific bits of
#tscmod_st::an_type.

The autonegotiation mode is indicated by setting an_type with

\li #TSCMOD_AN_NONE   (disable AN)
\li #TSCMOD_CL73
\li #TSCMOD_CL37
\li #TSCMOD_CL73_BAM
\li #TSCMOD_CL37_BAM
\li #TSCMOD_CL37_SGMII
\li #TSCMOD_HPAM

<B>How to call:</B><br>
<p>Call this function directly or through the function
#tscmod_tier1_selector() with the upper-case string "AUTONEG_CONTROL".</p>
*/

int tscmod_autoneg_control(tscmod_st* pc)
{

  uint16 data, mask ;
  uint16 an_setup_enable, num_advertised_lanes, cl37_bam_enable, cl73_bam_enable  ;
  uint16 cl73_hpam_enable, cl73_enable, cl37_sgmii_enable, cl37_enable;
  uint16 cl73_nonce_match_over, cl73_nonce_match_val  ;
  uint16 base_selector, cl73_bam_code  ;
  uint16 next_page  ;
  uint16 cl37_next_page,  cl37_full_duplex;
  uint16 sgmii_full_duplex ;
  uint16 cl37_bam_code, over1g_ability, over1g_page_count;
  uint16 cl37_restart, cl73_restart ;
  int    no_an ;
  uint16 single_port_mode;
  uint16 hpam_autodetect_cl73 ;

  no_an                         = 0 ;

  an_setup_enable               = 0x1;
  num_advertised_lanes          = 0x2;   
  cl37_bam_enable               = 0x0;
  cl73_bam_enable               = 0x0;
  cl73_hpam_enable              = 0x0;
  cl73_enable                   = 0x0;
  cl37_sgmii_enable             = 0x0;
  cl37_enable                   = 0x0;
  cl73_nonce_match_over         = 0x1;
  cl73_nonce_match_val          = 0x0;
  base_selector                 = 0x1;
  cl73_bam_code                 = 0x0;
  next_page                     = 0x0;
  cl37_next_page                = 0x0;
  cl37_full_duplex              = 0x1;
  sgmii_full_duplex             = 0x1;
  cl37_bam_code                 = 0x0;
  over1g_ability                = 0x0;
  over1g_page_count             = 0x0;
  cl37_restart                  = 0x0;
  cl73_restart                  = 0x0;
  hpam_autodetect_cl73          = 0x0;

  if (pc->port_type == TSCMOD_MULTI_PORT) {
     num_advertised_lanes = 0x0;   /* one lane */
  } else if (pc->port_type == TSCMOD_SINGLE_PORT) {
     num_advertised_lanes = 0x2;  /* 4 lanes */
  } else if (pc->port_type == TSCMOD_DXGXS) {
     num_advertised_lanes = 0x1;  /* 2 lanes */
  } else printf("port mode not defined\n ");


  if (pc-> an_type == TSCMOD_CL73) {
    cl73_restart                = 0x1;
    cl73_enable                 = 0x1;
    cl73_nonce_match_over       = 0x1;
    cl73_nonce_match_val        = 0x0;
    base_selector               = 0x1;
  } else if ((pc->an_type == TSCMOD_CL37)||(pc->an_type == TSCMOD_CL37_10G)) {
    cl37_restart                = 0x1;
    cl37_enable                 = 0x1;
    cl37_bam_code               = 0x0;
    over1g_ability              = 0x0;
    over1g_page_count           = 0x0;
    sgmii_full_duplex           = 0x0;
  } else if (pc-> an_type == TSCMOD_CL37_BAM) {
    cl37_restart                = 0x1;
    cl37_enable                 = 0x1;
    cl37_bam_enable             = 0x1;
    cl37_bam_code               = 0x1;
    over1g_ability              = 0x1;
    over1g_page_count           = 0x1;   /* PHY-863: =1: 5 pages; =0: 3 pages; WC uses 3 pages */
    sgmii_full_duplex           = 0x0;
    cl37_next_page              = 0x1;
  } else if (pc-> an_type == TSCMOD_CL37_SGMII) {
    cl37_restart                = 0x1;
    cl37_sgmii_enable           = 0x1;
    cl37_enable                 = 0x1;
    cl37_bam_code               = 0x0;
    over1g_ability              = 0x0;
    over1g_page_count           = 0x0;
  } else if (pc-> an_type == TSCMOD_CL73_BAM) {
    cl73_restart                = 0x1;
    cl73_enable                 = 0x1;
    cl73_bam_enable             = 0x1;
    cl73_bam_code               = 0x3;
    next_page                   = 0x1;
    cl73_nonce_match_over       = 0x1;
    cl73_nonce_match_val        = 0x0;
    base_selector               = 0x1;
  } else if (pc-> an_type == TSCMOD_HPAM) {
    cl73_restart                = 0x1;
    cl73_enable                 = 0x1;
    cl73_hpam_enable            = 0x1;
    cl73_bam_code               = 0x3;
    next_page                   = 0x1;
    cl73_nonce_match_over       = 0x1;
    cl73_nonce_match_val        = 0x0;
    base_selector               = 0x1;
    if(pc->ctrl_type &TSCMOD_CTRL_TYPE_HPAM_AUTODET)
       hpam_autodetect_cl73 = 1;
  } else {
     no_an = 1 ;
     if((pc->verbosity&TSCMOD_DBG_AN)||(pc->verbosity&TSCMOD_DBG_FLOW))
        printf("%-22s: u=%0d p=%0d Autoneg mode not defined\n", FUNCTION_NAME(), pc->unit, pc->port);
  }

  /* single port mode fix */
  if (pc->port_type == TSCMOD_SINGLE_PORT) {
     single_port_mode = 1 ;
  } else {
     single_port_mode = 0 ;
  }
  if(pc->an_type == TSCMOD_CL37_10G) {
     single_port_mode = 0 ;
  }
  mask = MAIN0_SETUP_SINGLE_PORT_MODE_MASK ;
  data = single_port_mode << MAIN0_SETUP_SINGLE_PORT_MODE_SHIFT;
  /* 0x9000 */
  SOC_IF_ERROR_RETURN (MODIFY_WC40_MAIN0_SETUPr(pc -> unit, pc, data, mask));

  /* OS mode overwrite */
  if(pc->an_type ==TSCMOD_CL37_10G) {
     data = RX_X4_CONTROL0_PMA_CONTROL_0_OVERRIDE_OS_MODE_MASK | (7 <<RX_X4_CONTROL0_PMA_CONTROL_0_OS_MODE_SHIFT) ; /* OS 8.25 */
  } else {
     data = 0 ;
  }
  /* c137 */
  mask =  RX_X4_CONTROL0_PMA_CONTROL_0_OVERRIDE_OS_MODE_MASK | RX_X4_CONTROL0_PMA_CONTROL_0_OS_MODE_MASK ;
  if(pc->ctrl_type&TSCMOD_CTRL_TYPE_UC_WR_MASK_EN) {
     SOC_IF_ERROR_RETURN
        (tscmod_mask_wr_cmd(pc, TSC40_RX_X4_CONTROL0_PMA_CONTROL_0r, data, mask)) ;
  } else
     SOC_IF_ERROR_RETURN (MODIFY_WC40_RX_X4_CONTROL0_PMA_CONTROL_0r(pc->unit, pc, data, mask));

  /* set an advertised ports 0xc180 */
  data = an_setup_enable << AN_X4_ABILITIES_ENABLES_AN_SETUP_ENABLE_SHIFT |
     num_advertised_lanes << AN_X4_ABILITIES_ENABLES_NUM_ADVERTISED_LANES_SHIFT ;

  mask = AN_X4_ABILITIES_ENABLES_AN_SETUP_ENABLE_MASK | AN_X4_ABILITIES_ENABLES_NUM_ADVERTISED_LANES_MASK |
         AN_X4_ABILITIES_ENABLES_CL37_AN_RESTART_MASK | AN_X4_ABILITIES_ENABLES_CL73_AN_RESTART_MASK ;
  SOC_IF_ERROR_RETURN (MODIFY_WC40_AN_X4_ABILITIES_ENABLESr(pc->unit, pc, data, mask));

  /* set an abilities 0xc181 */
  data =
     cl37_next_page << AN_X4_ABILITIES_LOCAL_DEVICE_CL37_BASE_ABILITIES_CL37_NEXT_PAGE_SHIFT |
     cl37_full_duplex << AN_X4_ABILITIES_LOCAL_DEVICE_CL37_BASE_ABILITIES_CL37_FULL_DUPLEX_SHIFT |
     sgmii_full_duplex << AN_X4_ABILITIES_LOCAL_DEVICE_CL37_BASE_ABILITIES_SGMII_FULL_DUPLEX_SHIFT ;

  mask = AN_X4_ABILITIES_LOCAL_DEVICE_CL37_BASE_ABILITIES_CL37_NEXT_PAGE_MASK |
         AN_X4_ABILITIES_LOCAL_DEVICE_CL37_BASE_ABILITIES_CL37_FULL_DUPLEX_MASK |
         AN_X4_ABILITIES_LOCAL_DEVICE_CL37_BASE_ABILITIES_SGMII_FULL_DUPLEX_MASK ;

  SOC_IF_ERROR_RETURN
     (MODIFY_WC40_AN_X4_ABILITIES_LOCAL_DEVICE_CL37_BASE_ABILITIESr(pc->unit, pc, data, mask));

  /* cl37 bam abilities 0xc182 */
  data = cl37_bam_code << AN_X4_ABILITIES_LOCAL_DEVICE_CL37_BAM_ABILITIES_CL37_BAM_CODE_SHIFT |
            over1g_ability << AN_X4_ABILITIES_LOCAL_DEVICE_CL37_BAM_ABILITIES_OVER1G_ABILITY_SHIFT |
            over1g_page_count << AN_X4_ABILITIES_LOCAL_DEVICE_CL37_BAM_ABILITIES_OVER1G_PAGE_COUNT_SHIFT;

  mask = AN_X4_ABILITIES_LOCAL_DEVICE_CL37_BAM_ABILITIES_CL37_BAM_CODE_MASK |
            AN_X4_ABILITIES_LOCAL_DEVICE_CL37_BAM_ABILITIES_OVER1G_ABILITY_MASK |
            AN_X4_ABILITIES_LOCAL_DEVICE_CL37_BAM_ABILITIES_OVER1G_PAGE_COUNT_MASK ;

  SOC_IF_ERROR_RETURN
         (MODIFY_WC40_AN_X4_ABILITIES_LOCAL_DEVICE_CL37_BAM_ABILITIESr(pc->unit, pc, data, mask));

  /* set next page  bit set 0xc186 */
  data = next_page << AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BASE_ABILITIES_0_NEXT_PAGE_SHIFT;
  mask = AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BASE_ABILITIES_0_NEXT_PAGE_MASK;

  SOC_IF_ERROR_RETURN ( MODIFY_WC40_AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BASE_ABILITIES_0r(pc->unit, pc, data, mask));

  /* set cl73_bam_code set 0xc187 */
  data = cl73_bam_code << AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BAM_ABILITIES_CL73_BAM_CODE_SHIFT ;
  mask = AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BAM_ABILITIES_CL73_BAM_CODE_MASK ;

  SOC_IF_ERROR_RETURN ( MODIFY_WC40_AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BAM_ABILITIESr(pc->unit, pc, data, mask));

  /* disable ecc of uC 0xffcc */
  /*  SOC_IF_ERROR_RETURN (MODIFY_WC40_UC_COMMAND3r(pc->unit, pc, 0x0c04, *  0xffff)); */

  /* enable micro_controller 0xffc2 */
  /*
    SOC_IF_ERROR_RETURN (MODIFY_WC40_UC_COMMANDr(pc->unit, pc,
                UC_COMMAND_MDIO_UC_RESET_N_BITS << UC_COMMAND_MDIO_UC_RESET_N_SHIFT,
                UC_COMMAND_MDIO_UC_RESET_N_MASK));
  */
  /* set cl73 nonce set 0xc340 */
  data = cl73_nonce_match_over << CL73_UCTRL1_CL73_NONCE_MATCH_OVER_SHIFT |
           cl73_nonce_match_val  << CL73_UCTRL1_CL73_NONCE_MATCH_VAL_SHIFT;
  mask = CL73_UCTRL1_CL73_NONCE_MATCH_OVER_MASK |
           CL73_UCTRL1_CL73_NONCE_MATCH_VAL_MASK ;

  SOC_IF_ERROR_RETURN ( MODIFY_WC40_CL73_UCTRL1r(pc->unit, pc, data, mask));

  /* set cl73 base_selector to 802.3 set 0xc185 */
  data = base_selector << AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BASE_ABILITIES_1_BASE_SELECTOR_SHIFT;
  mask = AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BASE_ABILITIES_1_BASE_SELECTOR_MASK;

  SOC_IF_ERROR_RETURN ( MODIFY_WC40_AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BASE_ABILITIES_1r(pc->unit, pc, data, mask));

  /* 0x924a for PHY_892 */
  if((no_an==0)&&(pc->port_type==TSCMOD_SINGLE_PORT)) {
     /* data = 0x1a0a ; */ /* 100ms TX_RESET_TIMER_PERIOD */
     data = 0xd05 ;  /* 50ms */
  } else {
     data = 0x5 ;
  }
  SOC_IF_ERROR_RETURN
     (TSC_REG_WRITE(pc->unit, pc, 0x00, 0x0000924a, data)) ;

  /* set an enable 0xc180 */
  data = cl37_bam_enable << AN_X4_ABILITIES_ENABLES_CL37_BAM_ENABLE_SHIFT |
         cl73_bam_enable << AN_X4_ABILITIES_ENABLES_CL73_BAM_ENABLE_SHIFT |
         cl73_hpam_enable << AN_X4_ABILITIES_ENABLES_CL73_HPAM_ENABLE_SHIFT |
         cl73_enable << AN_X4_ABILITIES_ENABLES_CL73_ENABLE_SHIFT |
         cl37_sgmii_enable << AN_X4_ABILITIES_ENABLES_CL37_SGMII_ENABLE_SHIFT |
         cl37_enable << AN_X4_ABILITIES_ENABLES_CL37_ENABLE_SHIFT |
         hpam_autodetect_cl73 << AN_X4_ABILITIES_ENABLES_HPAM_TO_CL37_AUTO_ENABLE_SHIFT |
         cl37_restart << AN_X4_ABILITIES_ENABLES_CL37_AN_RESTART_SHIFT |
         cl73_restart << AN_X4_ABILITIES_ENABLES_CL73_AN_RESTART_SHIFT ;

  mask = AN_X4_ABILITIES_ENABLES_CL37_BAM_ENABLE_MASK |
         AN_X4_ABILITIES_ENABLES_CL73_BAM_ENABLE_MASK |
         AN_X4_ABILITIES_ENABLES_CL73_HPAM_ENABLE_MASK |
         AN_X4_ABILITIES_ENABLES_CL73_ENABLE_MASK |
         AN_X4_ABILITIES_ENABLES_CL37_SGMII_ENABLE_MASK |
         AN_X4_ABILITIES_ENABLES_CL37_ENABLE_MASK |
         AN_X4_ABILITIES_ENABLES_HPAM_TO_CL37_AUTO_ENABLE_MASK |
         AN_X4_ABILITIES_ENABLES_CL37_AN_RESTART_MASK |
         AN_X4_ABILITIES_ENABLES_CL73_AN_RESTART_MASK ;

  SOC_IF_ERROR_RETURN (MODIFY_WC40_AN_X4_ABILITIES_ENABLESr(pc->unit, pc, data, mask));

  /*
  data = cl37_restart << AN_X4_ABILITIES_ENABLES_CL37_AN_RESTART_SHIFT |
         cl73_restart << AN_X4_ABILITIES_ENABLES_CL73_AN_RESTART_SHIFT ;
  */
  mask = AN_X4_ABILITIES_ENABLES_CL37_AN_RESTART_MASK |
         AN_X4_ABILITIES_ENABLES_CL73_AN_RESTART_MASK ;

  /* SDK-50095 */
  SOC_IF_ERROR_RETURN (MODIFY_WC40_AN_X4_ABILITIES_ENABLESr(pc->unit, pc, 0x0, mask));

  if(no_an) {
     /* jir CRTSC-616 */
     data = 1 << AN_X4_ABILITIES_ENABLES_CL73_AN_RESTART_SHIFT ;
     mask = AN_X4_ABILITIES_ENABLES_CL73_AN_RESTART_MASK ;
     SOC_IF_ERROR_RETURN (MODIFY_WC40_AN_X4_ABILITIES_ENABLESr(pc->unit, pc, data, mask));

     data = 0 ;
     SOC_IF_ERROR_RETURN (MODIFY_WC40_AN_X4_ABILITIES_ENABLESr(pc->unit, pc, data, mask));
  }

  if(pc->verbosity&TSCMOD_DBG_AN)
     printf("%-22s: u=%0d p=%0d Autoneg mode(%s) setting cl37_en=%0d cl73_en=%0d d=%0x cl73re=%0d, cl37re=%0d\n",
            FUNCTION_NAME(), pc->unit, pc->port, e2s_tscmod_an_type[pc->an_type], cl37_enable, cl73_enable, data, cl37_restart, cl73_restart);

  return SOC_E_NONE;
} /* tscmod_autoneg_control(tscmod_st* pc) */

/*!
\brief sets the port_mode of TSC

\param  pc handle to current TSC context (#tscmod_st)

\returns The value #SOC_E_NONE upon successful completion.

\details

This function sets the port mode of the TSC. A TSC_4 can be set for sinle lane
lane, dual lane of quad lane mode. This is 3 bit field. 3'b000 sets for logical
lane_0, 2'b001 sets for logical lane_1, 2'b010 sets for logical lane_2, 2'b011
sets for logical lane_3, 2'b100 dual lane with lane_0 & lane_2, 2'b101 sets for
dual lane mode with lane_2 & lane_3, 2'b110 sets for quad lane mode with all 4
lanes.

Note for dual-lane port not using dual-lane bcst port mode, users must set
dxgxs=0 temporally and set per_lane_control to correct lane_select.

This function is controlled by the following register bits:

\li AER_AddressExpansion (0xffde[2:0])

<B>How to call:</B><br>
<p>Call this function directly or through the function #tscmod_tier1_selector()
using the upper case string "SET_AER".</p>
*/

int tscmod_set_aer(tscmod_st* pc)   /* SET_AER */
{

  uint16 data;
  int txcntl ;

  txcntl = 0 ;
  if (pc->per_lane_control == TSCMOD_LANE_BCST) {
      txcntl = 6 ;
  } else if(pc->dxgxs & 2) {
      txcntl = 5 ;
  } else if(pc->dxgxs & 1) {
      txcntl = 4 ;
  } else {
     switch(pc->per_lane_control) {
      case TSCMOD_LANE_0_0_0_1: txcntl = 0 ;   break ;
      case TSCMOD_LANE_0_0_1_0: txcntl = 1 ;   break ;
      case TSCMOD_LANE_0_1_0_0: txcntl = 2 ;   break ;
      case TSCMOD_LANE_1_0_0_0: txcntl = 3 ;   break ;
      default:
          printf("%-22s: FATAL error for lane input=%0d\n", FUNCTION_NAME(), pc->per_lane_control) ;
      }
  }

  data = txcntl << AER_ADDRESSEXPANSION_PORT_MODE_SHIFT;

  SOC_IF_ERROR_RETURN
      (MODIFY_WC40_AER_ADDRESSEXPANSIONr(pc->unit, pc, data,
                                         AER_ADDRESSEXPANSION_PORT_MODE_MASK));

  return SOC_E_NONE;

}

/*!
\brief Control turning on and off the PLL Sequencer.

\param  pc handle to current TSC context (#tscmod_st)

\returns The value #SOC_E_NONE upon successful completion.

\details

This function turns on/off the PLL Sequencer.  To turn the PLL Sequencer on,
set #tscmod_st::per_lane_control to 1 to turn on the sequencer and set it to 0
turn it off. Call this function once for all lanes. Do not call it once per
lane, even in independent mode.

This function is controlled by the following register bit:

\li Main0 (0x9000[7])

<B>How to call:</B><br>
<p>Call this function directly or through the function #tscmod_tier1_selector()
using the upper case string "PLL_SEQUENCER_CONTROL".</p>
*/

int tscmod_pll_sequencer_control(tscmod_st* pc)   /* PLL_SEQUENCER_CONTROL */
{

int cntl = pc -> per_lane_control;

  if (cntl) {
    /* enable pll sequencer */
    SOC_IF_ERROR_RETURN (MODIFY_WC40_MAIN0_SETUPr(pc->unit, pc,
			 MAIN0_SETUP_START_SEQUENCER_MASK,
 			 MAIN0_SETUP_START_SEQUENCER_MASK));
    /*
    SOC_IF_ERROR_RETURN (MODIFY_WC40_MAIN0_SETUPr(pc->unit, pc,
			 0x7a80,
 			 0xffff));
    */
  } else {
    SOC_IF_ERROR_RETURN (MODIFY_WC40_MAIN0_SETUPr(pc->unit, pc, 0,
			 MAIN0_SETUP_START_SEQUENCER_MASK));
  }
  return SOC_E_NONE;
}

/* internal function */
static int _tscmod_getRevDetails(tscmod_st* pc)
{
  uint16 data;
  int revLet, revNum, modelNum; /* needed for vers. and rev. id. */
  pc->model_type = TSCMOD_MODEL_TYPE_ILLEGAL; /* assume non-standard model */
  SOC_IF_ERROR_RETURN(READ_WC40_MAIN0_SERDESIDr(pc->unit,pc,&data));
  revLet = ((data & MAIN0_SERDESID_REV_LETTER_MASK)
               >> MAIN0_SERDESID_REV_LETTER_SHIFT);
  revNum = ((data & MAIN0_SERDESID_REV_NUMBER_MASK)
               >> MAIN0_SERDESID_REV_NUMBER_SHIFT);
  modelNum = ((data & MAIN0_SERDESID_MODEL_NUMBER_MASK)
               >> MAIN0_SERDESID_MODEL_NUMBER_SHIFT);

  if(modelNum == TSCMOD_MODEL_TSC) {
     TSCMOD_MODEL_TYPE_SET(pc, TSCMOD_WC) ;
     TSCMOD_MODEL_LET_SET(pc, revLet) ;
     TSCMOD_MODEL_REV_SET(pc, revNum) ;
     if(revLet == 0) {  /* A0, A1, A2 */
        if (revNum >2) {
           pc->model_type = TSCMOD_MODEL_TYPE_ILLEGAL;
        }
     } else {
        pc->model_type = TSCMOD_MODEL_TYPE_ILLEGAL;
     }
     if(pc->verbosity & TSCMOD_DBG_SUB)
        if(pc->model_type == TSCMOD_MODEL_TYPE_ILLEGAL)
           printf("%-22s: Bad revNum:%d for revLet:%d port=%d\n", FUNCTION_NAME(), revNum, revLet, pc->port);
  }
  pc->accData = data ;
  return SOC_E_NONE;
}

/*!
\brief Reset control of the each lane's analog modules.

\param  pc handle to current TSC context (#tscmod_st)

\returns The value #SOC_E_NONE upon successful completion.

\details

This function controls the reset function of the analog modules of each lanes .
To reset any lane, set #tscmod_st::per_lane_control to 1  and set the lane
specif bit to 0 turn off reset. Lane0 is controlled by (0x9001[12]), lane_1 is
controlled by (0x9001[13]), lane_2 is controlled by (0x9001[14]) and lane_3 is
controlled by (0x9001[15]).

This function is controlled by the following register bit:

\li reset_analog (0x9001[15:12])

<B>How to call:</B><br>
<p>Call this function directly or through the function #tscmod_tier1_selector()
using the upper case string "AFE_RESET_ANALOG".</p>
*/

int tscmod_afe_reset_analog(tscmod_st* pc)   /* AFE_RESET_ANALOG */
{
  uint16 data;
  int rxcntl, txcntl;

  rxcntl  = (pc->per_lane_control & 0xf00) >> 8;
  txcntl  = pc->per_lane_control & 0xf;

  /* reset the lanes as per control */
  data = txcntl << MAIN0_RESET_TX_RESET_ANALOG_SHIFT;
  data |= (rxcntl << MAIN0_RESET_RX_RESET_ANALOG_SHIFT);
  SOC_IF_ERROR_RETURN (MODIFY_WC40_MAIN0_RESETr(pc->unit, pc, data,
                                          (MAIN0_RESET_TX_RESET_ANALOG_MASK |
                                           MAIN0_RESET_RX_RESET_ANALOG_MASK)));
  return SOC_E_NONE;
}


/*!
\brief Controls rx sequencer start from autoneg module

\param  pc handle to current TSC context (#tscmod_st)

\returns The value #SOC_E_NONE upon successful completion.

\details

This function controls the reset of rx sequencer state machine from autoneg
module.
Set #tscmod_st::per_lane_control to 1 to disable rx sequencer start from
autoneg.

This function is controlled by the following register bit:

\li rxSeqStart_AN_disable (0xc21e[11])

<B>How to call:</B><br>
<p>Call this function directly or through the function #tscmod_tier1_selector()
using the upper case string "AFE_RXSEQ_START_CONTROL".</p>
*/

int tscmod_afe_rxseq_start_control(tscmod_st* pc)   /* AFE_RXSEQ_START_CONTROL */
{

   int cntl = pc -> per_lane_control;
   uint16 data ;
    /* reset the lanes as per control */
    if (cntl) {
      data = DSC2B0_DSC_MISC_CTRL0_RXSEQSTART_AN_DISABLE_BITS <<
           DSC2B0_DSC_MISC_CTRL0_RXSEQSTART_AN_DISABLE_SHIFT;
      if (pc->ctrl_type & TSCMOD_CTRL_TYPE_UC_WR_MASK_EN) {
         SOC_IF_ERROR_RETURN
            (tscmod_mask_wr_cmd(pc, TSC40_DSC2B0_DSC_MISC_CTRL0r, data, 
                                DSC2B0_DSC_MISC_CTRL0_RXSEQSTART_AN_DISABLE_MASK)) ;
      } else {
         SOC_IF_ERROR_RETURN (MODIFY_WC40_DSC2B0_DSC_MISC_CTRL0r(pc->unit, pc, data,
                                DSC2B0_DSC_MISC_CTRL0_RXSEQSTART_AN_DISABLE_MASK));
      }
    } else {
       /* DSC2B0_DSC_MISC_CTRL0_RXSEQSTART_AN_DISABLE_ALIGN=0 */
      data = DSC2B0_DSC_MISC_CTRL0_RXSEQSTART_AN_DISABLE_ALIGN <<
           DSC2B0_DSC_MISC_CTRL0_RXSEQSTART_AN_DISABLE_SHIFT;
      if (pc->ctrl_type & TSCMOD_CTRL_TYPE_UC_WR_MASK_EN) {
         SOC_IF_ERROR_RETURN
            (tscmod_mask_wr_cmd(pc, TSC40_DSC2B0_DSC_MISC_CTRL0r, data,
                                DSC2B0_DSC_MISC_CTRL0_RXSEQSTART_AN_DISABLE_MASK)) ;
      } else {
         SOC_IF_ERROR_RETURN (MODIFY_WC40_DSC2B0_DSC_MISC_CTRL0r(pc->unit, pc, data,
                                DSC2B0_DSC_MISC_CTRL0_RXSEQSTART_AN_DISABLE_MASK));
      }
    }
    return SOC_E_NONE;
}


/*!
\brief Speeds up DSC and VGA funnctions for simulation purpose only

\param  pc handle to current TSC context (#tscmod_st)

\returns The value #SOC_E_NONE upon successful completion.

\details

This function speeds up DSC and VGA functions for any speed mode. Please don't
call this function in normal operatio.
Set #tscmod_st::per_lane_control to 1 for speed up.

This function is controlled by the following register bit:

\li acq2_timeout, acq1_timeout, acqcdr_timeout (0xc210) -> 0x0421
\li acqvga_timeout (0xc211) -> 0x0800

<B>How to call:</B><br>
<p>Call this function directly or through the function #tscmod_tier1_selector()
using the upper case string "AFE_SPEED_UP_DSC_VGA".</p>
*/

int tscmod_afe_speed_up_dsc_vga(tscmod_st* pc)   /* AFE_SPEED_UP_DSC_VGA */
{

int cntl = pc -> per_lane_control;
 uint16 data;

    /* reset the lanes as per control */
    if (cntl) {
      data = 0x0421; /* fored for speed_up 0xc210 */
      SOC_IF_ERROR_RETURN (MODIFY_WC40_DSC2B0_ACQ_SM_CTRL0r(pc->unit, pc, data, 0xffff));
      data = 0x0800; /* fored for speed_up 0xc211 */
      SOC_IF_ERROR_RETURN (MODIFY_WC40_DSC2B0_ACQ_SM_CTRL1r(pc->unit, pc, data, 0xffff));
    } else {
      data = 0x29a6;  /* default value */
      SOC_IF_ERROR_RETURN (MODIFY_WC40_DSC2B0_ACQ_SM_CTRL0r(pc->unit, pc, data, 0xffff));
      data = 0x3800;  /* default value */
      SOC_IF_ERROR_RETURN (MODIFY_WC40_DSC2B0_ACQ_SM_CTRL1r(pc->unit, pc, data, 0xffff));
    }
    return SOC_E_NONE;
}

/* internal supporting function */
static int tscmod_get_plldiv(tscmod_st* pc)
{
   int plldiv_vec, plldiv_r_val ;
   uint16 data ;
   /* read back plldiv */
   SOC_IF_ERROR_RETURN (READ_WC40_MAIN0_SETUPr(pc ->unit, pc, &data)) ;

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
   return plldiv_r_val ;
}

/* to update port_mode_select value.  If the update warrants a pll reset,
   then return accData=1, otherwise return accData=0.  The update shall
   support flex port technology. Called forced speed modes */
static int tscmod_update_port_mode_select(tscmod_st* pc)
{
   int port_mode_sel, port_mode_sel_reg, accData ;
   uint16 data, single_port_mode;
   uint16 mask;

   port_mode_sel = 0 ;

   SOC_IF_ERROR_RETURN (READ_WC40_MAIN0_SETUPr(pc ->unit, pc, &data));
   port_mode_sel_reg = (data & MAIN0_SETUP_PORT_MODE_SEL_MASK ) >>
                               MAIN0_SETUP_PORT_MODE_SEL_SHIFT;

   single_port_mode = (data & MAIN0_SETUP_SINGLE_PORT_MODE_MASK) >>
                              MAIN0_SETUP_SINGLE_PORT_MODE_SHIFT ;

   if((pc->port_type ==TSCMOD_MULTI_PORT)||(pc->port_type ==TSCMOD_DXGXS)||
      (pc->port_type ==TSCMOD_SINGLE_PORT)){
   } else {
      printf("Error: u=%0d p=%0d update port mode port_type=%0d undefined\n", pc->unit, pc->port, pc->port_type) ;
   }
   accData   = 0 ;
   switch(port_mode_sel_reg) {
   case 0: /* 0, 1, 2, 3 */
      switch(pc->port_type) {
      case TSCMOD_MULTI_PORT:   port_mode_sel = 0;  break ;
      case TSCMOD_DXGXS:
          if(pc->spd_intf==TSCMOD_SPD_10000_XFI) {
              port_mode_sel = 0;
          } else {
              port_mode_sel = (pc->this_lane)?1:2;
          }
          break ;
      case TSCMOD_SINGLE_PORT:
          if(pc->spd_intf==TSCMOD_SPD_10000_XFI) {
              port_mode_sel = 0;
          } else if(pc->spd_intf==TSCMOD_SPD_20G_MLD_DXGXS) {
              port_mode_sel = (pc->this_lane)?1:2;
          } else {
              port_mode_sel = 4;  accData =1 ; 
          }
          break ;
      default: break ;
      }
      break ;
   case 1: /* 0, 1, {2,3} */
      switch(pc->port_type) {
      case TSCMOD_MULTI_PORT:   port_mode_sel = (pc->this_lane>=2)?0:1; break ;
      case TSCMOD_DXGXS:
          if(pc->spd_intf==TSCMOD_SPD_10000_XFI) {
              port_mode_sel = (pc->this_lane>=2)?0:1;
          } else {
              port_mode_sel = (pc->this_lane==2)?1:3;
          }
          break ;
      case TSCMOD_SINGLE_PORT:  
          if(pc->spd_intf==TSCMOD_SPD_10000_XFI) {
              port_mode_sel = 1;
          } else if(pc->spd_intf==TSCMOD_SPD_20G_MLD_DXGXS) {
              port_mode_sel = 3;
          } else { 
              port_mode_sel = 4;  accData =1 ;
          } 
          break ;
      default: break ;
      }
      break ;
   case 2: /* {0,1}, 2, 3 */
      switch(pc->port_type) {
      case TSCMOD_MULTI_PORT:   port_mode_sel = (pc->this_lane>=2)?2:0; break ;
      case TSCMOD_DXGXS:
          if(pc->spd_intf==TSCMOD_SPD_10000_XFI) {
              port_mode_sel = (pc->this_lane>=2)?2:0; 
          } else {
              port_mode_sel = (pc->this_lane==2)?3:2;
          }
          break ;
      case TSCMOD_SINGLE_PORT:
          if(pc->spd_intf==TSCMOD_SPD_10000_XFI) {
              port_mode_sel = 0;
          } else if(pc->spd_intf==TSCMOD_SPD_20G_MLD_DXGXS) {
              port_mode_sel = 2;
          } else {
              port_mode_sel = 4;  accData =1;
          }
          break ;
      default: break ;
      }
      break ;
   case 3:  /* {0,1}, {2,3} */
      switch(pc->port_type) {
      case TSCMOD_MULTI_PORT:   port_mode_sel = (pc->this_lane>=2)?2:1; break ;
      case TSCMOD_DXGXS:
          if(pc->spd_intf==TSCMOD_SPD_10000_XFI) {
              port_mode_sel = (pc->this_lane>=2)?2:1;
          } else {
              port_mode_sel = 3;
          }
          break ;
      case TSCMOD_SINGLE_PORT:
          if(pc->spd_intf==TSCMOD_SPD_10000_XFI) {
              port_mode_sel = 1;
          } else if(pc->spd_intf==TSCMOD_SPD_20G_MLD_DXGXS) {
              port_mode_sel = 3;
          } else {
              port_mode_sel = 4;  accData =1; 
          } 
          break ;
      default: break ;
      }
      break ;
   case 4: /* {0,1,2,3} */
      accData =1 ;
      switch(pc->port_type) {
      case TSCMOD_MULTI_PORT:   port_mode_sel = 0;  break ;
      case TSCMOD_DXGXS:
          if(pc->spd_intf==TSCMOD_SPD_10000_XFI) {
              port_mode_sel = 0;
          } else {
              port_mode_sel = (pc->this_lane==2)?1:2;
          }
          break ;
      case TSCMOD_SINGLE_PORT:
          if(pc->spd_intf==TSCMOD_SPD_10000_XFI) {
              port_mode_sel = 0;
          } else if(pc->spd_intf==TSCMOD_SPD_20G_MLD_DXGXS) {
              port_mode_sel = 2;
          } else {
              port_mode_sel = 4;  accData =0;
          }
          break ;
      default: break ;
      }
      break ;
   default:
      printf("Error: u=%0d p=%0d port_mode_sel=%0d undefined\n", pc->unit, pc->port, port_mode_sel_reg);
      break ;
   }
   if(single_port_mode) {
      accData          = 1;
      single_port_mode = 0;
   }
   if(pc->verbosity & (TSCMOD_DBG_INIT|TSCMOD_DBG_SPD))
      printf("%-22s u=%0d p=%0d port_mode_sel old=%0d new=%0d accData=%0d\n", FUNCTION_NAME(),
             pc->unit, pc->port, port_mode_sel_reg, port_mode_sel, accData) ;

   data = port_mode_sel << MAIN0_SETUP_PORT_MODE_SEL_SHIFT |
          single_port_mode << MAIN0_SETUP_SINGLE_PORT_MODE_SHIFT ;
   mask = MAIN0_SETUP_PORT_MODE_SEL_MASK | MAIN0_SETUP_SINGLE_PORT_MODE_MASK ;
   SOC_IF_ERROR_RETURN (MODIFY_WC40_MAIN0_SETUPr(pc -> unit, pc, data, mask));

   pc->accData = accData ;
   return SOC_E_NONE ;
}

/*!
\brief Init routine sets various operating modes of TSC.

\param  pc handle to current TSC context (#tscmod_st)

\returns #SOC_E_NONE if successful. SOC_E_ERROR else.

\details

This function is called once per TSC. It cannot be called multiple times
and should be called immediately after reset. Elements of #tscmod_st should be
initialized to required values prior to calling this function. The following
sub-configurations are performed here.

\li Read the revision Id.
\li Set reference clock (0x9000[15:13])
\li Set pll divider for VCO setting (0x9000[12, 11:8]) . pll divider is calculated from max_speed.
\li Port mode select single/dual/quad combo lane mode (#tscmod_st:::port_type)
\li PMA/PMD mux/demux (lane swap) (0x9004[15:0])
\li Load Firmware. (In MDK/SDK this is done externally. The platform provides a
method to load firmware. TSCMod cannot load firmware via MDIO.)

<B>How to call:</B><br>
<p>Call this function directly or through the function
#tscmod_tier1_selector() with the upper-case string "SET_PORT_MODE".</p>
*/

int tscmod_set_port_mode(tscmod_st* pc)           /* SET_PORT_MODE */
{
  uint16 data;
  uint16 mask;
  uint16 dataref;
  uint16 datapll;

  tscmod_update_port_mode_select(pc) ;

  switch(pc->refclk) {
    case 25 : dataref=MAIN0_SETUP_REFCLK_SEL_clk_25MHz;     break;
    case 50 : dataref=MAIN0_SETUP_REFCLK_SEL_clk_50Mhz;     break;
    case 100: dataref=MAIN0_SETUP_REFCLK_SEL_clk_100MHz;    break;
    case 106: dataref=MAIN0_SETUP_REFCLK_SEL_clk_106p25Mhz; break;
    case 125: dataref=MAIN0_SETUP_REFCLK_SEL_clk_125MHz;    break;
    case 156: dataref=MAIN0_SETUP_REFCLK_SEL_clk_156p25MHz; break;
    case 161: dataref=MAIN0_SETUP_REFCLK_SEL_clk_161p25Mhz; break;
    case 187: dataref=MAIN0_SETUP_REFCLK_SEL_clk_187p5MHz;  break;
    default : dataref=MAIN0_SETUP_REFCLK_SEL_clk_156p25MHz;
  }

  mask = MAIN0_SETUP_REFCLK_SEL_MASK;
  data = dataref << MAIN0_SETUP_REFCLK_SEL_SHIFT;
  if( pc->verbosity &(TSCMOD_DBG_INIT|TSCMOD_DBG_SPD)) printf("%-22s: refclk:%d data:%x/%x\n", FUNCTION_NAME(), pc->refclk, dataref, data);

  switch(pc->plldiv) {
    case 32 : datapll=MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div32;  break;
    case 36 : datapll=MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div36;  break;
    case 40 : datapll=MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div40;  break;
    case 42 : datapll=MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div42;  break;
    case 48 : datapll=MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div48;  break;
    case 50 : datapll=MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div50;  break;
    case 52 : datapll=MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div52;  break;
    case 54 : datapll=MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div54;  break;
    case 60 : datapll=MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div60;  break;
    case 64 : datapll=MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div64;  break;
    case 66 : datapll=MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div66;  break;
    case 68 : datapll=MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div68;  break;
    case 70 : datapll=MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div70;  break;
    case 80 : datapll=MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div80;  break;
    case 92 : datapll=MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div92;  break;
    case 100: datapll=MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div100; break;
    default : datapll=MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div66;
  }

  data = data |     MAIN0_SETUP_FORCE_PLL_MODE_AFE_SEL_MASK |
         datapll << MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_SHIFT ;

  mask = MAIN0_SETUP_REFCLK_SEL_MASK |
         MAIN0_SETUP_FORCE_PLL_MODE_AFE_SEL_MASK |
         MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_MASK  ;

  if( pc->verbosity  &(TSCMOD_DBG_INIT|TSCMOD_DBG_SPD))
     printf("%-22s: plldiv:%d data:%x\n", FUNCTION_NAME(), pc->plldiv, data);

  /*
    SOC_IF_ERROR_RETURN (WRITE_WC40_MAIN0_SETUPr(pc -> unit, pc, data));
    SOC_IF_ERROR_RETURN (READ_WC40_MAIN0_SETUPr(pc -> unit, pc, &data));
  */

  if( pc->verbosity &(TSCMOD_DBG_INIT|TSCMOD_DBG_SPD))
     printf("%-22s: main0_setup=%x mask=%0x\n", FUNCTION_NAME(), data, mask);

  /* 0x9000 */
  SOC_IF_ERROR_RETURN (MODIFY_WC40_MAIN0_SETUPr(pc -> unit, pc, data, mask));

  return SOC_E_NONE;
} /* tscmod_set_port_mode(tscmod_st* pc) */

/*!
\brief Sets the core for autoneg

\param  pc handle to current TSC context (#tscmod_st)

\returns The value #SOC_E_NONE upon successful completion

This function sets the tsc core for autoneg generic settings.
If per_lane_control = TSCMOD_CTL_AN_CHIP_INIT (0x1), then the PLL and RX path are reset.
If per_lane_control = TSCMOD_CTL_AN_MODE_INIT (0x2), then the PLL is not reset.
If per_lane_control = TSCMOD_CTL_AN_MODE_SHARE(0x3), then the PLL is not reset.

<B>How to call:</B><br>
<p>Call this function directly or through the function #tscmod_tier1_selector() using the
case-sensitive string "SET_AN_PORT_MODE".</p>

*/
int tscmod_set_an_port_mode(tscmod_st* pc)    /* SET_AN_PORT_MODE */
{

  int plldiv, plldiv_r_val;
   uint16 data, mask;
#if defined (_DV_TRIDENT2)
  uint16 ignore_link_timer_period, tick_denominator, tick_numerator_lower ;
  uint16 tick_override, tick_numerator_upper ;
  uint16 tx_disable_timer_period, cl73_error_timer_period ;
  uint16 cl37_restart_timer_period, cl37_ack_timer_period, cl73_link_up_timer_period ;
  uint16 link_fail_inhibit_no_cl72_period  ;
#endif
  uint16 cl82_dswin, cl48_dswin64b66b, cl48_dswin8b10b;
  uint16 mld_swap_count, am_timer_init_rx_val;
  uint16 single_port_mode;

  data                          = 0 ;

#if defined (_DV_TRIDENT2)
  ignore_link_timer_period      = 0xf;
  tick_denominator              = 0x1;
  tick_numerator_lower          = 0x0;
  tick_override                 = 0x1;
  tick_numerator_upper          = 0x8;
  tx_disable_timer_period       = 0x5;
  cl73_error_timer_period       = 0x64;
  cl37_restart_timer_period     = 0x1f;
  cl37_ack_timer_period         = 0x1f;
  cl73_link_up_timer_period     = 0x0;
  link_fail_inhibit_no_cl72_period = 64;
#endif
  cl82_dswin                    = 0x1e;
  cl48_dswin64b66b              = 0x1;
  cl48_dswin8b10b               = 0x7;
  mld_swap_count                = 0xfffc;
  am_timer_init_rx_val          = 0x3fff;
  single_port_mode              = 0x1;  


  if ((pc->dxgxs & 0xc) == 0x4) {
      single_port_mode = 0x0;
  } else if ((pc->dxgxs & 0xc) == 0x8) {
      single_port_mode = 0x0;
  } else if (pc->port_type == TSCMOD_MULTI_PORT) {
      single_port_mode = 0x0;
  } else if (pc->port_type == TSCMOD_SINGLE_PORT) {
      single_port_mode = 0x1;
  } else if (pc->port_type == TSCMOD_DXGXS) {
      single_port_mode = 0x0;
  } else printf("port mode not defined\n ");

  if(pc->verbosity&(TSCMOD_DBG_AN|TSCMOD_DBG_INIT))
     printf("%-22s: u=%0d p=%0d AN(%s) setting per_lane=%x lane_sel=%x \
this_lane=%0d dxgxs=%0d\n", FUNCTION_NAME(), pc->unit, pc->port,
            e2s_tscmod_an_type[pc->an_type], pc->per_lane_control, pc->lane_select,
            pc->this_lane, pc->dxgxs);

  if ((pc-> an_type == TSCMOD_HPAM) | (pc-> an_type == TSCMOD_CL73_BAM)) {
#if defined (_DV_TRIDENT2)
    cl73_error_timer_period       = 0x64;
#endif
    cl82_dswin                    = 0x1e;
  }

  if ((pc-> an_type == TSCMOD_CL37)| (pc-> an_type == TSCMOD_CL37_10G) | (pc-> an_type == TSCMOD_CL37_BAM)) {
#if defined (_DV_TRIDENT2)
    tick_numerator_upper          = 0x1;
    cl73_link_up_timer_period     = 0x0;
#endif
  } else if (pc-> an_type == TSCMOD_CL37_SGMII) {
    single_port_mode              = 0x0;
#if defined (_DV_TRIDENT2)
    tick_numerator_upper          = 0x1;
    cl73_link_up_timer_period     = 0x0;
#endif
  }

  if(pc->an_type == TSCMOD_CL37_10G) {
     single_port_mode = 0 ;
  }

  if ((pc->an_type == TSCMOD_CL73) | (pc->an_type == TSCMOD_HPAM) |
      (pc->an_type == TSCMOD_CL73_BAM)||(pc->an_type ==TSCMOD_CL37_10G) ) {
      if(pc->an_hg_vco) {
          data = MAIN0_SETUP_REFCLK_SEL_clk_156p25MHz <<
                          MAIN0_SETUP_REFCLK_SEL_SHIFT|
                 MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div70 <<
                          MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_SHIFT |
                 MAIN0_SETUP_FORCE_PLL_MODE_AFE_SEL_MASK |
                 single_port_mode 
                        << MAIN0_SETUP_SINGLE_PORT_MODE_SHIFT ;
          plldiv = 70 ;
      } else {
          data = MAIN0_SETUP_REFCLK_SEL_clk_156p25MHz <<
                           MAIN0_SETUP_REFCLK_SEL_SHIFT|
                 MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div66 << 
                           MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_SHIFT |
                 single_port_mode 
                        << MAIN0_SETUP_SINGLE_PORT_MODE_SHIFT ;
          plldiv = 66 ;
      }
  } else if ((pc-> an_type == TSCMOD_CL37_SGMII)|(pc-> an_type == TSCMOD_CL37)|
             (pc-> an_type == TSCMOD_CL37_BAM)) {
      if(pc->an_hg_vco) {
          data = MAIN0_SETUP_REFCLK_SEL_clk_156p25MHz <<
                     MAIN0_SETUP_REFCLK_SEL_SHIFT |
                 MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div70 <<
                     MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_SHIFT |
                 MAIN0_SETUP_FORCE_PLL_MODE_AFE_SEL_MASK |
                 single_port_mode << 
                     MAIN0_SETUP_SINGLE_PORT_MODE_SHIFT;
          plldiv = 70 ;
      } else {
          data = MAIN0_SETUP_REFCLK_SEL_clk_156p25MHz <<
                     MAIN0_SETUP_REFCLK_SEL_SHIFT |
                 MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_div40 <<
                     MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_SHIFT |
                single_port_mode << 
                     MAIN0_SETUP_SINGLE_PORT_MODE_SHIFT;
          plldiv = 40 ;
      }
  } else {
     plldiv = 40 ;
     if(!(pc->ctrl_type&TSCMOD_CTRL_TYPE_ERR)||(pc->verbosity&TSCMOD_DBG_PRINT))
        printf("Error: AN port mode u=%0d p=%0d should not be called for invlaid cfg\n",
               pc->unit, pc->port) ;
     pc->err_code  |= TSCMOD_ERR_AN_BAD_CFG ;
     pc->ctrl_type |= TSCMOD_CTRL_TYPE_ERR ;
  }

  if(pc->per_lane_control == TSCMOD_CTL_AN_CHIP_INIT) {
     pc->plldiv = plldiv ;
  } else {
     plldiv_r_val = tscmod_get_plldiv(pc) ;
     if(plldiv_r_val != plldiv) {
        if(pc->verbosity&TSCMOD_DBG_PRINT) {
           printf("Error: %-22s u=%0d p=%0d plldiv (reg=%0d exp=%0d) is not \
consistent for all ports\n", FUNCTION_NAME(), pc->unit, pc->port, plldiv_r_val, plldiv);
        } else {
           if(!(pc->ctrl_type&TSCMOD_CTRL_TYPE_ERR)) {
              printf("Error: AN mode u=%0d p=%0d plldiv (reg=%0d exp=%0d) is not \
consistent for all ports\n", pc->unit, pc->port, plldiv_r_val, plldiv);
           }
        }
        pc->err_code  |= TSCMOD_ERR_AN_PLL_DIFF ;
        pc->ctrl_type |= TSCMOD_CTRL_TYPE_ERR ;
     }
     pc->plldiv = plldiv_r_val ;
  }
  if(pc->per_lane_control == TSCMOD_CTL_AN_CHIP_INIT) {
     mask =  MAIN0_SETUP_REFCLK_SEL_MASK | MAIN0_SETUP_FORCE_PLL_MODE_AFE_SEL_MASK |
             MAIN0_SETUP_DEFAULT_PLL_MODE_AFE_MASK | MAIN0_SETUP_SINGLE_PORT_MODE_MASK ;
  } else if(pc->per_lane_control ==TSCMOD_CTL_AN_MODE_SHARE){
     mask = MAIN0_SETUP_FORCE_PLL_MODE_AFE_SEL_MASK |  /* remove this bit from forced speed mode */
            MAIN0_SETUP_SINGLE_PORT_MODE_MASK ;
  } else {
     mask = MAIN0_SETUP_SINGLE_PORT_MODE_MASK ;
  }
  mask |= MAIN0_SETUP_PORT_MODE_SEL_MASK ;  /* always 4-port mode: need to review for dual port mode */
  /* 0x9000 */
  SOC_IF_ERROR_RETURN (MODIFY_WC40_MAIN0_SETUPr(pc -> unit, pc, data, mask));

  /* XXXX: need to do sleep sal_usleep(1000);  */

#if defined (_DV_TRIDENT2)
  /* ignore_link_timer_period 0x925c  */
  data = ignore_link_timer_period << AN_X1_TIMERS_IGNORE_LINK_TIMER_IGNORE_LINK_TIMER_PERIOD_SHIFT;
  mask = AN_X1_TIMERS_IGNORE_LINK_TIMER_IGNORE_LINK_TIMER_PERIOD_MASK;

  SOC_IF_ERROR_RETURN ( MODIFY_WC40_AN_X1_TIMERS_IGNORE_LINK_TIMERr(pc->unit, pc, data, mask));
#endif


#if defined (_DV_TRIDENT2)
  /* set tick values 0x9008 x1 (only in sim) */
  data = tick_denominator << MAIN0_TICK_CONTROL_0_TICK_DENOMINATOR_SHIFT |
     tick_numerator_lower << MAIN0_TICK_CONTROL_0_TICK_NUMERATOR_LOWER_SHIFT;
  mask = MAIN0_TICK_CONTROL_0_TICK_DENOMINATOR_MASK |
         MAIN0_TICK_CONTROL_0_TICK_NUMERATOR_LOWER_MASK;

  SOC_IF_ERROR_RETURN ( MODIFY_WC40_MAIN0_TICK_CONTROL_0r(pc->unit, pc, data, mask));

  /* set tick values 0x9007 (only in sim) */
  data = tick_override << MAIN0_TICK_CONTROL_1_TICK_OVERRIDE_SHIFT |
         tick_numerator_upper << MAIN0_TICK_CONTROL_1_TICK_NUMERATOR_UPPER_SHIFT;
  mask = MAIN0_TICK_CONTROL_1_TICK_OVERRIDE_MASK |
         MAIN0_TICK_CONTROL_1_TICK_NUMERATOR_UPPER_MASK;

  SOC_IF_ERROR_RETURN ( MODIFY_WC40_MAIN0_TICK_CONTROL_1r(pc->unit, pc, data, mask));

  /* set cl73 cl73_link_up_timer_period 0x9256 (only in sim) need a production value */
  data = cl73_link_up_timer_period << AN_X1_TIMERS_LINK_UP_CL73_LINK_UP_TIMER_PERIOD_SHIFT ;
  mask = AN_X1_TIMERS_LINK_UP_CL73_LINK_UP_TIMER_PERIOD_MASK;
  SOC_IF_ERROR_RETURN ( MODIFY_WC40_AN_X1_TIMERS_LINK_UPr(pc->unit, pc, data, mask));

  /* set cl73 break link timer values 0x9253 (only in sim) need a production value */
  data = tx_disable_timer_period << AN_X1_TIMERS_CL73_BREAK_LINK_TX_DISABLE_TIMER_PERIOD_SHIFT ;
  mask = AN_X1_TIMERS_CL73_BREAK_LINK_TX_DISABLE_TIMER_PERIOD_MASK;
  SOC_IF_ERROR_RETURN ( MODIFY_WC40_AN_X1_TIMERS_CL73_BREAK_LINKr(pc->unit, pc, data, mask));

  /* set cl37 autoneg restart timer 0x9250 write once per core (production ?) */
  data = cl37_restart_timer_period << AN_X1_TIMERS_CL37_RESTART_CL37_RESTART_TIMER_PERIOD_SHIFT ;
  mask = AN_X1_TIMERS_CL37_RESTART_CL37_RESTART_TIMER_PERIOD_MASK;
  SOC_IF_ERROR_RETURN ( MODIFY_WC40_AN_X1_TIMERS_CL37_RESTARTr(pc->unit, pc, data, mask));


  /* set cl37 autoneg autoneg complete-acknowledge timer 0x9251 (production ?) */
  data = cl37_ack_timer_period << AN_X1_TIMERS_CL37_ACK_CL37_ACK_TIMER_PERIOD_SHIFT ;
  mask = AN_X1_TIMERS_CL37_ACK_CL37_ACK_TIMER_PERIOD_MASK;
  SOC_IF_ERROR_RETURN ( MODIFY_WC40_AN_X1_TIMERS_CL37_ACKr(pc->unit, pc, data, mask));

  /* set cl73 error timer period values 0x9254 write once per core (timeout timer) */
  data = cl73_error_timer_period << AN_X1_TIMERS_CL73_ERROR_CL73_ERROR_TIMER_PERIOD_SHIFT ;
  mask = AN_X1_TIMERS_CL73_ERROR_CL73_ERROR_TIMER_PERIOD_MASK;

  SOC_IF_ERROR_RETURN ( MODIFY_WC40_AN_X1_TIMERS_CL73_ERRORr(pc->unit, pc, data, mask));

  /* 0x9258 */
  data = link_fail_inhibit_no_cl72_period ;
  mask = AN_X1_TIMERS_LINK_FAIL_INHIBIT_TIMER_NOT_CL72_LINK_FAIL_INHIBIT_TIMER_NCL72_PERIOD_MASK ;

  SOC_IF_ERROR_RETURN
     (WRITE_WC40_AN_X1_TIMERS_LINK_FAIL_INHIBIT_TIMER_NOT_CL72r
      (pc->unit, pc, data));
#endif

  /* --Addr=0x9222 ------------------------- */
  data = cl82_dswin << RX_X1_CONTROL0_DESKEW_WINDOWS_CL82_DSWIN_SHIFT |
         cl48_dswin64b66b
                    << RX_X1_CONTROL0_DESKEW_WINDOWS_CL48_DSWIN64B66B_SHIFT |
         cl48_dswin8b10b
                    << RX_X1_CONTROL0_DESKEW_WINDOWS_CL48_DSWIN8B10B_SHIFT ;
  mask = RX_X1_CONTROL0_DESKEW_WINDOWS_CL82_DSWIN_MASK |
     RX_X1_CONTROL0_DESKEW_WINDOWS_CL48_DSWIN64B66B_MASK |
     RX_X1_CONTROL0_DESKEW_WINDOWS_CL48_DSWIN8B10B_MASK ;
  SOC_IF_ERROR_RETURN
     (MODIFY_WC40_RX_X1_CONTROL0_DESKEW_WINDOWSr(pc->unit, pc, data, mask));

   /* MLD swap_count 0xa000 */

#if defined (_DV_TRIDENT2)
  mld_swap_count = 0xffc ;
#else
  mld_swap_count = 0xfffc ;
#endif

  data = mld_swap_count << TX_X2_CONTROL0_MLD_SWAP_COUNT_MLD_SWAP_COUNT_SHIFT ;
  mask = TX_X2_CONTROL0_MLD_SWAP_COUNT_MLD_SWAP_COUNT_MASK ;

  SOC_IF_ERROR_RETURN
     (MODIFY_WC40_TX_X2_CONTROL0_MLD_SWAP_COUNTr(pc->unit, pc, data, mask));

  /* cl82 shared rc_am_timer  0x9123 */

#if defined (_DV_TRIDENT2)
  am_timer_init_rx_val = 0x3ff ;
#else
  am_timer_init_rx_val = 0x3fff ;
#endif

  data = am_timer_init_rx_val << CL82_SHARED_CL82_RX_AM_TIMER_AM_TIMER_INIT_RX_VAL_SHIFT ;
  mask = CL82_SHARED_CL82_RX_AM_TIMER_AM_TIMER_INIT_RX_VAL_MASK ;

  SOC_IF_ERROR_RETURN
     (MODIFY_WC40_CL82_SHARED_CL82_RX_AM_TIMERr(pc->unit, pc, data, mask));

  if(0) {
      /* cl72_link_control=1 0xc253 */
      data = 1 << CL72_MISC1_CONTROL_LINK_CONTROL_FORCE_SHIFT ;
      mask =      CL72_MISC1_CONTROL_LINK_CONTROL_FORCE_MASK ;
      if (pc->ctrl_type & TSCMOD_CTRL_TYPE_UC_WR_MASK_EN) {
         SOC_IF_ERROR_RETURN
            (tscmod_mask_wr_cmd(pc, TSC40_CL72_MISC1_CONTROLr, data, mask)) ;
      } else {
         SOC_IF_ERROR_RETURN
            (MODIFY_WC40_CL72_MISC1_CONTROLr(pc->unit, pc, data, mask));
      }
  }

  if(pc->per_lane_control != TSCMOD_CTL_AN_MODE_SHARE) {
     /* disable ecc of uC 0xffcc */
     SOC_IF_ERROR_RETURN (MODIFY_WC40_UC_COMMAND3r(pc->unit, pc, 0x0c04, 0xffff));

     /* enable micro_controller 0xffc2 */
     SOC_IF_ERROR_RETURN
        (MODIFY_WC40_UC_COMMANDr(pc->unit, pc,
                                 UC_COMMAND_MDIO_UC_RESET_N_BITS << UC_COMMAND_MDIO_UC_RESET_N_SHIFT,
                                 UC_COMMAND_MDIO_UC_RESET_N_MASK));
  }

  if(pc->verbosity&(TSCMOD_DBG_AN|TSCMOD_DBG_INIT))
     printf("%-22s: u=%0d p=%0d AN(%s) clear credits setting lane_sel=%x this_lane=%0d dxgxs=%0d\n",
            FUNCTION_NAME(), pc->unit, pc->port, e2s_tscmod_an_type[pc->an_type], pc->lane_select, pc->this_lane, pc->dxgxs);

  /* 0xc104 */
  SOC_IF_ERROR_RETURN (MODIFY_WC40_TX_X4_CREDIT0_PCS_CLOCKCNT0r(pc->unit, pc, 0x0, 0xffff));
  /* clear credtis 0xc100, 0xc104  */
  SOC_IF_ERROR_RETURN (MODIFY_WC40_TX_X4_CREDIT0_CREDIT0r(pc->unit, pc, 0x0, 0xffff));
  

  return SOC_E_NONE;
} /* tscmod_set_an_port_mode(tscmod_st* pc) */


/*!
\brief Sets the 1G and 10G parallel detect mode

\param  pc handle to current TSC context (#tscmod_st)

\returns The value #SOC_E_NONE upon successful completion

This function enables or disables parallel detection for a particular lane for
both 1G and 10G modes.  To control the parallel detect mode, set the
bit-encoded #tscmod_st::per_lane_control field as follows

The parallel detect mode is controlled by setting specific bits of
#tscmod_st::per_lane_control. Setting specific bit to 0 turns off the feature
and setting to 1 implies turns it on.

For lane n(this_lane), the 2 bits (0-1) are defined thus.
\li bit 0: controls 1g parallel detect
\li bit 1: controls 10g parallel detect

The register affected :
    AN_X4_ABILITIES_CONTROLSr  0xc188

<B>How to call:</B><br>
<p>Call this function directly or through the function #tscmod_tier1_selector() using the
case-sensitive string "PARALLEL_DETECT_CONTROL".</p>

*/
int tscmod_parallel_detect_control(tscmod_st* pc)    /* PARALLEL_DETECT_CONTROL */
{
   uint16 data, mask;
   data = 0, mask = 0;
   if(pc->per_lane_control & TSCMOD_PDET_CONTROL_1G) {
       mask  |= AN_X4_ABILITIES_CONTROLS_PD_KX_EN_MASK;
       if (pc->per_lane_control & TSCMOD_PDET_CONTROL_MASK) {
           data |= AN_X4_ABILITIES_CONTROLS_PD_KX_EN_MASK; /* 1g */
       } else {
           data |= 0;
       }
   }
   if(pc->per_lane_control & TSCMOD_PDET_CONTROL_10G) {
       mask  |= AN_X4_ABILITIES_CONTROLS_PD_KX4_EN_MASK;
       if (pc->per_lane_control & TSCMOD_PDET_CONTROL_MASK) {
           data |= AN_X4_ABILITIES_CONTROLS_PD_KX4_EN_MASK; /* 10g */
       } else {
           data |= 0;
       }
   }
   if (pc->per_lane_control & TSCMOD_PDET_CONTROL_CMD_MASK) {
       SOC_IF_ERROR_RETURN
          (MODIFY_WC40_AN_X4_ABILITIES_CONTROLSr(pc->unit, pc, data, mask));
   } else {
       data = 0;
       SOC_IF_ERROR_RETURN
          (READ_WC40_AN_X4_ABILITIES_CONTROLSr(pc->unit, pc, &data));
       pc->accData = (data & mask);
   }

   return SOC_E_NONE;
} /* tscmod_parallel_detect_control(tscmod_st* pc) */

/*!
\brief Sets loopback mode from Tx to Rx at PCS/PMS parallel interface. (gloop).

\param  pc handle to current TSC context (#tscmod_st)

\returns The value #SOC_E_NONE upon successful completion

This function sets the TX-to-RX digital loopback mode, which is set
independently for each lane at the PCS/PMS parallel interface.

The 1st, 2nd, 3rd and 4th byte of #tscmod_st::per_lane_control is associated
with lanes 0, 1, 2, and 3 respectively. The bits of each byte control their
lanes as follows.

\li 0:1 : Enable  TX->RX loopback
\li 0:0 : Disable TX-RX loopback

Note that this function can program <B>multiple lanes simultaneously</B>.
As an example, to enable gloop on all lanes and enable TX on lane 0 only,
set #tscmod_st::per_lane_control to 0x01010103

Modified registers :
\li 0x9009 (MAIN0_LOOPBACK_CONTROLr)
\li   bit 7:4 for LOCAL_PCS_LOOPBACK_ENABLE ;

<B>How to call:</B><br>
<p>Call this function directly or through the function
#tscmod_tier1_selector() with the upper-case string
"TX_LOOPBACK_CONTROL".</p>
*/

int tscmod_tx_loopback_control(tscmod_st* pc)           /* TX_LOOPBACK_CONTROL  */
{
   int idx ;  int tmp_sel, tmp_lane, tmp_dxgxs ;  uint16 data, mask ;
   tmp_sel  = pc->lane_select ;
   tmp_lane = pc->this_lane ;
   tmp_dxgxs= pc->dxgxs ;

   pc->dxgxs       = 0 ;  data = 0 ;

   for(idx=0; idx<4; idx++) {
      if( (pc->per_lane_control>> 8*idx) & 0x1 ) {
         data = data | (1<<idx) ;
      }
   }
   if(pc->verbosity & TSCMOD_DBG_PATH)
   printf("%-22s: u=%0d p=%0d data=%x ", FUNCTION_NAME(),
          pc->unit, pc->port, data) ;
   pc->lane_select = getLaneSelect(0) ;
   pc->this_lane   = 0 ;
   data = (data  << MAIN0_LOOPBACK_CONTROL_LOCAL_PCS_LOOPBACK_ENABLE_SHIFT) &
                    MAIN0_LOOPBACK_CONTROL_LOCAL_PCS_LOOPBACK_ENABLE_MASK;
   mask = MAIN0_LOOPBACK_CONTROL_LOCAL_PCS_LOOPBACK_ENABLE_MASK ;

   if(pc->verbosity & TSCMOD_DBG_PATH)
   printf("per_lane_control=%x data=%x\n",
           pc->per_lane_control, data) ;

   SOC_IF_ERROR_RETURN
      (MODIFY_WC40_MAIN0_LOOPBACK_CONTROLr(pc->unit, pc, data, mask));
   pc->lane_select = tmp_sel ;
   pc->this_lane   = tmp_lane ;
   pc->dxgxs       = tmp_dxgxs ;
   return SOC_E_NONE;
}

/*!
\brief Set remote loopback mode for GMII, cl49, aggregate(XAUI) and R2 modes.

\param  pc handle to current TSC context (#tscmod_st)

\returns The value #SOC_E_NONE upon successful completion

This function sets the remote loopback (RX-to-TX) mode for one lane at a time,
where the lane is indicated by the #tscmod_st::this_lane field. To enable remote
loopback, set the field #tscmod_st::en_dis to 1; to disable remote loopback, set
the field #tscmod_st::en_dis to 0.

Note that the speed field tscmod_st::spd_intf must be set prior to calling this
function because the loopback register and path are speed/interface dependent.

The first two bits of #tscmod_st::per_lane_control decide for PCS pr PMD.  After
right shifting the first two bits of #tscmod_st::per_lane_control  the 1st, 2nd,
3rd and 4th byte is associated with lanes 0, 1, 2, and 3 respectively.

The bits of each byte control their
lanes as follows.  !! Also 1st byte decides it is for PCS or PMD remote loopback.

\li 0:0 : Enable PMD Loopback (rloop)
\li 0:1 : Enable PCS Loopback (rloop)
\li 1:1 : Enable loopback
\li 1:0 : Disable loopback


The function modifies the following registers:
\li 0x9009 (MAIN0_LOOPBACK_CONTROLr)
\li   bit 15:12 for REMOTE_PCS_LOOPBACK_ENABLE
\li   bit 11:08 for REMOTE_PMD_LOOPBACK_ENABLE

<B>How to call:</B><br>
<p>Call this function directly or through the function
#tscmod_tier1_selector() with the upper-case string "RX_LOOPBACK_CONTROL".</p>
*/

int tscmod_rx_loopback_control(tscmod_st* pc)
{
   int idx, offset, cnt, p_l_control ;  int tmp_sel, tmp_lane, tmp_dxgxs ;
   uint16 mode, data, dataP, dataN, dataR, mask ;
   tmp_sel  = pc->lane_select ;
   tmp_lane = pc->this_lane ;
   tmp_dxgxs= pc->dxgxs ;

   pc->dxgxs   = 0 ;  dataN =  0 ;
   p_l_control = pc->per_lane_control ;

   SOC_IF_ERROR_RETURN
      (READ_WC40_MAIN0_LOOPBACK_CONTROLr(pc->unit, pc, &dataP));

   if(pc->port_type == TSCMOD_SINGLE_PORT ) {
      cnt = 4 ;  offset = 0 ;
   } else if (pc->port_type == TSCMOD_DXGXS ) {
      if( tmp_dxgxs & 0x2 ) offset = 2 ; else offset = 0 ;
      cnt = 2 ;
   } else {
      cnt = 1 ; offset = tmp_lane;
   }
   mode  = pc->per_lane_control & 0x1;/*mode has to be consistent cross lanes*/
   if (mode) {
      dataP = (dataP&MAIN0_LOOPBACK_CONTROL_REMOTE_PCS_LOOPBACK_ENABLE_MASK)>>
                     MAIN0_LOOPBACK_CONTROL_REMOTE_PCS_LOOPBACK_ENABLE_SHIFT;
   } else {
      dataP = (dataP&MAIN0_LOOPBACK_CONTROL_REMOTE_PMD_LOOPBACK_ENABLE_MASK)>>
                     MAIN0_LOOPBACK_CONTROL_REMOTE_PMD_LOOPBACK_ENABLE_SHIFT;
   }

   for(idx=offset; idx<(cnt+offset); idx++) {
      dataP = dataP & ~(1<<idx) ;
      if( (pc->per_lane_control>>(8*idx+1)) & 0x1 ) {
         dataN = dataN | (1<<idx) ;
      }
   }
   dataR = dataN | dataP ;
   if(mode) {
      data = (dataR<<MAIN0_LOOPBACK_CONTROL_REMOTE_PCS_LOOPBACK_ENABLE_SHIFT)&
                     MAIN0_LOOPBACK_CONTROL_REMOTE_PCS_LOOPBACK_ENABLE_MASK;
      mask =  MAIN0_LOOPBACK_CONTROL_REMOTE_PCS_LOOPBACK_ENABLE_MASK ;
   } else {
      data = (dataR<<MAIN0_LOOPBACK_CONTROL_REMOTE_PMD_LOOPBACK_ENABLE_SHIFT)&
                     MAIN0_LOOPBACK_CONTROL_REMOTE_PMD_LOOPBACK_ENABLE_MASK;
      mask =  MAIN0_LOOPBACK_CONTROL_REMOTE_PMD_LOOPBACK_ENABLE_MASK ;
   }

   if(pc->verbosity & TSCMOD_DBG_PATH)
      printf("%-22s: u=%0d p=%0d rloop ctrl=%x N=%x P=%x R=%x data=%x mask=%x mode=%d(1=PCS;0=PMD)\n",
             FUNCTION_NAME(), pc->unit, pc->port, p_l_control, dataN, dataP, dataR, data, mask, mode);

   SOC_IF_ERROR_RETURN
      (MODIFY_WC40_MAIN0_LOOPBACK_CONTROLr(pc->unit, pc, data, mask));

   if(data & mask) {
      if (mode) {
         data = (data & mask)>>MAIN0_LOOPBACK_CONTROL_REMOTE_PCS_LOOPBACK_ENABLE_SHIFT;
      } else {
         data = (data & mask)>>MAIN0_LOOPBACK_CONTROL_REMOTE_PMD_LOOPBACK_ENABLE_SHIFT;
      }

#if 0
      SOC_IF_ERROR_RETURN     /* 0x9201 */
         (MODIFY_WC40_TX_X1_CONTROL0_TX_PI_CONTROL2r
          (pc->unit, pc, TX_X1_CONTROL0_TX_PI_CONTROL2_TX_PI_LOOPBACK_MODE_MASK,
                         TX_X1_CONTROL0_TX_PI_CONTROL2_TX_PI_LOOPBACK_MODE_MASK));

      for(idx=0;idx<4; idx++) {
         if(data&(1<<idx)) {
            pc->this_lane   = idx ;
            pc->lane_select = getLaneSelect(idx) ;
            SOC_IF_ERROR_RETURN
               (MODIFY_WC40_TX_X1_CONTROL0_TX_PI_CONTROL2r
                (pc->unit, pc,
                 idx<<TX_X1_CONTROL0_TX_PI_CONTROL2_TX_PI_RX_LANE_SEL_SHIFT,
                      TX_X1_CONTROL0_TX_PI_CONTROL2_TX_PI_RX_LANE_SEL_MASK));
            break ;
         }
      }
#endif

      SOC_IF_ERROR_RETURN   /* 0x9200 */
         (MODIFY_WC40_TX_X1_CONTROL0_TX_PI_CONTROL1r
          (pc->unit, pc, TX_X1_CONTROL0_TX_PI_CONTROL1_TX_PI_EN_MASK,
                         TX_X1_CONTROL0_TX_PI_CONTROL1_TX_PI_EN_MASK)) ;

   } else {

#if 0
      SOC_IF_ERROR_RETURN
         (MODIFY_WC40_TX_X1_CONTROL0_TX_PI_CONTROL2r
          (pc->unit, pc, 0,
           TX_X1_CONTROL0_TX_PI_CONTROL2_TX_PI_LOOPBACK_MODE_MASK |
           TX_X1_CONTROL0_TX_PI_CONTROL2_TX_PI_RX_LANE_SEL_MASK));
#endif

      SOC_IF_ERROR_RETURN
         (MODIFY_WC40_TX_X1_CONTROL0_TX_PI_CONTROL1r
          (pc->unit, pc, 0,
           TX_X1_CONTROL0_TX_PI_CONTROL1_TX_PI_EN_MASK)) ;

   }

   pc->lane_select = tmp_sel ;
   pc->this_lane   = tmp_lane ;
   pc->dxgxs       = tmp_dxgxs ;
   return SOC_E_NONE;

}

/*!
\brief <B>Do not directly call this function.</B>  This is an internal
function, which sets the lane number for subsequent MDIO accesses.

\param  pc handle to current TSC context (#tscmod_st)

\details
Each TSC register is addressed by a 32-bit address.  Bits 16 and 17
indicate the particular lane with which the register is associated.  Some
registers have 1 instance for all lanes, while some registers have 1 instance
per lane.  In this later case, then the lane selection bits need to be set in
order to program a register.  This function facilitates programming of
lane-specific registers.  However, currently this function sets bits 18 through
31 to 0 in the process of setting the lane bits.

\returns The value #SOC_E_NONE upon successful completion
*/

int tscmod_aer_lane_select(tscmod_st* pc)         /* AER_LANE_SELECT */
{
  /* fix this */
  return SOC_E_ERROR;
}

/*!
\brief set tx encoder of any particular lane

\param  pc handle to current TSC context (#tscmod_st)

\returns The value #SOC_E_NONE upon successful completion.

This function is used to select encoder of any transmit lane.

\li select encodeMode

This function reads/modifies the following registers:
\li Address (Name)
\li 0xc111 (TX_X4_CONTROL0_ENCODE_0)
\i     field               bit position     variable defined in this function
\li    ENCODEMODE         [4:2]             encode_mode
\li    CL49_BYPASS_TXSM   [9]               -
\li
\li 0xc113 (TX_X4_CONTROL0_MISC)
\li    SCR_MODE           [15:14]           scr_mode
\li    RESERVED0          [13:11]           block_sync_mode
\li    CL49_TX_LI_ENABLE  [8]               -
\li    CL49_TX_LF_ENABLE  [7]               -
\li    CL49_TX_RF_ENABLE  [6]               -
\li
\li 0xa000 (TX_X2_CONTROL0_MLD_SWAP_COUNT)
\li    MLD_SWAP_COUNT     [15:0]            -
\li

<B> ENCODE_SET Prorgamming values
\li 10G_XFI
\li   encodeMode	   :   5
\li   scr_mode	       :   3
\li
\li 10G_XAUI
\li   encode_mode	   : 0x1     cl48 8b10b
\li   CL49_BYPASS_TXSM : 0
\li   scr_mode   	   : 0x0     bypass scramble
\li
\li 40G_MLD
\li
\li
\li

<B>How to call:</B><br>
<p>Call this function directly or through the function
#tscmod_tier1_selector() with the upper-case string "ENCODE_SET".</p>
*/

int tscmod_encode_set(tscmod_st* pc)         /* ENCODE_SET */
{
    uint16 encode_mode, cl49_bypass_txsm, hg2_message_invalid_code_enable, hg2_codec, hg2_en ;
  uint16 scr_mode ;
  uint16 mld_swap_count, mld_swap_en ;
  uint16 disable_packet_misalign;
  uint16 data, mask ;
  int cntl;

  cntl = pc->per_lane_control;
  data = 0 ; mask = 0  ;

  encode_mode       = 0 ;
  cl49_bypass_txsm  = 0 ;
  hg2_message_invalid_code_enable = 0 ;
  hg2_en            = 0 ;    /* 001 cl48 8b10b
                                010 cl48 8b10b rxaui
                                011 cl36 8b10b
                                100 cl82 64b66b
                                101 cl49 64b66b
                                110 brcm 64b66b */
  hg2_codec         = 0 ;
  scr_mode          = 0 ;   /* 00 bypass scrambler
                               01 64b66b scrambler (all 66 bits)
                               10 8b10b scrambler (all 80 bits)
                               11 64b66b scrambler (sync bits not scrambled)*/
  mld_swap_count    = 0 ;
  mld_swap_en       = 0 ;
  disable_packet_misalign = 0x0 ;

  if((pc->spd_intf == TSCMOD_SPD_10_SGMII) |
     (pc->spd_intf == TSCMOD_SPD_100_SGMII) |
     (pc->spd_intf == TSCMOD_SPD_1000_SGMII) |
     (pc->spd_intf == TSCMOD_SPD_2500)) {
      encode_mode       = 0x3 ;         /* cl36 8b10b */
      hg2_message_invalid_code_enable = 0x0 ;
      /* ------------------------------------- 0xc114 */
        disable_packet_misalign = 0x1 ;
  } else if((pc->spd_intf == TSCMOD_SPD_10000_XFI)||
            (pc->spd_intf == TSCMOD_SPD_10600_XFI_HG)||
            (pc->spd_intf == TSCMOD_SPD_5000)) {
     /* ------------------------------------ 0xc111 */
     encode_mode       = 0x5;          /* cl49 64b/66b */
     cl49_bypass_txsm  = 0 ;
     hg2_message_invalid_code_enable = 0 ;
     /* ------------------------------------ 0xc113 */
     scr_mode          = 0x3;          /* 64b66b scrambled */
     /* ------------------------------------ 0xa000 */
     mld_swap_en       = 0 ;
  } else if ((pc->spd_intf == TSCMOD_SPD_10000)||
             (pc->spd_intf == TSCMOD_SPD_13000)||
             (pc->spd_intf == TSCMOD_SPD_15000)||
             (pc->spd_intf == TSCMOD_SPD_16000)) {   /* 10G XAUI */
     /* ------------------------------------ 0xc111 */
     encode_mode       = 0x1;         /* cl48 8b10b */
     cl49_bypass_txsm  = 1 ;
     hg2_message_invalid_code_enable = 0 ;
     /* ------------------------------------ 0xc113 */
     scr_mode          = 0x0;         /* bypass scramble */
     /* ------------------------------------ 0xa000 */
     mld_swap_en       = 0 ;
  } else if (pc->spd_intf == TSCMOD_SPD_10000_HI ) {   /* 10.6G HI XAUI */
     /* ------------------------------------ 0xc111 */
     encode_mode       = 0x1;         /* cl48 8b10b */
     cl49_bypass_txsm  = 1 ;
     hg2_message_invalid_code_enable = 0 ;
     /* ------------------------------------ 0xc113 */
     scr_mode          = 0x0;         /* bypass scramble */
     /* ------------------------------------ 0xa000 */
     mld_swap_en       = 0 ;
  } else if (pc->spd_intf == TSCMOD_SPD_10000_DXGXS) {
     /* ------------------------------------ 0xc111 */
     encode_mode       = 0x2;         /* rxaui */
     cl49_bypass_txsm  = 1 ;
     hg2_message_invalid_code_enable = 0 ;
     /* ------------------------------------ 0xc113 */
     scr_mode          = 0x0;         /* bypass scramble */
     /* ------------------------------------ 0xa000 */
     mld_swap_en       = 0 ;
  } else if (pc->spd_intf == TSCMOD_SPD_20000) {  /* 20G XAIU */
     /* ------------------------------------ 0xc111 */
     encode_mode       = 0x01;
     cl49_bypass_txsm  = 1 ;
     hg2_message_invalid_code_enable = 0 ;
     /* ------------------------------------ 0xc113 */
     scr_mode          = 0x0;
     /* ------------------------------------ 0xa000 */
     mld_swap_en       = 0 ;
  } else if ((pc->spd_intf == TSCMOD_SPD_21000)||
             (pc->spd_intf == TSCMOD_SPD_20000_SCR)) {  /* 21G XAIU */
     /* ------------------------------------ 0xc111 */
     encode_mode       = 0x01;
     cl49_bypass_txsm  = 1 ;
     hg2_message_invalid_code_enable = 0 ;
     /* ------------------------------------ 0xc113 */
     scr_mode          = 0x2;
     /* ------------------------------------ 0xa000 */
     mld_swap_en       = 0 ;
  } else if (pc->spd_intf == TSCMOD_SPD_20G_MLD_DXGXS ) {
     /* ------------------------------------ 0xc111 */
     encode_mode       = 0x4;         /* cl82 64b/66b */
     cl49_bypass_txsm  = 0 ;
     hg2_message_invalid_code_enable = 0 ;
     /* --------------------- ---------------0xc113 */
     scr_mode          = 0x3;         /* 64b66b scrambled */
     /* ------------------------------------ 0xa000 */
#if defined (_DV_TRIDENT2)
     mld_swap_count   = 0x0ffc ;        /* shorten from 0xfffc.  For sim ? */
     mld_swap_en      = 1 ;
#else
     mld_swap_count   = 0xfffc ;
     mld_swap_en      = 1 ;
#endif
     /* 0xa000 default value in silicon is for dual port */
  } else if ((pc->spd_intf ==TSCMOD_SPD_21G_HI_MLD_DXGXS)||
             (pc->spd_intf ==TSCMOD_SPD_12773_HI_DXGXS)) {
     /* ------------------------------------ 0xc111 */
     encode_mode       = 0x4;         /* cl82 64b/66b */
     cl49_bypass_txsm  = 0 ;
     hg2_message_invalid_code_enable = 1 ;
     hg2_en            = 1 ;
     hg2_codec         = 1 ;
     /* --------------------- ---------------0xc113 */
     scr_mode          = 0x3;         /* 64b66b scrambled */
     /* ------------------------------------ 0xa000 */
#if defined (_DV_TRIDENT2)
     mld_swap_count = 0x0ffc ;        /* shorten from 0xfffc.  For sim ? */
     mld_swap_en      = 1 ;
#else
     mld_swap_count   = 0xfffc ;
     mld_swap_en      = 1 ;
#endif

  } else if ((pc->spd_intf == TSCMOD_SPD_20G_DXGXS)||
             (pc->spd_intf == TSCMOD_SPD_21G_HI_DXGXS)) {
     /* ------------------------------------ 0xc111 */
     encode_mode       = 0x6;        /* brcm 64b/66b */
     cl49_bypass_txsm  = 1 ;
     hg2_message_invalid_code_enable = 0 ;
     /* ------------------------------------ 0xc113 */
     scr_mode          = 0x1;        /* 64b66b scrambled */
     /* ------------------------------------ 0xa000 */
     mld_swap_en      = 0 ;
  } else if ((pc->spd_intf == TSCMOD_SPD_42G_X4)||
             (pc->spd_intf == TSCMOD_SPD_40G_X4)||
             (pc->spd_intf == TSCMOD_SPD_25455) ||
             (pc->spd_intf == TSCMOD_SPD_12773_DXGXS)) {
     /* ------------------------------------ 0xc111 */
     encode_mode  = 0x6;              /* brcm 64b/66b */
     cl49_bypass_txsm  = 1 ;
     hg2_message_invalid_code_enable = 0 ;
     /* ------------------------------------ 0xc113 */
     if (pc->ctrl_type & TSCMOD_CTRL_TYPE_FS_FEC_ON) {
         scr_mode     = 0x3;
     } else {
         scr_mode     = 0x1;              /* 64b66b scrambled */
     }
     /* ------------------------------------ 0xa000 */
     mld_swap_en      = 0 ;
  } else if (pc->spd_intf == TSCMOD_SPD_31500 ) {
     /* ------------------------------------ 0xc111 */
     encode_mode  = 0x6;              /* brcm 64b/66b */
     cl49_bypass_txsm  = 1 ;
     hg2_message_invalid_code_enable = 0 ;
     /* ------------------------------------ 0xc113 */
     if (pc->ctrl_type & TSCMOD_CTRL_TYPE_FS_FEC_ON) {
         scr_mode     = 0x3;
     } else {
         scr_mode     = 0x1;              /* 64b66b scrambled */
     }
     /* ------------------------------------ 0xa000 */
     mld_swap_en      = 0 ;
  } else if (pc->spd_intf == TSCMOD_SPD_40G_MLD ) {
     /* ------------------------------------ 0xc111 */
     encode_mode  = 0x4;              /* cl82 64b/66b */
     cl49_bypass_txsm  = 0 ;
     hg2_message_invalid_code_enable = 0 ;
     /* ------------------------------------ 0xc113 */
     scr_mode     = 0x3;              /* 64b66b scrambled */
     /* ------------------------------------ 0xa000 */
#if defined (_DV_TRIDENT2)
     mld_swap_count = 0x0ffc ;
#else
     mld_swap_count = 0xfffc ;
#endif
     mld_swap_en      = 1 ;
  } else if (pc->spd_intf == TSCMOD_SPD_42G_MLD ) {
     /* ------------------------------------ 0xc111 */
     encode_mode  = 0x4;              /* cl82 64b/66b */
     cl49_bypass_txsm  = 0 ;
     hg2_message_invalid_code_enable = 1 ;
     hg2_en            = 1 ;
     hg2_codec         = 1 ;
     /* ------------------------------------ 0xc113 */
     scr_mode     = 0x3;              /* 64b66b scrambled */
     /* ------------------------------------ 0xa000 */
#if defined (_DV_TRIDENT2)
     mld_swap_count = 0x0ffc ;
#else
     mld_swap_count = 0xfffc ;
#endif
     mld_swap_en      = 1 ;
  }
  if (cntl) {
     if((pc->ctrl_type & TSCMOD_CTRL_TYPE_HG)&&(encode_mode>=3)) {
        /* encode mode is not CL48 */
        hg2_en            = 1 ;
        hg2_codec         = 1 ;
        hg2_message_invalid_code_enable = 1 ;
     }

      /* set encoder  0xc111 */
      data =hg2_codec << TX_X4_CONTROL0_ENCODE_0_HG2_CODEC_SHIFT |
            hg2_message_invalid_code_enable <<
                TX_X4_CONTROL0_ENCODE_0_HG2_MESSAGE_INVALID_CODE_ENABLE_SHIFT|
            hg2_en << TX_X4_CONTROL0_ENCODE_0_HG2_ENABLE_SHIFT|
            cl49_bypass_txsm << TX_X4_CONTROL0_ENCODE_0_CL49_BYPASS_TXSM_SHIFT|
            encode_mode << TX_X4_CONTROL0_ENCODE_0_ENCODEMODE_SHIFT ;
      mask =TX_X4_CONTROL0_ENCODE_0_HG2_CODEC_MASK|
            TX_X4_CONTROL0_ENCODE_0_HG2_MESSAGE_INVALID_CODE_ENABLE_MASK |
            TX_X4_CONTROL0_ENCODE_0_HG2_ENABLE_MASK |
            TX_X4_CONTROL0_ENCODE_0_ENCODEMODE_MASK |
            TX_X4_CONTROL0_ENCODE_0_CL49_BYPASS_TXSM_MASK ;
      SOC_IF_ERROR_RETURN
          (MODIFY_WC40_TX_X4_CONTROL0_ENCODE_0r(pc->unit, pc, data, mask));

      /* 0xc113 */
      data = scr_mode  << TX_X4_CONTROL0_MISC_SCR_MODE_SHIFT  ;
      mask =              TX_X4_CONTROL0_MISC_SCR_MODE_MASK  ;
      SOC_IF_ERROR_RETURN
          (MODIFY_WC40_TX_X4_CONTROL0_MISCr(pc->unit, pc, data, mask));

      /* 0xc114 */
      data = disable_packet_misalign  << TX_X4_CONTROL0_CL36_TX_0_DISABLE_PACKET_MISALIGN_SHIFT  ;
      mask = TX_X4_CONTROL0_CL36_TX_0_DISABLE_PACKET_MISALIGN_MASK ;
      SOC_IF_ERROR_RETURN
         (MODIFY_WC40_TX_X4_CONTROL0_CL36_TX_0r(pc->unit, pc, data, mask));

      /* 0xa000 */
      data = mld_swap_count << TX_X2_CONTROL0_MLD_SWAP_COUNT_MLD_SWAP_COUNT_SHIFT;
      mask = mld_swap_en ? TX_X2_CONTROL0_MLD_SWAP_COUNT_MLD_SWAP_COUNT_MASK : 0   ;
      if(mask)
         SOC_IF_ERROR_RETURN
            (MODIFY_WC40_TX_X2_CONTROL0_MLD_SWAP_COUNTr(pc->unit, pc, data, mask));

      if(encode_mode == 0x6) { /* BRCM mode */
         data = 0x66 ;
      } else {
         data = 0x2 ;
      }
      mask = TX_X2_CONTROL0_BRCM_MODE_ACOL_SWAP_COUNT64B66B_MASK ;
      SOC_IF_ERROR_RETURN
         (MODIFY_WC40_TX_X2_CONTROL0_BRCM_MODEr(pc->unit, pc, data, mask));
    } else {
       /* no op */
       return SOC_E_NONE;
    }
    return SOC_E_NONE;
}

/*!
\brief set rx decoder of any particular lane

\param  pc handle to current TSC context (#tscmod_st)

\returns The value #SOC_E_NONE upon successful completion.

This function is used to select decoder of any receive lane.

\li select encodeMode

This function reads/modifies the following registers:

\li descramblemode :   0xc130[15:14]
\li decodemode	   :   0xc130[13:11]
\li deskewmode	   :   0xc130[10:8]
\li desc2_mode	   :   0xc130[7:5]
\li cl36_en	       :   0xc134[0]

<B>How to call:</B><br>
<p>Call this function directly or through the function
#tscmod_tier1_selector() with the upper-case string "DECODE_SET".</p>
*/

int tscmod_decode_set(tscmod_st* pc)         /* DECODE_SET */
{
  uint16 descramblemode, decodemode, deskewmode, desc2_mode ;
  uint16 cl36_en, disable_carrier_extend, reorder_en ;
  uint16 cl48_syncacq_en, block_sync_mode, cl48_cgbad_en, bypass_cl49rxsm, hg2_message_ivalid_code_enable, hg2_en, hg2_codec;
  uint16 cl36byte_delete_mode, os_mode, rx_gbox_afrst_en;
  uint16 cl82_dswin, cl48_dswin64b66b, cl48_dswin8b10b, rx_x1_cntl0_deskw_en ;
  uint16 sync_code_sm_en ;
  uint16 rx_control0_misc0_en ;
  uint16 data, mask;
  int cntl;
  int hw_tuning ;
  uint16 brcm64b66_descr ;

  cntl = pc->per_lane_control;
  descramblemode  = 0x0; /* 00=bypass descrambler; 01=64b66b descrambler;
                            10=8b10b descrambler */
  decodemode      = 0x0; /* 3'b000 - None; 3'b001 - cl49 64b66b mode;
                            3'b010 - BRCM 64b66b mode;
                            3'b011 - ATL - cl49/BRCM 64b66b mode;
                            3'b100 - 8b10b mode - cl48 mode
                            3'b101 - 8b10b mode - cl36 mode */
  deskewmode 	   = 0x0;
  desc2_mode      = 0x0; /* {001:CL49}, {010:BRCM}, {011:ATL},
                            {100:CL48}, {101:CL36}, {110:CL82}, {111:NONE} */
  brcm64b66_descr = 0x0;
  cl36_en         = 0x0;
  cl36byte_delete_mode = 0x0;
  disable_carrier_extend = 0x0;
  reorder_en      = 0x0;
  cl48_syncacq_en = 0x1;
  block_sync_mode = 0x0;
  cl48_cgbad_en   = 0x0;
  bypass_cl49rxsm = 0x0;
  hg2_message_ivalid_code_enable = 0x1 ;
  hg2_en          = 0 ;
  hg2_codec       = 0 ;
  os_mode         = 0x0;  /* 0= OS MODE 1;  1= OS MODE 2; 2=OS MODE 3;
                             3=OS MODE 3.3; 4=OS MODE 4; 5=OS MODE 5;
                             6=OS MODE 8;   7=OS MODE 8.25; 8: OS MODE 10*/
  rx_gbox_afrst_en = 0x0;
  rx_x1_cntl0_deskw_en = 0 ;
  cl82_dswin           = 0 ;
  cl48_dswin64b66b     = 0 ;
  cl48_dswin8b10b      = 0 ;
  sync_code_sm_en      = 0 ;
  rx_control0_misc0_en = 0 ;

  if (cntl) { /* set decoder */
    pc->os_type = TSCMOD_OS1 ;

    if(pc->spd_intf == TSCMOD_SPD_10_SGMII) {
        decodemode      = 0x5;
        deskewmode      = 0x4;
        desc2_mode      = 0x5;
        cl36byte_delete_mode = 0x1;
        hg2_message_ivalid_code_enable = 0x0 ;
        cl48_syncacq_en = 0x0;
        block_sync_mode = 0x3;
        cl36_en         = 0x1;
        rx_gbox_afrst_en = 0x1;
        if (pc->plldiv == 40) {
          os_mode         = 0x5;
          pc->os_type     = TSCMOD_OS5 ;
        } else if (pc->plldiv == 66){
          os_mode         = 0x7;
          pc->os_type     = TSCMOD_OS8 ;  /* 8.25 and 8 are in the same group for now */
        }
    } else if(pc->spd_intf == TSCMOD_SPD_100_SGMII) {
        decodemode      = 0x5;
        deskewmode      = 0x4;
        desc2_mode      = 0x5;
        cl36byte_delete_mode = 0x0;
        hg2_message_ivalid_code_enable = 0x0 ;
        cl48_syncacq_en = 0x0;
        block_sync_mode = 0x3;
        cl36_en         = 0x1;
        rx_gbox_afrst_en = 0x1;
        if (pc->plldiv == 40) {
          os_mode         = 0x5;
          pc->os_type     = TSCMOD_OS5 ;
        } else if (pc->plldiv == 66){
          os_mode         = 0x7;
          pc->os_type     = TSCMOD_OS8 ;
        }
    } else if(pc->spd_intf == TSCMOD_SPD_1000_SGMII) {
        decodemode      = 0x5;
        deskewmode      = 0x4;
        desc2_mode      = 0x5;
        cl36byte_delete_mode = 0x2;
        hg2_message_ivalid_code_enable = 0x0 ;
        cl48_syncacq_en = 0x0;
        block_sync_mode = 0x3;
        cl36_en         = 0x1;
        rx_gbox_afrst_en = 0x1;
        if (pc->plldiv == 40) {
          os_mode         = 0x5;
          pc->os_type     = TSCMOD_OS5 ;
        } else if (pc->plldiv == 66){
          os_mode         = 0x7;
          pc->os_type     = TSCMOD_OS8 ;
        }
    } else if(pc->spd_intf == TSCMOD_SPD_2500) {
        decodemode      = 0x5;
        deskewmode      = 0x4;
        desc2_mode      = 0x5;
        cl36byte_delete_mode = 0x2;
        hg2_message_ivalid_code_enable = 0x0 ;
        cl48_syncacq_en = 0x0;
        block_sync_mode = 0x3;
        cl36_en         = 0x1;
        rx_gbox_afrst_en = 0x1;
        if (pc->plldiv == 40) {
          os_mode         = 0x1;
          pc->os_type     = TSCMOD_OS2 ;
        } else if (pc->plldiv == 66){
          os_mode         = 0x3;
          pc->os_type     = TSCMOD_OS3p3 ;  /* OS3 and OS3.3 in the smae group */
        }
    } else if((pc->spd_intf == TSCMOD_SPD_10000_XFI)||
              (pc->spd_intf == TSCMOD_SPD_10600_XFI_HG)||
              (pc->spd_intf == TSCMOD_SPD_5000)) {
        /* ------------------------------------  0xc130 */
        descramblemode  = 0x0;
        decodemode      = 0x1;
        deskewmode 	    = 0x0;
        desc2_mode      = 0x1;
        /* ------------------------------------  0xc136 */
        cl36_en         = 0x0;
        disable_carrier_extend = 0x0;
        reorder_en      = 0x0 ;
        /* ------------------------------------  0xc134 */
        cl48_syncacq_en = 0x0;
        block_sync_mode = 0x1;
        cl48_cgbad_en   = 0x0;
        bypass_cl49rxsm = 0x0;
        hg2_message_ivalid_code_enable = 0x1 ;

        if(pc->spd_intf == TSCMOD_SPD_5000) {
           os_mode = 0x1 ;
           pc->os_type = TSCMOD_OS2 ;
        } else {
           os_mode = 0 ;
        }
    } else if((pc->spd_intf == TSCMOD_SPD_10000)||
              (pc->spd_intf == TSCMOD_SPD_13000)||
              (pc->spd_intf == TSCMOD_SPD_15000)||
              (pc->spd_intf == TSCMOD_SPD_16000)) {  /* 10G XAUI */
        /* ------------------------------------  0xc130 */
        descramblemode  = 0x0;
        decodemode      = 0x4;  /* cl48 8b18b */
        deskewmode 	    = 0x1;  /* byte deskew for 8b10b */
        desc2_mode      = 0x4;  /* cl48 */
        /* ------------------------------------  0xc136 */
        cl36_en         = 0x0;
        disable_carrier_extend = 0x1;
        reorder_en      = 0x1 ;
        /* ------------------------------------  0xc134 */
        cl48_syncacq_en = 0x1;
        block_sync_mode = 0x3;  /* 10b8b */
        cl48_cgbad_en   = 0x1;
        bypass_cl49rxsm = 0x1;
        hg2_message_ivalid_code_enable = 0x0 ;
        /* ------------------------------------  0xc137 */
        os_mode         = 0x1;
        pc->os_type     = TSCMOD_OS2 ;
        /* ------------------------------------  0x9222 */
        rx_x1_cntl0_deskw_en = 1 ;
        cl82_dswin           = 0 ;
        cl48_dswin64b66b     = 1 ;
        cl48_dswin8b10b      = 7 ;
    } else if( pc->spd_intf == TSCMOD_SPD_10000_HI ) {  /* 10G XAUI */
        /* ------------------------------------  0xc130 */
        descramblemode  = 0x0;
        decodemode      = 0x4;  /* cl48 8b18b */
        deskewmode 	    = 0x1;  /* byte deskew for 8b10b */
        desc2_mode      = 0x4;  /* cl48 */
        /* ------------------------------------  0xc136 */
        cl36_en         = 0x0;
        disable_carrier_extend = 0x1;
        reorder_en      = 0x1 ;
        /* ------------------------------------  0xc134 */
        cl48_syncacq_en = 0x1;
        block_sync_mode = 0x3;  /* 10b8b */
        cl48_cgbad_en   = 0x1;
        bypass_cl49rxsm = 0x1;
        hg2_message_ivalid_code_enable = 0x0 ;
        /* ------------------------------------  0xc137 */
        os_mode         = 0x1;
        pc->os_type     = TSCMOD_OS2 ;
        /* ------------------------------------  0x9222 */
        rx_x1_cntl0_deskw_en = 1 ;
        cl82_dswin           = 0 ;
        cl48_dswin64b66b     = 1 ;
        cl48_dswin8b10b      = 7 ;
    } else if( pc->spd_intf == TSCMOD_SPD_10000_DXGXS ) {  /* 10G RXAUI */
       /* ------------------------------------  0xc130 */
       descramblemode  = 0x0;
       decodemode      = 0x4;  /* cl48 8b18b */
       deskewmode 	  = 0x1;  /* byte deskew for 8b10b */
       desc2_mode      = 0x4;  /* cl48 */
       /* ------------------------------------  0xc136 */
       cl36_en         = 0x0;
       disable_carrier_extend = 0x1;
       reorder_en      = 0x1 ;
       /* ------------------------------------  0xc134 */
       cl48_syncacq_en = 0x1;
       block_sync_mode = 0x3;  /* 10b8b */
       cl48_cgbad_en   = 0x1;
       bypass_cl49rxsm = 0x1;
       hg2_message_ivalid_code_enable = 0x0 ;
       /* ------------------------------------  0xc137 */
       os_mode         = 0x0;
       /* ------------------------------------  0x9222 */
       rx_x1_cntl0_deskw_en = 1 ;
       cl82_dswin           = 0 ;
       cl48_dswin64b66b     = 1 ;
       cl48_dswin8b10b      = 7 ;
    } else if(pc->spd_intf == TSCMOD_SPD_20000) {  /* 20G XAUI */
       /* ------------------------------------  0xc130 */
        descramblemode  = 0x0;
        decodemode      = 0x4;  /* cl48 8b18b */
        deskewmode 	   = 0x1;  /* byte deskew for 8b10b */
        desc2_mode      = 0x4;  /* cl48 */
        /* ------------------------------------  0xc136 */
        cl36_en         = 0x0;
        disable_carrier_extend = 0x1;
        reorder_en      = 0x1 ;
        /* ------------------------------------  0xc134 */
        cl48_syncacq_en = 0x1;
        block_sync_mode = 0x3;  /* 10b8b */
        cl48_cgbad_en   = 0x1;
        bypass_cl49rxsm = 0x1;
        hg2_message_ivalid_code_enable = 0x0 ;
        /* ------------------------------------  0xc137 */
        os_mode         = 0x0;
        /* ------------------------------------  0x9222 */
        rx_x1_cntl0_deskw_en = 1 ;
        cl82_dswin           = 0 ;
        cl48_dswin64b66b     = 1 ;
        cl48_dswin8b10b      = 7 ;
    } else if((pc->spd_intf == TSCMOD_SPD_21000)||
              (pc->spd_intf == TSCMOD_SPD_20000_SCR)) {  /* 21G XAUI */
       /* ------------------------------------  0xc130 */
        descramblemode  = 0x2;
        decodemode      = 0x4;  /* cl48 8b18b */
        deskewmode 	   = 0x1;  /* byte deskew for 8b10b */
        desc2_mode      = 0x4;  /* cl48 */
        /* ------------------------------------  0xc136 */
        cl36_en         = 0x0;
        disable_carrier_extend = 0x1;
        reorder_en      = 0x1 ;
        /* ------------------------------------  0xc134 */
        cl48_syncacq_en = 0x1;
        block_sync_mode = 0x3;  /* 10b8b */
        cl48_cgbad_en   = 0x1;
        bypass_cl49rxsm = 0x1;
        hg2_message_ivalid_code_enable = 0x0 ;
        /* ------------------------------------  0xc137 */
        os_mode         = 0x0;
        /* ------------------------------------  0x9222 */
        rx_x1_cntl0_deskw_en = 1 ;
        cl82_dswin           = 0 ;
        cl48_dswin64b66b     = 1 ;
        cl48_dswin8b10b      = 7 ;
    } else if(pc->spd_intf == TSCMOD_SPD_20G_MLD_DXGXS ) {
        /* ------------------------------------  0xc130 */
        descramblemode  = 0x0;
        decodemode      = 0x6;  /* cl48 8b18b */
        deskewmode 	    = 0x3;  /* byte deskew for IEEE CL82 */
        desc2_mode      = 0x6;  /* cl82 */
        /* ------------------------------------  0xc136 */
        cl36_en         = 0x0;
        disable_carrier_extend = 0x1;
        reorder_en      = 0x0 ;
        /* ------------------------------------  0xc134 */
        cl48_syncacq_en = 0x0;
        block_sync_mode = 0x2;  /* cl82 mode */
        cl48_cgbad_en   = 0x0;
        bypass_cl49rxsm = 0x0;
        hg2_message_ivalid_code_enable = 0x1 ;
        /* ------------------------------------  0xc137 */
        os_mode         = 0x0;
        /* ------------------------------------  0x9222 */
        rx_x1_cntl0_deskw_en = 1 ;
        cl82_dswin           = 0x1e ;
        cl48_dswin64b66b     = 0x1 ;
        cl48_dswin8b10b      = 0x7 ;
    } else if((pc->spd_intf ==TSCMOD_SPD_21G_HI_MLD_DXGXS)||
              (pc->spd_intf ==TSCMOD_SPD_12773_HI_DXGXS)) {
        /* ------------------------------------  0xc130 */
        descramblemode  = 0x0;
        decodemode      = 0x6;  /* cl48 8b18b */
        deskewmode 	    = 0x3;  /* byte deskew for IEEE CL82 */
        desc2_mode      = 0x6;  /* cl82 */
        /* ------------------------------------  0xc136 */
        cl36_en         = 0x0;
        disable_carrier_extend = 0x1;
        reorder_en      = 0x0 ;
        /* ------------------------------------  0xc134 */
        cl48_syncacq_en = 0x0;
        block_sync_mode = 0x2;  /* cl82 mode */
        cl48_cgbad_en   = 0x0;
        bypass_cl49rxsm = 0x0;
        hg2_message_ivalid_code_enable = 0x1 ;
        hg2_en          = 1 ;
        hg2_codec       = 1 ;
        /* ------------------------------------  0xc137 */
        os_mode         = 0x0;
        /* ------------------------------------  0x9222 */
        rx_x1_cntl0_deskw_en = 1 ;
        cl82_dswin           = 0x1e ;
        cl48_dswin64b66b     = 0x1 ;
        cl48_dswin8b10b      = 0x7 ;
    } else if((pc->spd_intf == TSCMOD_SPD_20G_DXGXS)||
              (pc->spd_intf == TSCMOD_SPD_21G_HI_DXGXS)) {
       /* ------------------------------------  0xc130 */
        descramblemode  = 0x1 ;
        decodemode      = 0x2 ;
        deskewmode 	   = 0x2 ;
        desc2_mode      = 0x4 ;
        /* ------------------------------------  0xc136 */
        cl36_en         = 0x0;
        disable_carrier_extend = 0x1;
        reorder_en      = 0x1 ;
        /* ------------------------------------  0xc134 */
        cl48_syncacq_en = 0x1;
        block_sync_mode = 0x5;     /* brcm 64/66 */
        cl48_cgbad_en   = 0x1;
        bypass_cl49rxsm = 0x1;
        hg2_message_ivalid_code_enable = 0x0 ;
        /* ------------------------------------  0xc137 */
        os_mode         = 0x0;      /* os mode div by 1 */
        /* ------------------------------------  0x9222 */
        rx_x1_cntl0_deskw_en = 1 ;
        cl82_dswin           = 0x0 ;
        cl48_dswin64b66b     = 0x1 ;
        cl48_dswin8b10b      = 0x7 ;

    } else if((pc->spd_intf == TSCMOD_SPD_42G_X4) ||
              (pc->spd_intf == TSCMOD_SPD_40G_X4) ||
              (pc->spd_intf == TSCMOD_SPD_25455) ||
              (pc->spd_intf == TSCMOD_SPD_12773_DXGXS)) {
        /* ------------------------------------  0xc130 */
        descramblemode  = 0x1  ;
        decodemode      = 0x2 ;
        deskewmode 	   = 0x2 ;
        desc2_mode      = 0x4 ;
        if(pc->ctrl_type&TSCMOD_CTRL_TYPE_FS_FEC_ON) {
            brcm64b66_descr = 0x1;
            descramblemode  = 0x0;
        }
        /* ------------------------------------  0xc136 */
        cl36_en         = 0x0;
        disable_carrier_extend = 0x1;
        reorder_en      = 0x1 ;
        /* ------------------------------------  0xc134 */
        cl48_syncacq_en = 0x1;
        if (pc->ctrl_type & TSCMOD_CTRL_TYPE_FS_FEC_ON) {
            block_sync_mode = 0x4;
        } else {
            block_sync_mode = 0x5;    /* brcm 64/66 */
        }
        cl48_cgbad_en   = 0x1;
        bypass_cl49rxsm = 0x1;
        hg2_message_ivalid_code_enable = 0x1 ;
        /* ------------------------------------  0xc137 */
        os_mode         = 0x0;      /* os mode div by 1 */
        /* ------------------------------------  0x9222 */
        rx_x1_cntl0_deskw_en = 1 ;
        cl82_dswin           = 0x0 ;
        cl48_dswin64b66b     = 0x1 ;
        cl48_dswin8b10b      = 0x7 ;
    } else if( pc->spd_intf == TSCMOD_SPD_31500 ) {
        /* ------------------------------------  0xc130 */
        descramblemode  = 0x1  ;
        decodemode      = 0x2 ;
        deskewmode 	   = 0x2 ;
        desc2_mode      = 0x4 ;
        if(pc->ctrl_type & TSCMOD_CTRL_TYPE_FS_FEC_ON) {
            brcm64b66_descr = 0x1;
            descramblemode  = 0x0;
        }
        /* ------------------------------------  0xc136 */
        cl36_en         = 0x0;
        disable_carrier_extend = 0x1;
        reorder_en      = 0x1 ;
        /* ------------------------------------  0xc134 */
        cl48_syncacq_en = 0x1;
        if (pc->ctrl_type & TSCMOD_CTRL_TYPE_FS_FEC_ON) {
            block_sync_mode = 0x4;
        } else {
            block_sync_mode = 0x5;    /* brcm 64/66 */
        }
        cl48_cgbad_en   = 0x1;
        bypass_cl49rxsm = 0x1;
        hg2_message_ivalid_code_enable = 0x1 ;
        /* ------------------------------------  0xc137 */
        os_mode         = 0x0;      /* os mode div by 1 */
        /* ------------------------------------  0x9222 */
        rx_x1_cntl0_deskw_en = 1 ;
        cl82_dswin           = 0x0 ;
        cl48_dswin64b66b     = 0x1 ;
        cl48_dswin8b10b      = 0x7 ;
    } else if( pc->spd_intf == TSCMOD_SPD_40G_MLD ) {
        /* ------------------------------------  0xc130 */
        decodemode      = 0x6 ;
        desc2_mode      = 0x6 ;
        deskewmode 	    = 0x3 ;
        /* ------------------------------------  0xc136 */
        cl36_en         = 0x0;
        disable_carrier_extend = 0x1;
        reorder_en      = 0x0 ;
        /* ------------------------------------  0xc134 */
        cl48_syncacq_en = 0x0;
        if (pc->ctrl_type & TSCMOD_CTRL_TYPE_FS_FEC_ON) {
            block_sync_mode = 0x4;
        } else {
            block_sync_mode = 0x2;  /* cl82 mode */
        }
        cl48_cgbad_en   = 0x0;
        bypass_cl49rxsm = 0x0;
        hg2_message_ivalid_code_enable = 0x1 ;
        /* ------------------------------------  0xc137 */
        os_mode         = 0x0;      /* os mode div by 1 */
        /* ------------------------------------  0x9222 */
        rx_x1_cntl0_deskw_en = 1 ;
        cl82_dswin           = 0x1e ;
        cl48_dswin64b66b     = 0x1 ;
        cl48_dswin8b10b      = 0x7 ;
    } else if( pc->spd_intf == TSCMOD_SPD_42G_MLD ) {  /* 40 MLD Hig2 */
        /* ------------------------------------  0xc130 */
        decodemode      = 0x6 ;
        desc2_mode      = 0x6 ;
        deskewmode 	    = 0x3 ;
        /* ------------------------------------  0xc136 */
        cl36_en         = 0x0;
        disable_carrier_extend = 0x1;
        reorder_en      = 0x0 ;
        /* ------------------------------------  0xc134 */
        cl48_syncacq_en = 0x0;
        if (pc->ctrl_type & TSCMOD_CTRL_TYPE_FS_FEC_ON) {
            block_sync_mode = 0x4;
        } else {
            block_sync_mode = 0x2;  /* cl82 mode */
        }
        cl48_cgbad_en   = 0x0;
        bypass_cl49rxsm = 0x0;
        hg2_message_ivalid_code_enable = 0x1 ;
        hg2_en          = 1 ;
        hg2_codec       = 1 ;
        /* ------------------------------------  0xc137 */
        os_mode         = 0x0;      /* os mode div by 1 */
        /* ------------------------------------  0x9222 */
        rx_x1_cntl0_deskw_en = 1 ;
        cl82_dswin           = 0x1e ;
        cl48_dswin64b66b     = 0x1 ;
        cl48_dswin8b10b      = 0x7 ;
    } else {
       if(!(pc->ctrl_type&TSCMOD_CTRL_TYPE_ERR)||(pc->verbosity&TSCMOD_DBG_PRINT)) {
          printf("Error: p=%0d undefined spd_intf=%0d(%s)\n", pc->port, pc->spd_intf,
              e2s_tscmod_spd_intfc_type[pc->spd_intf]) ;
       }
       pc->err_code  |= TSCMOD_ERR_FS_SPD_MODE ;
       pc->ctrl_type |= TSCMOD_CTRL_TYPE_ERR ;
    }
    /* HG setting */
    if((pc->ctrl_type & TSCMOD_CTRL_TYPE_HG)) {
       /* not CL48 */
       hg2_en          = 1 ;
       hg2_codec       = 1 ;
    }

    /* 0xc130 */
    data = descramblemode << RX_X4_CONTROL0_PCS_CONTROL_0_DESCRAMBLERMODE_SHIFT |
	       decodemode << RX_X4_CONTROL0_PCS_CONTROL_0_DECODERMODE_SHIFT |
	       deskewmode << RX_X4_CONTROL0_PCS_CONTROL_0_DESKEWMODE_SHIFT |
           desc2_mode << RX_X4_CONTROL0_PCS_CONTROL_0_DESC2_MODE_SHIFT |
           cl36byte_delete_mode << RX_X4_CONTROL0_PCS_CONTROL_0_CL36BYTEDELETEMODE_SHIFT | 
           brcm64b66_descr << RX_X4_CONTROL0_PCS_CONTROL_0_BRCM64B66_DESCRAMBLER_ENABLE_SHIFT ;
    
    mask = RX_X4_CONTROL0_PCS_CONTROL_0_DESCRAMBLERMODE_MASK |
	       RX_X4_CONTROL0_PCS_CONTROL_0_DECODERMODE_MASK |
	       RX_X4_CONTROL0_PCS_CONTROL_0_DESKEWMODE_MASK |
	       RX_X4_CONTROL0_PCS_CONTROL_0_DESC2_MODE_MASK |
           RX_X4_CONTROL0_PCS_CONTROL_0_CL36BYTEDELETEMODE_MASK | 
           RX_X4_CONTROL0_PCS_CONTROL_0_BRCM64B66_DESCRAMBLER_ENABLE_MASK ;

    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_RX_X4_CONTROL0_PCS_CONTROL_0r(pc->unit, pc, data, mask));

    /* 0xc136 */
    data = cl36_en    << RX_X4_CONTROL0_CL36_RX_0_CL36_EN_SHIFT |
           disable_carrier_extend
                      << RX_X4_CONTROL0_CL36_RX_0_DISABLE_CARRIER_EXTEND_SHIFT |
           reorder_en << RX_X4_CONTROL0_CL36_RX_0_REORDER_EN_SHIFT ;
    mask = RX_X4_CONTROL0_CL36_RX_0_CL36_EN_MASK |
           RX_X4_CONTROL0_CL36_RX_0_DISABLE_CARRIER_EXTEND_MASK|
           RX_X4_CONTROL0_CL36_RX_0_REORDER_EN_MASK ;

    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_RX_X4_CONTROL0_CL36_RX_0r(pc->unit, pc, data, mask));

    if(pc->ctrl_type&TSCMOD_CTRL_TYPE_FS_FEC_ON) {
        block_sync_mode = 0x4;
    }
    /* 0xc134 */
    data =
      bypass_cl49rxsm << RX_X4_CONTROL0_DECODE_CONTROL_0_BYPASS_CL49RXSM_SHIFT|
      hg2_message_ivalid_code_enable <<
        RX_X4_CONTROL0_DECODE_CONTROL_0_HG2_MESSAGE_INVALID_CODE_ENABLE_SHIFT |
      hg2_en << RX_X4_CONTROL0_DECODE_CONTROL_0_HG2_ENABLE_SHIFT |
      cl48_syncacq_en<< RX_X4_CONTROL0_DECODE_CONTROL_0_CL48_SYNCACQ_EN_SHIFT|
      cl48_cgbad_en  << RX_X4_CONTROL0_DECODE_CONTROL_0_CL48_CGBAD_EN_SHIFT  |
      hg2_codec << RX_X4_CONTROL0_DECODE_CONTROL_0_HG2_CODEC_SHIFT |
      block_sync_mode<< RX_X4_CONTROL0_DECODE_CONTROL_0_BLOCK_SYNC_MODE_SHIFT ;

    mask =
        RX_X4_CONTROL0_DECODE_CONTROL_0_BYPASS_CL49RXSM_MASK |
        RX_X4_CONTROL0_DECODE_CONTROL_0_HG2_MESSAGE_INVALID_CODE_ENABLE_MASK |
        RX_X4_CONTROL0_DECODE_CONTROL_0_HG2_ENABLE_MASK |
        RX_X4_CONTROL0_DECODE_CONTROL_0_CL48_SYNCACQ_EN_MASK |
        RX_X4_CONTROL0_DECODE_CONTROL_0_CL48_CGBAD_EN_MASK |
        RX_X4_CONTROL0_DECODE_CONTROL_0_HG2_CODEC_MASK |
        RX_X4_CONTROL0_DECODE_CONTROL_0_BLOCK_SYNC_MODE_MASK ;

    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_RX_X4_CONTROL0_DECODE_CONTROL_0r(pc->unit, pc, data, mask));

    /* -ADDR=0xc135 -- */
    if((pc->spd_intf ==TSCMOD_SPD_42G_X4) ||
       (pc->spd_intf ==TSCMOD_SPD_40G_X4) ||
       (pc->spd_intf ==TSCMOD_SPD_25455) ||
       (pc->spd_intf ==TSCMOD_SPD_21G_HI_DXGXS)||
       (pc->spd_intf ==TSCMOD_SPD_20G_DXGXS)||
       (pc->spd_intf ==TSCMOD_SPD_12773_DXGXS)) {
       if(pc->spd_intf ==TSCMOD_SPD_12773_DXGXS) { data = 0x900f ; }
       else { data = 0x8001 ; }
       mask = 0xffff ;
       SOC_IF_ERROR_RETURN
          (MODIFY_WC40_RX_X4_CONTROL0_DECODE_CONTROL_1r(pc->unit, pc, data, mask));
    }

    /* --Addr=0xc137-------------------------- */
        data = os_mode << RX_X4_CONTROL0_PMA_CONTROL_0_OS_MODE_SHIFT |
               rx_gbox_afrst_en << RX_X4_CONTROL0_PMA_CONTROL_0_RX_GBOX_AFRST_EN_SHIFT;
        mask = RX_X4_CONTROL0_PMA_CONTROL_0_OS_MODE_MASK |
               RX_X4_CONTROL0_PMA_CONTROL_0_RX_GBOX_AFRST_EN_MASK;
    if (pc->ctrl_type & TSCMOD_CTRL_TYPE_UC_WR_MASK_EN) {
       SOC_IF_ERROR_RETURN
          (tscmod_mask_wr_cmd(pc, TSC40_RX_X4_CONTROL0_PMA_CONTROL_0r, data, mask)) ;
    } else {
       SOC_IF_ERROR_RETURN
          (MODIFY_WC40_RX_X4_CONTROL0_PMA_CONTROL_0r(pc->unit, pc, data, mask));
    }
    /* --Addr=0x9220 ------------------------- */
    if((pc->spd_intf ==TSCMOD_SPD_10000)||
       (pc->spd_intf ==TSCMOD_SPD_13000)||
       (pc->spd_intf ==TSCMOD_SPD_15000)||
       (pc->spd_intf ==TSCMOD_SPD_16000)||
       (pc->spd_intf ==TSCMOD_SPD_10000_HI)||
       (pc->spd_intf ==TSCMOD_SPD_10000_DXGXS)||
       (pc->spd_intf ==TSCMOD_SPD_20000)){
        data = 0x0444 ;
        mask = 0xffff ;
        sync_code_sm_en = 1 ;
    } else if ((pc->spd_intf ==TSCMOD_SPD_42G_X4)||
               (pc->spd_intf ==TSCMOD_SPD_40G_X4)||
               (pc->spd_intf ==TSCMOD_SPD_25455)||
               (pc->spd_intf ==TSCMOD_SPD_21G_HI_DXGXS)||
               (pc->spd_intf ==TSCMOD_SPD_20G_DXGXS)||
               (pc->spd_intf == TSCMOD_SPD_12773_DXGXS) ) {
        data = 0x0323 ;
        mask = 0xffff ;
        sync_code_sm_en = 1 ;
    }
    if(sync_code_sm_en) {
        SOC_IF_ERROR_RETURN
        (MODIFY_WC40_RX_X1_CONTROL0_DECODE_CONTROL_0r(pc->unit, pc, data, mask));
    }
    /* --Addr=0x9222 ------------------------- */
    data = cl82_dswin << RX_X1_CONTROL0_DESKEW_WINDOWS_CL82_DSWIN_SHIFT |
           cl48_dswin64b66b
                      << RX_X1_CONTROL0_DESKEW_WINDOWS_CL48_DSWIN64B66B_SHIFT |
           cl48_dswin8b10b
                      << RX_X1_CONTROL0_DESKEW_WINDOWS_CL48_DSWIN8B10B_SHIFT ;
    mask = RX_X1_CONTROL0_DESKEW_WINDOWS_CL82_DSWIN_MASK |
           RX_X1_CONTROL0_DESKEW_WINDOWS_CL48_DSWIN64B66B_MASK |
           RX_X1_CONTROL0_DESKEW_WINDOWS_CL48_DSWIN8B10B_MASK ;
    if( rx_x1_cntl0_deskw_en )
        SOC_IF_ERROR_RETURN
           (MODIFY_WC40_RX_X1_CONTROL0_DESKEW_WINDOWSr(pc->unit, pc, data, mask));

    /* --Addr=0x9233 ------------------------- */
    if((pc->spd_intf ==TSCMOD_SPD_42G_X4)||
       (pc->spd_intf ==TSCMOD_SPD_40G_X4)||
       (pc->spd_intf ==TSCMOD_SPD_25455) ||
       (pc->spd_intf ==TSCMOD_SPD_21G_HI_DXGXS)||
       (pc->spd_intf ==TSCMOD_SPD_20G_DXGXS)||
       (pc->spd_intf ==TSCMOD_SPD_12773_DXGXS)) {
       data = 0x8090 ;
       mask = 0xffff ;
       SOC_IF_ERROR_RETURN
          (MODIFY_WC40_RX_X1_CONTROL1_SCW0r(pc->unit, pc, data, mask));
    }
    /* --Addr=0x9234 ------------------------- */
    if((pc->spd_intf ==TSCMOD_SPD_42G_X4)||
       (pc->spd_intf ==TSCMOD_SPD_40G_X4)||
       (pc->spd_intf ==TSCMOD_SPD_25455) ||
       (pc->spd_intf ==TSCMOD_SPD_21G_HI_DXGXS)||
       (pc->spd_intf ==TSCMOD_SPD_20G_DXGXS)||
       (pc->spd_intf ==TSCMOD_SPD_12773_DXGXS)) {
       data = 0xa0b0 ;
       mask = 0xffff ;
       SOC_IF_ERROR_RETURN
          (MODIFY_WC40_RX_X1_CONTROL1_SCW1r(pc->unit, pc, data, mask));
    }
    /* --Addr=0x9235 ------------------------- */
    if((pc->spd_intf ==TSCMOD_SPD_42G_X4)||
       (pc->spd_intf ==TSCMOD_SPD_40G_X4)||
       (pc->spd_intf ==TSCMOD_SPD_25455) ||
       (pc->spd_intf ==TSCMOD_SPD_21G_HI_DXGXS)||
       (pc->spd_intf ==TSCMOD_SPD_20G_DXGXS)||
       (pc->spd_intf ==TSCMOD_SPD_12773_DXGXS)) {
       data = 0xc0d0 ;
       mask = 0xffff ;
       SOC_IF_ERROR_RETURN
          (MODIFY_WC40_RX_X1_CONTROL1_SCW2r(pc->unit, pc, data, mask));
    }
    /* --Addr=0x9236 ------------------------- */
    if((pc->spd_intf ==TSCMOD_SPD_42G_X4)||
       (pc->spd_intf ==TSCMOD_SPD_40G_X4)||
       (pc->spd_intf ==TSCMOD_SPD_25455) ||
       (pc->spd_intf ==TSCMOD_SPD_21G_HI_DXGXS) ||
       (pc->spd_intf ==TSCMOD_SPD_20G_DXGXS)||
       (pc->spd_intf ==TSCMOD_SPD_12773_DXGXS)) {
       data = 0xe070 ;
       mask = 0xffff ;
       SOC_IF_ERROR_RETURN
          (MODIFY_WC40_RX_X1_CONTROL1_SCW3r(pc->unit, pc, data, mask));
    }
    /* --Addr=0x9237 ------------------------- */
    if((pc->spd_intf ==TSCMOD_SPD_42G_X4)||
       (pc->spd_intf ==TSCMOD_SPD_40G_X4)||
       (pc->spd_intf ==TSCMOD_SPD_25455) ||
       (pc->spd_intf ==TSCMOD_SPD_21G_HI_DXGXS)||
       (pc->spd_intf ==TSCMOD_SPD_20G_DXGXS)||
       (pc->spd_intf ==TSCMOD_SPD_12773_DXGXS)) {
       data = 0x0001 ;                          /* default is 0x01 (no change) */
       mask = 0xffff ;
       SOC_IF_ERROR_RETURN
          (MODIFY_WC40_RX_X1_CONTROL1_SCW4r(pc->unit, pc, data, mask));
    }
    /* --Addr=0x9238 ------------------------- */
    if((pc->spd_intf ==TSCMOD_SPD_42G_X4)||
       (pc->spd_intf ==TSCMOD_SPD_40G_X4)||
       (pc->spd_intf ==TSCMOD_SPD_25455) ||
       (pc->spd_intf ==TSCMOD_SPD_21G_HI_DXGXS)||
       (pc->spd_intf ==TSCMOD_SPD_20G_DXGXS)||
       (pc->spd_intf ==TSCMOD_SPD_12773_DXGXS)) {
       data = 0xf0f0 ;                          /* default is 0xf0f0 (no change) */
       mask = 0xffff ;
       SOC_IF_ERROR_RETURN
          (MODIFY_WC40_RX_X1_CONTROL1_SCW0_MASKr(pc->unit, pc, data, mask));
    }
    /* --Addr=0x9239 ------------------------- */
    if((pc->spd_intf ==TSCMOD_SPD_42G_X4)||
       (pc->spd_intf ==TSCMOD_SPD_40G_X4)||
       (pc->spd_intf ==TSCMOD_SPD_25455) ||
       (pc->spd_intf ==TSCMOD_SPD_21G_HI_DXGXS)||
       (pc->spd_intf ==TSCMOD_SPD_20G_DXGXS)||
       (pc->spd_intf ==TSCMOD_SPD_12773_DXGXS)) {
       data = 0xf0f0 ;                          /* default is 0xf0f0 (no change) */
       mask = 0xffff ;
       SOC_IF_ERROR_RETURN
          (MODIFY_WC40_RX_X1_CONTROL1_SCW1_MASKr(pc->unit, pc, data, mask));
    }
    /* --Addr=0x923a ------------------------- */
    if((pc->spd_intf ==TSCMOD_SPD_42G_X4)||
       (pc->spd_intf ==TSCMOD_SPD_40G_X4)||
       (pc->spd_intf ==TSCMOD_SPD_25455) ||
       (pc->spd_intf ==TSCMOD_SPD_21G_HI_DXGXS)||
       (pc->spd_intf ==TSCMOD_SPD_20G_DXGXS)||
       (pc->spd_intf ==TSCMOD_SPD_12773_DXGXS)) {
        data = 0xf0f0 ;                          /* default is 0xf0f0 (no change) */
        mask = 0xffff ;
        SOC_IF_ERROR_RETURN
          (MODIFY_WC40_RX_X1_CONTROL1_SCW2_MASKr(pc->unit, pc, data, mask));
    }
    /* --Addr=0x923b ------------------------- */
    if((pc->spd_intf ==TSCMOD_SPD_42G_X4)||
       (pc->spd_intf ==TSCMOD_SPD_40G_X4)||
       (pc->spd_intf ==TSCMOD_SPD_25455) ||
       (pc->spd_intf ==TSCMOD_SPD_21G_HI_DXGXS)||
       (pc->spd_intf ==TSCMOD_SPD_20G_DXGXS)||
       (pc->spd_intf ==TSCMOD_SPD_12773_DXGXS)) {
       data = 0xf0f0 ;                          /* default is 0xf0f0 (no change) */
       mask = 0xffff ;
       SOC_IF_ERROR_RETURN
          (MODIFY_WC40_RX_X1_CONTROL1_SCW3_MASKr(pc->unit, pc, data, mask));
    }
    /* --Addr=0x923c ------------------------- */
    if((pc->spd_intf ==TSCMOD_SPD_42G_X4)||
       (pc->spd_intf ==TSCMOD_SPD_40G_X4)||
       (pc->spd_intf ==TSCMOD_SPD_25455) ||
       (pc->spd_intf ==TSCMOD_SPD_21G_HI_DXGXS)||
       (pc->spd_intf ==TSCMOD_SPD_20G_DXGXS)||
       (pc->spd_intf ==TSCMOD_SPD_12773_DXGXS)) {
       data = 0x0003 ;                          /* default is 0x0003 (no change) */
       mask = 0xffff ;
       SOC_IF_ERROR_RETURN
          (MODIFY_WC40_RX_X1_CONTROL1_SCW4_MASKr(pc->unit, pc, data, mask));
    }
    /* --Addr=0xa023 ------------------------- */
    if((pc->spd_intf ==TSCMOD_SPD_10000)||
       (pc->spd_intf ==TSCMOD_SPD_13000)||
       (pc->spd_intf ==TSCMOD_SPD_15000)||
       (pc->spd_intf ==TSCMOD_SPD_16000)||
       (pc->spd_intf ==TSCMOD_SPD_10000_HI)||
       (pc->spd_intf ==TSCMOD_SPD_20000)){
        data = RX_X2_CONTROL0_MISC_0_LINK_EN_MASK | RX_X2_CONTROL0_MISC_0_CHK_END_EN_MASK ;
        mask = RX_X2_CONTROL0_MISC_0_LINK_EN_MASK | RX_X2_CONTROL0_MISC_0_CHK_END_EN_MASK ;
        rx_control0_misc0_en = 1 ;
    } else if ((pc->spd_intf==TSCMOD_SPD_42G_X4)||
               (pc->spd_intf==TSCMOD_SPD_40G_X4)||
               (pc->spd_intf==TSCMOD_SPD_25455)||
               (pc->spd_intf==TSCMOD_SPD_21G_HI_DXGXS)||
               (pc->spd_intf==TSCMOD_SPD_20G_DXGXS)||
               (pc->spd_intf==TSCMOD_SPD_12773_DXGXS)) {
        data = RX_X2_CONTROL0_MISC_0_LINK_EN_MASK | RX_X2_CONTROL0_MISC_0_CHK_END_EN_MASK ;
        mask = RX_X2_CONTROL0_MISC_0_LINK_EN_MASK | RX_X2_CONTROL0_MISC_0_CHK_END_EN_MASK ;
        rx_control0_misc0_en = 1 ;
    }
    if(rx_control0_misc0_en) {
        SOC_IF_ERROR_RETURN (MODIFY_WC40_RX_X2_CONTROL0_MISC_0r(pc->unit, pc, data, mask));
    }
    if(pc->ctrl_type&TSCMOD_CTRL_TYPE_HG) {
       /* TD2-3612 */
       data = 0 ;
       mask = RX_X2_CONTROL0_MISC_0_CHK_END_EN_MASK ;
       SOC_IF_ERROR_RETURN (MODIFY_WC40_RX_X2_CONTROL0_MISC_0r(pc->unit, pc, data, mask));
    } else {
       data = RX_X2_CONTROL0_MISC_0_CHK_END_EN_MASK ;
       mask = RX_X2_CONTROL0_MISC_0_CHK_END_EN_MASK ;
       SOC_IF_ERROR_RETURN (MODIFY_WC40_RX_X2_CONTROL0_MISC_0r(pc->unit, pc, data, mask));
    }
    /* --Addr=0xc131 ------------------------- */
    if((pc->spd_intf == TSCMOD_SPD_20G_MLD_DXGXS) ||
       (pc->spd_intf == TSCMOD_SPD_21G_HI_MLD_DXGXS) ||
       (pc->spd_intf == TSCMOD_SPD_40G_MLD) ||
       (pc->spd_intf == TSCMOD_SPD_42G_MLD) ||
       (pc->spd_intf == TSCMOD_SPD_12773_HI_DXGXS) ) {
        data = 0 ;
        mask = 0xffff ;
        SOC_IF_ERROR_RETURN
        (MODIFY_WC40_RX_X4_CONTROL0_FEC_0r(pc->unit, pc, data, mask));
    }
    /* --Addr=0x9123 ------------------------- */
    /* for simualtion                          */
#if defined (_DV_TRIDENT2)
    if((pc->spd_intf == TSCMOD_SPD_40G_MLD)||(pc->spd_intf == TSCMOD_SPD_42G_MLD)) {
        data = 0x03ff ;
        mask = 0xffff ;
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_CL82_SHARED_CL82_RX_AM_TIMERr
             (pc->unit, pc, data, mask));
    }
    if((pc->spd_intf == TSCMOD_SPD_20G_MLD_DXGXS)||
       (pc->spd_intf == TSCMOD_SPD_21G_HI_MLD_DXGXS) ||
       (pc->spd_intf == TSCMOD_SPD_12773_HI_DXGXS)) {
        data = 0x03ff ;       /* shorthen from 0x3fff */
        mask = 0xffff ;
        SOC_IF_ERROR_RETURN
            (MODIFY_WC40_CL82_SHARED_CL82_RX_AM_TIMERr
             (pc->unit, pc, data, mask));
    }
#endif

    /* Sample snippet showing how to access the alignment marker regs 
      --Addr=0x9130 ------------------------- 
    if((pc->spd_intf == TSCMOD_SPD_20G_MLD_DXGXS)) {
        data = 0x7690 ;
        mask = 0xffff ;
        SOC_IF_ERROR_RETURN
        (MODIFY_WC40_CL82_AM_REGS_LANE_0_AM_BYTE10r(pc->unit, pc, data, mask));
    } 
      --Addr=0x9131 ------------------------- 
    if((pc->spd_intf == TSCMOD_SPD_20G_MLD_DXGXS)) {
        data = 0xc4f0 ;
        mask = 0xffff ;
        SOC_IF_ERROR_RETURN
        (MODIFY_WC40_CL82_AM_REGS_LANE_1_AM_BYTE10r(pc->unit, pc, data, mask));
    } 
      --Addr=0x9132 ------------------------- 
    if((pc->spd_intf == TSCMOD_SPD_20G_MLD_DXGXS)) {
        data = 0xe647 ;
        mask = 0xffff ;
        SOC_IF_ERROR_RETURN
        (MODIFY_WC40_CL82_AM_REGS_LANES_1_0_AM_BYTE2r(pc->unit, pc, data, mask));
    } */


    /* CDRBR select. 0xc21e. when uCode running, no need for the codes later*/
    /* SDK-46061  no SW DSC update
    if( pc->ctrl_type &  TSCMOD_CTRL_TYPE_FW_AVAIL ) {
       data = 0 << DSC2B0_DSC_MISC_CTRL0_CDRBR_SEL_FORCE_SHIFT |
              0 << DSC2B0_DSC_MISC_CTRL0_CDRBR_SEL_FORCE_VAL_SHIFT ;
    } else {
       if( os_mode ) {
          data = 1 << DSC2B0_DSC_MISC_CTRL0_CDRBR_SEL_FORCE_SHIFT |
                 0 << DSC2B0_DSC_MISC_CTRL0_CDRBR_SEL_FORCE_VAL_SHIFT ;
       } else {
          data = 1 << DSC2B0_DSC_MISC_CTRL0_CDRBR_SEL_FORCE_SHIFT |
                 1 << DSC2B0_DSC_MISC_CTRL0_CDRBR_SEL_FORCE_VAL_SHIFT ;
       }
    }
    mask = DSC2B0_DSC_MISC_CTRL0_CDRBR_SEL_FORCE_MASK |
           DSC2B0_DSC_MISC_CTRL0_CDRBR_SEL_FORCE_VAL_MASK ;
    SOC_IF_ERROR_RETURN
       (MODIFY_WC40_DSC2B0_DSC_MISC_CTRL0r(pc->unit, pc, data, mask));
    */

    /* CL72 tap selection. 0xc253 */
    /* add HW tuning  */
    hw_tuning = 0 ; data = 0 ;
    if(os_mode==0) {  /* os_mode 1 */
       if(pc->plldiv>=66) { /* >= 10.3125G, select KR mode */
          data = 3 << CL72_MISC1_CONTROL_TAP_DEFAULT_MUXSEL_FORCEVAL_SHIFT ;
          hw_tuning = 1 ;
       } else if (pc->plldiv >= 40 ) {  /* >= 6.25, select BR mode */
          data = 2 << CL72_MISC1_CONTROL_TAP_DEFAULT_MUXSEL_FORCEVAL_SHIFT ;
          hw_tuning = 1 ;
       } else if (pc->plldiv >= 20) { /* >= 3.125, select OS mode */
          data = 1 << CL72_MISC1_CONTROL_TAP_DEFAULT_MUXSEL_FORCEVAL_SHIFT ;
       } else {  /* < 3.125, select KX mode */
          data = 0 << CL72_MISC1_CONTROL_TAP_DEFAULT_MUXSEL_FORCEVAL_SHIFT ;
       }
    } else {
       if((pc->plldiv==66)&&(os_mode==3)) {  /* special case os_mode 3.3, select 2p5 mode */
          data = 4 << CL72_MISC1_CONTROL_TAP_DEFAULT_MUXSEL_FORCEVAL_SHIFT ;
       } else if(pc->spd_intf==TSCMOD_SPD_10000_HI){
          /* special os_mode 2 case that pll_rate=date rate. The rest of os_mode 2 has pll_rate=2*data_rate */
          data = 2 << CL72_MISC1_CONTROL_TAP_DEFAULT_MUXSEL_FORCEVAL_SHIFT ;
       } else if(os_mode==1) {  /* os_mode 2 */
          data = 1 << CL72_MISC1_CONTROL_TAP_DEFAULT_MUXSEL_FORCEVAL_SHIFT ;
       } else {
          /* so far the reset is below 2.5G, so KX mode */
          data = 0 << CL72_MISC1_CONTROL_TAP_DEFAULT_MUXSEL_FORCEVAL_SHIFT ;
       }
    }
    data = data | CL72_MISC1_CONTROL_TAP_DEFAULT_MUXSEL_FORCE_MASK ;
    mask = CL72_MISC1_CONTROL_TAP_DEFAULT_MUXSEL_FORCE_MASK |
           CL72_MISC1_CONTROL_TAP_DEFAULT_MUXSEL_FORCEVAL_MASK ;
    if (pc->ctrl_type & TSCMOD_CTRL_TYPE_UC_WR_MASK_EN) {
         SOC_IF_ERROR_RETURN
            (tscmod_mask_wr_cmd(pc, TSC40_CL72_MISC1_CONTROLr, data, mask)) ;
    } else {
       SOC_IF_ERROR_RETURN
          (MODIFY_WC40_CL72_MISC1_CONTROLr(pc->unit, pc, data, mask));
    }

    if(pc->ctrl_type & TSCMOD_CTRL_TYPE_FW_AVAIL) hw_tuning = 0 ;
    if(hw_tuning) {
       data = DSC4B0_TUNING_SM_CTRL0_TUNING_SM_EN_MASK ;
       mask = DSC4B0_TUNING_SM_CTRL0_TUNING_SM_EN_MASK ;
       /* SDK-46061 no SW DSC update.
       SOC_IF_ERROR_RETURN
          (MODIFY_WC40_DSC4B0_TUNING_SM_CTRL0r(pc->unit, pc, data, mask));
       */
    } else {
       /* SDK-46061 no SW DSC update.
       data = 0 ;
       mask = DSC4B0_TUNING_SM_CTRL0_TUNING_SM_EN_MASK ;
       SOC_IF_ERROR_RETURN
          (MODIFY_WC40_DSC4B0_TUNING_SM_CTRL0r(pc->unit, pc, data, mask));
       */
    }

    if(pc->ctrl_type & TSCMOD_CTRL_TYPE_FAULT_DIS) {
       pc->per_lane_control = TSCMOD_FAULT_CL49 | TSCMOD_FAULT_CL48 | TSCMOD_FAULT_CL82 ;
       /* li, lf, rf =0 */
       tscmod_lf_rf_control(pc) ;
    } else {
       pc->per_lane_control =
          (TSCMOD_FAULT_RF | TSCMOD_FAULT_LF | TSCMOD_FAULT_LI)<<4 |
          (TSCMOD_FAULT_CL49 | TSCMOD_FAULT_CL48 | TSCMOD_FAULT_CL82) ;
       tscmod_lf_rf_control(pc) ;
    }
  } else {  /* if ~cntl */
    /* no op */
     return SOC_E_NONE;
  }
  return SOC_E_NONE;
}



/*!
\brief Programs the speed mode for a particular lane

\param  pc handle to current TSC context (#tscmod_st)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.
         pc->accData nozero if PLL is needed to restarted with different value;
\details

This function sets the TSC into a particular speed mode if per_lane_control= (1|(aspd<<4)).

If per_lane_control=0, the function tells us PLLdiv is required to be changed or
not by return the new pll value in pc->accData.  If no change is required, then return 0 in
accData.
If per_lane_control & 1; set to the desired speed.

To set the
desired speed, set the following fields of #tscmod_st

\li #tscmod_st::lane_select to the lane number(s)
\li #tscmod_st::spd_intf to a value from the enumerated type #tscmod_spd_intfc_type
\li #tscmod_st::port_type to a type from the enumerated type #tscmod_port_type
\li #tscmod_st::os_type to a type from the enumerated type #tscmod_os_type

Note that legitimate combinations for these fields must be used; otherwise
ill-defined behavior may result.

This function must be called once for combo mode and once per lane for
indepedent lane mode. Only if all lanes are being set to the same
speed-interface can this function be called once for all lanes. Before calling
this function, the PLL sequencer must be disabled via the function
#tscmod_pll_sequencer_control().  After calling this function, the PLL sequencer
must be enabled via the same function.

To program multiple lanes in independent lane mode, it is recommended to
disable the PLL sequencer, call this function once for every lane being
programmed, and then enable the PLL sequencer.

This function modifies the following registers:

\li force_ln_mode (0xC001[8]), force_speed (0XC001[7:0]), force_oscdr_mode_val (0xC001[14:11])
\li set pcs mode (encodeMode) (0xC110[4:2])

The following speeds are not implemented.
\li TSCMOD_SPD_10000_DXGXS_SCR
\li TSCMOD_SPD_15000
\li TSCMOD_SPD_2000
\li TSCMOD_SPD_5000_SINGLE
\li TSCMOD_SPD_6364_SINGLE

When accData return 1, upper layer needs to adjust the plldiv of other lanes/tscmod
to prevent unnecessary PLL restart when other lanes/tscmod is called.

<B>How to call:</B><br>
<p>Call this function directly or through the function #tscmod_tier1_selector()
with the upper-case string "SET_SPD_INTF".</p>
*/

int tscmod_set_spd_intf(tscmod_st* pc)                  /* SET_SPD_INTF */
{

   /* uint16 speed_val, speed_mii, oscdr_val; */
   /* uint32 this_lane; */
   int    plldiv, plldiv_r_val,  rv;

  tscmod_spd_intfc_type spd_intf;
  int    actual_spd ;
  uint16 data ;
  int    port_mode_sel_restart ;
  int    firmware_mode = 0 ;
  /*
     0x0 = DEFAULT
     0x1 = SFP_OPT
     0x2 = SFP_DAC
     0x3 = XLAUI
     0x4 = FORCE_OSDFE
     0x5 = FORCE_BRDFE
     0x6 = SW_CL72
     0x7 = CL72_woAN
     0x8 = SFP_OPT_PF_TEMP_COMP
  */
  /* #define SR4                     0x1
     #define SFI_DAC                 0x2
     #define XLAUI                   0x3
     #define FORCE_OSDFE             0x4
     #define FORCE_BRDFE             0x5
     #define SW_CL72                 0x6
     #define FORCED_MODE_CL72        0x7
     #define FORCE_OS                0xF
  */

  spd_intf    = pc->spd_intf;

  pc->accAddr = ((pc->unit) << 16) | (pc->port) ;

  if (pc->spd_intf > TSCMOD_SPD_ILLEGAL) {
    if (pc->verbosity > 0) printf("%-22s FATAL: Bad spd-intf: %d > TSCMOD_SPD_ILLEGAL\n",
                                                        FUNCTION_NAME(), pc->spd_intf);
    return SOC_E_PARAM;
  } else {
     if (pc->verbosity & TSCMOD_DBG_SPD)
        printf("%-22s: %s[%d]\n", FUNCTION_NAME(), e2s_tscmod_spd_intfc_type[pc->spd_intf],
                                        e2n_tscmod_spd_intfc_type[pc->spd_intf]);
  }

  /* pll div calculation; assume refclk=156 */
  plldiv = 66 ; /* default */
  if        (spd_intf == TSCMOD_SPD_10_SGMII)   {
      if((pc->plldiv==40)||(pc->plldiv==66)) {
          plldiv = pc->plldiv;
      } else {
          plldiv      = 40;
      }
  } else if (spd_intf == TSCMOD_SPD_100_SGMII)  {
      if((pc->plldiv==40)||(pc->plldiv==66)) {
          plldiv = pc->plldiv;
      } else {
          plldiv      = 40;
      }
  } else if (spd_intf == TSCMOD_SPD_1000_SGMII) {
      if((pc->plldiv==40)||(pc->plldiv==66)) {
          plldiv = pc->plldiv;
      } else {
          plldiv      = 40;
      }
  } else if (spd_intf == TSCMOD_SPD_2500) {
      if((pc->plldiv==40)||(pc->plldiv==66)) {
          plldiv = pc->plldiv;
      } else {
          plldiv      = 40;
      }
      /* JIRA CRTSC-698 */
      plldiv      = 40;
  } else if (spd_intf == TSCMOD_SPD_5000) {
     plldiv      = 66 ;

  } else if (spd_intf == TSCMOD_SPD_10000_XFI) {
     plldiv = 66 ;
     if(pc->iintf==TSCMOD_IIF_SFPDAC) firmware_mode = 2 ;
     else                             firmware_mode = 0 ;
  } else if (spd_intf == TSCMOD_SPD_10600_XFI_HG) {
     plldiv = 70 ;
     if(pc->iintf==TSCMOD_IIF_SFPDAC) firmware_mode = 2 ;
     else                             firmware_mode = 0 ;
  } else if (spd_intf == TSCMOD_SPD_10000_HI) {
     plldiv      = 42;   /* dr_10GHiGig_X4; */
  } else if (spd_intf == TSCMOD_SPD_10000) {
     plldiv      = 40;   /* dr_10GBASE_CX4; */
  } else if (spd_intf == TSCMOD_SPD_12000_HI) {
     /* speed_val = aer_actualSpeedsMisc1_dr_12GHiGig_X4; */
  } else if (spd_intf == TSCMOD_SPD_13000) {
     plldiv = 52 ;
  } else if (spd_intf == TSCMOD_SPD_15000) {
     plldiv = 60 ;
  } else if (spd_intf == TSCMOD_SPD_16000) {
     plldiv = 64 ;
  } else if (spd_intf == TSCMOD_SPD_20000) {
     /* use this mode for 20G XAUI */
     plldiv = 40 ;
  } else if (spd_intf == TSCMOD_SPD_20000_SCR) {
     plldiv = 40 ;
  } else if (spd_intf == TSCMOD_SPD_21000) {
     plldiv = 42 ;
  } else if (spd_intf == TSCMOD_SPD_25455) {
     plldiv = 42 ;   /* 40G_X4 like */
  } else if (spd_intf == TSCMOD_SPD_31500) {
     plldiv = 52; 
  } else if (spd_intf == TSCMOD_SPD_40G_X4) {
     plldiv = 66 ;
     firmware_mode = 3 ;
  } else if (spd_intf == TSCMOD_SPD_42G_X4) {
     plldiv = 70 ;
     firmware_mode = 3 ;
  } else if(spd_intf == TSCMOD_SPD_40G_MLD) {
     plldiv = 66 ;
     if (pc->iintf == TSCMOD_IIF_XLAUI) {
         firmware_mode = 3 ;
     } else if(pc->iintf == TSCMOD_IIF_SR4) {
         firmware_mode = 1 ;
     } else {
         firmware_mode = 0 ;
     }

  } else if(spd_intf == TSCMOD_SPD_42G_MLD) {
     plldiv = 70 ;
     if (pc->iintf == TSCMOD_IIF_XLAUI) {
         firmware_mode = 3 ;
     } else if(pc->iintf == TSCMOD_IIF_SR4) {
         firmware_mode = 1 ;
     } else {
         firmware_mode = 0 ;
     }

  } else if (spd_intf == TSCMOD_SPD_10000_DXGXS) {
     /* use this mode for 10G RXAUI */
     plldiv = 40 ;
  } else if (spd_intf == TSCMOD_SPD_10000_HI_DXGXS) {
     /* speed_val = aer_actualSpeedsMisc1_dr_10G_HiG_DXGXS; */
  } else if (spd_intf == TSCMOD_SPD_10500_HI_DXGXS) {
     /* speed_val =  0x21; */
  } else if (spd_intf == TSCMOD_SPD_12773_HI_DXGXS) {
     plldiv =  42;   /* MLD stype. IPG 20b */
  } else if (spd_intf == TSCMOD_SPD_12773_DXGXS) {
     plldiv =  42;   /* use HG */
  } else if (spd_intf == TSCMOD_SPD_15750_HI_DXGXS)  {

  } else if (spd_intf == TSCMOD_SPD_10000_HI_DXGXS_SCR) {

  } else if (spd_intf == TSCMOD_SPD_10000_X2)  {

  } else if (spd_intf == TSCMOD_SPD_20G_DXGXS) {
     plldiv = 66 ;
  } else if (spd_intf == TSCMOD_SPD_20G_MLD_DXGXS) {
     plldiv = 66 ;


  } else if (spd_intf == TSCMOD_SPD_21G_HI_DXGXS) {
    /* 21G is same as 20G, plldiv=70)*/
    plldiv = 70 ;
  } else if (spd_intf == TSCMOD_SPD_21G_HI_MLD_DXGXS) {
    /* 21G is same as 20G, plldiv=70)*/
    plldiv = 70 ;
  }

  rv = SOC_E_NONE;

  port_mode_sel_restart = 0 ;

  if(pc->per_lane_control !=0x2) {
     tscmod_update_port_mode_select(pc) ;
     port_mode_sel_restart = pc->accData ;
  }

  pc->accData = 0;
  /* read back plldiv  */
  plldiv_r_val = tscmod_get_plldiv(pc) ;

  if((plldiv != plldiv_r_val)||port_mode_sel_restart) {  /* re-do PLL sequence!! */
     /* include port_mode_sel_restart for case single_port_mode 1(AN) ->0  */
     pc->accData = plldiv ;

     if(pc->verbosity & TSCMOD_DBG_SPD)
        printf("%-22s: p=%0d should restart new plldiv:%d from %0d reg=%0d restart=%0d\n",
               FUNCTION_NAME(), pc->port, plldiv, pc->plldiv, plldiv_r_val, port_mode_sel_restart);
     if( pc->per_lane_control & 0x1 ) {
        if(plldiv != plldiv_r_val) {
           pc->ctrl_type |= TSCMOD_CTRL_TYPE_ERR ;
           pc->err_code  |= TSCMOD_ERR_PLLDIV_DIFF   ;
        }
        if(port_mode_sel_restart) {
           pc->ctrl_type |= TSCMOD_CTRL_TYPE_MSG ;
           pc->msg_code  |= TSCMOD_MSG_PORT_MODE_SEL_RST ;
        }
        pc->plldiv = plldiv ;
        if((pc->verbosity & (TSCMOD_DBG_PRINT|TSCMOD_DBG_INIT|TSCMOD_DBG_SPD)))
           printf("Warning: u=%0d p=%0d TSC unexpected port mode change invoked plldiv %0d %0d reg=%0d restart_mode=%0d cntl=%0d pc=%p\n",
                  pc->unit, pc->port, plldiv, pc->plldiv, plldiv_r_val, port_mode_sel_restart, pc->per_lane_control, (void *)pc) ;
        pc->msg_code  |= TSCMOD_MSG_UC_STALL ;
        pc->ctrl_type |= TSCMOD_CTRL_TYPE_MSG ;
        if(plldiv != plldiv_r_val)
           tscmod_set_port_mode(pc) ;
     }
  }

  pc->plldiv = plldiv_r_val ;  /* sync up with reality away from user inputs */
  rv = SOC_E_NONE;

  if(pc->per_lane_control == 0 ) {
     if( pc->verbosity & TSCMOD_DBG_SPD)
        printf("%-22s: p=%0d set_spd_intf probed\n", FUNCTION_NAME(), pc->port);
     return rv ;
  }
  if(pc->per_lane_control == 2) {
     pc->accData = plldiv_r_val ;
     if( pc->verbosity & TSCMOD_DBG_SPD)
        printf("%-22s: p=%0d set_spd_intf get reg plldiv=%0d\n",
               FUNCTION_NAME(), pc->port, plldiv_r_val);
     return rv ;
  }

  /* setting actual speed 0xc254 */
  actual_spd = pc->per_lane_control >> 4 ;
  data = (actual_spd << CL72_MISC2_CONTROL_SW_ACTUAL_SPEED_SHIFT ) ;
  if( pc->verbosity & TSCMOD_DBG_SPD)
     printf("%-22s: p=%0d wr actual_spd_vec=%0x verb=%x\n", FUNCTION_NAME(), pc->port, actual_spd, pc->verbosity);
  SOC_IF_ERROR_RETURN
     (MODIFY_WC40_CL72_MISC2_CONTROLr
      (pc->unit, pc, data, CL72_MISC2_CONTROL_SW_ACTUAL_SPEED_MASK));
  /*
  SOC_IF_ERROR_RETURN
     (READ_WC40_CL72_MISC2_CONTROLr
      (pc->unit, pc, &data));
  actual_spd = (data & CL72_MISC2_CONTROL_SW_ACTUAL_SPEED_MASK)>>
     CL72_MISC2_CONTROL_SW_ACTUAL_SPEED_SHIFT;

  if( pc->verbosity & TSCMOD_DBG_SPD)
     printf("%-22s: p=%0d actual_spd_vec=%0x rd back\n", FUNCTION_NAME(), pc->port, actual_spd);
  */

  if(pc->firmware_mode>=0) {
     firmware_mode = pc->firmware_mode ;
  }
  /* 0xc260 */
  SOC_IF_ERROR_RETURN
     (WRITE_WC40_UCSS_X4_FIRMWARE_MODEr(pc->unit, pc, firmware_mode));

  /*workaround for 100M/10M.  for 100M/10M, SW needs to 
    call credit_set, encode_set, and decode_set directly */

  if ((spd_intf == TSCMOD_SPD_100_SGMII) ||
       (spd_intf == TSCMOD_SPD_10_SGMII)) {
     pc->spd_intf = TSCMOD_SPD_1000_SGMII;
  }
  pc->per_lane_control=1 ;
  rv |= tscmod_credit_set(pc) ;
  
  pc->per_lane_control=1 ;
  rv |= tscmod_encode_set(pc);
  pc->per_lane_control=1 ;
  rv |= tscmod_decode_set(pc);

  if ((spd_intf == TSCMOD_SPD_100_SGMII) ||
      (spd_intf == TSCMOD_SPD_10_SGMII)) {
      pc->spd_intf = spd_intf;
  }

  return rv ;

} /* tscmod_set_spd_intf(tscmod_st* pc) */

int tscmod_speed_intf_control(tscmod_st* pc)              /* SPEED_INTF_CONTROL */
{
  /* fix this */
  return SOC_E_ERROR;
}


/*!
\brief Hold/Release/Read receiver sequencer (CDR) for a particular lane

\param  pc handle to current TSC context (#tscmod_st)

\returns The value #SOC_E_NONE upon successful completion.

This function has different functions as defined by passed in value of
#tsmod_st::per_lane_control.

\li Here are the features per value of per_lane_control.
\li 0x0  = rmw to set RxSeqStart bit to 1 of register DSC2B0_DSC_MISC_CTRL0 (0xc21E).
\li 0x1  = rmw to set RxSeqStart bit to 0 of register DSC2B0_DSC_MISC_CTRL0 (0xc21E).
\li 0x10 = Returns the PMD lock bit of PMD live status register for lane as specified by tscmod_st::this_lane.

<B>How to call:</B><br>
<p>Call this function directly or through the function
#tscmod_tier1_selector() with the upper-case string "RX_SEQ_CONTROL".</p>
*/

int tscmod_rx_seq_control(tscmod_st* pc)         /* START_RX_SEQ or STOP_RX_SEQ */
{
   uint16 data, prev_aer;
   int cntl, new_lane ;

   cntl     = pc->per_lane_control;
   new_lane = pc->this_lane;

   /* Check to see if new lane is valid. */
   if ((new_lane != 0)&&(new_lane != 1)&&(new_lane != 2)&&(new_lane != 3)) {
      printf("Error: u=%0d p=%0d invalid this_lane %d\n", new_lane, pc->unit, pc->port);
      return SOC_E_ERROR;
   }

   /* Save the current AER. */
   SOC_IF_ERROR_RETURN(READ_WC40_AER_ADDRESSEXPANSIONr(pc->unit,pc,&prev_aer));

   /* Select the new lane. */
   SOC_IF_ERROR_RETURN(WRITE_WC40_AER_ADDRESSEXPANSIONr(pc->unit,pc,new_lane));

   if (cntl & 0x10) {
      /*
       * Description of per_lane_control code of 0x10, which is the same as in WC.
       *  This option returns the status of the link.  Returns 1 if link is up and live,
       *  returns 0 otherwise.
       */

      /* Read back the PMD live status. */
      SOC_IF_ERROR_RETURN(READ_WC40_RX_X4_STATUS1_PMA_PMD_LIVE_STATUSr(pc->unit,pc,&data));
      if (data & RX_X4_STATUS1_PMA_PMD_LIVE_STATUS_PMD_LOCK_MASK) {
          pc->accData = 1;
      } else {
          pc->accData = 0;
      }
   } else if (cntl & 0x1) {
      /* This was ported from WC C0 driver. Code didn't change. */
      if (pc->ctrl_type & TSCMOD_CTRL_TYPE_UC_WR_MASK_EN) {
         SOC_IF_ERROR_RETURN
            (tscmod_mask_wr_cmd(pc, TSC40_DSC2B0_DSC_MISC_CTRL0r, 0, DSC2B0_DSC_MISC_CTRL0_RXSEQSTART_MASK)) ;
      } else {
         SOC_IF_ERROR_RETURN (MODIFY_WC40_DSC2B0_DSC_MISC_CTRL0r(pc->unit, pc, 0,
                                    DSC2B0_DSC_MISC_CTRL0_RXSEQSTART_MASK));
      }
   } else if (cntl == 0x0) {
      /* This was ported from WC C0 driver. Code didn't change. */
      if (pc->ctrl_type & TSCMOD_CTRL_TYPE_UC_WR_MASK_EN) {
         SOC_IF_ERROR_RETURN
            (tscmod_mask_wr_cmd(pc, TSC40_DSC2B0_DSC_MISC_CTRL0r,
                                 DSC2B0_DSC_MISC_CTRL0_RXSEQSTART_MASK,
                                 DSC2B0_DSC_MISC_CTRL0_RXSEQSTART_MASK)) ;
      } else {
         SOC_IF_ERROR_RETURN (MODIFY_WC40_DSC2B0_DSC_MISC_CTRL0r(pc->unit, pc,
                                 DSC2B0_DSC_MISC_CTRL0_RXSEQSTART_MASK,
                                 DSC2B0_DSC_MISC_CTRL0_RXSEQSTART_MASK));
      }
   } else {
      /* Restore the saved AER. */
      SOC_IF_ERROR_RETURN(WRITE_WC40_AER_ADDRESSEXPANSIONr(pc->unit,pc,prev_aer));

      printf("Error: u=%0d p=%0d invalid per_lane_control %d\n", cntl, pc->unit, pc->port);
      return SOC_E_ERROR;
   }

   /* Restore the saved AER. */
   SOC_IF_ERROR_RETURN(WRITE_WC40_AER_ADDRESSEXPANSIONr(pc->unit,pc,prev_aer));

   return SOC_E_NONE ;
}


int tscmod_prbs_credit_set(tscmod_st* pc)
{
   uint16 clockcnt0, clockcnt1, clockcnt1_rsv, loopcnt0, loopcnt1, mac_credit, programmed ;
   uint16 data_credit0, data_clkcnt1, data_loopcnt, data_mac_credit ;
   uint16 mask_loopcnt ;

   clockcnt0        = 0;
   clockcnt1        = 0;
   clockcnt1_rsv    = 0;
   loopcnt0         = 0;
   loopcnt1         = 0;
   programmed       = 0;
   mac_credit       = 0;

   if((pc->spd_intf == TSCMOD_SPD_10000)||
      (pc->spd_intf == TSCMOD_SPD_10000_HI)) {
      programmed       = 1 ;
      clockcnt0        = 10 ;
      loopcnt0         = 1 ;
      loopcnt1         = 0 ;
      mac_credit       = 0x6;
   }
   data_credit0     = clockcnt0 << TX_X4_CREDIT0_CREDIT0_CLOCKCNT0_SHIFT;
   data_clkcnt1     = clockcnt1 << TX_X4_CREDIT0_CREDIT1_CLOCKCNT1_SHIFT |
                      clockcnt1_rsv << TX_X4_CREDIT0_CREDIT1_RESERVED0_SHIFT;
   data_loopcnt     = loopcnt0 << TX_X4_CREDIT0_LOOPCNT_LOOPCNT0_SHIFT |
		                loopcnt1 << TX_X4_CREDIT0_LOOPCNT_LOOPCNT1_SHIFT;
   data_mac_credit  = mac_credit << TX_X4_CREDIT0_MAC_CREDITGENCNT_MAC_CREDITGENCNT_SHIFT;
   mask_loopcnt = TX_X4_CREDIT0_LOOPCNT_LOOPCNT0_MASK | TX_X4_CREDIT0_LOOPCNT_LOOPCNT1_MASK;

   if(programmed) {
      SOC_IF_ERROR_RETURN
         (MODIFY_WC40_TX_X4_CREDIT0_CREDIT0r(pc->unit, pc, data_credit0, TX_X4_CREDIT0_CREDIT0_CLOCKCNT0_MASK));
      SOC_IF_ERROR_RETURN
         (MODIFY_WC40_TX_X4_CREDIT0_CREDIT1r(pc->unit, pc, data_clkcnt1, 0xffff));
      SOC_IF_ERROR_RETURN
         (MODIFY_WC40_TX_X4_CREDIT0_LOOPCNTr(pc->unit, pc, data_loopcnt, mask_loopcnt));
      SOC_IF_ERROR_RETURN
         (MODIFY_WC40_TX_X4_CREDIT0_MAC_CREDITGENCNTr(pc->unit, pc, data_mac_credit,
                              TX_X4_CREDIT0_MAC_CREDITGENCNT_MAC_CREDITGENCNT_MASK));
   }
   return SOC_E_NONE;
}

/*!
\brief Control of resetting and setting of credits based on speed interface setelction

\param  pc handle to current TSC context (#tscmod_st)

\returns The value #SOC_E_NONE upon successful completion.

\details

This function resets all the credits. And sets the credits for PCS and MAC as per speed interface selected.
set #tscmod_st::per_lane_control to 1 to set the credits and set it to 0
to clear the redits.

This function is controlled by the following register bit:

\li set credits: clockcnt0 (0xC100[13:0])
\li clockcnt1 (0xC101[7:0])
\li loopcnt0 (0xC102[13:6]), loopcnt1 (0xC102[5:0])
\li mac_creditgencnt (0xC103[12:0])
\li pcs_clockcnt0 (0xC104[13:0])
\li pcs_creditgencnt (0xC105[12:0])

<B>How to call:</B><br>
<p>Call this function directly or through the function #tscmod_tier1_selector()
using the upper case string "CREDIT_SET".</p>
*/

int tscmod_credit_set(tscmod_st* pc)   /* CREDIT_SET */
{
  uint16 data_credit0, data_clkcnt1, data_loopcnt, replication_cnt, pcs_clkcnt0, pcs_creditgencnt;
  uint16 data_mac_credit,data_pcs_clkcnt0, data_pcs_credit;
  uint16 clockcnt0, clockcnt1, clockcnt1_rsv, loopcnt0, loopcnt1, mac_credit;
  uint16 sgmii_spd_switch, mask_credit0, mask_loopcnt;
  /* uint16 data_sgmii_spd_switch;  */
  int cntl;

  cntl = pc->per_lane_control;
  /* data_sgmii_spd_switch = 0; */
  sgmii_spd_switch = 0;
  data_credit0     = 0;
  data_clkcnt1     = 0;
  data_loopcnt     = 0;
  data_mac_credit  = 0;
  data_pcs_clkcnt0 = 0;
  data_pcs_credit  = 0;
  clockcnt0        = 0;
  clockcnt1        = 0;
  clockcnt1_rsv    = 0;
  loopcnt0         = 1;
  loopcnt1         = 0;
  mac_credit       = 0;
  replication_cnt  = 0;
  pcs_clkcnt0      = 0x0;
  pcs_creditgencnt = 0x0;


  if (cntl) {
    if(pc->spd_intf == TSCMOD_SPD_10_SGMII) {
      if (pc->plldiv == 40) {
        sgmii_spd_switch    = 0x1;
        clockcnt0           = 0x9c4;
        clockcnt1_rsv       = 0x80;
        loopcnt0            = 0x1;
        mac_credit          = 0x9c3;
        replication_cnt     = 0x1;
        pcs_clkcnt0         = 0x19;
        pcs_creditgencnt    = 0x18;
      } else if (pc->plldiv == 66){
        sgmii_spd_switch    = 0x1;
        clockcnt0           = 0x101d;
        clockcnt1_rsv       = 0x80;
        loopcnt0            = 0x1;
        mac_credit          = 0x101c;
        replication_cnt     = 0x1;
        pcs_clkcnt0         = 0xa5;
        pcs_creditgencnt    = 0x29;
      }
    } else if(pc->spd_intf == TSCMOD_SPD_100_SGMII) {
      if (pc->plldiv == 40) {
        sgmii_spd_switch    = 0x1;
        clockcnt0           = 0xfa;
        clockcnt1_rsv       = 0x81;
        loopcnt0            = 0x1;
        mac_credit          = 0xf9;
        pcs_clkcnt0         = 0x19;
        pcs_creditgencnt    = 0x18;
      } else if (pc->plldiv == 66){
        sgmii_spd_switch    = 0x1;
        clockcnt0           = 0x339;
        clockcnt1_rsv       = 0x81;
        loopcnt0            = 0x1;
        mac_credit          = 0x19c;
        pcs_clkcnt0         = 0xa5;
        pcs_creditgencnt    = 0x29;
      }
    } else if(pc->spd_intf == TSCMOD_SPD_1000_SGMII) {
      if (pc->plldiv == 40) {
          clockcnt0           = 0x19;
          clockcnt1_rsv       = 0x82;
          loopcnt0            = 0x1;
          mac_credit          = 0x18;
      } else if (pc->plldiv == 66){
          clockcnt0           = 0xa5;
          clockcnt1_rsv       = 0x82;
          loopcnt0            = 0x1;
          mac_credit          = 0x29;
      }
    } else if(pc->spd_intf == TSCMOD_SPD_2500) {
        if (pc->plldiv == 40) {
            clockcnt0           = 0xa;
            clockcnt1_rsv       = 0x83;
            loopcnt0            = 0x1;
            mac_credit          = 0x6;
        } else if (pc->plldiv == 66){
            clockcnt0           = 0x21;
            clockcnt1_rsv       = 0x83;
            loopcnt0            = 0x1;
            mac_credit          = 0x10;
        }
    } else if((pc->spd_intf == TSCMOD_SPD_10000_XFI)||
              (pc->spd_intf == TSCMOD_SPD_10600_XFI_HG)||
              (pc->spd_intf == TSCMOD_SPD_5000)) {
        sgmii_spd_switch    = 0x0;
        clockcnt0           = 0x21;
        clockcnt1_rsv       = 0x84;
        loopcnt0            = 0x1;
        mac_credit          = 0x4;
        pcs_clkcnt0         = 0x0;
        pcs_creditgencnt    = 0x0;

        if(pc->spd_intf == TSCMOD_SPD_5000)
           mac_credit          = 0x8;

    } else if(pc->spd_intf == TSCMOD_SPD_10000 ) {   /* 10G-XAUI */
        sgmii_spd_switch    = 0x0;
        clockcnt0           = 0x5;
        clockcnt1_rsv       = 0x8d;
        loopcnt1            = 0x0;
        mac_credit          = 0x2;
    } else if(pc->spd_intf == TSCMOD_SPD_10000_HI){   /* 10G-XAUI */
        sgmii_spd_switch    = 0x0;
        clockcnt0           = 0x5;
        clockcnt1_rsv       = 0x8d;
        loopcnt1            = 0x0;
        mac_credit          = 0x2;
    } else if(pc->spd_intf == TSCMOD_SPD_10000_DXGXS) { /* 10G RXAUI */
       sgmii_spd_switch    = 0x0;
       clockcnt0           = 0x5;
       clockcnt1_rsv       = 0x0;
       loopcnt1            = 0x0;
       mac_credit          = 0x2;
    } else if((pc->spd_intf == TSCMOD_SPD_13000)||
              (pc->spd_intf == TSCMOD_SPD_15000)||
              (pc->spd_intf == TSCMOD_SPD_16000)) { /* 13G XAUI */
       sgmii_spd_switch    = 0x0;
       clockcnt0           = 0x5;
       clockcnt1_rsv       = 0x0;
       loopcnt1            = 0x0;
       mac_credit          = 0x2;
    } else if((pc->spd_intf == TSCMOD_SPD_20000)||
              (pc->spd_intf == TSCMOD_SPD_20000_SCR)) { /* 20G XAUI */
       sgmii_spd_switch    = 0x0;
       clockcnt0           = 0x5;
       clockcnt1_rsv       = 0x0;
       loopcnt1            = 0x0;
       mac_credit          = 0x1;
    } else if(pc->spd_intf == TSCMOD_SPD_21000) { /* 21G XAUI */
       sgmii_spd_switch    = 0x0;
       clockcnt0           = 0x5;
       clockcnt1_rsv       = 0x0;
       loopcnt1            = 0x0;
       mac_credit          = 0x1;
    } else if(pc->spd_intf == TSCMOD_SPD_20G_MLD_DXGXS ) {
        sgmii_spd_switch    = 0x0;
        clockcnt0           = 0x21;
        clockcnt1_rsv       = 0x8b;
        loopcnt1            = 0x0;
        mac_credit          = 0x2;
        pcs_clkcnt0         = 0x0;
        pcs_creditgencnt    = 0x0;
    } else if((pc->spd_intf == TSCMOD_SPD_21G_HI_MLD_DXGXS)||
              (pc->spd_intf == TSCMOD_SPD_12773_DXGXS) ||
              (pc->spd_intf == TSCMOD_SPD_12773_HI_DXGXS)) {
        sgmii_spd_switch    = 0x0;
        clockcnt0           = 0x21;
        clockcnt1_rsv       = 0x8c;
        loopcnt1            = 0x0;
        mac_credit          = 0x2;
        pcs_clkcnt0         = 0x0;
        pcs_creditgencnt    = 0x0;
    } else if((pc->spd_intf == TSCMOD_SPD_20G_DXGXS) ||
              (pc->spd_intf == TSCMOD_SPD_21G_HI_DXGXS)) {
        sgmii_spd_switch    = 0x0;
        clockcnt0           = 0x21;
        clockcnt1_rsv       = 0x00;
        loopcnt1            = 0x0;
        mac_credit          = 0x2;
    } else if (pc->spd_intf ==TSCMOD_SPD_25455) {
       sgmii_spd_switch    = 0x0;
       clockcnt0           = 0x21;
       clockcnt1_rsv       = 0x00;
       loopcnt1            = 0x0;
       mac_credit          = 0x1;
    } else if (pc->spd_intf ==TSCMOD_SPD_31500) {
       sgmii_spd_switch    = 0x0;
       clockcnt0           = 0x21;
       clockcnt1_rsv       = 0x00;
       loopcnt1            = 0x0;
       loopcnt0            = 0x1;
       mac_credit          = 0x1;
    } else if((pc->spd_intf == TSCMOD_SPD_42G_X4)||
              (pc->spd_intf == TSCMOD_SPD_40G_X4)) {
        sgmii_spd_switch    = 0x0;
        clockcnt0           = 0x21;
        clockcnt1_rsv       = 0x98;
        loopcnt1            = 0x0;
        mac_credit          = 0x1;
    } else if(pc->spd_intf == TSCMOD_SPD_40G_MLD ) {
        sgmii_spd_switch    = 0x0;
        clockcnt0           = 0x21;
        clockcnt1_rsv       = 0x98;
        loopcnt1            = 0x0;
        mac_credit          = 0x1;
    } else if(pc->spd_intf == TSCMOD_SPD_42G_MLD ) {
        sgmii_spd_switch    = 0x0;
        clockcnt0           = 0x21;
        clockcnt1_rsv       = 0x96;
        loopcnt1            = 0x0;
        mac_credit          = 0x1;
    } else {
       if(!(pc->ctrl_type&TSCMOD_CTRL_TYPE_ERR)||(pc->verbosity&TSCMOD_DBG_PRINT)) {
          printf("Error: p=%0d spd_intf=%d undefined %s cntl=%0x verb=%x\n", pc->port, pc->spd_intf,
                 e2s_tscmod_spd_intfc_type[pc->spd_intf], cntl, pc->verbosity) ;
       }
       pc->err_code  |= TSCMOD_ERR_FS_SPD_MODE ;
       pc->ctrl_type |= TSCMOD_CTRL_TYPE_ERR ;
    }

    data_credit0     = clockcnt0 << TX_X4_CREDIT0_CREDIT0_CLOCKCNT0_SHIFT;

    data_clkcnt1     = clockcnt1 << TX_X4_CREDIT0_CREDIT1_CLOCKCNT1_SHIFT |
                       clockcnt1_rsv << TX_X4_CREDIT0_CREDIT1_RESERVED0_SHIFT;

    data_loopcnt     = loopcnt0 << TX_X4_CREDIT0_LOOPCNT_LOOPCNT0_SHIFT |
		               loopcnt1 << TX_X4_CREDIT0_LOOPCNT_LOOPCNT1_SHIFT;
    data_mac_credit  = mac_credit << TX_X4_CREDIT0_MAC_CREDITGENCNT_MAC_CREDITGENCNT_SHIFT;
    data_pcs_clkcnt0 = replication_cnt << TX_X4_CREDIT0_PCS_CLOCKCNT0_REPLICATION_CNT_SHIFT |
                       pcs_clkcnt0 << TX_X4_CREDIT0_PCS_CLOCKCNT0_PCS_CLOCKCNT0_SHIFT;
    data_pcs_credit  = pcs_creditgencnt << TX_X4_CREDIT0_PCS_CREDITGENCNT_PCS_CREDITGENCNT_SHIFT;

    mask_credit0 =  TX_X4_CREDIT0_CREDIT0_CLOCKCNT0_MASK;

    mask_loopcnt = TX_X4_CREDIT0_LOOPCNT_LOOPCNT0_MASK | TX_X4_CREDIT0_LOOPCNT_LOOPCNT1_MASK;

    /* set credtis 0xc100 */
    SOC_IF_ERROR_RETURN (MODIFY_WC40_TX_X4_CREDIT0_CREDIT0r(pc->unit, pc, data_credit0, mask_credit0));

    /* 0xc101 */
    SOC_IF_ERROR_RETURN (MODIFY_WC40_TX_X4_CREDIT0_CREDIT1r(pc->unit, pc, data_clkcnt1, 0xffff));

    /* 0xc102 */
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_TX_X4_CREDIT0_LOOPCNTr(pc->unit, pc, data_loopcnt, mask_loopcnt));

    /* 0xc103 */
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_TX_X4_CREDIT0_MAC_CREDITGENCNTr(pc->unit, pc, data_mac_credit,
			 TX_X4_CREDIT0_MAC_CREDITGENCNT_MAC_CREDITGENCNT_MASK));

    /* 0xc104 */
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_TX_X4_CREDIT0_PCS_CLOCKCNT0r(pc->unit, pc, data_pcs_clkcnt0,
			 (TX_X4_CREDIT0_PCS_CLOCKCNT0_REPLICATION_CNT_MASK |
                         TX_X4_CREDIT0_PCS_CLOCKCNT0_PCS_CLOCKCNT0_MASK)));

    /* 0xc105 */
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_TX_X4_CREDIT0_PCS_CREDITGENCNTr
             (pc->unit, pc, data_pcs_credit,
              TX_X4_CREDIT0_PCS_CREDITGENCNT_PCS_CREDITGENCNT_MASK));

    if(sgmii_spd_switch) {
        /* enable pcs_creditenable before sgmii_spd_switch=1 */
        SOC_IF_ERROR_RETURN (MODIFY_WC40_TX_X4_CREDIT0_PCS_CLOCKCNT0r(pc->unit, pc,
                             TX_X4_CREDIT0_PCS_CLOCKCNT0_PCS_CREDITENABLE_BITS <<
                             TX_X4_CREDIT0_PCS_CLOCKCNT0_PCS_CREDITENABLE_SHIFT,
                             TX_X4_CREDIT0_PCS_CLOCKCNT0_PCS_CREDITENABLE_MASK));
    }

    data_credit0 = 0;
    mask_credit0 = 0;
        
    /* sal_usleep(100); */
    
    data_credit0     = sgmii_spd_switch << TX_X4_CREDIT0_CREDIT0_SGMII_SPD_SWITCH_SHIFT;
    mask_credit0 = TX_X4_CREDIT0_CREDIT0_SGMII_SPD_SWITCH_MASK;
    SOC_IF_ERROR_RETURN (MODIFY_WC40_TX_X4_CREDIT0_CREDIT0r(pc->unit, pc, data_credit0, mask_credit0));
    
  } else {
    /* clear credits */
    SOC_IF_ERROR_RETURN (MODIFY_WC40_TX_X4_CREDIT0_PCS_CLOCKCNT0r(pc->unit, pc, 0, 0xffff));
    SOC_IF_ERROR_RETURN (MODIFY_WC40_TX_X4_CREDIT0_CREDIT0r(pc->unit, pc, 0, 0xffff));
   /* SOC_IF_ERROR_RETURN(MODIFY_WC40_TX_X4_CONTROL0_MISCr(pc->unit, pc, 0, 0xffff)); */
  }
  return SOC_E_NONE;
}  /* CREDIT_SET  */

/*!
\brief Enable / Disable credits for any particular lane

\param  pc handle to current TSC context (#tscmod_st)

\returns The value #SOC_E_NONE upon successful completion.

This function is used to enable or disable the credit generation.

\li set to 0x1 to enable credit
\li Set to 0x0 disable selected lane.

This function reads/modifies the following registers:

\li enable credits: clockcnt0 (0xC100[14])

<B>How to call:</B><br>
<p>Call this function directly or through the function
#tscmod_tier1_selector() with the upper-case string "CREDIT_CONTROL".</p>
*/

int tscmod_credit_control(tscmod_st* pc)         /* CREDIT_CONTROL */
{
  int cntl;
  cntl = pc->per_lane_control;

  if(pc->verbosity &(TSCMOD_DBG_PATH)) {
     printf("%-22s u=%0d p=%0d sel=%x ln=%0d dxgxs=%0d cntl=%x\n", FUNCTION_NAME(),
            pc->unit, pc->port, pc->lane_select, pc->this_lane, pc->dxgxs, pc->per_lane_control) ;
  }

  if (cntl) {
    /* enable credtis */
    SOC_IF_ERROR_RETURN (MODIFY_WC40_TX_X4_CREDIT0_CREDIT0r(pc->unit, pc,
			 TX_X4_CREDIT0_CREDIT0_CREDITENABLE_BITS << TX_X4_CREDIT0_CREDIT0_CREDITENABLE_SHIFT,
			 TX_X4_CREDIT0_CREDIT0_CREDITENABLE_MASK));
    /* revist 10M/100M: SDK-51443: move to credit_set() 
    if((pc->spd_intf == TSCMOD_SPD_10_SGMII) |
       (pc->spd_intf == TSCMOD_SPD_100_SGMII)) {
        SOC_IF_ERROR_RETURN (MODIFY_WC40_TX_X4_CREDIT0_PCS_CLOCKCNT0r(pc->unit, pc,
			 TX_X4_CREDIT0_PCS_CLOCKCNT0_PCS_CREDITENABLE_BITS <<
                         TX_X4_CREDIT0_PCS_CLOCKCNT0_PCS_CREDITENABLE_SHIFT,
			 TX_X4_CREDIT0_PCS_CLOCKCNT0_PCS_CREDITENABLE_MASK));
        }
    */
   } else {
    SOC_IF_ERROR_RETURN (MODIFY_WC40_TX_X4_CREDIT0_PCS_CLOCKCNT0r(pc->unit, pc,
			 TX_X4_CREDIT0_PCS_CLOCKCNT0_PCS_CREDITENABLE_ALIGN <<
                         TX_X4_CREDIT0_PCS_CLOCKCNT0_PCS_CREDITENABLE_SHIFT,
			 TX_X4_CREDIT0_PCS_CLOCKCNT0_PCS_CREDITENABLE_MASK));

    /* disable credtis */
    SOC_IF_ERROR_RETURN (MODIFY_WC40_TX_X4_CREDIT0_CREDIT0r(pc->unit, pc,
			 TX_X4_CREDIT0_CREDIT0_CREDITENABLE_ALIGN << TX_X4_CREDIT0_CREDIT0_CREDITENABLE_SHIFT,
			 TX_X4_CREDIT0_CREDIT0_CREDITENABLE_MASK));    
   }
  return SOC_E_NONE;
}



/*!
\brief tx lane reset and enable of any particular lane

\param  pc handle to current TSC context (#tscmod_st)

\returns The value #SOC_E_NONE upon successful completion.

This function is used to reset tx lane and enable/disable tx lane of any transmit lane.
Set bit 0 of #tscmod_st::per_lane_control to enable the TX_LANE (1) or disable the TX lane (0).
When diable TX lane, two types of operations are inovked independently.
If #tscmod_st::per_lane_control bit [7:4] = 1, only traffic is disabled.
      (TSCMOD_TX_LANE_TRAFFIC =0x10)
If bit [7:4] = 2, only reset function is invoked.
      (TSCMOD_TX_LANE_RESET = 0x20)


This function reads/modifies the following registers:

\li rstb_tx_lane  :   0xc113[1]
\li enable_tx_lane:   0xc113[0]

<B>How to call:</B><br>
<p>Call this function directly or through the function
#tscmod_tier1_selector() with the upper-case string "ENCODE_SET".</p>
*/

int tscmod_tx_lane_control(tscmod_st* pc)         /* TX_LANE_CONTROL */
{
  uint16 data, mask;
  int cntl;

  cntl = (pc->per_lane_control & 1) ;

  mask = 0 ;
  if(pc->verbosity &(TSCMOD_DBG_PATH)) {
     printf("%-22s u=%0d p=%0d sel=%x ln=%0d dxgxs=%0d cntl=%x\n", FUNCTION_NAME(),
            pc->unit, pc->port, pc->lane_select, pc->this_lane, pc->dxgxs, pc->per_lane_control) ;
  }
  if (cntl) {
     /* set encoder */

     if(pc->per_lane_control & 0x2) {
        if(pc->per_lane_control & TSCMOD_TX_LANE_TRAFFIC ){
           mask = TX_X4_CONTROL0_MISC_ENABLE_TX_LANE_MASK ;
        } else if (pc->per_lane_control & TSCMOD_TX_LANE_RESET) {
           mask = TX_X4_CONTROL0_MISC_RSTB_TX_LANE_MASK ;
        }
        SOC_IF_ERROR_RETURN (MODIFY_WC40_TX_X4_CONTROL0_MISCr(pc->unit, pc, mask, mask));
     } else {

        data = TX_X4_CONTROL0_MISC_RSTB_TX_LANE_BITS << TX_X4_CONTROL0_MISC_RSTB_TX_LANE_SHIFT ;
        mask = TX_X4_CONTROL0_MISC_RSTB_TX_LANE_MASK ;
        SOC_IF_ERROR_RETURN (MODIFY_WC40_TX_X4_CONTROL0_MISCr(pc->unit, pc, data, mask));


        data = TX_X4_CONTROL0_MISC_ENABLE_TX_LANE_BITS << TX_X4_CONTROL0_MISC_ENABLE_TX_LANE_SHIFT;
        mask = TX_X4_CONTROL0_MISC_ENABLE_TX_LANE_MASK;
        SOC_IF_ERROR_RETURN (MODIFY_WC40_TX_X4_CONTROL0_MISCr(pc->unit, pc, data, mask));

     }
   } else {
     data = 0 ;
     if(pc->per_lane_control & TSCMOD_TX_LANE_TRAFFIC ){
        mask = TX_X4_CONTROL0_MISC_ENABLE_TX_LANE_MASK ;
     } else if (pc->per_lane_control & TSCMOD_TX_LANE_RESET) {
        mask = TX_X4_CONTROL0_MISC_RSTB_TX_LANE_MASK ;
     } else {
        mask = TX_X4_CONTROL0_MISC_RSTB_TX_LANE_MASK |
               TX_X4_CONTROL0_MISC_ENABLE_TX_LANE_MASK;
     }
     /*
     printf("TX lane control modify data=%x mask=%x this_lane=%0d\n", data, mask, pc->this_lane) ;
     */
     SOC_IF_ERROR_RETURN (MODIFY_WC40_TX_X4_CONTROL0_MISCr(pc->unit, pc, data, mask));
     /*
     SOC_IF_ERROR_RETURN (READ_WC40_TX_X4_CONTROL0_MISCr(pc->unit, pc, &data));
     printf("TX lane control read back data=%x mask=%x\n", data, mask) ;
     */
   }
  return SOC_E_NONE;
}


/*!
\brief rx lane reset and enable of any particular lane

\param  pc handle to current TSC context (#tscmod_st)

\returns The value #SOC_E_NONE upon successful completion.

This function is used to enable/disable rx lane (RSTB_LANE), or
enabling uC RXP logic which is to set RSTB_LANE, or read back control bit.

For per_lane_control =
 TSCMOD_RXP_REG_OFF          0x0      set the RSTB_LANE bit OFF directory (disable RXP)
 TSCMOD_RXP_REG_ON           0x1      set the RSTB_LANE bit ON  directory (enable RXP)
 TSCMOD_RXP_UC_OFF           0x2      enable uC RXP EN logic.
 TSCMOD_RXP_UC_ON            0x3      disable uC RXP logic.
 TSCMOD_RXP_REG_RD           0xa      read back
 TSCMOD_RXP_UC_RD            0xb      read back uC RXP EN info

\li select encodeMode

This function reads/modifies the following registers:

\li rstb_rx_lane  :   0xc137[0]

<B>How to call:</B><br>
<p>Call this function directly or through the function
#tscmod_tier1_selector() with the upper-case string "ENCODE_SET".</p>
*/

int tscmod_rx_lane_control(tscmod_st* pc)         /* RX_LANE_CONTROL */
{
  uint16 data, mask;
  int cntl; int tmp_lane, tmp_sel, tmp_dxgxs, lane_s, lane_e, lane, value ;

  cntl = pc->per_lane_control;
  tmp_lane = pc->this_lane ;
  tmp_sel  = pc->lane_select ;
  tmp_dxgxs= pc->dxgxs ;
  lane_s = 0 ; lane_e = 0 ; lane = 0 ; value = 0 ;

  if(pc->verbosity &(TSCMOD_DBG_PATH)) {
     printf("%-22s u=%0d p=%0d sel=%x ln=%0d dxgxs=%0d cntl=%x\n", __func__,
            pc->unit, pc->port, pc->lane_select, pc->this_lane, pc->dxgxs, cntl) ;
  }
  if (cntl==TSCMOD_RXP_REG_RD) {
    /* set encoder */
    SOC_IF_ERROR_RETURN (READ_WC40_RX_X4_CONTROL0_PMA_CONTROL_0r(pc->unit, pc, &data));
    if (data & RX_X4_CONTROL0_PMA_CONTROL_0_RSTB_LANE_MASK) {
        pc->accData =1 ;
    } else {
        pc->accData =0 ;
    }
  } else if (cntl==TSCMOD_RXP_REG_ON) {
    /* set encoder */
    data = RX_X4_CONTROL0_PMA_CONTROL_0_RSTB_LANE_BITS << RX_X4_CONTROL0_PMA_CONTROL_0_RSTB_LANE_SHIFT;
    mask = RX_X4_CONTROL0_PMA_CONTROL_0_RSTB_LANE_MASK;
    SOC_IF_ERROR_RETURN (MODIFY_WC40_RX_X4_CONTROL0_PMA_CONTROL_0r(pc->unit, pc, data, mask));
  } else if(cntl==TSCMOD_RXP_REG_OFF) {
     /* bit set to 0 for disabling RXP */
    data = RX_X4_CONTROL0_PMA_CONTROL_0_RSTB_LANE_ALIGN << RX_X4_CONTROL0_PMA_CONTROL_0_RSTB_LANE_SHIFT;
    mask = RX_X4_CONTROL0_PMA_CONTROL_0_RSTB_LANE_MASK;
    SOC_IF_ERROR_RETURN (MODIFY_WC40_RX_X4_CONTROL0_PMA_CONTROL_0r(pc->unit, pc, data, mask));
  } else {
     /* need to handle it per lane */
     if(pc->port_type   == TSCMOD_SINGLE_PORT ) {
        lane_s = 0 ; lane_e = 4 ;
     } else if (pc->port_type == TSCMOD_DXGXS ) {
        if((pc->dxgxs & 3)==2) { lane_s = 2; lane_e = 4 ; }
        else { lane_s = 0; lane_e = 2 ; }
     } else {
        lane_s = pc->this_lane ; lane_e = lane_s + 1 ;
     }
     pc->dxgxs= 0 ;
     pc->lane_select = getLaneSelect(lane_s) ;
     if((cntl==TSCMOD_RXP_UC_ON)||(cntl==TSCMOD_RXP_UC_OFF)) {
        if(cntl==TSCMOD_RXP_UC_ON) value = 1;

        for(lane=lane_s; lane<lane_e; lane++) {
           pc->lane_select = getLaneSelect(lane) ;
           pc->this_lane   = lane ;
           tscmod_uc_rx_lane_control(pc, lane, value) ;
        }
     } else if(cntl==TSCMOD_RXP_UC_RD) {
        value = 3 ;
        pc->lane_select = getLaneSelect(pc->this_lane) ;
        tscmod_uc_rx_lane_control(pc, pc->this_lane, value);
     } else if(cntl==TSCMOD_RXP_UC_CLR) {
        value = 4 ;
        for(lane=lane_s; lane<lane_e; lane++) {
            pc->lane_select = getLaneSelect(lane) ;
            pc->this_lane   = lane ;
            tscmod_uc_rx_lane_control(pc, pc->this_lane, value);
        }
     } else {
        printf("Error: u=%0d p=%0d lane_control sel=%x ln=%0d dxgxs=%0d cntl=%x\n",
               pc->unit, pc->port, pc->lane_select, pc->this_lane, pc->dxgxs, cntl) ;
     }
  }
  pc->this_lane  = tmp_lane ;
  pc->lane_select= tmp_sel ;
  pc->dxgxs      = tmp_dxgxs ;
  return SOC_E_NONE;
}
/* to call uC command registers to set FW_RXP_EN 1 or 0 if value = 0 or 1 */
static int tscmod_uc_rx_lane_control(tscmod_st* pc, int lane, int value)
{
   int offset ; uint16 data, mask ;

   switch(lane) {
   case 1: offset = 0x5d0 ; break ;
   case 2: offset = 0x6d0 ; break ;
   case 3: offset = 0x7d0 ; break ;
   default: offset = 0x4d0 ; break;
   }

   if(value==4) {
      mask                 = 0xffff ;
      pc->accAddr          = offset ;
      data = 0x1e00 ; 
      pc->accData          = data ;
      pc->per_lane_control = (mask<<4)|TSCMOD_UC_SYNC_RAM_MODE | TSCMOD_UC_SYNC_WR_MODE ;
      tscmod_uc_sync_cmd(pc) ;
      if(pc->verbosity & TSCMOD_DBG_UC)
         printf("%-22s: u=%0d p=%0d ram write per_lane_control=%0x l=%0d data=%0x\n",
                FUNCTION_NAME(), pc->unit, pc->port, pc->per_lane_control, lane, pc->accData) ;
   } else if(value>=2) {
      /* READ */
      pc->accAddr          = offset ;
      pc->per_lane_control = TSCMOD_UC_SYNC_RAM_MODE | TSCMOD_UC_SYNC_RD_MODE ;
      tscmod_uc_sync_cmd(pc) ;
      /* pc->accData = pc->accData >>8 ; */
      if(pc->verbosity & TSCMOD_DBG_UC)
         printf("%-22s: u=%0d p=%0d ram read per_lane_control=%0x l=%0d data=%0x\n",
                FUNCTION_NAME(), pc->unit, pc->port, pc->per_lane_control, lane, pc->accData) ;

   } else {
      /* WRITE */
      if(pc->verbosity & TSCMOD_DBG_UC) {
         pc->accAddr          = offset ;
         pc->per_lane_control = TSCMOD_UC_SYNC_RAM_MODE | TSCMOD_UC_SYNC_RD_MODE ;
         tscmod_uc_sync_cmd(pc) ;
         printf("%-22s: u=%0d p=%0d ram read per_lane_control=%0x l=%0d data=%0x\n",
                FUNCTION_NAME(), pc->unit, pc->port, pc->per_lane_control, lane, pc->accData) ;
      }

      mask                 = 0xffff ;
      pc->accAddr          = offset ;
      if (value) {
        data = 0xe00 ;
      } else {
        data = 0xd00 ;
      }
      pc->accData          = data ;
      pc->per_lane_control = (mask<<4)|TSCMOD_UC_SYNC_RAM_MODE | TSCMOD_UC_SYNC_WR_MODE ;
      tscmod_uc_sync_cmd(pc) ;
      if(pc->verbosity & TSCMOD_DBG_UC)
         printf("%-22s: u=%0d p=%0d ram write per_lane_control=%0x l=%0d data=%0x\n",
                FUNCTION_NAME(), pc->unit, pc->port, pc->per_lane_control, lane, pc->accData) ;
   }
   return SOC_E_NONE ;
}

/*!
\brief HiGig2 enable/disable for Tx and RX

\param  pc handle to current TSC context (#tscmod_st)

\returns The value #SOC_E_NONE upon successful completion.

This function is used to enable/disable HiGig2 signalling of any lane .

\li control HigGig2 signaling Tx and Rx lanes.

This function reads/modifies the following registers:

\li HG2_CODEC          :   0xc111[12]
\li HG2_ENABLE         :   0xc111[10]
\li HG2_ENABLE         :   0xc134[12]
\li HG2_CODEC          :   0xc134[9]

<B>How to call:</B><br>
<p>Call this function directly or through the function
#tscmod_tier1_selector() with the upper-case string "HIGIG2_CONTROL".</p>
*/

int tscmod_higig2_control(tscmod_st* pc)         /* HIGIG2_CONTROL */
{
  uint16 hg2_codec, hg2_enable;
  uint16 data_tx, mask_tx;
  uint16 data_rx, mask_rx;
  int cntl;

  cntl = pc->per_lane_control;

  data_tx       = 0x0;
  mask_tx       = 0x0;
  data_rx       = 0x0;
  mask_rx       = 0x0;
  hg2_codec     = 0x1;
  hg2_enable    = 0x1;

  /* removing the Speed interface details for now
   * We might need to revisit this later*/
#if 0
  hg2_codec     = 0x0;
  hg2_enable    = 0x0;

  if ((pc->spd_intf == TSCMOD_SPD_10000_XFI)||
      (pc->spd_intf == TSCMOD_SPD_10600_XFI_HG)) {
    hg2_codec     = 0x0;
    hg2_enable    = 0x1;
  } else if ((pc->spd_intf == TSCMOD_SPD_40G_MLD) ||
             (pc->spd_intf == TSCMOD_SPD_42G_MLD)) {
    hg2_codec     = 0x1;
    hg2_enable    = 0x1;
  }
#endif
  if ((cntl == TSCMOD_HG2_CONTROL_ENABLE_ALL) ||
      (cntl == TSCMOD_HG2_CONTROL_DISABLE_ALL) ) {
      data_tx = hg2_codec << TX_X4_CONTROL0_ENCODE_0_HG2_CODEC_SHIFT |
                hg2_enable << TX_X4_CONTROL0_ENCODE_0_HG2_ENABLE_SHIFT;

      mask_tx = TX_X4_CONTROL0_ENCODE_0_HG2_CODEC_MASK | TX_X4_CONTROL0_ENCODE_0_HG2_ENABLE_MASK;

      data_rx = hg2_codec << RX_X4_CONTROL0_DECODE_CONTROL_0_HG2_CODEC_SHIFT |
                hg2_enable << RX_X4_CONTROL0_DECODE_CONTROL_0_HG2_ENABLE_SHIFT;

      mask_rx = RX_X4_CONTROL0_DECODE_CONTROL_0_HG2_CODEC_MASK |
                RX_X4_CONTROL0_DECODE_CONTROL_0_HG2_ENABLE_MASK;
  } else if ((cntl == TSCMOD_HG2_CONTROL_ENABLE_CODEC) ||
             (cntl == TSCMOD_HG2_CONTROL_DISABLE_CODEC) ||
             (cntl == TSCMOD_HG2_CONTROL_READ_CODEC)) {
      data_tx = hg2_codec << TX_X4_CONTROL0_ENCODE_0_HG2_CODEC_SHIFT;
      data_rx = hg2_codec << RX_X4_CONTROL0_DECODE_CONTROL_0_HG2_CODEC_SHIFT;
      mask_tx = TX_X4_CONTROL0_ENCODE_0_HG2_CODEC_MASK;
      mask_rx = RX_X4_CONTROL0_DECODE_CONTROL_0_HG2_CODEC_MASK;
  }
  if (cntl == TSCMOD_HG2_CONTROL_ENABLE_ALL) {
    SOC_IF_ERROR_RETURN (MODIFY_WC40_TX_X4_CONTROL0_ENCODE_0r(pc->unit, pc, data_tx, mask_tx));
    SOC_IF_ERROR_RETURN (MODIFY_WC40_RX_X4_CONTROL0_DECODE_CONTROL_0r(pc->unit, pc, data_rx, mask_rx));
   } else if (cntl == TSCMOD_HG2_CONTROL_DISABLE_ALL) {/* */
    SOC_IF_ERROR_RETURN (MODIFY_WC40_TX_X4_CONTROL0_ENCODE_0r(pc->unit, pc, 0x0, mask_tx));
    SOC_IF_ERROR_RETURN (MODIFY_WC40_RX_X4_CONTROL0_DECODE_CONTROL_0r(pc->unit, pc, 0x0, mask_rx));
   } else if (cntl == TSCMOD_HG2_CONTROL_DISABLE_CODEC) {
       SOC_IF_ERROR_RETURN (MODIFY_WC40_TX_X4_CONTROL0_ENCODE_0r(pc->unit, pc, 0x0, mask_tx));
       SOC_IF_ERROR_RETURN (MODIFY_WC40_RX_X4_CONTROL0_DECODE_CONTROL_0r(pc->unit, pc, 0x0, mask_rx));
   } else if (cntl == TSCMOD_HG2_CONTROL_ENABLE_CODEC) {
       SOC_IF_ERROR_RETURN (MODIFY_WC40_TX_X4_CONTROL0_ENCODE_0r(pc->unit, pc, data_tx, mask_tx));
       SOC_IF_ERROR_RETURN (MODIFY_WC40_RX_X4_CONTROL0_DECODE_CONTROL_0r(pc->unit, pc, data_rx, mask_rx));
   } else if (cntl == TSCMOD_HG2_CONTROL_READ_CODEC) {
       pc->accData = 0;
       data_tx = 0;
       data_rx = 0;
       SOC_IF_ERROR_RETURN (READ_WC40_TX_X4_CONTROL0_ENCODE_0r(pc->unit, pc, &data_tx));
       pc->accData |= ((data_tx & mask_tx) >> TX_X4_CONTROL0_ENCODE_0_HG2_CODEC_SHIFT);
       SOC_IF_ERROR_RETURN (READ_WC40_RX_X4_CONTROL0_DECODE_CONTROL_0r(pc->unit, pc, &data_rx));
       pc->accData |= (((data_rx & mask_rx) >> RX_X4_CONTROL0_DECODE_CONTROL_0_HG2_CODEC_SHIFT) << 1);
   } else {
       /* Dummy Else */
   }
    return SOC_E_NONE;
}

/*!
\brief Local Fault/Remote Fault signalling enable for Tx and RX

\param  pc handle to current TSC context (#tscmod_st)

\returns The value #SOC_E_NONE upon successful completion.

This function is used to enable/disable Link Interrupt, Local Fault and
Remote Fault signalling of any lane .

\li
#tscmod_st::per_lane_control[3:0] represents the mode (CL49, CL48, CL82).
#tscmod_st::per_lane_control[6]   is for li ;
#tscmod_st::per_lane_control[5]   is for lf ;
#tscmod_st::per_lane_control[4]   is for rf ;

This function reads/modifies the following registers:

\li cl49_tx            :   0xc113
\li cl49_rx            :   0xc134
\li cl48_tx            :   0xa001
\li cl82_tx            :   0xa002
\li cl48_cl82_rx       :   0xa024

<B>How to call:</B><br>
<p>Call this function directly or through the function
#tscmod_tier1_selector() with the upper-case string "LF_RF_CONTROL".</p>
*/

int tscmod_lf_rf_control(tscmod_st* pc)         /* LF_RF_CONTROL */
{
  uint16 data, mask;
  int cntl, mode ;

  mode = pc->per_lane_control & 0xf  ;
  cntl =(pc->per_lane_control>>4) & 0xf ;

  if(mode & TSCMOD_FAULT_CL49) {
     data = 0 ;
     if(cntl & TSCMOD_FAULT_RF)
        { data |= TX_X4_CONTROL0_MISC_CL49_TX_RF_ENABLE_MASK ; }
     if(cntl & TSCMOD_FAULT_LF)
        { data |= TX_X4_CONTROL0_MISC_CL49_TX_LF_ENABLE_MASK ; }
     if(cntl & TSCMOD_FAULT_LI)
        { data |= TX_X4_CONTROL0_MISC_CL49_TX_LI_ENABLE_MASK ; }

     mask = TX_X4_CONTROL0_MISC_CL49_TX_LI_ENABLE_MASK |
            TX_X4_CONTROL0_MISC_CL49_TX_LF_ENABLE_MASK |
            TX_X4_CONTROL0_MISC_CL49_TX_RF_ENABLE_MASK;
     /* 0xc113 */
     SOC_IF_ERROR_RETURN (MODIFY_WC40_TX_X4_CONTROL0_MISCr(pc->unit, pc, data, mask));

     data = 0 ;
     if(cntl & TSCMOD_FAULT_RF)
        { data |= RX_X4_CONTROL0_DECODE_CONTROL_0_CL49_RX_RF_ENABLE_MASK ;}
     if(cntl & TSCMOD_FAULT_LF)
        { data |= RX_X4_CONTROL0_DECODE_CONTROL_0_CL49_RX_LF_ENABLE_MASK ;}
     if(cntl & TSCMOD_FAULT_LI)
        { data |= RX_X4_CONTROL0_DECODE_CONTROL_0_CL49_RX_LI_ENABLE_MASK ;}

     mask = RX_X4_CONTROL0_DECODE_CONTROL_0_CL49_RX_LI_ENABLE_MASK |
            RX_X4_CONTROL0_DECODE_CONTROL_0_CL49_RX_LF_ENABLE_MASK |
            RX_X4_CONTROL0_DECODE_CONTROL_0_CL49_RX_RF_ENABLE_MASK;
     /* 0xc134 */
     SOC_IF_ERROR_RETURN (MODIFY_WC40_RX_X4_CONTROL0_DECODE_CONTROL_0r
                          (pc->unit, pc, data, mask));

  }
  if(mode & TSCMOD_FAULT_CL48) {
     data = 0 ;
     if(cntl & TSCMOD_FAULT_RF)
        data |= TX_X2_CONTROL0_CL48_0_CL48_TX_RF_ENABLE_MASK ;
     if(cntl & TSCMOD_FAULT_LF)
        data |= TX_X2_CONTROL0_CL48_0_CL48_TX_LF_ENABLE_MASK ;
     if(cntl & TSCMOD_FAULT_LI)
        data |= TX_X2_CONTROL0_CL48_0_CL48_TX_LI_ENABLE_MASK ;

     mask = TX_X2_CONTROL0_CL48_0_CL48_TX_LI_ENABLE_MASK |
            TX_X2_CONTROL0_CL48_0_CL48_TX_LF_ENABLE_MASK |
            TX_X2_CONTROL0_CL48_0_CL48_TX_RF_ENABLE_MASK ;
     /* 0xa001 */
     SOC_IF_ERROR_RETURN
        (MODIFY_WC40_TX_X2_CONTROL0_CL48_0r(pc->unit, pc, data, mask));

     data = 0 ;
     if(cntl & TSCMOD_FAULT_RF)
        data |= RX_X2_CONTROL0_MISC_1_CL48_RX_RF_ENABLE_MASK ;
     if(cntl & TSCMOD_FAULT_LF)
        data |= RX_X2_CONTROL0_MISC_1_CL48_RX_LF_ENABLE_MASK ;
     if(cntl & TSCMOD_FAULT_LI)
        data |= RX_X2_CONTROL0_MISC_1_CL48_RX_LI_ENABLE_MASK ;

     mask = RX_X2_CONTROL0_MISC_1_CL48_RX_LI_ENABLE_MASK |
            RX_X2_CONTROL0_MISC_1_CL48_RX_LF_ENABLE_MASK |
            RX_X2_CONTROL0_MISC_1_CL48_RX_RF_ENABLE_MASK ;
     /* 0xa024 */
     SOC_IF_ERROR_RETURN
        (MODIFY_WC40_RX_X2_CONTROL0_MISC_1r(pc->unit, pc, data, mask));
  }
  if(mode & TSCMOD_FAULT_CL82) {
     data = 0 ;
     if(cntl & TSCMOD_FAULT_RF)
        data |= TX_X2_CONTROL0_CL82_0_CL82_TX_RF_ENABLE_MASK ;
     if(cntl & TSCMOD_FAULT_LF)
        data |= TX_X2_CONTROL0_CL82_0_CL82_TX_LF_ENABLE_MASK ;
     if(cntl & TSCMOD_FAULT_LI)
        data |= TX_X2_CONTROL0_CL82_0_CL82_TX_LI_ENABLE_MASK ;

     mask = TX_X2_CONTROL0_CL82_0_CL82_TX_LI_ENABLE_MASK |
            TX_X2_CONTROL0_CL82_0_CL82_TX_LF_ENABLE_MASK |
            TX_X2_CONTROL0_CL82_0_CL82_TX_RF_ENABLE_MASK ;
     /* 0xa002 */
     SOC_IF_ERROR_RETURN
        (MODIFY_WC40_TX_X2_CONTROL0_CL82_0r(pc->unit, pc, data, mask));

     data = 0 ;
     if(cntl & TSCMOD_FAULT_RF)
        data |= RX_X2_CONTROL0_MISC_1_CL82_RX_RF_ENABLE_MASK ;
     if(cntl & TSCMOD_FAULT_LF)
        data |= RX_X2_CONTROL0_MISC_1_CL82_RX_LF_ENABLE_MASK ;
     if(cntl & TSCMOD_FAULT_LI)
        data |= RX_X2_CONTROL0_MISC_1_CL82_RX_LI_ENABLE_MASK ;

     mask =  RX_X2_CONTROL0_MISC_1_CL82_RX_LI_ENABLE_MASK |
             RX_X2_CONTROL0_MISC_1_CL82_RX_LF_ENABLE_MASK |
             RX_X2_CONTROL0_MISC_1_CL82_RX_RF_ENABLE_MASK ;
     /* 0xa024 */
     SOC_IF_ERROR_RETURN
        (MODIFY_WC40_RX_X2_CONTROL0_MISC_1r(pc->unit, pc, data, mask));
  }
  return SOC_E_NONE;
}

/*!
\brief cl49/cl82 fast block lock control of a  particular lane

\param  pc handle to current TSC context (#tscmod_st)

\returns The value #SOC_E_NONE upon successful completion.

This function is used to enable/disable rx lane's fast lock control.

\li control fast_lock

This function reads/modifies the following registers:

\li fast_lock_cya  :   0xc132[9]

<B>How to call:</B><br>
<p>Call this function directly or through the function
#tscmod_tier1_selector() with the upper-case string "RX_FAST_LOCK_CONTROL".</p>
*/

int tscmod_rx_fast_lock_control(tscmod_st* pc)         /* RX_FAST_LOCK_CONTROL */
{
/*
  uint16 data, mask;
  int cntl;

  cntl = pc->per_lane_control;
  if (cntl) {
    set fast lock
    data = RX_X4_CONTROL0_DECODE_CONTROL_0_FAST_LOCK_CYA_BITS <<
  	   RX_X4_CONTROL0_DECODE_CONTROL_0_FAST_LOCK_CYA_SHIFT;
    mask = RX_X4_CONTROL0_DECODE_CONTROL_0_FAST_LOCK_CYA_MASK;
    SOC_IF_ERROR_RETURN (MODIFY_WC40_RX_X4_CONTROL0_DECODE_CONTROL_0r(pc->unit, pc, data, mask));
   } else {
    data = RX_X4_CONTROL0_DECODE_CONTROL_0_FAST_LOCK_CYA_ALIGN <<
  	   RX_X4_CONTROL0_DECODE_CONTROL_0_FAST_LOCK_CYA_SHIFT;
    mask = RX_X4_CONTROL0_DECODE_CONTROL_0_FAST_LOCK_CYA_MASK;
    SOC_IF_ERROR_RETURN (MODIFY_WC40_RX_X4_CONTROL0_DECODE_CONTROL_0r(pc->unit, pc, data, mask));
   }
*/
     return SOC_E_NONE;
}


/*!
\brief cl72 signal detect vs rxSeqDone to FEC module of a  particular lane

\param  pc handle to current TSC context (#tscmod_st)

\returns The value #SOC_E_NONE upon successful completion.

This function is used to select rxSeqDone vs cl72 signal detect to FEC module. A value of '1'
selects rxSeqDone to FEC module. And '0' selects cl72 signal detect to FEC module.

\li control signal_detect_fec_sel

This function reads/modifies the following registers:

\li signal_detect_fec_sel  :   0xc131[7]

<B>How to call:</B><br>
<p>Call this function directly or through the function
#tscmod_tier1_selector() with the upper-case string "SIGNAL_DETECT_FEC_SEL".</p>
*/

int tscmod_signal_detect_fec_sel(tscmod_st* pc)         /* SIGNAL_DETECT_FEC_SEL */
{
/*
  uint16 data, mask;
  int cntl;

  cntl = pc->per_lane_control;

  if (cntl) {
    set signal rxSeqDone or CL72 signal detect

    data = RX_X4_CONTROL0_FEC_SIGNAL_DETECT_FEC_SEL_BITS <<
  	   RX_X4_CONTROL0_FEC_SIGNAL_DETECT_FEC_SEL_SHIFT;
    mask = RX_X4_CONTROL0_FEC_SIGNAL_DETECT_FEC_SEL_MASK;
    SOC_IF_ERROR_RETURN (MODIFY_WC40_RX_X4_CONTROL0_FECr(pc->unit, pc, data, mask));
   } else {
    data = RX_X4_CONTROL0_FEC_SIGNAL_DETECT_FEC_SEL_ALIGN <<
  	   RX_X4_CONTROL0_FEC_SIGNAL_DETECT_FEC_SEL_SHIFT;
    mask = RX_X4_CONTROL0_FEC_SIGNAL_DETECT_FEC_SEL_MASK;
    SOC_IF_ERROR_RETURN (MODIFY_WC40_RX_X4_CONTROL0_FECr(pc->unit, pc, data, mask));
   }
*/
     return SOC_E_NONE;
}

/*!
\brief Performs soft reset of selected port.

\param  pc handle to current TSC context (#tscmod_st)

\returns The value #SOC_E_NONE upon successful completion.

\details

This function performs soft reset on selected lane.
If #tscmod_st::per_lane_control =1.  TLA related registers are written with default values.

This function modifies the following registers:


<B>How to call:</B><br>
<p>Call this function directly or through the function
#tscmod_tier1_selector() with the upper-case string "SOFT_RESET".</p>
*/

int tscmod_soft_reset(tscmod_st* pc)                      /* SOFT_RESET */
{
   uint16 data, mask, cntl, offset ;
   int mode  ; uint16 val ;
   int tmp_sel, tmp_lane, tmp_dxgxs ;

   offset = TSCMOD_SRESET_SHIFT ;
   cntl = pc->per_lane_control &TSCMOD_SRESET_MASK ;

   tmp_sel  = pc->lane_select ;
   tmp_lane = pc->this_lane ;
   tmp_dxgxs= pc->dxgxs ;

   mode = ( pc->per_lane_control >> offset ) ;
   if(pc->lane_select == TSCMOD_LANE_BCST ) {
      mask = 0xf ;
   } else if (pc->dxgxs & 0x3) {
      mask = (pc->dxgxs & 0x1) ? 0x3 : 0xc ;
   } else {
      mask = 1 << pc->this_lane ;
   }
   val = mode ? mask : 0 ;

   offset = 8 ;
   cntl = pc->per_lane_control & 0xff ;
   if( cntl == TSCMOD_SRESET_AN_UNLOCK) {  /* jir CRTSC-616 workaround trial
         TLA reset per lane: in A0 to reset TLA after CL37 unfinished
         or failed. The workaround is to switch to CL73 and restart to CL73 AN */
      /* 0xc180 */
      data = 0 ;
      mask = AN_X4_ABILITIES_ENABLES_CL37_BAM_ENABLE_MASK |
           AN_X4_ABILITIES_ENABLES_CL73_BAM_ENABLE_MASK |
           AN_X4_ABILITIES_ENABLES_CL73_HPAM_ENABLE_MASK |
           AN_X4_ABILITIES_ENABLES_CL73_ENABLE_MASK |
           AN_X4_ABILITIES_ENABLES_CL37_SGMII_ENABLE_MASK |
           AN_X4_ABILITIES_ENABLES_CL37_ENABLE_MASK |
           AN_X4_ABILITIES_ENABLES_CL37_AN_RESTART_MASK |
           AN_X4_ABILITIES_ENABLES_CL73_AN_RESTART_MASK ;
      SOC_IF_ERROR_RETURN
         (MODIFY_WC40_AN_X4_ABILITIES_ENABLESr(pc->unit, pc, data, mask));

      /* 0xc1ad */
      SOC_IF_ERROR_RETURN
         (READ_WC40_AN_X4_SW_MANAGEMENT_TLA_SEQUENCER_STATUSr(pc->unit,pc,&data));
      printf("SW_MANAGEMENT_TLA_SEQUENCER_STATUSr(%0x) d=%0x pre_soft_reset\n", pc->accAddr, data) ;

      /* may not need this register setting */
      /* 0x9008 */
      data = 0 << MAIN0_TICK_CONTROL_0_TICK_NUMERATOR_LOWER_SHIFT |
             1 << MAIN0_TICK_CONTROL_0_TICK_DENOMINATOR_SHIFT ;
      mask = MAIN0_TICK_CONTROL_0_TICK_NUMERATOR_LOWER_MASK |
             MAIN0_TICK_CONTROL_0_TICK_DENOMINATOR_MASK ;
      SOC_IF_ERROR_RETURN
         (MODIFY_WC40_MAIN0_TICK_CONTROL_0r(pc->unit, pc, data, mask));

      /* 0x9007 */
      data = 0 << MAIN0_TICK_CONTROL_1_TICK_OVERRIDE_SHIFT |
             2 << MAIN0_TICK_CONTROL_1_TICK_NUMERATOR_UPPER_SHIFT ;
      mask = MAIN0_TICK_CONTROL_1_TICK_OVERRIDE_MASK | MAIN0_TICK_CONTROL_1_TICK_NUMERATOR_UPPER_MASK ;
      SOC_IF_ERROR_RETURN
         (MODIFY_WC40_MAIN0_TICK_CONTROL_1r(pc->unit, pc, data, mask));


      /* 0x9258 */
      data = 1 ;
      mask = AN_X1_TIMERS_LINK_FAIL_INHIBIT_TIMER_NOT_CL72_LINK_FAIL_INHIBIT_TIMER_NCL72_PERIOD_MASK ;
      SOC_IF_ERROR_RETURN
         (MODIFY_WC40_AN_X1_TIMERS_LINK_FAIL_INHIBIT_TIMER_NOT_CL72r(pc->unit, pc, data, mask));

      /* 0x9257 */
      data = 1 ;
      mask = AN_X1_TIMERS_LINK_FAIL_INHIBIT_TIMER_CL72_LINK_FAIL_INHIBIT_TIMER_CL72_PERIOD_MASK ;
      SOC_IF_ERROR_RETURN
         (MODIFY_WC40_AN_X1_TIMERS_LINK_FAIL_INHIBIT_TIMER_CL72r(pc->unit, pc, data, mask));


      /* 0x9254 */
      data = 1 ;
      mask = AN_X1_TIMERS_CL73_ERROR_CL73_ERROR_TIMER_PERIOD_MASK ;
      SOC_IF_ERROR_RETURN
         (MODIFY_WC40_AN_X1_TIMERS_CL73_ERRORr(pc->unit, pc, data, mask));

      /* 0x9253 */
      data = 1 ;
      mask = AN_X1_TIMERS_CL73_BREAK_LINK_TX_DISABLE_TIMER_PERIOD_MASK ;
      SOC_IF_ERROR_RETURN
         (MODIFY_WC40_AN_X1_TIMERS_CL73_BREAK_LINKr(pc->unit, pc, data, mask));


      /* 0xc188 */
      data = 1 << AN_X4_ABILITIES_CONTROLS_AN_FAIL_COUNT_LIMIT_SHIFT ;
      mask =      AN_X4_ABILITIES_CONTROLS_AN_FAIL_COUNT_LIMIT_MASK ;
      SOC_IF_ERROR_RETURN
         (MODIFY_WC40_AN_X4_ABILITIES_CONTROLSr(pc->unit, pc, data, mask));

      /* 0xc180 */
      data = AN_X4_ABILITIES_ENABLES_CL73_ENABLE_MASK | AN_X4_ABILITIES_ENABLES_CL73_AN_RESTART_MASK ;
      mask = AN_X4_ABILITIES_ENABLES_CL37_BAM_ENABLE_MASK |
           AN_X4_ABILITIES_ENABLES_CL73_BAM_ENABLE_MASK |
           AN_X4_ABILITIES_ENABLES_CL73_HPAM_ENABLE_MASK |
           AN_X4_ABILITIES_ENABLES_CL73_ENABLE_MASK |
           AN_X4_ABILITIES_ENABLES_CL37_SGMII_ENABLE_MASK |
           AN_X4_ABILITIES_ENABLES_CL37_ENABLE_MASK |
           AN_X4_ABILITIES_ENABLES_CL37_AN_RESTART_MASK |
           AN_X4_ABILITIES_ENABLES_CL73_AN_RESTART_MASK ;
      SOC_IF_ERROR_RETURN
         (MODIFY_WC40_AN_X4_ABILITIES_ENABLESr(pc->unit, pc, data, mask));

      SOC_IF_ERROR_RETURN
         (READ_WC40_AN_X4_SW_MANAGEMENT_TLA_SEQUENCER_STATUSr(pc->unit,pc,&data));
      printf("SW_MANAGEMENT_TLA_SEQUENCER_STATUSr(%0x) d=%0x post_soft_reset\n", pc->accAddr, data) ;
      /* 0xc180 */
      data = 0 ;
      mask = AN_X4_ABILITIES_ENABLES_CL37_BAM_ENABLE_MASK |
           AN_X4_ABILITIES_ENABLES_CL73_BAM_ENABLE_MASK |
           AN_X4_ABILITIES_ENABLES_CL73_HPAM_ENABLE_MASK |
           AN_X4_ABILITIES_ENABLES_CL73_ENABLE_MASK |
           AN_X4_ABILITIES_ENABLES_CL37_SGMII_ENABLE_MASK |
           AN_X4_ABILITIES_ENABLES_CL37_ENABLE_MASK |
           AN_X4_ABILITIES_ENABLES_CL37_AN_RESTART_MASK |
           AN_X4_ABILITIES_ENABLES_CL73_AN_RESTART_MASK ;
      SOC_IF_ERROR_RETURN
         (MODIFY_WC40_AN_X4_ABILITIES_ENABLESr(pc->unit, pc, data, mask));


      /* restore registers back */
      /* 0x9008 */
      data = 0xf << MAIN0_TICK_CONTROL_0_TICK_NUMERATOR_LOWER_SHIFT |
          0x4 << MAIN0_TICK_CONTROL_0_TICK_DENOMINATOR_SHIFT ;
      mask = MAIN0_TICK_CONTROL_0_TICK_NUMERATOR_LOWER_MASK |
             MAIN0_TICK_CONTROL_0_TICK_DENOMINATOR_MASK ;
      SOC_IF_ERROR_RETURN
         (MODIFY_WC40_MAIN0_TICK_CONTROL_0r(pc->unit, pc, data, mask));

      /* 0x9007 */
      data = 0 << MAIN0_TICK_CONTROL_1_TICK_OVERRIDE_SHIFT |
             0x249 << MAIN0_TICK_CONTROL_1_TICK_NUMERATOR_UPPER_SHIFT ;
      mask = MAIN0_TICK_CONTROL_1_TICK_OVERRIDE_MASK |
             MAIN0_TICK_CONTROL_1_TICK_NUMERATOR_UPPER_MASK ;
      SOC_IF_ERROR_RETURN
         (MODIFY_WC40_MAIN0_TICK_CONTROL_1r(pc->unit, pc, data, mask));

      /* 0x9258 */
      data = 0xbb8 ;
      mask = AN_X1_TIMERS_LINK_FAIL_INHIBIT_TIMER_NOT_CL72_LINK_FAIL_INHIBIT_TIMER_NCL72_PERIOD_MASK ;
      SOC_IF_ERROR_RETURN
         (MODIFY_WC40_AN_X1_TIMERS_LINK_FAIL_INHIBIT_TIMER_NOT_CL72r(pc->unit, pc, data, mask));

      /* 0x9257 */
      data = 0x8236 ;
      mask = AN_X1_TIMERS_LINK_FAIL_INHIBIT_TIMER_CL72_LINK_FAIL_INHIBIT_TIMER_CL72_PERIOD_MASK ;
      SOC_IF_ERROR_RETURN
         (MODIFY_WC40_AN_X1_TIMERS_LINK_FAIL_INHIBIT_TIMER_CL72r(pc->unit, pc, data, mask));

      /* 0x9254 */
      data = 0x55d ;
      mask = AN_X1_TIMERS_CL73_ERROR_CL73_ERROR_TIMER_PERIOD_MASK ;
      SOC_IF_ERROR_RETURN
         (MODIFY_WC40_AN_X1_TIMERS_CL73_ERRORr(pc->unit, pc, data, mask));

      /* 0x9253 */
      data = 0x10ed ;
      mask = AN_X1_TIMERS_CL73_BREAK_LINK_TX_DISABLE_TIMER_PERIOD_MASK ;
      SOC_IF_ERROR_RETURN
         (MODIFY_WC40_AN_X1_TIMERS_CL73_BREAK_LINKr(pc->unit, pc, data, mask));

      /* 0xc188 */
      data = 0 << AN_X4_ABILITIES_CONTROLS_AN_FAIL_COUNT_LIMIT_SHIFT ;
      mask =      AN_X4_ABILITIES_CONTROLS_AN_FAIL_COUNT_LIMIT_MASK ;
      SOC_IF_ERROR_RETURN
         (MODIFY_WC40_AN_X4_ABILITIES_CONTROLSr(pc->unit, pc, data, mask));

   } else if(cntl == TSCMOD_SRESET_DSC_HW) {
      SOC_IF_ERROR_RETURN
         (READ_WC40_MAIN0_RESETr(pc->unit, pc, &data)) ;
      data = (data & MAIN0_RESET_RX_DSC_RESET_MASK)
         >>  MAIN0_RESET_RX_DSC_RESET_SHIFT;
      data = (data & ~mask) | (val & mask) ;
      data = data << MAIN0_RESET_RX_DSC_RESET_SHIFT;
      SOC_IF_ERROR_RETURN
         (MODIFY_WC40_MAIN0_RESETr
          (pc->unit, pc, data,MAIN0_RESET_RX_DSC_RESET_MASK)) ;
   } else if(cntl == TSCMOD_SRESET_AFE_TX_ANALOG) {
      uint16 tx ;
      SOC_IF_ERROR_RETURN
         (READ_WC40_MAIN0_RESETr(pc->unit, pc, &data)) ;
      tx = (data & MAIN0_RESET_TX_RESET_ANALOG_MASK)
               >>  MAIN0_RESET_TX_RESET_ANALOG_SHIFT ;
      tx = (tx & ~mask) | (val & mask) ;
      data =  (tx<< MAIN0_RESET_TX_RESET_ANALOG_SHIFT);
      mask = MAIN0_RESET_TX_RESET_ANALOG_MASK;
      SOC_IF_ERROR_RETURN
         (MODIFY_WC40_MAIN0_RESETr
          (pc->unit, pc, data, mask)) ;
   } else if(cntl == TSCMOD_SRESET_AFE_RX_ANALOG) {
      uint16 rx ;
      SOC_IF_ERROR_RETURN
         (READ_WC40_MAIN0_RESETr(pc->unit, pc, &data)) ;
      rx = (data & MAIN0_RESET_RX_RESET_ANALOG_MASK)
               >>  MAIN0_RESET_RX_RESET_ANALOG_SHIFT ;
      rx = (rx & ~mask) | (val & mask) ;
      data = (rx<< MAIN0_RESET_RX_RESET_ANALOG_SHIFT) ;
      mask = MAIN0_RESET_RX_RESET_ANALOG_MASK ;
      SOC_IF_ERROR_RETURN
         (MODIFY_WC40_MAIN0_RESETr
          (pc->unit, pc, data, mask)) ;
   } else if(cntl == TSCMOD_SRESET_AFE_ANALOG) {
      uint16 tx, rx ;
      SOC_IF_ERROR_RETURN
         (READ_WC40_MAIN0_RESETr(pc->unit, pc, &data)) ;
      rx = (data & MAIN0_RESET_RX_RESET_ANALOG_MASK)
               >>  MAIN0_RESET_RX_RESET_ANALOG_SHIFT ;
      tx = (data & MAIN0_RESET_TX_RESET_ANALOG_MASK)
               >>  MAIN0_RESET_TX_RESET_ANALOG_SHIFT ;
      rx = (rx & ~mask) | (val & mask) ;
      tx = (tx & ~mask) | (val & mask) ;
      data = (rx<< MAIN0_RESET_RX_RESET_ANALOG_SHIFT)|
             (tx<< MAIN0_RESET_TX_RESET_ANALOG_SHIFT);
      mask = MAIN0_RESET_RX_RESET_ANALOG_MASK|MAIN0_RESET_TX_RESET_ANALOG_MASK;
      SOC_IF_ERROR_RETURN
         (MODIFY_WC40_MAIN0_RESETr
          (pc->unit, pc, data, mask)) ;
   } else if(cntl == TSCMOD_SRESET_DSC_SM) {
      if (val) {
        data = DSC2B0_DSC_MISC_CTRL0_RXSEQSTART_MASK ;
      } else {
        data = 0 ;     /* enable */
      }
      if (pc->ctrl_type & TSCMOD_CTRL_TYPE_UC_WR_MASK_EN) {
         SOC_IF_ERROR_RETURN
            (tscmod_mask_wr_cmd(pc, TSC40_DSC2B0_DSC_MISC_CTRL0r, data, DSC2B0_DSC_MISC_CTRL0_RXSEQSTART_MASK)) ;
      } else {
         SOC_IF_ERROR_RETURN    /* 0xc21e */
            (MODIFY_WC40_DSC2B0_DSC_MISC_CTRL0r
             (pc->unit, pc, data, DSC2B0_DSC_MISC_CTRL0_RXSEQSTART_MASK));
      }
   } else if(cntl == TSCMOD_SRESET_PORT_MODE) {
      SOC_IF_ERROR_RETURN
         (MODIFY_WC40_MAIN0_SETUPr
          (pc -> unit, pc, 0, MAIN0_SETUP_PORT_MODE_SEL_MASK));
   } else if(cntl == TSCMOD_SRESET_ANATX_INPUT) {
      if (val) {
        data = ANATX_ACONTROL6_DISABLE_TX_R_MASK ;
      } else {
        data = 0 ;
      }
      /* c01a. ctrl to gearbox, not real electrical idle */
      SOC_IF_ERROR_RETURN
         (MODIFY_WC40_ANATX_ACONTROL6r(pc -> unit, pc, data,
          ANATX_ACONTROL6_DISABLE_TX_R_MASK));
   } else if(cntl == TSCMOD_SRESET_FIRMWARE_MODE) {
      data = 0 ;  /* firmware_mode */
      /* 0xc260 */
      SOC_IF_ERROR_RETURN
         (WRITE_WC40_UCSS_X4_FIRMWARE_MODEr(pc->unit, pc, data));
   } else if(cntl == TSCMOD_SRESET_CL72_FW_MODE) {
      data = 7 ;  /* firmware_mode for CL72 */
      /* 0xc260 */
      SOC_IF_ERROR_RETURN
         (WRITE_WC40_UCSS_X4_FIRMWARE_MODEr(pc->unit, pc, data));
   } else if(cntl == TSCMOD_SRESET_AFE_TX_VOLT ) {
      int lane_s, lane_e, idx ;
      /* TSCOP-003: this register is shared with AMP control which might be per lane base */
      /*            Electrical idle control is better to be per port base */
      if (val) {
        data = ANATX_TX_DRIVER_ELEC_IDLE_MASK ; /* EIDLE */
      } else {
        data = 0 ;                              /* normal */
      }

      if(pc->port_type   == TSCMOD_SINGLE_PORT ) {
         lane_s = 0 ; lane_e = 4 ;
      } else if (pc->port_type == TSCMOD_DXGXS ) {
         if((pc->dxgxs & 0x3)==0x2) { lane_s = 2; }
         else { lane_s = 0;  }
         lane_e = lane_s + 2 ;
      } else {
         lane_s = pc->this_lane ;  lane_e = lane_s + 1;
      }
      pc->dxgxs = 0 ;
      for(idx=lane_s; idx<lane_e; idx++) {
         pc->this_lane   = idx ;
         pc->lane_select = getLaneSelect(pc->this_lane) ;
         mask = ANATX_TX_DRIVER_ELEC_IDLE_MASK ;
         SOC_IF_ERROR_RETURN(MODIFY_WC40_ANATX_TX_DRIVERr(pc->unit, pc, data, mask));
      }
   } else if(cntl == TSCMOD_SRESET_CL72_LINK_FAIL_TIMER) {
      if (val) {
        data = 0 ;
      } else {
        data = 0x8236 ;
      }
      SOC_IF_ERROR_RETURN    /* 9x257 */
         (WRITE_WC40_AN_X1_TIMERS_LINK_FAIL_INHIBIT_TIMER_CL72r(pc->unit, pc, data));
   } else if(cntl == TSCMOD_SRESET_CL72_MAX_WAIT_TIMER) {
      if (val) {
        data = 0 ;
      } else {
        data = 0x8236 ;
      }
      SOC_IF_ERROR_RETURN     /* 0x925a */
         (WRITE_WC40_AN_X1_TIMERS_CL72_MAX_WAIT_TIMERr(pc->unit, pc, data));
   } else if(cntl == TSCMOD_SRESET_TAP_MUXSEL) {
      /* 0xc253 */
      data = 0 ;
      mask = CL72_MISC1_CONTROL_TAP_DEFAULT_MUXSEL_FORCE_MASK |
         CL72_MISC1_CONTROL_TAP_DEFAULT_MUXSEL_FORCEVAL_MASK ;
      if (pc->ctrl_type & TSCMOD_CTRL_TYPE_UC_WR_MASK_EN) {
         SOC_IF_ERROR_RETURN
            (tscmod_mask_wr_cmd(pc, TSC40_CL72_MISC1_CONTROLr, data, mask)) ;
      } else {
         SOC_IF_ERROR_RETURN
            (MODIFY_WC40_CL72_MISC1_CONTROLr(pc->unit, pc, data, mask));
      }
   } else if(cntl == TSCMOD_SRESET_RX_SIG_DET) {
      if(val) {
         data = RX_X4_CONTROL0_RX_SIGDET_OVERRIDE_SIGNAL_OK_MASK ;
      } else {
         data = 0 ;
      }
      mask = RX_X4_CONTROL0_RX_SIGDET_OVERRIDE_SIGNAL_OK_MASK |
             RX_X4_CONTROL0_RX_SIGDET_OVERRIDE_VALUE_SIGNAL_OK_MASK ;
      /* 0xc138 */
      SOC_IF_ERROR_RETURN
         (MODIFY_WC40_RX_X4_CONTROL0_RX_SIGDETr(pc->unit, pc, data, mask));
   } else if(cntl == TSCMOD_SRESET_BAM37_ACOL_SWAP) {
      if(val) {
         data = 0x66 ;
      } else {
         data = 0x2 ;
      }
      mask = TX_X2_CONTROL0_BRCM_MODE_ACOL_SWAP_COUNT64B66B_MASK ;
      SOC_IF_ERROR_RETURN
         (MODIFY_WC40_TX_X2_CONTROL0_BRCM_MODEr(pc->unit, pc, data, mask));
   } else if(cntl == TSCMOD_SRESET_AER_ALL_PROXY) {
      pc->dxgxs= 0 ;
      pc->lane_select = getLaneSelect(pc->this_lane) ;
      data = pc->this_lane ;
      SOC_IF_ERROR_RETURN
         (WRITE_WC40_AER_ADDRESSEXPANSIONr(pc->unit, pc, data)) ;

   } else if(cntl == TSCMOD_SRESET_AER_ALL_PROXY_BLK_ADR) {
      SOC_IF_ERROR_RETURN
         (tscmod_reg_set_porxy_blk_addr(pc->unit, pc)) ;
   } else if(cntl == TSCMOD_SRESET_HP_BAM_AM) {
   } else if(cntl == TSCMOD_SRESET_HP_BAM_SWM_TIMER) {
      if(val) {
         /* allow run longer time */
         data = 0xfff0 ;
      } else {
         data = 0 ;
      }
      SOC_IF_ERROR_RETURN
         (WRITE_WC40_AN_X1_TIMERS_CL73_ERRORr(pc->unit, pc, data)) ;

   } else if(cntl == TSCMOD_SRESET_AN_MODES) {
      mask = AN_X4_ABILITIES_ENABLES_CL37_BAM_ENABLE_MASK |
             AN_X4_ABILITIES_ENABLES_CL73_BAM_ENABLE_MASK |
             AN_X4_ABILITIES_ENABLES_CL73_HPAM_ENABLE_MASK |
             AN_X4_ABILITIES_ENABLES_CL73_ENABLE_MASK |
             AN_X4_ABILITIES_ENABLES_CL37_SGMII_ENABLE_MASK |
             AN_X4_ABILITIES_ENABLES_CL37_ENABLE_MASK ;
      SOC_IF_ERROR_RETURN (MODIFY_WC40_AN_X4_ABILITIES_ENABLESr(pc->unit, pc, 0, mask));
      
      /* jir CRTSC-616 */
      data = 1 << AN_X4_ABILITIES_ENABLES_CL73_AN_RESTART_SHIFT ;
      mask = AN_X4_ABILITIES_ENABLES_CL73_AN_RESTART_MASK ;
      SOC_IF_ERROR_RETURN (MODIFY_WC40_AN_X4_ABILITIES_ENABLESr(pc->unit, pc, data, mask));
      
      data = 0 ;
      SOC_IF_ERROR_RETURN (MODIFY_WC40_AN_X4_ABILITIES_ENABLESr(pc->unit, pc, data, mask));
      
   }
   pc->lane_select = tmp_sel ;
   pc->this_lane   = tmp_lane ;
   pc->dxgxs       = tmp_dxgxs ;
   return SOC_E_NONE;
}
/*!
\brief Read the 16 bit rev. id value etc.

\param  pc handle to current TSC context (#tscmod_st)

\returns The value #SOC_E_NONE upon successful completion.  The revision id is
stored within #tscmod_st::accData.

\details
This  fucntion reads the revid register that contains core number, revision
number and stores the 16-bit value in the field #tscmod_st::accData.

<B>How to call:</B><br>
<p>Call this function directly or through the function
#tscmod_tier1_selector() with the upper-case string "REVID_READ".</p>
*/
int tscmod_revid_read(tscmod_st* pc)              /* REVID_READ */
{
   if(pc->verbosity & TSCMOD_DBG_INIT)
      printf("%-22s u=%0d p=%0d sel=%x ln=%0d dxgxs=%0d\n", FUNCTION_NAME(),
             pc->unit, pc->port, pc->lane_select, pc->this_lane, pc->dxgxs) ;
   return _tscmod_getRevDetails(pc) ;
}

/*!
\brief Control per lane clause 72 auto tuning function  (training patterns)

\param  pc handle to current TSC context (#tscmod_st)

\returns The value #SOC_E_NONE upon successful completion

\details

This function enables/disables clause-72 auto-tuning per lane.  It is used
in 40G_KR4, 40G_CR4, 10G_KR modes etc.
Lane is selected via #tscmod_st::lane_select.
This Function is controlled by #tscmod_st::per_lane_control as follows.

\li 0Xxxxxxxx3      (b11): Enable  CL72 hardware block    (TSCMOD_CL72_HW_ENABLE)
\li 0Xxxxxxxx2      (b01): Disable CL72 hardware block    (TSCMOD_CL72_HW_DISABLE)
\li 0Xxxxxxxx0      (b00): no changed.
\li 0Xxxxxxxx8    (b1000): assert restart bit             (TSCMOD_CL72_HW_RESTART)
\li 0Xxxxxxx3- (b011----): Forced  enable CL72 under AN.  (TSCMOD_CL72_AN_FORCED_ENABLE)
\li 0Xxxxxxx7- (b111----): Forced  disable CL72 under AN. (TSCMOD_CL72_AN_FORCED_DISABLE)
\li 0Xxxxxxx1- (b001----): Default CL72 under AN.         (TSCMOD_CL72_AN_NO_FORCED)
\li 0Xxxxxxx0- (b000----): no changed
\li 0Xxxxxx100           : reset CL72 coarse lock and trained status.
\li 0Xxxxxx200           : read back CL72 HW en bit and put in accData.

This function modifies the following registers:

\li DEV1_IEEE9_PMD_10GBASE_KR_PMD_CONTROL_REGISTER_150 (0x0800_0096)
\li CL72_MISC1_CONTROL                                 (0xc253)

<B>How to call:</B><br>
<p>Call this function directly or through the function
#tscmod_tier1_selector() with the upper-case string
"CLAUSE_72_CONTROL".</p>
*/

int tscmod_clause72_control(tscmod_st* pc)                /* CLAUSE_72_CONTROL */
{
   uint16 data , mask;
   int cntl ;

   cntl = pc->per_lane_control & 0xf ;

   if(cntl & TSCMOD_CL72_HW_DISABLE) {  /* enable or disable CL72 HW. */
      data = (cntl>>1) <<
       DEV1_IEEE9_PMD_10GBASE_KR_PMD_CONTROL_REGISTER_150_TRAINING_ENABLE_SHIFT;
      mask =
       DEV1_IEEE9_PMD_10GBASE_KR_PMD_CONTROL_REGISTER_150_TRAINING_ENABLE_MASK|
       DEV1_IEEE9_PMD_10GBASE_KR_PMD_CONTROL_REGISTER_150_RESTART_TRAINING_MASK ;
      SOC_IF_ERROR_RETURN
         (MODIFY_WC40_DEV1_IEEE9_PMD_10GBASE_KR_PMD_CONTROL_REGISTER_150r
          (pc->unit, pc, data, mask));
   }

   if(cntl & TSCMOD_CL72_HW_RESTART) {
      data = 1 ;
      mask = DEV1_IEEE9_PMD_10GBASE_KR_PMD_CONTROL_REGISTER_150_RESTART_TRAINING_MASK ;

      SOC_IF_ERROR_RETURN
         (MODIFY_WC40_DEV1_IEEE9_PMD_10GBASE_KR_PMD_CONTROL_REGISTER_150r
          (pc->unit, pc, data, mask));
   }

   cntl = pc->per_lane_control ;
   if(cntl & TSCMOD_CL72_STATUS_RESET ) {
      data = 0 ;
      mask = CL72_MISC1_CONTROL_TR_COARSE_LOCK_MASK |
             CL72_MISC1_CONTROL_RX_TRAINED_MASK ;
      if(pc->ctrl_type&TSCMOD_CTRL_TYPE_UC_WR_MASK_EN)
         SOC_IF_ERROR_RETURN
            (tscmod_mask_wr_cmd(pc, TSC40_CL72_MISC1_CONTROLr, data, mask)) ;
      else
         SOC_IF_ERROR_RETURN
            (MODIFY_WC40_CL72_MISC1_CONTROLr(pc->unit, pc, data, mask));

      /* 0xc250 */
      SOC_IF_ERROR_RETURN
         (WRITE_WC40_CL72_XMT_CONTROL_REGISTERr(pc->unit, pc, 0)) ;
   }
   if(cntl & TSCMOD_CL72_STATUS_READ ) {
      SOC_IF_ERROR_RETURN
         (READ_WC40_DEV1_IEEE9_PMD_10GBASE_KR_PMD_CONTROL_REGISTER_150r
          (pc->unit, pc, &data));
      if(data&DEV1_IEEE9_PMD_10GBASE_KR_PMD_CONTROL_REGISTER_150_TRAINING_ENABLE_MASK) {
          pc->accData = 1 ;
      } else {
          pc->accData = 0 ;
      }
   }
   
   cntl = (pc->per_lane_control & 0xf0) >> 4 ;
   if(cntl&1) {
      switch(cntl) {
      case 7:   /* disable CL72 under AN */
         data = 0 << CL72_MISC1_CONTROL_LINK_CONTROL_FORCEVAL_SHIFT |
                1 << CL72_MISC1_CONTROL_LINK_CONTROL_FORCE_SHIFT ;
         break ;
      case 3:   /* enable CL72 under AN */
         data = 1 << CL72_MISC1_CONTROL_LINK_CONTROL_FORCEVAL_SHIFT |
                1 << CL72_MISC1_CONTROL_LINK_CONTROL_FORCE_SHIFT ;

         /* may need to enable CL72 HW */
         break ;
      default:  /* default CL72 under AN */
         data = 0 << CL72_MISC1_CONTROL_LINK_CONTROL_FORCEVAL_SHIFT |
            0 << CL72_MISC1_CONTROL_LINK_CONTROL_FORCE_SHIFT ;

         /* may need to enable CL72 HW */
         break ;
      }
      mask = CL72_MISC1_CONTROL_LINK_CONTROL_FORCEVAL_MASK |
             CL72_MISC1_CONTROL_LINK_CONTROL_FORCE_MASK ;
      if (pc->ctrl_type&TSCMOD_CTRL_TYPE_UC_WR_MASK_EN) {
         SOC_IF_ERROR_RETURN
            (tscmod_mask_wr_cmd(pc, TSC40_CL72_MISC1_CONTROLr, data, mask)) ;
      } else {
         SOC_IF_ERROR_RETURN(MODIFY_WC40_CL72_MISC1_CONTROLr
                             (pc->unit, pc, data, mask));
      }
   }
   return SOC_E_NONE;
}

/*!
\brief Check PRBS sync status

\param  pc handle to current TSC context (#tscmod_st)

\returns #SOC_E_NONE if prbs lock with no errors; #SOC_E_TIMEOUT for a timeout,
meaning that either the prbs did not lock, or that prbs had errors, or both

\details
This function reads from one of PATGEN0_PRBSSTAT0 register within 4 lanes.
Retunr value reads 0x8000, meaning that prbs has locked and there are no
prbs errors, OR no lock (bit[15]=0) or errors (bit[14:0]!=0)

The errors are stored in #tscmod_st::accData, as -2 if lost lock, -1 if no lock,
0 is locked and error free.  1-n means erros found.

\li PATGEN0_PRBSSTAT0 (0xC032)
\li PATGEN0_PRBSSTAT1 (0xC033)
\li PATGEN0_TESTMISCSTATUSr (0xC034)

<B>How to call:</B><br>
<p>Call this function directly or through the function #tscmod_tier1_selector()
with the upper-case string "PRBS_CHECK".</p>

*/
int tscmod_prbs_check(tscmod_st* pc)
{
   uint16 data, dataL, dataH ;  int lock, lock_lost, err ;
   SOC_IF_ERROR_RETURN
      (READ_WC40_PATGEN0_TESTMISCSTATUSr(pc->unit, pc, &data));
   SOC_IF_ERROR_RETURN
      (READ_WC40_PATGEN0_PRBSSTAT0r(pc->unit, pc, &dataH));
   SOC_IF_ERROR_RETURN
      (READ_WC40_PATGEN0_PRBSSTAT1r(pc->unit, pc, &dataL));


   lock      = (data&PATGEN0_TESTMISCSTATUS_PRBS_LOCK_MASK) ? 1: 0 ;
   lock_lost = (dataH & PATGEN0_PRBSSTAT0_PRBS_LOCK_LOST_LH_MASK)?1:0 ;
   err       = dataH & ~PATGEN0_PRBSSTAT0_PRBS_LOCK_LOST_LH_MASK ;
   err       = err <<16 | dataL ;
   if(pc->verbosity & TSCMOD_DBG_PAT) {
      printf("prbs_check u=%0d p=%0d ln=%0d lck=%0d lost=%0d err=%0d(H%x L%x)\n",
             pc->unit, pc->port, pc->this_lane, lock, lock_lost, err, dataH, dataL);
   }
   if(lock) {
      if(lock_lost) {
         /* locked now but lost before */
         pc->accData = -2;
      } else {
         pc->accData = err;
         SOC_IF_ERROR_RETURN (READ_WC40_PATGEN0_PRBSCTRLr(pc->unit, pc, &data));
         if(data & PATGEN0_PRBSCTRL_PRBS_CHECK_MODE_MASK ) {
             /* assume no reporting 2X factor of bit error */
         } else {
             pc->accData = ((pc->accData+1) >>1) ;
         }
      }
   } else {
      pc->accData = -1 ;  /* never locked */
   }
   return SOC_E_NONE ;
}

/*!
\brief program sets prbs seed value for A1 an A0

\param  pc handle to current TSC context (#tscmod_st)

\returns The vluae #SOC_E_NONE upon successful completion

Some side effects of this are.

This function is used to load lower 16 bits of prbs seed values
To change the prbs settings, set the #tscmod_st::lane_select field to the
desired lane number(s) and set the #tscmod_st::per_lane_control field to the
desired lane number. Only the prbs settings for the lane number as indicated by
the #tscmod_st::lane_select field are updated;  prbs settings for the other lanes
are not affected by a call to this function.


<B>How to call:</B><br>
<p>Call this function directly or through the function #tscmod_tier1_selector()
with the upper-case string "PRBS_SEED_A1A0".</p>

*/
int tscmod_prbs_seed_a1a0(tscmod_st* pc)                /* PRBS_SEED_A1A0 */
{
  uint16 data , mask;

  int seed_a0, seed_a1 ;

  seed_a0 = (pc->per_lane_control & 0xffff);
  seed_a1 = (pc->per_lane_control & 0xffff0000) >> 16;

  data = seed_a0 << PKTGEN0_PCS_SEEDA0_SEEDA0_SHIFT ;
  mask = PKTGEN0_PCS_SEEDA0_SEEDA0_MASK ;
  /* 0x9037 */
  SOC_IF_ERROR_RETURN (MODIFY_WC40_PKTGEN0_PCS_SEEDA0r(pc->unit, pc, data, mask));

  data = seed_a1 << PKTGEN0_PCS_SEEDA1_SEEDA1_SHIFT ;
  mask = PKTGEN0_PCS_SEEDA1_SEEDA1_MASK ;
  /* 0x9038 */
  SOC_IF_ERROR_RETURN (MODIFY_WC40_PKTGEN0_PCS_SEEDA1r(pc->unit, pc, data, mask));

  return SOC_E_NONE;
} /* PRBS_SEED_A1A0 */

/*!
\brief program sets prbs seed value for A3 an A2

\param  pc handle to current TSC context (#tscmod_st)

\returns The vluae #SOC_E_NONE upon successful completion

Some side effects of this are.

This function is used to load lower 16 bits of prbs seed values
To change the prbs settings, set the #tscmod_st::lane_select field to the
desired lane number(s) and set the #tscmod_st::per_lane_control field to the
desired lane number. Only the prbs settings for the lane number as indicated by
the #tscmod_st::lane_select field are updated;  prbs settings for the other lanes
are not affected by a call to this function.


<B>How to call:</B><br>
<p>Call this function directly or through the function #tscmod_tier1_selector()
with the upper-case string "PRBS_SEED_A1A0".</p>

*/
int tscmod_prbs_seed_a3a2(tscmod_st* pc)                /* PRBS_SEED_A3A2 */
{
  uint16 data , mask;

  int seed_a2, seed_a3 ;

  seed_a2 = (pc->per_lane_control & 0xffff);
  seed_a3 = (pc->per_lane_control & 0xffff0000) >> 16;

  data = seed_a2 << PKTGEN0_PCS_SEEDA2_SEEDA2_SHIFT ;
  mask = PKTGEN0_PCS_SEEDA2_SEEDA2_MASK ;
  /* 0x9039 */
  SOC_IF_ERROR_RETURN (MODIFY_WC40_PKTGEN0_PCS_SEEDA2r(pc->unit, pc, data, mask));

  data = seed_a3 << PKTGEN0_PCS_SEEDA3_SEEDA3_SHIFT ;
  mask = PKTGEN0_PCS_SEEDA3_SEEDA3_MASK ;
  /* 0x903a */
  SOC_IF_ERROR_RETURN (MODIFY_WC40_PKTGEN0_PCS_SEEDA3r(pc->unit, pc, data, mask));

  return SOC_E_NONE;
} /* PRBS_SEED_A3A2 */

/*!
\brief program the prbs settings

\param  pc handle to current TSC context (#tscmod_st)

\returns The vluae #SOC_E_NONE upon successful completion

<B>How to call:</B><br>
<p>Call this function directly or through the function #tscmod_tier1_selector()
with the upper-case string "PRBS_SEED_LOAD_CONTROL".</p>

*/
int tscmod_prbs_seed_load_control(tscmod_st* pc)                /* PRBS_SEED_LOAD_CONTROL */
{

  int cntl = pc -> per_lane_control;

  if (cntl) {
    SOC_IF_ERROR_RETURN (MODIFY_WC40_PATGEN0_PRBSCTRLr(pc->unit, pc,
                         PATGEN0_PRBSCTRL_LOAD_PRBS_SEED_BITS << PATGEN0_PRBSCTRL_LOAD_PRBS_SEED_SHIFT,
                         PATGEN0_PRBSCTRL_LOAD_PRBS_SEED_MASK));
  } else {
    SOC_IF_ERROR_RETURN (MODIFY_WC40_PATGEN0_PRBSCTRLr(pc->unit, pc, 0x0,
                         PATGEN0_PRBSCTRL_LOAD_PRBS_SEED_MASK));
  }

  return SOC_E_NONE;
} /* PRBS_SEED_LOAD_CONTROL */

/*!
\brief program the prbs settings

\param  pc handle to current TSC context (#tscmod_st)

\returns The vluae #SOC_E_NONE upon successful completion

The 1st byte of #tscmod_st::per_lane_control is for TX and 3rd byte is
for RX.  The bits of each byte control their
lanes as follows.

\li 3:3 invert_data
\li 2:0 polynomial mode (prbs7, prbs9, prbs11, prbs15, prbs23, prbs31, prbs58).

<B>How to call:</B><br>
<p>Call this function directly or through the function #tscmod_tier1_selector()
with the upper-case string "PRBS_MODE".</p>

*/
int tscmod_prbs_mode(tscmod_st* pc)                /* PRBS_MODE */
{
     uint16 data , mask;

  int prbs_sel_tx, prbs_inv_tx ;
  int prbs_sel_rx, prbs_inv_rx ;
  int prbs_chk_mode;

  prbs_sel_rx = (pc->per_lane_control & 0x700) >> 8;
  prbs_inv_rx = (pc->per_lane_control & 0x800) >> 11 ;
  prbs_sel_tx = (pc->per_lane_control & 0x7) ;
  prbs_inv_tx = (pc->per_lane_control & 0x8)>>3;
  prbs_chk_mode = (pc->per_lane_control & 0x7000) >> 12;

  data = prbs_sel_tx << PATGEN0_PRBSCTRL_PRBS_SEL_TX_SHIFT |
         prbs_inv_tx << PATGEN0_PRBSCTRL_PRBS_INV_TX_SHIFT |
         prbs_sel_rx << PATGEN0_PRBSCTRL_PRBS_SEL_RX_SHIFT |
         prbs_inv_rx << PATGEN0_PRBSCTRL_PRBS_INV_RX_SHIFT |
         prbs_chk_mode << PATGEN0_PRBSCTRL_PRBS_CHECK_MODE_SHIFT;


  mask = PATGEN0_PRBSCTRL_PRBS_SEL_TX_MASK | PATGEN0_PRBSCTRL_PRBS_INV_TX_MASK |
         PATGEN0_PRBSCTRL_PRBS_SEL_RX_MASK | PATGEN0_PRBSCTRL_PRBS_INV_RX_MASK |
         PATGEN0_PRBSCTRL_PRBS_CHECK_MODE_MASK;

  SOC_IF_ERROR_RETURN (MODIFY_WC40_PATGEN0_PRBSCTRLr(pc->unit, pc, data, mask));

  return SOC_E_NONE;

} /* PRBS_MODE */

/*!
\brief program the prbs settings

\param  pc handle to current TSC context (#tscmod_st)

\returns The vluae #SOC_E_NONE upon successful completion

Some side effects of this are.

<ol>
<li> the  first byte [3:0] (0x-f) is for TX enable
<li> the  first byte [7:4] (0xf-) is for TX enable mask ;
<li> the second byte [3:0] is for RX enable
<li> the second byte [7:4] is for RX enable mask ;
</ol>


The 1st (TX), 2nd(RX) byte of #tscmod_st::per_lane_control is associated
with TX and RX respectively. The bits of each byte control their
lanes as follows.

\li 0: this_lane

Port mode should become multiple lane.  Flex port suport is TBD.

The bus witdh selection(testgen_bus_width) is based on prior speed mode's
code block. Thus the credit parameters doesn't require re-calculation.

<B>How to call:</B><br>
<p>Call this function directly or through the function #tscmod_tier1_selector()
with the upper-case string "PRBS_CONTROL".</p>

*/
int tscmod_prbs_control(tscmod_st* pc)                /* PRBS_CONTROL */
{
   uint16 data , mask, tx_mask, rx_mask ;

   int prbs_rx_en, prbs_tx_en, tx_bus_width, rx_bus_width;   int tmp_sel, tmp_lane ;

   tx_bus_width    = 0 ;
   rx_bus_width    = 0 ;

   prbs_rx_en = ((pc->per_lane_control & 0x0f00) >> 8) ? 1: 0 ;
      rx_mask = ((pc->per_lane_control & 0xf000) >> 8) ? 1: 0 ;
   prbs_tx_en = (pc->per_lane_control & 0x0f) ? 1 : 0 ;
      tx_mask = (pc->per_lane_control & 0xf0) ? 1 : 0 ;

  tmp_sel         = pc->lane_select ;
  tmp_lane        = pc->this_lane ;

  pc->lane_select = TSCMOD_LANE_BCST ;
  /* setup multi-port mode for prbs 0x9000 */
  /* no flex port support */
  /* coverity [end_of_scope] */
  SOC_IF_ERROR_RETURN (MODIFY_WC40_MAIN0_SETUPr(pc -> unit, pc, 0, MAIN0_SETUP_PORT_MODE_SEL_MASK));
  pc->lane_select = tmp_sel ;
  pc->this_lane   = tmp_lane ;

  /* enable PRBS */
  if((pc->spd_intf <= TSCMOD_SPD_5000) || /* for all speed modes with cl36 80bit mode should be 1*/
     (pc->spd_intf == TSCMOD_SPD_10000) ||
     (pc->spd_intf == TSCMOD_SPD_10000_HI) ||/* 80bit CL48 (xaui) */
     (pc->spd_intf == TSCMOD_SPD_20000) ||
     (pc->spd_intf == TSCMOD_SPD_20000_SCR)) { 
     tx_bus_width = 1 ; rx_bus_width = 1 ;     /* =1: 80bit, =0: 66 bit */
  } else if (pc->spd_intf ==TSCMOD_SPD_10000_XFI) {
     tx_bus_width = 0 ; rx_bus_width = 0 ;
  } else {
     tx_bus_width = 0 ; rx_bus_width = 0 ;
  }

  data = prbs_rx_en << PATGEN0_PATGENCTRL_PRBS_RX_EN_SHIFT |
         prbs_tx_en << PATGEN0_PATGENCTRL_PRBS_TX_EN_SHIFT |
         tx_bus_width << PATGEN0_PATGENCTRL_TESTGEN_BUS_WIDTH_TX_SHIFT |
         rx_bus_width << PATGEN0_PATGENCTRL_TESTGEN_BUS_WIDTH_RX_SHIFT |
         0 << PATGEN0_PATGENCTRL_TESTSOURCE_SEL_SHIFT ;

  mask = 0 ;
  if(tx_mask) mask |= PATGEN0_PATGENCTRL_PRBS_TX_EN_MASK ;
  if(rx_mask) mask |= PATGEN0_PATGENCTRL_PRBS_RX_EN_MASK ;

  mask |=
         PATGEN0_PATGENCTRL_TESTGEN_BUS_WIDTH_TX_MASK |
         PATGEN0_PATGENCTRL_TESTGEN_BUS_WIDTH_RX_MASK |
         PATGEN0_PATGENCTRL_TESTSOURCE_SEL_MASK ;

  /* 0xc030 */
  SOC_IF_ERROR_RETURN (MODIFY_WC40_PATGEN0_PATGENCTRLr(pc->unit, pc, data, mask));

  return SOC_E_NONE;
}  /* PRBS_CONTROL */

/*!
\brief This is the decoupled PRBS control Tier1

\param  pc handle to current TSC context (#tscmod_st)

\returns #SOC_E_NONE upon successful completion

\details

Set TX and RX to different PRBS polys. Also supports PRBS9 and PRBS11. Uses
different set of registers from #tscmod_prbs_control.

The 1st, 2nd, 3rd and 4th byte of #tscmod_st::per_lane_control is associated
with lanes 0, 1, 2, and 3 respectively.

\li 7:7 : Set TX parameters
\li 6:6 : Set RX parameters
\li 5:5 : PRBS decouple enable or disable
\li 4:4 : PRBS enable or disable
\li 3:3 : PRBS inversion
\li 0:2 : PRBS order

<B>How to call:</B><br>
<p>Call this function directly or through the function #tscmod_tier1_selector() using the
case-sensitive string "PRBS_DECOUPLE_CONTROL".</p>

*/
int tscmod_prbs_decouple_control(tscmod_st* pc)        /* PRBS_DECOUPLE_CONTROL */
{
  /* fix this */
  return SOC_E_ERROR;
}

/*!
\brief Sets CJPAT/CRPAT parameters for a particular port

\param  pc handle to current TSC context (#tscmod_st)

\returns The value #SOC_E_NONE upon successful completion

This function enables either a CJPAT or CRPAT for a particular port.
The speed mode of the lane must be chosen from the enumerated type
#tscmod_spd_intfc_type and set in the
#tscmod_st::spd_intf field.  To enable CJPAT, set #tscmod_st::per_lane_control to
the value #USE_CJPAT; to enable CRPAT, set #tscmod_st::per_lane_control to the
value #USE_CRPAT.  This function is used in tandem with tscmod_tx_bert_control() and
tscmod_cjpat_crpat_check().

This function modifies the following registers:

\li PKTGEN0_PKTGENCTRL1 (0x9030)
\li PKTGEN0_PKTGENCTRL2 (0x9031)

<B>How to call:</B><br>
<p>Call this function directly or through the function #tscmod_tier1_selector()
using the upper case string "CJPAT_CRPAT_CONTROL".</p>
*/

int tscmod_cjpat_crpat_control(tscmod_st* pc)     /* CJPAT_CRPAT_CONTROL  */
{
   int number_pkt, crc_check, intf_type, rx_port_sel, tx_port_sel, pkt_or_prtp ;
   int pkt_size, ipg_size, payload_type ;
   uint16 data16 ;


   number_pkt = 2 ; /* unlimited, 0: idles, 1: single pkt */
   crc_check  = 1 ;
   intf_type  = 0 ; /* 0: XLGMII/XGMII;  1: MIII/GMII */
   rx_port_sel= 1 ;
   tx_port_sel= 1 ;
   pkt_or_prtp= 0 ; /* 0: pakcet generator;  1: PRTP(pseudo random test pattern) */

   pkt_size   = 1 ;
   ipg_size   = 8 ;

   /* need to check the interface type */
   if(pc->per_lane_control == USE_CJPAT ) {
      payload_type = 3 ;
   } else {
      payload_type = 2 ;  /* CRPAT */
   }

   /* 0x9030 */
   data16 = number_pkt << PKTGEN0_PKTGENCTRL1_NUMBER_PKT_SHIFT |
            crc_check  << PKTGEN0_PKTGENCTRL1_RX_PKT_CHECK_EN_SHIFT |
            intf_type  << PKTGEN0_PKTGENCTRL1_RX_MSBUS_TYPE_SHIFT |
            rx_port_sel << PKTGEN0_PKTGENCTRL1_RX_PORT_SEL_SHIFT |
            tx_port_sel << PKTGEN0_PKTGENCTRL1_TX_TEST_PORT_SEL_SHIFT |
            pkt_or_prtp << PKTGEN0_PKTGENCTRL1_PKT_OR_PRTP_SHIFT ;
   SOC_IF_ERROR_RETURN
      (WRITE_WC40_PKTGEN0_PKTGENCTRL1r(pc -> unit, pc, data16));

   /* 0x9031 */
   data16 = intf_type << PKTGEN0_PKTGENCTRL2_TX_MSBUS_TYPE_SHIFT |
            payload_type << PKTGEN0_PKTGENCTRL2_PAYLOAD_TYPE_SHIFT |
            pkt_size     << PKTGEN0_PKTGENCTRL2_PKT_SIZE_SHIFT |
            ipg_size     << PKTGEN0_PKTGENCTRL2_IPG_SIZE_SHIFT ;
   SOC_IF_ERROR_RETURN
      (WRITE_WC40_PKTGEN0_PKTGENCTRL2r(pc -> unit, pc, data16));

   return SOC_E_NONE;
}

/*!
\brief Checks CJPAT/CRPAT parameters for a particular port

\param  pc handle to current TSC context (#tscmod_st)

\returns The value #SOC_E_NONE upon successful completion

This function checks for CJPAT or CRPAT for a particular port.
The speed mode of the lane must be chosen from the
enumerated type #tscmod_spd_intfc_type and set in the #tscmod_st::spd_intf field.
CJPAT/CRPAT must first enabled using #tscmod_cjpat_crpat_control(). Packet
transmission is next enabled using #tscmod_tx_bert_control() . Packet
transmission is disabled using #tscmod_tx_bert_control() again. The duration of
packet transmission is controlled externally and is the time duration between
the two function calls above.

This function compares the CJPAT/CRPAT TX and RX counters and prints the
results.  Currently only packet count is supported.

This function reads the following registers:

\li PATGEN1_TXPKTCNT_U (0xC040) Upper 16b of count of Transmitted packets
\li PATGEN1_TXPKTCNT_L (0xC041) Lower 16b of count of Transmitted packets
\li PATGEN1_RXPKTCNT_U (0xC042) Upper 16b of count of Recieved    packets
\li PATGEN1_RXPKTCNT_L (0xC043) Lower 16b of count of Recieved    packets
\li PKTGEN0_CRCERRORCOUNT (0x9033) CRC error conunt

<B>How to call:</B><br>
<p>Call this function directly or through the function #tscmod_tier1_selector()
using the upper case string "CJPAT_CRPAT_CHECK".</p>
*/

int tscmod_cjpat_crpat_check(tscmod_st* pc)     /* CJPAT_CRPAT_CHECK */
{
   uint32 tx_cnt, rx_cnt ;
   uint16 data16 ;

   SOC_IF_ERROR_RETURN(READ_WC40_PATGEN1_TXPKTCNT_Ur(pc->unit,pc,&data16));
   tx_cnt = data16 ; tx_cnt = (tx_cnt <<16) ;
   SOC_IF_ERROR_RETURN(READ_WC40_PATGEN1_TXPKTCNT_Lr(pc->unit,pc,&data16));
   tx_cnt |= data16 ;

   SOC_IF_ERROR_RETURN(READ_WC40_PATGEN1_TXPKTCNT_Ur(pc->unit,pc,&data16));
   rx_cnt = data16 ; rx_cnt = (rx_cnt <<16) ;
   SOC_IF_ERROR_RETURN(READ_WC40_PATGEN1_TXPKTCNT_Lr(pc->unit,pc,&data16));
   rx_cnt |= data16 ;

   pc->accData = 1 ;
   if( (tx_cnt != rx_cnt) ||(tx_cnt==0) ) {
      printf("Error: u=%0d p=%0d tx_cnt=%0d rx_cnt=%0d mismatch\n",
             pc->unit, pc->port, tx_cnt, rx_cnt);
      pc->accData = 0 ;
      return SOC_E_PARAM;
   } else {
      printf("Note: u=%0d p=%0d tx_cnt=%0d rx_cnt=%0d mismatch\n",
             pc->unit, pc->port, tx_cnt, rx_cnt);
   }

   SOC_IF_ERROR_RETURN(READ_WC40_PKTGEN0_CRCERRORCOUNTr(pc->unit,pc,&data16));
   if(data16) {
      printf("Error: u=%0d p=%0d crc error=%0d\n", pc->unit, pc->port, data16);
      pc->accData = 0 ;
      return SOC_E_NONE;
   }
   return SOC_E_NONE;
}
/*!
\brief  Enables or disables the bit error rate testing for a particular lane.

\param  pc handle to current TSC context (#tscmod_st)

\returns The value #SOC_E_NONE upon successful completion.

\details

This function enables or disables bit-error-rate testing (BERT)
for a particular lane as indicated by #tscmod_st::this_lane from data
generated by PRBS, CJPAT, or CRPAT.  This function must be used in
conjunction with #tscmod_prbs_control() or #tscmod_cjpat_crpat_control().

Using PRBS without this function results in traffic that is
continuous, but not packetized; using PRBS with this function
results in traffic that is packetized.  Currently only enabling CJPAT/CRPAT
is supported.

This function modifies the following registers:

\li  PKTGEN0_PKTGENCTRL1 (0x9030)
\li  PKTGEN0_PKTGENCTRL2 (0x9031)

<B>How to call:</B><br>
<p>Call this function directly or through the function #tscmod_tier1_selector()
with the upper-case string "TX_BERT_CONTROL".</p>

*/
int tscmod_tx_bert_control(tscmod_st* pc)         /* TX_BERT_CONTROL  */
{
   uint16 data, mask, cntl ;

   cntl = pc->per_lane_control ;

   if( cntl ) {
      /* clr crc error */
      data = PKTGEN0_PKTGENCTRL1_RX_MSBUS_TYPE_MASK ;
      mask = PKTGEN0_PKTGENCTRL1_RX_MSBUS_TYPE_MASK ;
      SOC_IF_ERROR_RETURN
         (MODIFY_WC40_PKTGEN0_PKTGENCTRL2r(pc->unit, pc, data, mask)) ;

      SOC_IF_ERROR_RETURN
         (MODIFY_WC40_PKTGEN0_PKTGENCTRL2r(pc->unit, pc, 0, mask)) ;

      data = 1 << PKTGEN0_PKTGENCTRL2_PKTGEN_EN_SHIFT ;
      mask =      PKTGEN0_PKTGENCTRL2_PKTGEN_EN_MASK  ;  /* bit 15 */

      SOC_IF_ERROR_RETURN
         (MODIFY_WC40_PKTGEN0_PKTGENCTRL2r(pc->unit, pc, data, mask)) ;

   } else {

      /* disable the traffic */
      mask =      PKTGEN0_PKTGENCTRL2_PKTGEN_EN_MASK  ;  /* bit 15 */

      SOC_IF_ERROR_RETURN
         (MODIFY_WC40_PKTGEN0_PKTGENCTRL2r(pc->unit, pc, 0, mask)) ;

   }
   return SOC_E_NONE;
}

/*!
\brief sets both the RX and TX lane swap values for all lanes simultaneously.

\param  pc handle to current TSC context (#tscmod_st)

\returns The value #SOC_E_NONE upon successful completion

This function sets the RX and TX lane swap values for all lanes simultaneously.

The TSC has several two sets of lane swap registers per lane.  This function uses the
lane swap registers closest to the pads.  For TX, the lane swap occurs after all other
analog/digital functionality.  For RX, the lane swap occurs prior to all other
analog/digital functionality.

It is not advisable to swap lanes without going through reset.

How the swap is done is indicated in the #tscmod_st::per_lane_control field;
bits 0 through 15 represent the swap settings for RX while bits 16
through 31 represent the swap settings for TX.

Each 4 bit chunk within both the RX and TX sections represent the swap settings
for a particular lane as follows:

Bits lane assignment

\li 31:28  RX lane 3
\li 27:24  RX lane 2
\li 23:20  RX lane 1
\li 19:16  RX lane 0

\li 15:12  TX lane 3
\li 11:8   TX lane 2
\li 7:4    TX lane 1
\li 3:0    TX lane 0

Each 4-bit value may be either 0, 1, 2 or 3, with the value indicating the lane
mapping.  By default, the equivalent per_lane_control settings are 0x3210_3210,
indicating that there is no re-routing of traffic.  As an example, to swap lanes
0 and 1 for both RX and TX, the per_lane_control should be set to 0x3201_3201.
To swap lanes 0 and 1, and also lanes 2 and 3 for both RX and TX, the
per_lane_control should be set to 0x2301_2301.

This function modifies the following registers:

\li MAIN0_LANE_SWAP (0x9003)

<B>How to call:</B><br>
<p>Call this function directly or through the function "tscmod_tier1_selector"
with the upper-case string "LANE_SWAP".</p>

*/
int tscmod_lane_swap(tscmod_st *pc)
{
  unsigned int map, shift, rx_map, tx_map, n, mask;

  pc->lane_num_ignore = 0;

  map = (unsigned int)pc->per_lane_control;
  rx_map = 0;
  tx_map = 0;
  /* 3 2 1 0 needs to map to 3 2 1 0 for RX,  for TX */
  for(n = 0, shift = 0; n < 4; ++n, shift += 2) {
    mask = 0x3 << shift;
    rx_map |= (map >> (shift+16)) & mask;
    tx_map |= (map >> (shift)) & mask;
  }
  if(pc->verbosity & TSCMOD_DBG_FUNC)
     printf("%s u=%0d p=%0d map=%x tx_map=%x rx_map=%x\n", FUNCTION_NAME(), pc->unit, pc->port, map, tx_map, rx_map) ;

  map = (rx_map <<8) | tx_map ;

  SOC_IF_ERROR_RETURN
     (MODIFY_WC40_MAIN0_LANE_SWAPr(pc->unit, pc, map, 0xffff)) ;

  return SOC_E_NONE ;
}

/*!
\brief Set polarity for all lanes on the RX and TX simultaneously.

\param  pc handle to current TSC context (#tscmod_st)

\returns The value #SOC_E_NONE upon successful completion.

\details
The polarity is updated with the settings as indicated in the
#tscmod_st::per_lane_control field. The bit assignments are as follows

Bits

\li 11  TX/RX lane3
\li 10  TX/RX lane2
\li 9   TX/RX lane1
\li 8   TX/RX lane0

\li 7   TX/RX lane3 enable
\li 6   TX/RX lane2 enable
\li 5   TX/RX lane1 enable
\li 4   TX/RX lane0 enable

\li 3   -
\li 2   -
\li 1   1 for TX; 0 for RX
\li 0   enabling or not

Update the registers below

\li TX  ANATX_ACONTROL0 (0xc011) per lane
\li RX  ANARX_ACONTROL0 (0xc021) per lane

<B>How to call:</B><br>
<p>Call this function directly or through the function #tscmod_tier1_selector()
using the upper case string "SET_POLARITY".</p>
*/

int tscmod_tx_rx_polarity(tscmod_st *pc)
{
   int tmp_sel, tmp_lane, tmp_dxgxs, vec, mask, i, flip, is_tx ;
   int lane_s, lane_size ;

   if( (pc->per_lane_control & 0x1) ==0 ) {
      return SOC_E_NONE ;
   }
   tmp_sel  = pc->lane_select ;
   tmp_lane = pc->this_lane ;
   tmp_dxgxs= pc->dxgxs ;

   vec   = (pc->per_lane_control >> 8) & 0xf ;
   mask  = (pc->per_lane_control >> 4) & 0xf ;
   is_tx = (pc->per_lane_control & 0x2) ? 1 : 0 ;
   if(pc->port_type   == TSCMOD_SINGLE_PORT ) {
      lane_s = 0 ; lane_size = 4 ;
   } else if (pc->port_type == TSCMOD_DXGXS ) {
      lane_size = 2 ;
      if((pc->dxgxs & 0x3)==0x2) { lane_s = 2; }
      else { lane_s = 0;  }
   } else {
      lane_s = pc->this_lane ;  lane_size = 1;
   }
   if(pc->verbosity&TSCMOD_DBG_PATH)
      printf("%-22s: u=%0d p=%0d tx=%0d vec=%x mask=%x lane_s=%0d size=%0d\n",
             FUNCTION_NAME(), pc->unit, pc->port, is_tx, vec, mask, lane_s, lane_size);
   pc->lane_select = TSCMOD_LANE_0_0_0_1 ;
   pc->this_lane   = 0 ;
   pc->dxgxs       = 0 ;

   for(i=0; i<lane_size; i++) {
      if(mask & (1<<i)) {
         pc->this_lane = i+lane_s ;
         flip = (vec & (1<<i)) ? 1: 0 ;
         if(is_tx) { /*tx*/
            SOC_IF_ERROR_RETURN
               (MODIFY_WC40_ANATX_ACONTROL0r(pc->unit, pc,
                  (flip<<ANATX_ACONTROL0_TXPOL_FLIP_SHIFT),
                         ANATX_ACONTROL0_TXPOL_FLIP_MASK));
         } else { /* rx */
            SOC_IF_ERROR_RETURN
               (MODIFY_WC40_ANARX_ACONTROL0r(pc->unit, pc,
                  (flip<<ANARX_ACONTROL0_RXPOL_FLIP_SHIFT),
                         ANARX_ACONTROL0_RXPOL_FLIP_MASK));
         }
      }
   }
   pc->lane_select = tmp_sel ;
   pc->this_lane   = tmp_lane ;
   pc->dxgxs       = tmp_dxgxs ;

   return SOC_E_NONE;
}

/*!
\brief Program a data pattern to be transmitted out on a particular lane

\param  pc handle to current TSC context (#tscmod_st)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.

\details

This function sets a pattern to be repeatedly transmitted for a particular lane.
The pattern is transmitted LSB first.  12, 40, 66, 126, 130, 256 bits are supported
in TSC.

To set the data pattern set the first two bytes and the last two bytes of
#tscmod_st::per_lane_control to the lower and upper 10 pattern bits respectively.

Example: Upper=0x3ff and Lower=0x3aa, per_lane_control = 0x03ff03aa.

\li Set #tscmod_st::per_lane_control & 0x0f =0b001 for enabling the pattern.
                                            =0b010 for enabling the checker.(TBD)
                                     & 0xf0 (>>4) 0:12 bit, 1:40bit, 6:256bit
                                     -- or --
                                     & 0x0f =0b100 for reading back regs
                                     & 0xf0 (>>4) for 0-7 pair 32-bit selection.
\li Set #tscmod_st::per_lane_control & 0x0f = 0 for writting/programming the
                                                pattern registers.
        #tscmod_st::per_lane_control & 0xf0 (>>4) pattern registers 0/1, 2/3,
                                                  ..., 14/15.
        #tscmod_st::accData          for the data pattern.

This function modifies the following registers:

\li PATGEN_256_0_REG0   0x9050
\li PATGEN_256_0_REG1   0x9051
...
\li PATGEN_256_0_REG14  0x905e
\li PATGEN_256_0_REGF   0x9060
\li PATGEN0_PATGENCTRL  0xc030 (for control)

<B>How to call:</B><br>
<p>Call this function directly or through the function
#tscmod_tier1_selector() with the upper-case string "PROG_DATA".</p>
 */
int tscmod_prog_data(tscmod_st *pc)
{
   int count, mode, width_80b;  uint16 data, mask ;  uint32 dL ;
   width_80b = 1 ;
   if(pc->per_lane_control & 0xf) {
      if(pc->per_lane_control & 0x1) {  /* enable pattern generation */
         mode = (pc->per_lane_control>>4) & 0x7 ;
         data = mode << PATGEN0_PATGENCTRL_NUMBER_BIT_SHIFT |
                width_80b << PATGEN0_PATGENCTRL_TESTGEN_BUS_WIDTH_TX_SHIFT |
                width_80b << PATGEN0_PATGENCTRL_TESTGEN_BUS_WIDTH_RX_SHIFT |
                1 << PATGEN0_PATGENCTRL_PATGEN_EN_SHIFT ; /* enabled */
         mask = PATGEN0_PATGENCTRL_NUMBER_BIT_SHIFT |
                PATGEN0_PATGENCTRL_TESTGEN_BUS_WIDTH_TX_SHIFT |
                PATGEN0_PATGENCTRL_TESTGEN_BUS_WIDTH_RX_SHIFT |
                PATGEN0_PATGENCTRL_PATGEN_EN_SHIFT ;
         SOC_IF_ERROR_RETURN
            (MODIFY_WC40_PATGEN0_PATGENCTRLr(pc->unit, pc, data, mask));
      }
      if(pc->per_lane_control & 0x4) { /* read back regs */
         count = (pc->per_lane_control>>4) & 0xf ;
         pc->accData = 0 ;
         switch(count) {
         case 0:
            SOC_IF_ERROR_RETURN
               (READ_WC40_PATGEN_256_0_REG0r(pc->unit, pc, &data));
            pc->accData = data ;
            SOC_IF_ERROR_RETURN
               (READ_WC40_PATGEN_256_0_REG1r(pc->unit, pc, &data));
            dL = data ; dL = dL <<16 ;
            pc->accData |= dL ;
            break ;
         case 1:
            SOC_IF_ERROR_RETURN
               (READ_WC40_PATGEN_256_0_REG2r(pc->unit, pc, &data));
            pc->accData = data ;
            SOC_IF_ERROR_RETURN
               (READ_WC40_PATGEN_256_0_REG3r(pc->unit, pc, &data));
            dL = data ; dL = dL <<16 ;
            pc->accData |= dL ;
            break ;
         case 2:
            SOC_IF_ERROR_RETURN
               (READ_WC40_PATGEN_256_0_REG4r(pc->unit, pc, &data));
            pc->accData = data ;
            SOC_IF_ERROR_RETURN
               (READ_WC40_PATGEN_256_0_REG5r(pc->unit, pc, &data));
            dL = data ; dL = dL <<16 ;
            pc->accData |= dL ;
            break ;
         case 3:
            SOC_IF_ERROR_RETURN
               (READ_WC40_PATGEN_256_0_REG6r(pc->unit, pc, &data));
            pc->accData = data ;
            SOC_IF_ERROR_RETURN
               (READ_WC40_PATGEN_256_0_REG7r(pc->unit, pc, &data));
            dL = data ; dL = dL <<16 ;
            pc->accData |= dL ;
            break;
         case 4:
            SOC_IF_ERROR_RETURN
               (READ_WC40_PATGEN_256_0_REG8r(pc->unit, pc, &data));
            pc->accData = data ;
            SOC_IF_ERROR_RETURN
               (READ_WC40_PATGEN_256_0_REG9r(pc->unit, pc, &data));
            dL = data ; dL = dL <<16 ;
            pc->accData |= dL ;
            break ;
         case 5:
            SOC_IF_ERROR_RETURN
               (READ_WC40_PATGEN_256_0_REG10r(pc->unit, pc, &data));
            pc->accData = data ;
            SOC_IF_ERROR_RETURN
               (READ_WC40_PATGEN_256_0_REG11r(pc->unit, pc, &data));
            dL = data ; dL = dL <<16 ;
            pc->accData |= dL ;
            break ;
         case 6:
            SOC_IF_ERROR_RETURN
               (READ_WC40_PATGEN_256_0_REG12r(pc->unit, pc, &data));
            pc->accData = data ;
            SOC_IF_ERROR_RETURN
               (READ_WC40_PATGEN_256_0_REG13r(pc->unit, pc, &data));
            dL = data ; dL = dL <<16 ;
            pc->accData |= dL ;
            break;
         case 7:
            SOC_IF_ERROR_RETURN
               (READ_WC40_PATGEN_256_0_REG14r(pc->unit, pc, &data));
            pc->accData = data ;
            SOC_IF_ERROR_RETURN
               (READ_WC40_PATGEN_256_1_REGFr(pc->unit, pc, &data));
            dL = data ; dL = dL <<16 ;
            pc->accData |= dL ;
            break ;
         }
      } /* end of read back */
   } else {
      count = (pc->per_lane_control>>4) & 0xf ;  /* writing the pattern */
      switch(count) {
      case 0:
         data = pc->accData ;
         SOC_IF_ERROR_RETURN
            (WRITE_WC40_PATGEN_256_0_REG0r(pc->unit, pc, data));
         data = (pc->accData>>16) ;
         SOC_IF_ERROR_RETURN
            (WRITE_WC40_PATGEN_256_0_REG1r(pc->unit, pc, data));
         break ;
      case 1:
         data = pc->accData ;
         SOC_IF_ERROR_RETURN
            (WRITE_WC40_PATGEN_256_0_REG2r(pc->unit, pc, data));
         data = (pc->accData>>16) ;
         SOC_IF_ERROR_RETURN
            (WRITE_WC40_PATGEN_256_0_REG3r(pc->unit, pc, data));
         break ;
      case 2:
         data = pc->accData ;
         SOC_IF_ERROR_RETURN
            (WRITE_WC40_PATGEN_256_0_REG4r(pc->unit, pc, data));
         data = (pc->accData>>16) ;
         SOC_IF_ERROR_RETURN
            (WRITE_WC40_PATGEN_256_0_REG5r(pc->unit, pc, data));
         break ;
      case 3:
         data = pc->accData ;
         SOC_IF_ERROR_RETURN
            (WRITE_WC40_PATGEN_256_0_REG6r(pc->unit, pc, data));
         data = (pc->accData>>16) ;
         SOC_IF_ERROR_RETURN
            (WRITE_WC40_PATGEN_256_0_REG7r(pc->unit, pc, data));
         break;
      case 4:
         data = pc->accData ;
         SOC_IF_ERROR_RETURN
            (WRITE_WC40_PATGEN_256_0_REG8r(pc->unit, pc, data));
         data = (pc->accData>>16) ;
         SOC_IF_ERROR_RETURN
            (WRITE_WC40_PATGEN_256_0_REG9r(pc->unit, pc, data));
         break ;
      case 5:
         data = pc->accData ;
         SOC_IF_ERROR_RETURN
            (WRITE_WC40_PATGEN_256_0_REG10r(pc->unit, pc, data));
         data = (pc->accData>>16) ;
         SOC_IF_ERROR_RETURN
            (WRITE_WC40_PATGEN_256_0_REG11r(pc->unit, pc, data));
         break ;
      case 6:
         data = pc->accData ;
         SOC_IF_ERROR_RETURN
            (WRITE_WC40_PATGEN_256_0_REG12r(pc->unit, pc, data));
         data = (pc->accData>>16) ;
         SOC_IF_ERROR_RETURN
            (WRITE_WC40_PATGEN_256_0_REG13r(pc->unit, pc, data));
         break;
      case 7:
         data = pc->accData ;
         SOC_IF_ERROR_RETURN
            (WRITE_WC40_PATGEN_256_0_REG14r(pc->unit, pc, data));
         data = (pc->accData>>16) ;
         SOC_IF_ERROR_RETURN
            (WRITE_WC40_PATGEN_256_1_REGFr(pc->unit, pc, data));
         break ;
      }
   }
   return SOC_E_NONE;
}

/*!
\brief Apply reset to complete core (not per lane) by setting lane_select=BCST.

\param  pc handle to current TSC context (#tscmod_st)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.

\details
This reset is applied at the very beginning of the TSC silicon
initialization to reset the complete core. If the TSC hasn't been
initialized before, it is advisable to start with core reset first.
This sets all the MDIO registers to expected initial values.

 #tscmod_st::per_lane_control[0]  (TSCMOD_COLD_RESET_BASIC) =0, it is a specific timer overwrite,
  specified by #tscmod_st::per_lane_control[7:1]
 #tscmod_st::per_lane_control[31:8]  contains the new timer value.

 when
 #tscmod_st::per_lane_control[0]  (TSCMOD_COLD_RESET_BASIC) =1, it is to set default values.
 but use tsc->an_ctl to choose different defaults for various registers.

This function shall not call in DV simulation.

<B>How to call:</B><br>
<p>Call this function directly or through the function #tscmod_tier1_selector()
with the upper-case string "CORE_RESET".</p>

*/
int tscmod_core_reset(tscmod_st* pc)
{
   uint16 data, mask, ram_base ;
   data = 0 ; mask = 0 ; ram_base = 0 ;

   if((pc->per_lane_control&TSCMOD_COLD_RESET_MODE_MASK)==TSCMOD_AN_CTL_LINK_FAIL_NOCL72_WAIT_TIMER ) {
      data = (pc->per_lane_control >> TSCMOD_COLD_RESET_MODE_SHIFT) ;
      SOC_IF_ERROR_RETURN
         (WRITE_WC40_AN_X1_TIMERS_LINK_FAIL_INHIBIT_TIMER_NOT_CL72r
          (pc->unit, pc, data));
      return SOC_E_NONE ;
   }

   if((pc->per_lane_control&TSCMOD_COLD_RESET_MODE_MASK)==TSCMOD_PLL_VCO_STEP) {
      data = (pc->per_lane_control >> TSCMOD_COLD_RESET_MODE_SHIFT) ;
      data = data << ANAPLL_TIMER1_VCOSTEPTIME_SHIFT ;
      mask = ANAPLL_TIMER1_VCOSTEPTIME_MASK ;
      SOC_IF_ERROR_RETURN
         (MODIFY_WC40_ANAPLL_TIMER1r(pc->unit, pc, data, mask));
      return SOC_E_NONE ;
   }

   if((pc->per_lane_control&TSCMOD_COLD_RESET_MODE_MASK)==TSCMOD_RAM_BASE_ADDR) {
      /* set micro_controller ram_base address for AN 0x9248 */
      data = (pc->per_lane_control >> TSCMOD_COLD_RESET_MODE_SHIFT) ;   /* 0x0fbe; TD2 A1; for empty credit */
      ram_base                      = data ;
      data = ram_base << AN_X1_CONTROL_CONFIG_CONTROL_RAM_BASE_SHIFT;
      mask = AN_X1_CONTROL_CONFIG_CONTROL_RAM_BASE_MASK;

      SOC_IF_ERROR_RETURN ( MODIFY_WC40_AN_X1_CONTROL_CONFIG_CONTROLr(pc->unit, pc, data, mask));
      return SOC_E_NONE ;
   }

   /* basic setting */
   if((pc->per_lane_control & TSCMOD_COLD_RESET_BASIC)==0) {
      return SOC_E_NONE ;
   }
   /* enable PLL
   data = 0 << MAIN0_RESET_RESET_PLL_SHIFT ;
   mask = MAIN0_RESET_RESET_PLL_MASK ;
   SOC_IF_ERROR_RETURN
      (MODIFY_WC40_MAIN0_RESETr(pc->unit, pc, data, mask));
   sal_usleep(1000);
   */

   /* 0x9123 CL82_SHARED_CL82_RX_AM_TIMER */
   /* 0xa000 TX_X2_CONTROL0_MLD_SWAP_COUNTr */

   /* 0x9012 ANAPLL_TIMER1. */
   data = 1 << ANAPLL_TIMER1_VCOSTEPTIME_SHIFT ;
   mask = ANAPLL_TIMER1_VCOSTEPTIME_MASK ;
   SOC_IF_ERROR_RETURN
      (MODIFY_WC40_ANAPLL_TIMER1r(pc->unit, pc, data, mask));

   /* 0x9013 ANAPLL_TIMER2 */
   /* 0x9014 ANAPLL_TIMER3 */
   /* 0x9017 ANAPLL_FREQDETCNTR */

   /* 0x909a lfck_bypass:  move to some other place */

   /* 0x9008  not needed.
   data = 0xf << MAIN0_TICK_CONTROL_0_TICK_NUMERATOR_LOWER_SHIFT |
          0x4 << MAIN0_TICK_CONTROL_0_TICK_DENOMINATOR_SHIFT ;
   mask = MAIN0_TICK_CONTROL_0_TICK_NUMERATOR_LOWER_MASK |
      MAIN0_TICK_CONTROL_0_TICK_DENOMINATOR_MASK ;
   SOC_IF_ERROR_RETURN
      (MODIFY_WC40_MAIN0_TICK_CONTROL_0r(pc->unit, pc, data, mask));

      0x9007
   data = 0x249 << MAIN0_TICK_CONTROL_1_TICK_NUMERATOR_UPPER_SHIFT ;
   mask = MAIN0_TICK_CONTROL_1_TICK_NUMERATOR_UPPER_MASK ;
   SOC_IF_ERROR_RETURN
      (MODIFY_WC40_MAIN0_TICK_CONTROL_1r(pc->unit, pc, data, mask));
   */

   /* 0x9253 */
   data = 0x10ed << AN_X1_TIMERS_CL73_BREAK_LINK_TX_DISABLE_TIMER_PERIOD_SHIFT;
   mask = AN_X1_TIMERS_CL73_BREAK_LINK_TX_DISABLE_TIMER_PERIOD_MASK ;
   SOC_IF_ERROR_RETURN
      (MODIFY_WC40_AN_X1_TIMERS_CL73_BREAK_LINKr(pc->unit, pc, data, mask));

   /* 0x9250 
   data = 0x29a ;  10 ms */
   data = 0x350 ; /* SDK-51447 12.5 ms */
   /* AN_X1_TIMERS_CL37_RESTART_CL37_RESTART_TIMER_PERIOD_MASK */
   SOC_IF_ERROR_RETURN
      (WRITE_WC40_AN_X1_TIMERS_CL37_RESTARTr(pc->unit, pc, data)) ;

   /* 0X9251 
   data  = 0x29a ;  10ms */
   data = 0x350 ; /* SDK-51447 12.5 ms */
   /* AN_X1_TIMERS_CL37_ACK_CL37_ACK_TIMER_PERIOD_MASK */
   SOC_IF_ERROR_RETURN
      (WRITE_WC40_AN_X1_TIMERS_CL37_ACKr(pc->unit, pc, data)) ;


   /* 0x9252 */
   /* Fix for SDK-51442 - leave the cl37_err_timer to dafult value */
#if 0
   data = 0x55d ;
   SOC_IF_ERROR_RETURN
     (WRITE_WC40_AN_X1_TIMERS_CL37_ERRORr(pc->unit, pc, data)) ;
#endif
   /* 0x925e AN_X1_TIMERS_SGMII_CL73_TIMER_TYPE */

   /* 0x9254 */
   if (pc->an_ctl & TSCMOD_AN_CTL_CL73_ERROR_TIMER) {
      data = 0x55d ;  /* default */
   } else {
      data = 0x1a10 ;
   }
   SOC_IF_ERROR_RETURN
      (WRITE_WC40_AN_X1_TIMERS_CL73_ERRORr(pc->unit, pc, data)) ;

   /* 0x9255 */
   /*data = 0x7d0 ;*/
   data = 0x14d4 ;  /* CRTSC-719 (ignore_link_timer+pd_sd_timer)*2 */
   SOC_IF_ERROR_RETURN
      (WRITE_WC40_AN_X1_TIMERS_CL73_DME_LOCKr(pc->unit, pc, data)) ;

   /* 0x925c */
   data = 0x7d0 ;
   SOC_IF_ERROR_RETURN
      (WRITE_WC40_AN_X1_TIMERS_IGNORE_LINK_TIMERr(pc->unit, pc, data));

   /*0x9257 */
   data = 0x8236 ;  /* 500 msec */

   SOC_IF_ERROR_RETURN
      (WRITE_WC40_AN_X1_TIMERS_LINK_FAIL_INHIBIT_TIMER_CL72r
       (pc->unit, pc, data));

   /* 0x9258 */
   if (pc->an_ctl & TSCMOD_AN_CTL_LINK_FAIL_NOCL72_WAIT_TIMER) {
      data = 0xbb8 ;   /* 45ms : default */
   } else {
      data = 0x1a10 ;  /*  100ms */
   }

   SOC_IF_ERROR_RETURN
      (WRITE_WC40_AN_X1_TIMERS_LINK_FAIL_INHIBIT_TIMER_NOT_CL72r
       (pc->unit, pc, data));

   /* 0x9259 */
   data = 0x29a ;  /* parellel serial detect */
   SOC_IF_ERROR_RETURN
      (WRITE_WC40_AN_X1_TIMERS_PD_SD_TIMERr
       (pc->unit, pc, data));

   /* 0x9256 
   data = 0x8236 ;
   data = 0x029a ;     PHY-863 10 ms */
   data = 0x0350 ;  /* SDK-51447 12.5 ms */
   SOC_IF_ERROR_RETURN
      (WRITE_WC40_AN_X1_TIMERS_LINK_UPr(pc->unit, pc, data));

   /* 0x925d  use default */
   /* WRITE_WC40_AN_X1_TIMERS_DME_PAGE_TIMERr */

   /* 0x925a */
   data = 0x8236 ;
   SOC_IF_ERROR_RETURN
      (WRITE_WC40_AN_X1_TIMERS_CL72_MAX_WAIT_TIMERr(pc->unit, pc, data));

   /* 0x925b */
   /*data = 0xc8 ;*/
   data = 0x1fe ;
   SOC_IF_ERROR_RETURN
      (WRITE_WC40_AN_X1_TIMERS_CL72_WAIT_TIMERr(pc->unit, pc, data));

   /* 0x9222  no need ?
   data = 0x11 << RX_X1_CONTROL0_DESKEW_WINDOWS_CL82_DSWIN_SHIFT ;
   mask = RX_X1_CONTROL0_DESKEW_WINDOWS_CL82_DSWIN_MASK ;
   MODIFY_WC40_RX_X1_CONTROL0_DESKEW_WINDOWSr
   */

   /* 0xc210  no need default=0x29a6.
      data = 0xa << DSC2B0_ACQ_SM_CTRL0_ACQ2_TIMEOUT_SHIFT |
             0xd << DSC2B0_ACQ_SM_CTRL0_ACQ1_TIMEOUT_SHIFT |
             0x6 << DSC2B0_ACQ_SM_CTRL0_ACQCDR_TIMEOUT_SHIFT ;
   MODIFY_WC40_DSC2B0_ACQ_SM_CTRL0r
   */

   /* 0xc211 no need default =0x3800
      data=0x7<<DSC2B0_ACQ_SM_CTRL1_ACQVGA_TIMEOUT_SHIFT
   MODIFY_WC40_DSC2B0_ACQ_SM_CTRL1r
   */


   /* 0x924a no definition ? */

   /* 0x9249 AN_X1_CONTROL_PLL_RESET_TIMER */

   /* invert prbs data for backward to TD+: JIRA PHY-804 */
   data = PATGEN0_PRBSCTRL_PRBS_INV_TX_MASK | PATGEN0_PRBSCTRL_PRBS_INV_RX_MASK ;
   mask = PATGEN0_PRBSCTRL_PRBS_INV_TX_MASK | PATGEN0_PRBSCTRL_PRBS_INV_RX_MASK ;
   SOC_IF_ERROR_RETURN (MODIFY_WC40_PATGEN0_PRBSCTRLr(pc->unit, pc, data, mask));

   return SOC_E_NONE ;
}

/*!
\brief load 8051 microcode.  (one microcontroller per TSC).

\param  pc handle to current TSC context (a.k.a tscmod struct)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.

\details
#tscmod_st::per_lane_control & 0x0f = (TSCMOD_UC_INIT)1 for initializarion.
#tscmod_st::per_lane_control & 0x0f = (TSCMOD_UC_OFFSET)2 for loading offset.
                             & 0x0ffff0 >>4 is the offset value ;
#tscmod_st::per_lane_control & 0x0f = (TSCMOD_UC_SIZE)3 for specifying size.
                             & 0X0FFFF0 >> 4 is the size.
#tscmod_st::per_lane_control & 0x0f = (TSCMOD_UC_W_OP)4 for starting write operation
#tscmod_st::per_lane_control & 0x0f = (TSCMOD_UC_W_DATA)5 for data written
#tscmod_st::per_lane_control & 0x0f = (TSCMOD_UC_STOP)6 for stop operation
#tscmod_st::per_lane_control & 0x0f = (TSCMOD_UC_LOAD_STATUS)7 for reading back loading status.
            #tscmod_st::accData = 1 (pass) or 0(failed).
#tscmod_st::per_lane_control & 0x0f = (TSCMOD_UC_NO_CKSUM)8 for signaling uC not to perform checksum.
#tscmod_st::per_lane_control & 0x0f = (TSCMOD_UC_RELEASE)9 for releasing uC reset.
#tscmod_st::per_lane_control & 0x0f = (TSCMOD_UC_R_CKSUM)10 for reading back the checksum.
            #tscmod_st::accData is the checksum.
#tscmod_st::per_lane_control & 0x0f = (TSCMOD_UC_R_VER)11 for reading back the version
#tscmod_st::per_lane_control & 0x0f = (TSCMOD_UC_EXT_ON)12 for allowing ext access to uC.
#tscmod_st::per_lane_control & 0x0f = (TSCMOD_UC_EXT_OFF)13 for not allowaing ext access to uC.
#tscmod_st::per_lane_control & 0x0f = (TSCMOD_UC_R_OP)14 for starting read operation
#tscmod_st::per_lane_control & 0x0f = (TSCMOD_UC_R_DATA)15 for read back data.

When the function is called, the uCode will be loaded.

<B>This function is for loading uCodes through TSC MDIO which is not
the most efficient ways of loading firmware though.</B>

<B>How to call:</B><br>
<p>Call this function directly or through the function
#tscmod_tier1_selector() with the upper-case string "FIRMWARE_SET".</p>
*/
int tscmod_firmware_set (tscmod_st* pc)
{
   int    rv, cmd, offset, len;
   uint16 TSCMOD_UC_DOWNLOAD_STATUS_addr = 0xffc5 ;
   uint16 TSCMOD_UC_INFO_B1_CRCr_addr          = 0xf015 ;
   uint16 data, mask ;  uint32 mask_val ;  int shift ;

   rv  = SOC_E_NONE ;
   mask  = 0xff ;  mask_val = 0xffff00 ;
   shift = TSCMOD_UC_MACRO_SHIFT  ;
   cmd = pc->per_lane_control & mask ;
   switch(cmd) {
   case TSCMOD_UC_INIT:
      /* initialize the RAM @0xffc2 */
      SOC_IF_ERROR_RETURN (WRITE_WC40_UC_COMMANDr(pc->unit, pc, UC_COMMAND_INIT_CMD_MASK));

      /* wait for init done @0xffc5 */
      rv = tscmod_regbit_set_wait_check(pc, TSCMOD_UC_DOWNLOAD_STATUS_addr,
                                        UC_DOWNLOAD_STATUS_INIT_DONE_MASK,1,200000000);
      if (rv == SOC_E_TIMEOUT) {
         printf("TSCMOD : uC init fails due to init TO: u=%d p=%d\n",
                pc->unit, pc->port);
         return rv ;
      }
      /* enable uC timers @0xffca */
      SOC_IF_ERROR_RETURN
         (MODIFY_WC40_UC_COMMAND2r
          (pc->unit, pc, UC_COMMAND2_TMR_EN_MASK, UC_COMMAND2_TMR_EN_MASK));
      break ;
   case TSCMOD_UC_OFFSET:
      offset = ( pc->per_lane_control & mask_val ) >> shift  ;
      /* starting RAM location @0xffc1 */
      SOC_IF_ERROR_RETURN
         (WRITE_WC40_UC_ADDRESSr(pc->unit, pc, offset));
      break ;
   case TSCMOD_UC_SIZE:
      len =  ( pc->per_lane_control & mask_val ) >> shift ;
      /* 0xffc0 */
      SOC_IF_ERROR_RETURN
         (WRITE_WC40_UC_RAMWORDr(pc->unit, pc, len));
      break ;
   case TSCMOD_UC_W_OP:
      /* 0xffc2 */
      SOC_IF_ERROR_RETURN
         (WRITE_WC40_UC_COMMANDr
          (pc->unit, pc, UC_COMMAND_WRITE_MASK|UC_COMMAND_RUN_MASK));
      break ;
   case TSCMOD_UC_W_DATA:
      data = ( pc->per_lane_control & mask_val ) >> shift ;
      /* 0xffc3 */
      SOC_IF_ERROR_RETURN
         (WRITE_WC40_UC_WRDATAr(pc->unit, pc, data));
      break ;
   case TSCMOD_UC_STOP:
      SOC_IF_ERROR_RETURN
         (WRITE_WC40_UC_COMMANDr
          (pc->unit, pc,UC_COMMAND_STOP_MASK)) ;
      break ;
   case TSCMOD_UC_LOAD_STATUS:
      /* 0xffc5 */
      SOC_IF_ERROR_RETURN
         (READ_WC40_UC_DOWNLOAD_STATUSr(pc->unit, pc, &data)) ;
      pc->accData = data ;
      break ;
   case TSCMOD_UC_NO_CKSUM:
      /* 0xf015 */
      SOC_IF_ERROR_RETURN
         (WRITE_WC40_UC_INFO_B1_CRCr(pc->unit, pc, 0x1234)) ;
      break ;
   case TSCMOD_UC_RELEASE:
      /* 0xffc2 */
      SOC_IF_ERROR_RETURN
         (WRITE_WC40_UC_COMMANDr
          (pc->unit, pc,UC_COMMAND_MDIO_UC_RESET_N_MASK));
      break ;
   case TSCMOD_UC_R_CKSUM:
      /* 0xf015 */
      rv = tscmod_regbit_set_wait_check(pc, TSCMOD_UC_INFO_B1_CRCr_addr,0xffff,1,100000);
      if (rv == SOC_E_TIMEOUT) {
         printf("TSCMOD : uC download: u=%d p=%d timeout: wait for checksum\n",
                pc->unit, pc->port);
         pc->accData = 0 ;
         return rv ;
      }
      SOC_IF_ERROR_RETURN
         (READ_WC40_UC_INFO_B1_CRCr(pc->unit, pc, &data)) ;
      pc->accData = data ;
      break ;
   case TSCMOD_UC_R_VER:
      /* 0xf010 */
      SOC_IF_ERROR_RETURN
         (READ_WC40_UC_INFO_B1_VERSIONr(pc->unit, pc, &data)) ;
      pc->accData = data ;
      break ;
   case TSCMOD_UC_EXT_ON:
      SOC_IF_ERROR_RETURN
         (MODIFY_WC40_UC_COMMAND3r(pc->unit,
          pc, UC_COMMAND3_EXT_CLK_ENABLE_MASK, UC_COMMAND3_EXT_CLK_ENABLE_MASK));
      break ;
   case TSCMOD_UC_EXT_OFF:
      SOC_IF_ERROR_RETURN
         (MODIFY_WC40_UC_COMMAND3r(pc->unit,
          pc, 0, UC_COMMAND3_EXT_CLK_ENABLE_MASK));
      break ;
   case TSCMOD_UC_R_OP:
      /* ram only 0xffc7 */
      SOC_IF_ERROR_RETURN
         (MODIFY_WC40_UC_COMMAND4r
          (pc->unit, pc, UC_COMMAND4_DATA_RAM_R_W_MASK|UC_COMMAND4_DATA_RAM_ACCESS_MASK,
           UC_COMMAND4_DATA_RAM_R_W_MASK|UC_COMMAND4_DATA_RAM_ACCESS_MASK));
      /* 0xffc2 */
      /*
      SOC_IF_ERROR_RETURN
         (WRITE_WC40_UC_COMMANDr
          (pc->unit, pc, UC_COMMAND_READ_MASK));
      */
      break ;
   case TSCMOD_UC_R_DATA:
      /* ffc4 */
      SOC_IF_ERROR_RETURN
         (READ_WC40_UC_RDDATAr(pc->unit, pc, &data)) ;
      pc->accData = data ;
      break ;
   case TSCMOD_UC_W_RAM_DATA:
      /* 0xffc3 */
      data = ( pc->per_lane_control & mask_val ) >> shift ;
      SOC_IF_ERROR_RETURN
         (WRITE_WC40_UC_WRDATAr(pc->unit, pc, data));
      break ;
   case TSCMOD_UC_W_RAM_OP:
      /* 0xffc7 */
      data = 0 ; /* write */
      SOC_IF_ERROR_RETURN
         (MODIFY_WC40_UC_COMMAND4r
          (pc->unit, pc, data|UC_COMMAND4_DATA_RAM_ACCESS_MASK,
           UC_COMMAND4_DATA_RAM_R_W_MASK|UC_COMMAND4_DATA_RAM_ACCESS_MASK));
      break ;
   default:
      break ;
   }
   return rv;
}

/*!
\brief  Set the firmware mode.

\param  pc handle to current TSC context (#tscmod_st)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.

\details
    If per_lane_control =0x1; set to the desired firmwaremode (accData)
    If per_lane_control =0x0; get the firmware mode to accData
*/

int tscmod_fwmode_control(tscmod_st* pc)                  /* FWMODE_CONTROL */
{
   uint16 fw_mode;
   if(pc->per_lane_control &0x1) {
      fw_mode = pc->accData ;
      SOC_IF_ERROR_RETURN
         (WRITE_WC40_UCSS_X4_FIRMWARE_MODEr(pc->unit, pc, fw_mode));
   } else {
      SOC_IF_ERROR_RETURN
         (READ_WC40_UCSS_X4_FIRMWARE_MODEr(pc->unit, pc, &fw_mode));
      pc->accData = fw_mode ;
   }
   return SOC_E_NONE ;
}

/* check for tscmod_reg_rx_sig_det */
int tscmod_uc_rx_sig_det(tscmod_st *pc, int core_wise, int forced_det_0, int *restore_val, int access_mode,
                                uint32 a, uint16 d, uint16 m) 
{
   int rv, tmp_sel, tmp_lane, tmp_dxgxs, val, lane_s, lane_e, i ;  uint16 data, mask ;
   rv       = SOC_E_NONE ;
   tmp_sel  = pc->lane_select ;
   tmp_lane = pc->this_lane ;
   tmp_dxgxs= pc->dxgxs ;
   val      = 0 ;
   lane_s   = 0 ;
   lane_e   = 0 ;
   i        = 0 ;

   if(core_wise) {
      if(forced_det_0) {   /* create restore value under core_wise */
         for(i=0; i<4; i++) {
            pc->lane_select = getLaneSelect(i) ;
            pc->this_lane   = i ;
            pc->dxgxs       = 0 ;
            SOC_IF_ERROR_RETURN(TSC_REG_READ(pc->unit, pc, 0x0, 0xc138, &data));
            if(data & RX_X4_CONTROL0_RX_SIGDET_OVERRIDE_SIGNAL_OK_MASK) {
               val |= 1 << i ;
            }
         }
         *restore_val = val ;
      } else {             /* restore the original value under core_wise */
         for(i=0; i<4; i++) {
            pc->lane_select = getLaneSelect(i) ;
            pc->this_lane   = i ;
            val = *restore_val ;
            if(val & (1<<i)) {
               data = RX_X4_CONTROL0_RX_SIGDET_OVERRIDE_SIGNAL_OK_MASK ;
            } else {
               data = 0 ;
            }
            mask = RX_X4_CONTROL0_RX_SIGDET_OVERRIDE_SIGNAL_OK_MASK |
                   RX_X4_CONTROL0_RX_SIGDET_OVERRIDE_VALUE_SIGNAL_OK_MASK ;
            /* 0xc138 */
            SOC_IF_ERROR_RETURN(TSC_REG_MODIFY(pc->unit, pc, 0x0, 0xc138, data, mask)) ;
         }
      }
   }

   if(core_wise && forced_det_0) {
      pc->lane_select = TSCMOD_LANE_BCST ;
      pc->this_lane   = 0 ;
      lane_e          = 4 ;
   } else {
      switch(pc->port_type) {
      case TSCMOD_SINGLE_PORT:
         pc->lane_select = TSCMOD_LANE_BCST ;
         pc->this_lane   = 0 ;
         lane_e          = 4 ;
         break ;  
      case TSCMOD_MULTI_PORT:
         pc->lane_select = getLaneSelect(tmp_lane) ;
         lane_s          = tmp_lane ;
         lane_e          = tmp_lane+1 ;
         break ; 
      case TSCMOD_DXGXS:
         if(tmp_lane>=2) {  /* dual BCST */
            pc->dxgxs = 0x2 ;
            lane_s    = 2 ;
            lane_e    = 4 ;
         } else {
            pc->dxgxs = 0x1 ;
            lane_s    = 0 ;
            lane_e    = 2 ;
         }
         break ;
      default:
         break ;
      }

   }
   if(core_wise==0) {  /* port wise */
      if(forced_det_0) {
         /* create restore value */
         val = 0 ;
         for(i=0; i<4; i++) {
            if((i>=lane_s) &&(i<lane_e)) {
               pc->lane_select = getLaneSelect(i) ;
               pc->this_lane   = i ;
               pc->dxgxs       = 0 ;
               SOC_IF_ERROR_RETURN(TSC_REG_READ(pc->unit, pc, 0x0, 0xc138, &data)) ;
               if(data & RX_X4_CONTROL0_RX_SIGDET_OVERRIDE_SIGNAL_OK_MASK) {
                  val |= 1 << i ;
               }
            }
         }
         *restore_val = val ;

         switch(pc->port_type) {
         case TSCMOD_SINGLE_PORT:
            pc->lane_select = TSCMOD_LANE_BCST ;
            break ;
         case TSCMOD_DXGXS:
            if(tmp_lane>=2) {  /* dual BCST */
               pc->dxgxs = 0x2 ;
            } else {
               pc->dxgxs = 0x1 ;
            }
            break ;
         default:
            break ;
         }
         /* set sig det */
         data = RX_X4_CONTROL0_RX_SIGDET_OVERRIDE_SIGNAL_OK_MASK ;
         mask = RX_X4_CONTROL0_RX_SIGDET_OVERRIDE_SIGNAL_OK_MASK |
            RX_X4_CONTROL0_RX_SIGDET_OVERRIDE_VALUE_SIGNAL_OK_MASK ;
         /* 0xc138 */
         SOC_IF_ERROR_RETURN(TSC_REG_MODIFY(pc->unit, pc, 0x0, 0xc138, data, mask)) ;
      } else {
         /* port wise restore */
         for(i=0; i<4; i++) {
            if((i>=lane_s) &&(i<lane_e)) {
               pc->lane_select = getLaneSelect(i) ;
               pc->this_lane   = i ;
               val = *restore_val ;
               if (val & (1<<i)) {
                   data = RX_X4_CONTROL0_RX_SIGDET_OVERRIDE_SIGNAL_OK_MASK ;
               } else {
                   data = 0 ;
               }
               mask = RX_X4_CONTROL0_RX_SIGDET_OVERRIDE_SIGNAL_OK_MASK |
                  RX_X4_CONTROL0_RX_SIGDET_OVERRIDE_VALUE_SIGNAL_OK_MASK ;
               /* 0xc138 */
               SOC_IF_ERROR_RETURN(TSC_REG_MODIFY(pc->unit, pc, 0x0, 0xc138, data, mask)) ;
            }
         }
      }
   } else {
      /* core_wise */
      if(forced_det_0) {  /* set sig det */
         data = RX_X4_CONTROL0_RX_SIGDET_OVERRIDE_SIGNAL_OK_MASK ;
         mask = RX_X4_CONTROL0_RX_SIGDET_OVERRIDE_SIGNAL_OK_MASK |
            RX_X4_CONTROL0_RX_SIGDET_OVERRIDE_VALUE_SIGNAL_OK_MASK ;
         /* 0xc138 */
         SOC_IF_ERROR_RETURN(TSC_REG_MODIFY(pc->unit, pc, 0x0, 0xc138, data, mask)) ;
      }
   }

   pc->lane_select = tmp_sel ;
   pc->this_lane   = tmp_lane ;
   pc->dxgxs       = tmp_dxgxs ;

   if(forced_det_0==0) return rv ;

   if(core_wise) {
      if((pc->msg_code & TSCMOD_MSG_UC_SYNC_CMD_CORE_RECOVERY_MASK)==TSCMOD_MSG_UC_SYNC_CMD_CORE_RECOVERY_MASK) {

      } else {
         val = (pc->msg_code & TSCMOD_MSG_UC_SYNC_CMD_CORE_RECOVERY_MASK)>>TSCMOD_MSG_UC_SYNC_CMD_CORE_RECOVERY_SHIFT ;
         val = (((val<<1) | 1) <<TSCMOD_MSG_UC_SYNC_CMD_CORE_RECOVERY_SHIFT)&TSCMOD_MSG_UC_SYNC_CMD_CORE_RECOVERY_MASK  ;
         pc->msg_code |= val ;
         if(pc->verbosity&TSCMOD_DBG_PRINT) {
            printf("Warning: uC proxy core recovry attempt0 u=%0d p=%0d addr=%x access=%0d d=%x m=%x sel=0x%x l=%0d restore=%x\n",
                   pc->unit, pc->port, a, access_mode, d, m, pc->lane_select, pc->this_lane, *restore_val) ;
         }
      }
   } else {
      if((pc->msg_code & TSCMOD_MSG_UC_SYNC_CMD_PORT_RECOVERY_MASK)==TSCMOD_MSG_UC_SYNC_CMD_PORT_RECOVERY_MASK) {

      } else {
         val = (pc->msg_code & TSCMOD_MSG_UC_SYNC_CMD_PORT_RECOVERY_MASK)>>TSCMOD_MSG_UC_SYNC_CMD_PORT_RECOVERY_SHIFT ;
         val = (((val<<1) | 1) <<TSCMOD_MSG_UC_SYNC_CMD_PORT_RECOVERY_SHIFT)&TSCMOD_MSG_UC_SYNC_CMD_PORT_RECOVERY_MASK ;
         pc->msg_code |= val ;
         if(pc->verbosity&TSCMOD_DBG_PRINT) {
            printf("Warning: uC proxy port recovry attempt1 u=%0d p=%0d addr=%x access=%0d d=%x m=%x sel=0x%x l=%0d restore=%0x\n",
                   pc->unit, pc->port, a, access_mode, d, m, pc->lane_select, pc->this_lane, *restore_val) ;
         }
      }
   }
   return rv ;
}


/*!
\brief To use uC to access registers or uC data ram.

\param  pc handle to current TSC context (a.k.a tscmod struct)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.


\details

#tscmod_st::per_lane_control & 0x1 = 0 for register access, or 1 for ram access.
#tscmod_st::per_lane_control & 0x2 = 0 for read, or 1 for write
#tscmod_st::per_lane_control >> 4 & 0xffff = mask for write.

#tscmod_st::accAddr is the address.
#tscmod_st::accData is the data.

#define TSCMOD_UC_SYNC_CMDr             0xffc8
#define TSCMOD_UC_SYNC_LADDr            0xffc0
#define TSCMOD_UC_SYNC_HADDr            0xffc1
#define TSCMOD_UC_SYNC_DATAr            0xffc3

<B>How to call:</B><br>
<p>Call this function directly or through the function #tscmod_tier1_selector()
with the upper-case string "UC_SYNC_CMD".</p>
*/

int tscmod_uc_sync_cmd_core(tscmod_st* pc)
{
   uint16 mask, dd, cmd, wr_data ; int wr, ram, addr, rv, loop, cnt, strike, restore_val1, restore_val2, access_mode ;
   uint32            start_time, diff_time ;

   cmd = 0 ; ram = 0 ; wr = 0 ; dd = 0 ;
   addr    = (pc->accAddr & 0xf800ffff) ;
   wr_data = pc->accData ;
   cmd = 0 ; mask = 0 ;  access_mode = 0;
   rv  = SOC_E_NONE ;

   if(pc->per_lane_control & TSCMOD_UC_SYNC_RAM_MODE) {
      ram = 1 ; cmd |= TSCMOD_UC_SYNC_RAM_MASK ; }
   if(pc->per_lane_control & TSCMOD_UC_SYNC_WR_MODE) {
      wr = 1  ; mask = (pc->per_lane_control>>TSCMOD_UC_SYNC_MASK_SHIFT) & 0xffff ; access_mode =1;
   }
   if(ram==0) addr |= (pc->this_lane<<16) ;

   if(pc->tsc_touched &&(pc->ctrl_type&TSCMOD_CTRL_TYPE_SEMA_CHK)) {
      printf("Error: uC Proxy sema_viol u=%0d p=%0d addr=%x wd=%x wr=%0d ram=%0d mask=%x paddr=%0x\n",
             pc->unit, pc->port, addr, wr_data, wr, ram, mask, pc->tsc_touched) ;
   }
   if (addr) {
       pc->tsc_touched = addr;
   } else {
       pc->tsc_touched =1;
   }

   /* clear cmd */
   SOC_IF_ERROR_RETURN
      (TSC_REG_WRITE(pc->unit, pc, 0x00, TSCMOD_UC_SYNC_CMDr, 0)) ;
   /* set addr, set data if write */

   /* lower address */
   dd = addr  ;
   SOC_IF_ERROR_RETURN(TSC_REG_WRITE(pc->unit, pc, 0x00, TSCMOD_UC_SYNC_LADDr, dd)) ;

   /* upper address */
   dd = (addr >> 16) ;
   SOC_IF_ERROR_RETURN(TSC_REG_WRITE(pc->unit, pc, 0x00, TSCMOD_UC_SYNC_HADDr, dd)) ;

   if(wr && (mask==0xffff)) { /* write 2B */
      if ((pc->lane_select == TSCMOD_LANE_BCST)||(pc->dxgxs & 0x3)) {
         printf("%-22s: u=%0d p=%0d BCST not supported\n", FUNCTION_NAME(), pc->unit, pc->port) ;
      }
      cmd |= TSCMOD_UC_SYNC_WR_MASK ;
      SOC_IF_ERROR_RETURN(TSC_REG_WRITE(pc->unit, pc, 0x00, TSCMOD_UC_SYNC_DATAr, wr_data)) ;
   } else {  /* read-modify-write, or read */
      /* cmd = cmd */
   }

   /* write the cmd */
   cmd |= TSCMOD_UC_SYNC_REQ_MASK ;
   SOC_IF_ERROR_RETURN
      (TSC_REG_WRITE(pc->unit, pc, 0x00, TSCMOD_UC_SYNC_CMDr, cmd)) ;

   /* wait for uC done for write or read */
   loop = 0 ; cnt = 0 ;
   start_time = sal_time_usecs() ;
   diff_time  = 0 ;  restore_val1 =0 ;  restore_val2 =0 ; strike =0;
   while(1) {
      SOC_IF_ERROR_RETURN(TSC_REG_READ(pc->unit, pc, 0x00, TSCMOD_UC_SYNC_CMDr, &dd)) ;
      if( dd & TSCMOD_UC_SYNC_DONE_MASK ) break ;
      if(loop++ >= TSCMOD_UC_PROXY_LOOP_CNT) {
         loop = 0 ;  cnt ++ ;
         if (pc->verbosity & TSCMOD_DBG_REG) {
            diff_time = sal_time_usecs() - start_time ;
            printf("%-22s: u=%0d p=%0d addr=%x 1st wait done wr=%0d (%0d usc) pending\n",
                   FUNCTION_NAME(), pc->unit, pc->port, addr, wr, diff_time) ;
         }
      }
      if(cnt >TSCMOD_UC_PROXY_CNT_CAP) {
         diff_time = sal_time_usecs() - start_time  ;
         if(diff_time <= TSCMOD_UC_PROXY_WAIT_TIME) {
            cnt-- ;
         } else if(diff_time <= TSCMOD_UC_PROXY_WAIT_TIME_STRIKE1) {
            cnt-- ;
            if(strike==0) {
               strike = 1 ;
               /* port-wise recovery */
               rv |= tscmod_uc_rx_sig_det(pc, 0, 1, &restore_val1, access_mode, addr, wr_data, mask) ;
            }
         } else if(diff_time <= TSCMOD_UC_PROXY_WAIT_TIME_STRIKE2) {
            cnt-- ;
            if(strike==0) {
               strike = 1 ;
               /* port-wise recovery */
               rv |= tscmod_uc_rx_sig_det(pc, 0, 1, &restore_val1, access_mode, addr, wr_data, mask) ;
            } else if(strike==1) {
               strike = 2 ;
               /* core-wise recovery */
               rv |= tscmod_uc_rx_sig_det(pc, 1, 1, &restore_val2, access_mode, addr, wr_data, mask) ;
            }
         } else {
            if(pc->verbosity&TSCMOD_DBG_PRINT){
               printf("Error: uC proxy access aborted t1 u=%0d p=%0d addr=%x wr=%0d ram=%0d d=%x m=%x\
 sel=0x%x l=%0d usec=%0d lp=%0d cnt=%0d\n",
                      pc->unit, pc->port, addr, wr, ram, wr_data, mask, pc->lane_select, pc->this_lane, diff_time, loop, cnt) ;
            } else if(!(pc->err_code&TSCMOD_ERR_UC_SYNC_CMD_TO)) {
               printf("Error: uC proxy access aborted t1 u=%0d p=%0d addr=%x wr=%0d ram=%0d d=%x m=%x usec=%0d\n",
                      pc->unit, pc->port, addr, wr, ram, wr_data, mask, diff_time) ;
            }
            pc->err_code  |= TSCMOD_ERR_UC_SYNC_CMD_TO ;
            pc->ctrl_type |= TSCMOD_CTRL_TYPE_ERR ;
            pc->ctrl_type |=  TSCMOD_CTRL_TYPE_UC_STALL ;
            if(strike) {
               if(strike==2) { rv |= tscmod_uc_rx_sig_det(pc, 1, 0, &restore_val2, access_mode, addr, wr_data, mask) ; }
               rv |= tscmod_uc_rx_sig_det(pc, 0, 0, &restore_val1, access_mode, addr, wr_data, mask) ; 
            }
            pc->accAddr = addr ;
            pc->accData = wr_data ;
            if(pc->verbosity&(TSCMOD_DBG_PRINT|TSCMOD_DBG_LINK|TSCMOD_DBG_SPD|TSCMOD_DBG_AN|TSCMOD_DBG_INIT))
               tscmod_reg_uc_sync_error_debug(pc->unit, pc) ;
            pc->tsc_touched = 0 ;
            return SOC_E_NONE ;
         }
      }
   }
   if(strike) {
      if(strike==2) { rv |= tscmod_uc_rx_sig_det(pc, 1, 0, &restore_val2, access_mode, addr, wr_data, mask) ; }
      rv |= tscmod_uc_rx_sig_det(pc, 0, 0, &restore_val1, access_mode, addr, wr_data, mask) ; 
   }

   /* clear cmd */
   cmd = 0 ;
   SOC_IF_ERROR_RETURN
      (TSC_REG_WRITE(pc->unit, pc, 0x00, TSCMOD_UC_SYNC_CMDr, 0)) ;

   if( ram ) cmd = TSCMOD_UC_SYNC_RAM_MASK ;

   if(wr) {
      if(mask==0xffff) {
         pc->accAddr = addr ;
         pc->accData = wr_data ;
         pc->tsc_touched = 0 ;
         return rv ;
      }
      SOC_IF_ERROR_RETURN(TSC_REG_READ(pc->unit, pc, 0x00, TSCMOD_UC_SYNC_DATAr, &dd)) ;

      /* modify */
      wr_data = (wr_data & mask) | (dd & ~mask) ;
      SOC_IF_ERROR_RETURN(TSC_REG_WRITE(pc->unit, pc, 0x00, TSCMOD_UC_SYNC_DATAr, wr_data)) ;

      cmd |= (TSCMOD_UC_SYNC_WR_MASK | TSCMOD_UC_SYNC_REQ_MASK) ;
      SOC_IF_ERROR_RETURN
         (TSC_REG_WRITE(pc->unit, pc, 0x00, TSCMOD_UC_SYNC_CMDr, cmd)) ;
   } else {
      /* read back data */
      SOC_IF_ERROR_RETURN(TSC_REG_READ(pc->unit, pc, 0x00, TSCMOD_UC_SYNC_DATAr, &dd)) ;
      pc->accAddr = addr ;
      pc->accData = dd ;
      pc->tsc_touched = 0 ;
      return rv ;
   }
   /* wait for write Done */
   loop = 0 ; cnt = 0 ;
   start_time = sal_time_usecs() ;
   diff_time  = 0 ;  restore_val1 =0 ;  restore_val2 =0 ;  strike =0 ;
   while(1) {
      SOC_IF_ERROR_RETURN(TSC_REG_READ(pc->unit, pc, 0x00, TSCMOD_UC_SYNC_CMDr, &dd)) ;
      if( dd & TSCMOD_UC_SYNC_DONE_MASK ) break ;
      if(loop++ >= TSCMOD_UC_PROXY_LOOP_CNT) {
         loop = 0 ;  cnt ++ ;
         if (pc->verbosity & TSCMOD_DBG_REG) {
            diff_time = sal_time_usecs() - start_time ;
            printf("%-22s: u=%0d p=%0d addr=%x 2nd wait done wr=%0d (%0d usc) pending\n",
                   FUNCTION_NAME(), pc->unit, pc->port, addr, wr, diff_time) ;
         }
      }
      if(cnt >TSCMOD_UC_PROXY_CNT_CAP) {
         diff_time = sal_time_usecs() - start_time  ;
         if(diff_time <= TSCMOD_UC_PROXY_WAIT_TIME) {
            cnt-- ;
         } else if(diff_time <= TSCMOD_UC_PROXY_WAIT_TIME_STRIKE1) {
            cnt-- ;
            if(strike==0) {
               strike = 1 ;
               /* port-wise recovery */
               rv |= tscmod_uc_rx_sig_det(pc, 0, 1, &restore_val1, access_mode, addr, wr_data, mask) ;
            }
         } else if(diff_time <= TSCMOD_UC_PROXY_WAIT_TIME_STRIKE2) {
            cnt-- ;
            if(strike==0) {
               strike = 1 ;
               /* port-wise recovery */
               rv |= tscmod_uc_rx_sig_det(pc, 0, 1, &restore_val1, access_mode, addr, wr_data, mask) ;
            } else if(strike==1) {
               strike = 2 ;
               /* core-wise recovery */
               rv |= tscmod_uc_rx_sig_det(pc, 1, 1, &restore_val2, access_mode, addr, wr_data, mask) ;
            }
         } else {
            if(!(pc->err_code&TSCMOD_ERR_UC_SYNC_CMD_TO)||(pc->verbosity&TSCMOD_DBG_PRINT)) {
               printf("Error: uC proxy access aborted t2 u=%0d p=%0d addr=%x wr=%0d ram=%0d d=%x m=%x sel=0x%x l=%0d usec=%0d\n",
                      pc->unit, pc->port, addr, wr, ram, wr_data, mask, pc->lane_select, pc->this_lane, diff_time) ;
            }
            pc->err_code  |= TSCMOD_ERR_UC_SYNC_CMD_TO ;
            pc->ctrl_type |= TSCMOD_CTRL_TYPE_ERR ;
            pc->ctrl_type |=  TSCMOD_CTRL_TYPE_UC_STALL ;
            if(strike) {
               if(strike==2) { rv |= tscmod_uc_rx_sig_det(pc, 1, 0, &restore_val2, access_mode, addr, wr_data, mask) ; }
               rv |= tscmod_uc_rx_sig_det(pc, 0, 0, &restore_val1, access_mode, addr, wr_data, mask) ; 
            }
            pc->accAddr = addr ;
            pc->accData = wr_data ;
            if(pc->verbosity&(TSCMOD_DBG_PRINT|TSCMOD_DBG_LINK|TSCMOD_DBG_SPD|TSCMOD_DBG_AN|TSCMOD_DBG_INIT))
               tscmod_reg_uc_sync_error_debug(pc->unit, pc) ;
            pc->tsc_touched = 0 ;
            return SOC_E_NONE ;
         }
      }
   }
   if(strike) {
      if(strike==2) { rv |= tscmod_uc_rx_sig_det(pc, 1, 0, &restore_val2, access_mode, addr, wr_data, mask) ; }
      rv |= tscmod_uc_rx_sig_det(pc, 0, 0, &restore_val1, access_mode, addr, wr_data, mask) ; 
   }

   pc->accAddr = addr ;
   pc->accData = wr_data ;
   pc->tsc_touched = 0 ;
   return rv ;
}

int tscmod_uc_sync_cmd(tscmod_st* pc)
{
   int tmp_sel, tmp_lane, tmp_dxgxs, len_e, len_s, idx, rv;

   tmp_sel   = pc->lane_select ;
   tmp_lane  = pc->this_lane ;
   tmp_dxgxs = pc->dxgxs ;

   len_s = tmp_lane ; len_e = len_s+1 ;
   rv = SOC_E_NONE ;

   if(tscmod_is_warm_boot(pc->unit, pc)){
      return rv ;
   }

   if(pc->ctrl_type&TSCMOD_CTRL_TYPE_UC_STALL){
      uint32 addr; uint16 data ;
      if(!(pc->msg_code&TSCMOD_MSG_UC_STALL))
         printf("Warning: uC_sync_cmd access aborted u=%0d p=%0d ctl=%x addr=%x sel=%x l=%0d\n",
                pc->unit, pc->port, pc->per_lane_control, pc->accAddr, pc->lane_select, pc->this_lane) ;
      pc->msg_code  |= TSCMOD_MSG_UC_STALL ;
      pc->ctrl_type |= TSCMOD_CTRL_TYPE_MSG ;

      if(pc->per_lane_control & TSCMOD_UC_SYNC_RAM_MODE) {
         addr = pc->accAddr ;  data = 0 ;
         if(pc->per_lane_control & TSCMOD_UC_SYNC_WR_MODE) {
            pc->per_lane_control = TSCMOD_UC_OFFSET | (addr << TSCMOD_UC_MACRO_SHIFT) ;
            rv = tscmod_firmware_set(pc) ;

            pc->per_lane_control = TSCMOD_UC_W_RAM_DATA | (data << TSCMOD_UC_MACRO_SHIFT) ;
            rv = tscmod_firmware_set(pc) ;

            pc->per_lane_control = TSCMOD_UC_W_RAM_OP ;
            rv = tscmod_firmware_set(pc) ;
         } else {
            pc->per_lane_control = TSCMOD_UC_OFFSET | (addr << TSCMOD_UC_MACRO_SHIFT) ;
            rv = tscmod_firmware_set(pc) ;

            pc->per_lane_control = TSCMOD_UC_R_OP ;
            rv = tscmod_firmware_set(pc) ;

            pc->per_lane_control = TSCMOD_UC_R_DATA ;
            rv = tscmod_firmware_set(pc) ;
            data = pc->accData ;
         }
      } else {
         addr = pc->accAddr ;  data = 0 ;
         if(pc->per_lane_control & TSCMOD_UC_SYNC_WR_MODE) {
            data = pc->accData ; 
            SOC_IF_ERROR_RETURN
               (TSC_REG_WRITE(pc->unit, pc, 0x00, addr, data)) ;
         } else {
            SOC_IF_ERROR_RETURN
               (TSC_REG_READ(pc->unit, pc, 0x00, addr, &data)) ;
            pc->accData = data ; 
         }
      }
      return rv ;
   }
   if (pc->lane_select == TSCMOD_LANE_BCST) {
      len_e = 4 ; len_s = 0 ;
   } else if ((pc->dxgxs & 0x3) == 0x1 ) {
      len_e = 2 ;  len_s = 0 ;
   } else if((pc->dxgxs & 0x3) ==0x2) {
      len_e = 4 ; len_s = 2 ;
   }
   pc->dxgxs       = 0 ;
   if(pc->per_lane_control & TSCMOD_UC_SYNC_WR_MODE) {
      for(idx=len_s;idx<len_e; idx++) {
         pc->this_lane   = idx ;
         pc->lane_select = getLaneSelect(idx) ;
         rv |= tscmod_uc_sync_cmd_core(pc) ;
      }
   } else {
      pc->this_lane   = len_s ;
      pc->lane_select = getLaneSelect(len_s) ;
      rv |= tscmod_uc_sync_cmd_core(pc) ;
   }

   pc->lane_select = tmp_sel ;
   pc->this_lane   = tmp_lane ;
   pc->dxgxs       = tmp_dxgxs ;

   return rv ;
}

/*
#tscmod_st::per_lane_control >> 4 & 0xffff = mask for write.

#tscmod_st::accAddr is the address.
#tscmod_st::accData is the data.
*/
int tscmod_uc_sync_mask_wr_cmd(tscmod_st* pc) 
{
   int tmp_sel, tmp_lane, tmp_dxgxs, len_e, len_s, idx, rv;
   uint16 mask, addr, data, ram_ctl_addr, val;

   ram_ctl_addr =0x130 ;

   tmp_sel   = pc->lane_select ;
   tmp_lane  = pc->this_lane ;
   tmp_dxgxs = pc->dxgxs ;

   len_s = tmp_lane ; len_e = len_s+1 ;  val =0;
   rv = SOC_E_NONE ;

   if(tscmod_is_warm_boot(pc->unit, pc)){
      return rv ;
   }

   if(pc->ctrl_type&TSCMOD_CTRL_TYPE_UC_STALL){
      if(!(pc->msg_code&TSCMOD_MSG_UC_STALL))
         printf("Warning: uC_sync_mask_wr_cmd access aborted u=%0d p=%0d ctl=%x addr=%x sel=%x l=%0d\n",
                pc->unit, pc->port, pc->per_lane_control, pc->accAddr, pc->lane_select, pc->this_lane) ;
      pc->msg_code  |= TSCMOD_MSG_UC_STALL ;
      pc->ctrl_type |= TSCMOD_CTRL_TYPE_MSG ;

      addr = pc->accAddr ;
      data = pc->accData ;
      mask = (pc->per_lane_control>>TSCMOD_UC_SYNC_MASK_SHIFT) ;
      pc->per_lane_control = TSCMOD_UC_OFFSET | (ram_ctl_addr << TSCMOD_UC_MACRO_SHIFT) ;
      rv = tscmod_firmware_set(pc) ;

      pc->per_lane_control = TSCMOD_UC_W_RAM_DATA | (mask << TSCMOD_UC_MACRO_SHIFT) ;
      rv = tscmod_firmware_set(pc) ;

      pc->per_lane_control = TSCMOD_UC_W_RAM_OP ;
      rv = tscmod_firmware_set(pc) ;

      SOC_IF_ERROR_RETURN
         (TSC_REG_WRITE(pc->unit, pc, 0x00, addr, data)) ;
      return rv ;
   }
   if (pc->lane_select == TSCMOD_LANE_BCST) {
      len_e = 4 ; len_s = 0 ;
   } else if ((pc->dxgxs & 0x3) == 0x1 ) {
      len_e = 2 ;  len_s = 0 ;
   } else if((pc->dxgxs & 0x3) ==0x2) {
      len_e = 4 ; len_s = 2 ;
   }
   pc->dxgxs       = 0 ;
   data = pc->accData ; 
   addr = pc->accAddr ;
   mask = (pc->per_lane_control>>TSCMOD_UC_SYNC_MASK_SHIFT) ;
   val  = (mask <<8) | (mask>>8) ;  /* byte swap */
   for(idx=len_s;idx<len_e; idx++) {
      pc->this_lane   = idx ;
      pc->lane_select = getLaneSelect(idx) ;
      pc->accData = val ;
      pc->accAddr = ram_ctl_addr ;
      mask        = 0xffff ;
      pc->per_lane_control = (mask<<TSCMOD_UC_SYNC_MASK_SHIFT)| TSCMOD_UC_SYNC_RAM_MODE | TSCMOD_UC_SYNC_WR_MODE ;
      rv |= tscmod_uc_sync_cmd_core(pc) ;
#if 0
      pc->accAddr = ram_ctl_addr ;
      mask        = 0xffff ;
      pc->per_lane_control = (mask<<TSCMOD_UC_SYNC_MASK_SHIFT)| TSCMOD_UC_SYNC_RAM_MODE | TSCMOD_UC_SYNC_RD_MODE ;
      rv |= tscmod_uc_sync_cmd_core(pc) ;
#endif
      pc->accData = data ;
      pc->accAddr = addr ;
      mask        = 0xffff ;
      pc->per_lane_control = (mask<<TSCMOD_UC_SYNC_MASK_SHIFT) | TSCMOD_UC_SYNC_REG_MODE | TSCMOD_UC_SYNC_WR_MODE ;
      rv |= tscmod_uc_sync_cmd_core(pc) ;
   }

   pc->lane_select = tmp_sel ;
   pc->this_lane   = tmp_lane ;
   pc->dxgxs       = tmp_dxgxs ;

   return rv ;
}

int tscmod_mask_wr_cmd(tscmod_st* pc, uint32 addr, uint16 data, uint16 mask) 
{
   int tmp_control ; int tmp_addr, tmp_data, rv ;
   tmp_control = pc->per_lane_control ;
   tmp_addr    = pc->accAddr ;
   tmp_data    = pc->accData ;

   pc->accAddr          = addr ;
   pc->accData          = data ;
   pc->per_lane_control = (mask<<TSCMOD_UC_SYNC_MASK_SHIFT) ;
   rv = tscmod_uc_sync_mask_wr_cmd(pc) ; 

   pc->accAddr          = tmp_addr ; 
   pc->accData          = tmp_data ;
   pc->per_lane_control = tmp_control ;

   return rv ;
}   

/*!
\brief Controls BRCM64B66B encoder/decoder in 25.45G, 30G/40G_BRCM,
12.73G_dxgxs, 20G_dxgxs, 15.75G_dxgxs.

\param  pc handle to current TSC context (a.k.a tscmod struct)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.

\details
This function will take eight 16bit values for scw and scw_mask for each lane.

<B>Currently, this function is not implemented</B>

<B>How to call:</B><br>
<p>Call this function directly or through the function
#tscmod_tier1_selector() with the upper-case string "BRCM64B66B_CONTROL".</p>
*/
int tscmod_BRCM64B66B_control(tscmod_st* pc)
{
  /* fix this */
  return SOC_E_ERROR;
}

/*!
\brief enables/disables FEC function.

\param  pc handle to current TSC context (a.k.a tscmod struct)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.

\details
FEC is controlled by fields in #tscmod_st::per_lane_control.
The bit assigments are
     [3:0] ON/OFF
     [7:4] CNTR/CC/UC/INJECT (TSCMOD_FEC_CTL_CC/TSCMOD_FEC_CTL_UC/TSCMOD_FEC_CTL_INJECT)
    [15:8] lane_select_overwrite
   [31:16] sub modes.

#tscmod_st::lane_select and this_lane decide copies of registers set.
<B>How to call:</B><br>
<p>Call this function directly or through the function
#tscmod_tier1_selector() with the upper-case string "FEC_CONTROL".</p>
*/

int tscmod_FEC_control(tscmod_st* pc)
{
   int ctrl_mode, lane_mode, lane_over_wr, sub_mode, tmp_lane, tmp_sel, tmp_dxgxs; uint16 data, mask ;
   ctrl_mode = (pc->per_lane_control & TSCMOD_FEC_CTRL_MASK)>>TSCMOD_FEC_CTRL_SHIFT ;
   tmp_lane = pc->this_lane ;
   tmp_sel  = pc->lane_select ;
   tmp_dxgxs= pc->dxgxs ;
   lane_over_wr = (pc->per_lane_control&TSCMOD_FEC_LANE_OVRW_MASK)>>TSCMOD_FEC_LANE_OVRW_SHIFT ;
   lane_mode= (pc->per_lane_control&TSCMOD_FEC_LANE_MASK)>>TSCMOD_FEC_LANE_SHIFT ;
   sub_mode = (pc->per_lane_control&TSCMOD_FEC_SUB_MODE_MASK)>>TSCMOD_FEC_SUB_MODE_SHIFT;
   if(lane_over_wr) {
      pc->dxgxs = 0 ;
      switch(lane_mode){
         case 1: pc->this_lane = 0 ; break ;
         case 2: pc->this_lane = 1 ; break ;
         case 4: pc->this_lane = 2 ; break ;
         case 8: pc->this_lane = 3 ; break ;
         case 3:   pc->this_lane = 0 ; pc->dxgxs = 0x1 ; break ;
         case 0xc: pc->this_lane = 2 ; pc->dxgxs = 0x2 ; break ;
         default:
            pc->this_lane = 0 ; break ;
      }
      if(lane_mode==0xf) {
         pc->lane_select = TSCMOD_LANE_BCST ;
      } else {
         pc->lane_select = getLaneSelect(pc->this_lane) ;
      }
   }
   switch(ctrl_mode) {
   case TSCMOD_FEC_CTL_FS_RD:
      SOC_IF_ERROR_RETURN (READ_WC40_TX_X4_CONTROL0_MISCr(pc->unit, pc, &data)) ;
      if (data & TX_X4_CONTROL0_MISC_FEC_ENABLE_MASK) {
         pc->accData =1 ;
      } else {
         pc->accData =0 ;
      }
      break ;
   case TSCMOD_FEC_CTL_FS_SET_ON:
      /* 0xc113 */
      mask = TX_X4_CONTROL0_MISC_FEC_ENABLE_MASK ;
      SOC_IF_ERROR_RETURN (MODIFY_WC40_TX_X4_CONTROL0_MISCr(pc->unit, pc, mask, mask)) ;
      break ;
   case TSCMOD_FEC_CTL_FS_SET_OFF:
      /* 0xc113 */
      mask = TX_X4_CONTROL0_MISC_FEC_ENABLE_MASK ;
      SOC_IF_ERROR_RETURN (MODIFY_WC40_TX_X4_CONTROL0_MISCr(pc->unit, pc, 0, mask)) ;
      break ;
   case TSCMOD_FEC_CTL_AN_CL73_RD:
      SOC_IF_ERROR_RETURN
      (READ_WC40_AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BASE_ABILITIES_0r(pc->unit, pc, &data));
      if ((data & AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BASE_ABILITIES_0_FEC_MASK) ==
               AN_X4_ABILITIES_LOCAL_DEVICE_CL73_BASE_ABILITIES_0_FEC_MASK) {
         pc->accData =1 ;
      } else {
         pc->accData =0 ;
      }
      break ;
   case TSCMOD_FEC_CTL_AN_CL37_RD:
      SOC_IF_ERROR_RETURN (READ_WC40_AN_X4_ABILITIES_LOCAL_DEVICE_OVER1G_ABILITIES_1r(pc->unit, pc, &data));
      if (data & AN_X4_ABILITIES_LOCAL_DEVICE_OVER1G_ABILITIES_1_FEC_MASK) {
         pc->accData =1 ;
      } else {
         pc->accData =0 ;
      }
      break ;
   case TSCMOD_FEC_CTL_CNTR:
      if(TSCMOD_FEC_VAL_MASK & pc->per_lane_control) {
         data = 1 ;
      } else {
         data = 0 ;
      }
      /* 0xc131 */
      data = data << RX_X4_CONTROL0_FEC_0_FEC_ERR_ENABLE_SHIFT ;
      mask = RX_X4_CONTROL0_FEC_0_FEC_ERR_ENABLE_MASK ;
      SOC_IF_ERROR_RETURN (MODIFY_WC40_RX_X4_CONTROL0_FEC_0r(pc->unit, pc, data, mask)) ;
      break ;
   case TSCMOD_FEC_CTL_CC:
      switch(sub_mode) {
      case TSCMOD_FEC_SUB_MODE_ERR0:
         /* 0x9043: error 10 consecutive bits in the middle of the 64bit array */
         SOC_IF_ERROR_RETURN (WRITE_WC40_PKTGEN1_ERRORMASK2r(pc->unit, pc, 0x3ff)) ;
         break ;
      default:
         SOC_IF_ERROR_RETURN (WRITE_WC40_PKTGEN1_ERRORMASK2r(pc->unit, pc, 0x0)) ;
         data = 0 ;
         mask = PKTGEN0_PRTPCONTROL_ERRGEN_EN_MASK ;
         SOC_IF_ERROR_RETURN (MODIFY_WC40_PKTGEN0_PRTPCONTROLr(pc->unit, pc, data, mask)) ;
         break ;
      }
      break ;
   case TSCMOD_FEC_CTL_UC:
      switch(sub_mode) {
      case TSCMOD_FEC_SUB_MODE_ERR0:
         SOC_IF_ERROR_RETURN (WRITE_WC40_PKTGEN1_ERRORMASK2r(pc->unit, pc, 0xffff)) ;
         SOC_IF_ERROR_RETURN (WRITE_WC40_PKTGEN1_ERRORMASK3r(pc->unit, pc, 0xffff)) ;
         SOC_IF_ERROR_RETURN (WRITE_WC40_PKTGEN1_ERRORMASK4r(pc->unit, pc, 0xffff)) ;
         break ;
      default:
         SOC_IF_ERROR_RETURN (WRITE_WC40_PKTGEN1_ERRORMASK2r(pc->unit, pc, 0x0)) ;
         SOC_IF_ERROR_RETURN (WRITE_WC40_PKTGEN1_ERRORMASK3r(pc->unit, pc, 0x0)) ;
         SOC_IF_ERROR_RETURN (WRITE_WC40_PKTGEN1_ERRORMASK4r(pc->unit, pc, 0x0)) ;
         break ;
      }
      break ;
   case TSCMOD_FEC_CTL_INJECT:
      if(lane_over_wr) {
         switch(lane_mode){
         case 0xf: case 0x3: case 0xc:
            data = lane_mode ;
            break ;
         default:
            data = 1<<pc->this_lane ;
            break ;
         }
      } else {
         if (pc->lane_select == TSCMOD_LANE_BCST) {
            data = 0xf ;
         } else if((pc->dxgxs & 0x3)==2) {
            data = 0xc ;
         } else if(pc->dxgxs) {
            data = 0x3 ;
         } else {
            data = 1<<pc->this_lane ;
         }
      }
      mask = PKTGEN0_PRTPCONTROL_ERRGEN_EN_MASK ;
      if(TSCMOD_FEC_VAL_MASK & pc->per_lane_control) {
         data = data<< PKTGEN0_PRTPCONTROL_ERRGEN_EN_SHIFT ;
         /* 0x9032 */
         SOC_IF_ERROR_RETURN (MODIFY_WC40_PKTGEN0_PRTPCONTROLr(pc->unit, pc, data, mask)) ;
      } else {

         SOC_IF_ERROR_RETURN (MODIFY_WC40_PKTGEN0_PRTPCONTROLr(pc->unit, pc, 0, mask)) ;
      }
      break ;
   }
   pc->this_lane  = tmp_lane ;
   pc->lane_select= tmp_sel ;
   pc->dxgxs      = tmp_dxgxs ;
   return SOC_E_NONE ;
}

/*!
\brief sets Idriver, Ipredriver, and post2_coef values of transmitter.

\param  pc handle to current TSC context (a.k.a tscmod struct)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.

\details
Idriver (4 bits) and pdriver (4 bits) controls programmable TX amplitude.
idriver and pdriver are provided in fields in #tscmod_st::per_lane_control.
This function has to be called once per lane. (i.e. lane_select must be one hot)

\li first byte  Ipredriver
\li second byte Idriver
\li third byte  post2_coef
\li fourth byte control (bits 1, 2, and 4 choose idriver

This function modifies the following registers:
   ANATX_TX_DRIVERr    (0xc017)

<B>How to call:</B><br>
<p>Call this function directly or through the function
#tscmod_tier1_selector() with the upper-case string
"TX_AMP_CONTROL".</p>
*/
int tscmod_tx_amp_control(tscmod_st* pc)
{
   uint16      data ;
  int cntl = pc->per_lane_control;
  int  idrv, ipredrv, p2_coef, mask ;
  ipredrv = cntl & 0xff;
  idrv    = (cntl & 0xff00) >> 8;
  p2_coef = (cntl & 0xff0000) >> 16;

  mask = 0 ; data = 0 ;
  if (cntl & 0x1000000) {
     data |= (ipredrv << ANATX_TX_DRIVER_IPREDRIVER_SHIFT)
        & ANATX_TX_DRIVER_IPREDRIVER_MASK ;
     mask |= ANATX_TX_DRIVER_IPREDRIVER_MASK ;
  }
  if (cntl & 0x2000000) {
     data |=  (idrv << ANATX_TX_DRIVER_IDRIVER_SHIFT)
        & ANATX_TX_DRIVER_IDRIVER_MASK ;
     mask |=  ANATX_TX_DRIVER_IDRIVER_MASK ;
  }
  if (cntl & 0x4000000) {
     data |= (p2_coef << ANATX_TX_DRIVER_POST2_COEFF_SHIFT)
                       & ANATX_TX_DRIVER_POST2_COEFF_MASK ;
     mask |= ANATX_TX_DRIVER_POST2_COEFF_MASK ;
  }

  SOC_IF_ERROR_RETURN(MODIFY_WC40_ANATX_TX_DRIVERr(pc->unit, pc, data, mask));

  return SOC_E_NONE;
}

/*!
\brief sets p1 slicers for the receive path.

\param  pc handle to current TSC context (a.k.a tscmod struct)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.

\details

\li first byte  is p1  even control (6 bits)
\li second byte is p1  odd  control (6 bits)
\li third byte is ignored
\li fourth byte is set to 1 to enable even, 2 to enable odd
\li first and second byte (12 bits relevent) contain slicer value.

<B>How to call:</B><br>
<p>Call this function directly or through the function
#tscmod_tier1_selector() with the upper-case string
"RX_P1_SLICER_CONTROL".</p>
*/

int tscmod_rx_p1_slicer_control(tscmod_st* pc)
{
  /* fix this */
  return SOC_E_ERROR;
}

/*!
\brief sets m1 slicers for the receive path.

\param  pc handle to current TSC context (a.k.a tscmod struct)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.

\details

\li first byte  is m1  even control (6 bits)
\li second byte is m1  odd  control (6 bits)
\li third byte is ignored
\li fourth byte is set to 1 to enable even, 2 to enable odd

\li first and second byte (12 bits relevent) contain slicer value.

<B>How to call:</B><br>
<p>Call this function directly or through the function
#tscmod_tier1_selector() with the upper-case string
"RX_M1_SLICER_CONTROL".</p>
*/

int tscmod_rx_m1_slicer_control(tscmod_st* pc)
{
  /* fix this */
  return SOC_E_ERROR;
}

/*!
\brief sets d slicers for the receive path.

\param  pc handle to current TSC context (a.k.a tscmod struct)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.

\details

\li first byte  is d  even control (6 bits)
\li second byte is d  odd  control (6 bits)
\li third byte is ignored
\li fourth byte is set to 1 to enable even, 2 to enable odd

\li first and second byte (12 bits relevent) contain slicer value.

<B>How to call:</B><br>
<p>Call this function directly or through the function
#tscmod_tier1_selector() with the upper-case string
"RX_D_SLICER_CONTROL".</p>
*/

int tscmod_rx_d_slicer_control(tscmod_st* pc)
{
  /* fix this */
  return SOC_E_ERROR;
}

/*!
\brief Controls TX pre, main and post tap values.

\param  pc handle to current TSC context (a.k.a tscmod struct)

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
#tscmod_st::per_lane_control as follows.

If #tscmod_st::per_lane_control==0, remove the fored tap enable bit

\li 5:0   Pre tap value (7:6   Not used)
\li 24:24 Enable Pre Tap Override.                               (0x100_0000)
\li 13:8  Main tap value (15:14 Not used)
\li 25:25 Enable Main Tap Override.                              (0x200_0000)
\li 22:16 Post Tap value (23:22 Not used)
\li 26:26 Enable Post Tap Override.                              (0x400_0000)
\li 27:27 Set force enable and undo override capability.         (0x800_0000)
\li 30:28 1=KX, 2=OS, 3=BR, 4=KR, 5=2p5 to select the mode specific
\li       tap registers for programming. No forced tap.         (0xf000_0000)


To override pre tap with say 0x1a' set per_lane_control to 0x0100001a
To override pre tap and post tap with 0x1a' set per_lane_control to 0x03001a1a

The function modifies the following registers:
  CL72_TX_FIR_TAP_REGISTER             0xc252
  CL72_SHARED_AN_OS_DEFAULT_CONTROL    0x9111
  CL72_SHARED_AN_BR_DEFAULT_CONTROL    0x9112
  CL72_SHARED_AN_TWOP5_DEFAULT_CONTROL 0x9113
  CL72_MISC1_CONTROL                   0xc253 (KX)
  TX_X4_CONTROL0_KR_DEFAULT_CONTROL    0xc110 (KR)
  CL72_TX_FIR_TAP_REGISTER             0xc252 (forced)

  Most registers have the fields:
      TX_FIR_TAP_PRE_MASK   [3:0]
      TX_FIR_TAP_MAIN_MASK  [9:4]
      TX_FIR_TAP_POST_MASK[14:10]

<B>How to call:</B><br>
<p>Call this function directly or through the function
#tscmod_tier1_selector() with the upper-case string "TX_TAP_CONTROL".</p>
*/
int tscmod_tx_tap_control(tscmod_st* pc)
{
   int cntl; uint16 data, mask ;  int mode ; int pre_tap, main_tap, post_tap ;
   uint16 pre_tap_mask, main_tap_mask, post_tap_mask ;
   int    pre_tap_shift, main_tap_shift, post_tap_shift ;
   mask = 0 ;
   cntl = pc->per_lane_control;


   if(cntl==0) {
      mask = CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_FORCE_MASK ;
      data = 0 ;
      SOC_IF_ERROR_RETURN
         (MODIFY_WC40_CL72_TX_FIR_TAP_REGISTERr(pc->unit, pc, data, mask));
   } else {
      pre_tap  = (cntl & 0xff) ;
      main_tap = (cntl >> 8) & 0xff ;
      post_tap = (cntl>>16) & 0xff ;
      post_tap_mask =CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_POST_MASK;/* 5 bits */
      main_tap_mask =CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_MAIN_MASK;/* 6 bits */
      pre_tap_mask  =CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_PRE_MASK  ;/* 4 bits */
      post_tap_shift=CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_POST_SHIFT ;
      main_tap_shift=CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_MAIN_SHIFT ;
      pre_tap_shift =CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_PRE_SHIFT ;
      data = ((pre_tap <<pre_tap_shift) & pre_tap_mask ) |
             ((main_tap<<main_tap_shift)& main_tap_mask ) |
             ((post_tap<<post_tap_shift)& post_tap_mask ) ;
      mask = pre_tap_mask | main_tap_mask | post_tap_mask ;
      mode = (cntl >>28) & 0xf ;
      /*
      printf("pre_tap=%0d main_tap=%0d post_tap=%0d data=%x mode=%0d\n",
      pre_tap, main_tap, post_tap, data, mode) ; */
      if(cntl & 0xf0000000 ) {
         switch(mode) {
         case 1: /* KX */
            data =
            (main_tap<< CL72_MISC1_CONTROL_TX_FIR_TAP_MAIN_KX_INIT_VAL_SHIFT)&
                   CL72_MISC1_CONTROL_TX_FIR_TAP_MAIN_KX_INIT_VAL_MASK ;
            mask = CL72_MISC1_CONTROL_TX_FIR_TAP_MAIN_KX_INIT_VAL_MASK ;
            if (pc->ctrl_type & TSCMOD_CTRL_TYPE_UC_WR_MASK_EN) {
               SOC_IF_ERROR_RETURN
                  (tscmod_mask_wr_cmd(pc, TSC40_CL72_MISC1_CONTROLr, data, mask)) ;
            } else {
               SOC_IF_ERROR_RETURN
                  (MODIFY_WC40_CL72_MISC1_CONTROLr(pc->unit, pc, data, mask));
            }
            break ;
         case 2: /* OS */
            SOC_IF_ERROR_RETURN
               (MODIFY_WC40_CL72_SHARED_AN_OS_DEFAULT_CONTROLr
                (pc->unit, pc, data, mask));
            break ;
         case 3: /* BR */
            SOC_IF_ERROR_RETURN
               (MODIFY_WC40_CL72_SHARED_AN_BR_DEFAULT_CONTROLr
                (pc->unit, pc, data, mask));
            break ;
         case 4: /* KR */
            SOC_IF_ERROR_RETURN
               (MODIFY_WC40_TX_X4_CONTROL0_KR_DEFAULT_CONTROLr
                (pc->unit, pc, data, mask));
            break ;
         default: /* 2p5 */
            SOC_IF_ERROR_RETURN
               (MODIFY_WC40_CL72_SHARED_AN_TWOP5_DEFAULT_CONTROLr
                (pc->unit, pc, data, mask));
            break ;
         }
      } else {
         mask = 0 ; data = 0 ;
         if(cntl & 0x1000000) {
            data |= (pre_tap <<pre_tap_shift) & pre_tap_mask ;
            mask |= pre_tap_mask ;
         }
         if(cntl & 0x2000000) {
            data |= (main_tap<<main_tap_shift)& main_tap_mask;
            mask |= main_tap_mask ;
         }
         if(cntl & 0x4000000) {
            data |= (post_tap<<post_tap_shift)& post_tap_mask ;
            mask |= post_tap_mask ;
         }
         if(cntl & 0x8000000) {
            data |= CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_FORCE_MASK;
            mask |= CL72_TX_FIR_TAP_REGISTER_TX_FIR_TAP_FORCE_MASK;
         }
         SOC_IF_ERROR_RETURN
            (MODIFY_WC40_CL72_TX_FIR_TAP_REGISTERr(pc->unit, pc, data, mask));
      }
   }
   if(pc->verbosity & TSCMOD_DBG_TXDRV) {
      printf("tx_tap_control: u=%0d p=%0d l=%0d sel=%0x cntl=%0x addr=0x%0x data=%x mask=%0x\n",
             pc->unit, pc->port, pc->this_lane, pc->lane_select, pc->per_lane_control, pc->accAddr, data, mask);
   }
   return SOC_E_NONE;
}


/*!
\brief Controls transmit phase interpolator

\param  pc handle to current TSC context (a.k.a tscmod struct)

\returns nothing

\details
This function can be used to force the tansmit interpolator with any of the
four receive interpolator.  The core has only one Tx interpolator. Therefore
any selection of phase will be used for all four Tx lanes.

For per_lane_control:

\li [3:0] mode
\li [7:4] selected lane.
\li [31:15] value

This function modifies the following registers:

\li TX_X1_CONTROL0_TX_PI_CONTROL1r(0x9200[0]) for tx_pi_en
\li TX_X1_CONTROL0_TX_PI_CONTROL3r(0x9203)    for tx_pi_freq_ovr and tx_pi_freq_val

Reset value is 1.


<B>How to call:</B><br>
<p>Call this function directly or through the function
#tscmod_tier1_selector() with the upper-case string
"TX_PI_CONTROL".</p>
*/

int tscmod_tx_pi_control(tscmod_st* pc)
{
   int mode ; uint16 data, mask ;
   mode =  pc->per_lane_control&TSCMOD_TX_PI_MODE_MASK ;
   data = 0 ; mask = 0 ;
   if(mode==TSCMOD_TX_PI_PPM_SET) {
      mask = TX_X1_CONTROL0_TX_PI_CONTROL1_TX_PI_EN_MASK ;
      data = TX_X1_CONTROL0_TX_PI_CONTROL1_TX_PI_EN_MASK ;
      SOC_IF_ERROR_RETURN
         (MODIFY_WC40_TX_X1_CONTROL0_TX_PI_CONTROL1r(pc->unit, pc, data, mask));

      data = (pc->per_lane_control>>16) ;
      SOC_IF_ERROR_RETURN
         (WRITE_WC40_TX_X1_CONTROL0_TX_PI_CONTROL3r(pc->unit, pc, data)) ;

   } else if(mode==TSCMOD_TX_PI_PPM_RD) {
      SOC_IF_ERROR_RETURN
         (READ_WC40_TX_X1_CONTROL0_TX_PI_CONTROL3r(pc->unit, pc, &data)) ;
      pc->accData = data ;

   } else if(mode==TSCMOD_TX_PI_PPM_RESET) {
      mask = TX_X1_CONTROL0_TX_PI_CONTROL1_TX_PI_EN_MASK ;
      data = 0 ;
      SOC_IF_ERROR_RETURN
         (MODIFY_WC40_TX_X1_CONTROL0_TX_PI_CONTROL1r(pc->unit, pc, data, mask));

      SOC_IF_ERROR_RETURN
         (WRITE_WC40_TX_X1_CONTROL0_TX_PI_CONTROL3r(pc->unit, pc, data)) ;

   } else {
      /* fall back */
      mask = TX_X1_CONTROL0_TX_PI_CONTROL1_TX_PI_EN_MASK ;
      data = 0 ;
      SOC_IF_ERROR_RETURN
         (MODIFY_WC40_TX_X1_CONTROL0_TX_PI_CONTROL1r(pc->unit, pc, data, mask));

      SOC_IF_ERROR_RETURN
         (WRITE_WC40_TX_X1_CONTROL0_TX_PI_CONTROL3r(pc->unit, pc, data)) ;

   }
   return SOC_E_NONE;
}

/*!
\brief Controls receiver peaking filter value.

\param  pc handle to current TSC context (a.k.a #tscmod_st)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.

\details

This function is used to modify the peaking filter settings on a per lane
basis.  To change the peaking filter settings, set #tscmod_st::lane_select to
the desired lane selection setting and then modify #tscmod_st::per_lane_control,
which is a bit-wise ORed, value as desired.

The peaking filter amplitude can be adjusted with the lower four bits of
#tscmod_st::per_lane_control.  However, in order to effect a change in the
peaking filter amplitude, the desired value must be ORed with the constant
#CHANGE_RX_PF_VALUES.  As an example to set the peaking filter amplitude to
0xA, set #tscmod_st::per_lane_control to

#tscmod_st::per_lane_control = 0xA | #CHANGE_RX_PF_VALUES

By simultaneously bit-wise ORing other constants into
#tscmod_st::per_lane_control, other peaking filter programming settings may be
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
#tscmod_st::per_lane_control as follows

#tscmod_st::per_lane_control = 0x6 |
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
<p>Call this function directly or through the function #tscmod_tier1_selector()
with the upper-case string "RX_PF_CONTROL".</p>
*/
int tscmod_rx_PF_control(tscmod_st* pc)
{
  /* fix this */
  return SOC_E_ERROR;
}


/*!
\brief Override Rx DFE tap1 value.

\param  pc handle to current TSC context (a.k.a #tscmod_st)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.

\details

This function is to force receiver DFE tap1 of any lane to any desired value.

To change the 5-bit DFE tap1, set lower 5-bits of #tscmod_st::per_lane_control.

<B>How to call:</B><br>
<p>Call this function directly or through the function #tscmod_tier1_selector()
with the upper-case string "RX_DFE_TAP1_CONTROL".</p>
*/
int tscmod_rx_DFE_tap1_control(tscmod_st* pc)
{
  /* fix this */
  return SOC_E_ERROR;
}

/*!
\brief Control Rx DFE tap2 value.

\param  pc handle to current TSC context (a.k.a #tscmod_st)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.

This function is to force receiver DFE tap2 of any lane to any desired value.

To change the DFE tap2 value, set lower 5-bits of #tscmod_st::per_lane_control.

<B>How to call:</B><br>
<p>Call this function directly or through the function #tscmod_tier1_selector()
with the upper-case string "RX_DFE_TAP2_CONTROL".</p>
*/
int tscmod_rx_DFE_tap2_control(tscmod_st* pc)
{
  /* fix this */
  return SOC_E_ERROR;
}

/*!
\brief Override Rx DFE tap3 value.

\param  pc handle to current TSC context (a.k.a tscmod struct)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.

This function is to force receiver DFE tap3 of any lane to any desired value.

To change the DFE tap3 value, set lower 5-bits of #tscmod_st::per_lane_control.


<B>How to call:</B><br>
<p>Call this function directly or through the function #tscmod_tier1_selector()
with the upper-case string "RX_DFE_TAP3_CONTROL".</p>
*/
int tscmod_rx_DFE_tap3_control(tscmod_st* pc)
{
  /* fix this */
  return SOC_E_ERROR;
}

/*!
\brief Override Rx DFE tap4 value.

\param  pc handle to current TSC context (a.k.a #tscmod_st)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.

This function is to force receiver DFE tap4 of any lane to any desired value.

To change the DFE tap4 value, set lower 5-bits of #tscmod_st::per_lane_control.


\details
This function is to force receiver DFE tap4 to any desired value. Each lane can
be accessed through AER access.

<B>How to call:</B><br>
<p>Call this function directly or through the function #tscmod_tier1_selector()
with the upper-case string "RX_DFE_TAP4_CONTROL".</p>
*/
int tscmod_rx_DFE_tap4_control(tscmod_st* pc)
{
  /* fix this */
  return SOC_E_ERROR;
}

/*!
\brief Override Rx DFE tap5 value.

\param  pc handle to current TSC context (a.k.a #tscmod_st)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.

This function is to force receiver DFE tap5 of any lane to any desired value.

To change the DFE tap5 value, set lower 5-bits of #tscmod_st::per_lane_control.

\details
This function is to force receiver DFE tap5 to any desired value. Each lane can
be accessed through AER access.

<B>How to call:</B><br>
<p>Call this function directly or through the function #tscmod_tier1_selector()
with the upper-case string "RX_DFE_TAP5_CONTROL".</p>
*/
int tscmod_rx_DFE_tap5_control(tscmod_st* pc)
{
  /* fix this */
  return SOC_E_ERROR;
}


/*!
\brief Force Rx VGA value.

\param  pc handle to current TSC context (a.k.a tscmod struct)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.

\details

This function is to force receiver VGA of any lane to any desired value.

To change 5-bit VGA value, set the lower 5-bits of #tscmod_st::per_lane_control.

<B>How to call:</B><br>
<p>Call this function directly or through the function #tscmod_tier1_selector()
with the upper-case string "SET_RX_VGA_OVERRIDE".</p>
*/
int tscmod_rx_VGA_control(tscmod_st* pc)
{
  /* fix this */
  return SOC_E_ERROR;
}

/*!
\brief is used to power down transmitter or receiver per lane basis.

\param  pc handle to current TSC context (a.k.a #tscmod_st)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.

\details

This function is used to power on/off the TX and RX lanes in one swoop.  To
change the power down state for TX or RX, bit-wise OR one of the following

\li #CHANGE_TX0_PWRED_STATE    0x10
\li #CHANGE_TX1_PWRED_STATE    0x20
\li #CHANGE_TX2_PWRED_STATE    0x40
\li #CHANGE_TX3_PWRED_STATE    0x80

<br>

\li #CHANGE_RX0_PWRED_STATE  0x1000
\li #CHANGE_RX1_PWRED_STATE  0x2000
\li #CHANGE_RX2_PWRED_STATE  0x4000
\li #CHANGE_RX3_PWRED_STATE  0x8000

into #tscmod_st::per_lane_control.  These values serve as masks for whether the
power on/off for a particular lane should be changed.

To turn off the power for the selected TX/RX lane, bit-wise OR one of these

\li #PWR_DOWN_TX0   0x1
\li #PWR_DOWN_TX1   0x2
\li #PWR_DOWN_TX2   0x4
\li #PWR_DOWN_TX3   0x8

<br>

\li #PWR_DOWN_RX0  0x100
\li #PWR_DOWN_RX1  0x200
\li #PWR_DOWN_RX2  0x400
\li #PWR_DOWN_RX3  0x800

into #tscmod_st::per_lane_control.

As an example, to turn off the TX power on lane 0, turn on the TX power on lane1,
leave the RX power states along with the TX power on lanes 2 and 3 unaffected, set

#tscmod_st::per_lane_control = #CHANGE_TX0_PWRED_STATE |
                               #PWR_DOWN_TX0 |
                               #CHANGE_TX1_PWRED_STATE;

<B>How to call:</B><br>
<p>Call this function directly or through the function #tscmod_tier1_selector()
with the upper-case string "POWER_CONTROL".</p>
*/
int tscmod_power_control(tscmod_st* pc)
{
  /* fix this pkp */
  uint16 data;
  int rxcntl, txcntl, rxmask, txmask ;

  rxcntl = (pc->per_lane_control & 0xf00) >> 8;
  rxmask = (pc->per_lane_control & 0xf000) >> 12;
  txcntl =  pc->per_lane_control & 0xf ;
  txmask = (pc->per_lane_control & 0xf0) >>  4;

  rxmask = rxmask << MAIN0_POWER_RX_PWRDWN_ANALOG_SHIFT ;
  txmask = txmask << MAIN0_POWER_TX_PWRDWN_ANALOG_SHIFT ;

  /* 0x9002 reset the lanes as per control */
  data =   rxcntl << MAIN0_POWER_RX_PWRDWN_ANALOG_SHIFT;
  data |= (txcntl << MAIN0_POWER_TX_PWRDWN_ANALOG_SHIFT);
  SOC_IF_ERROR_RETURN (MODIFY_WC40_MAIN0_POWERr(pc->unit, pc, data, (txmask | rxmask)));
  return SOC_E_NONE;
}

/*!
\brief sets the lanes in Full Duplex/ half duplex mode.

\param  pc handle to current TSC context (a.k.a tscmod struct)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.

\details
This is bit encoded function. Bit 3 to 0 is for lane 3 to 0. High sets full
duplex. Low is for half duplex.

<B>Currently, this function is not implemented</B>

<B>How to call:</B><br>
<p>Call this function directly or through the function #tscmod_tier1_selector()
with the upper-case string "DUPLEX_CONTROL".</p>
*/
int tscmod_duplex_control(tscmod_st* pc)
{
  /* fix this */
  return SOC_E_ERROR;
}


/*!
\brief A function container to carry various misc tasks.

\param  pc handle to current TSC context (a.k.a tscmod struct)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.

\details
This tier function is a function container to invoke various miscellense
tasks through per_lane_control.

<B>Currently, if per_lane_control==
   TSCMOD_MISC_CTL_PORT_MODE_SELECT, call tscmod_update_port_mode_select;

<B>How to call:</B><br>
<p>Call this function directly or through the function #tscmod_tier1_selector()
with the upper-case string "MISC_CONTROL.</p>
*/
int tscmod_misc_control(tscmod_st* pc)
{
   uint16 data, mask ;
   data = 0 ; mask = 0 ;
   if(pc->per_lane_control==TSCMOD_MISC_CTL_PORT_MODE_SELECT){
      if(pc->verbosity & TSCMOD_DBG_SUB)
         printf("%-22s: u=%0d p=%0d call MISC_CTL_PORT_MODE_SELEC\n",
                FUNCTION_NAME(), pc->unit, pc->port) ;
      return tscmod_update_port_mode_select(pc) ;
   } else if(pc->per_lane_control==TSCMOD_MISC_CTL_CL72_STAT_GET){
      SOC_IF_ERROR_RETURN
         (READ_WC40_DEV1_IEEE9_PMD_10GBASE_KR_PMD_STATUS_REGISTER_151r
          (pc->unit, pc, &data)) ;
      pc->accData = data ;
   } else if(pc->per_lane_control==TSCMOD_MISC_CTL_DISABLE_DSC_SM){
      /* 0xc21e */
      data = 1 << DSC2B0_DSC_MISC_CTRL0_RXSEQSTART_SHIFT ;
      mask =      DSC2B0_DSC_MISC_CTRL0_RXSEQSTART_MASK ;
      if (pc->ctrl_type & TSCMOD_CTRL_TYPE_UC_WR_MASK_EN) {
         SOC_IF_ERROR_RETURN
            (tscmod_mask_wr_cmd(pc, TSC40_DSC2B0_DSC_MISC_CTRL0r, data, mask)) ;
      } else {
         SOC_IF_ERROR_RETURN
            (MODIFY_WC40_DSC2B0_DSC_MISC_CTRL0r(pc->unit, pc, data, mask));
      }
   } else if(pc->per_lane_control==TSCMOD_MISC_CTL_ENABLE_DSC_SM){
      data = 0 << DSC2B0_DSC_MISC_CTRL0_RXSEQSTART_SHIFT ;
      mask =      DSC2B0_DSC_MISC_CTRL0_RXSEQSTART_MASK ;
      if (pc->ctrl_type & TSCMOD_CTRL_TYPE_UC_WR_MASK_EN) {
         SOC_IF_ERROR_RETURN
            (tscmod_mask_wr_cmd(pc, TSC40_DSC2B0_DSC_MISC_CTRL0r, data, mask)) ;
      } else {
         SOC_IF_ERROR_RETURN
            (MODIFY_WC40_DSC2B0_DSC_MISC_CTRL0r(pc->unit, pc, data, mask));
      }
   } else if(pc->per_lane_control==TSCMOD_MISC_CTL_DISABLE_PLL){
      /* 0x9001 */
      data = 1 << MAIN0_RESET_RESET_PLL_SHIFT ;
      mask =      MAIN0_RESET_RESET_PLL_MASK ;
      SOC_IF_ERROR_RETURN
         (MODIFY_WC40_MAIN0_RESETr(pc->unit, pc, data, mask));
   } else if(pc->per_lane_control==TSCMOD_MISC_CTL_ENABLE_PLL){
      data = 0 << MAIN0_RESET_RESET_PLL_SHIFT ;
      mask =      MAIN0_RESET_RESET_PLL_MASK ;
      SOC_IF_ERROR_RETURN
         (MODIFY_WC40_MAIN0_RESETr(pc->unit, pc, data, mask));
   } else if(pc->per_lane_control==TSCMOD_MISC_CTL_ENABLE_PWR_TIMER) {
      /* 0x9002  */
      data = MAIN0_POWER_POWER_ON_TIMER_ENABLE_MASK ;
      mask = MAIN0_POWER_POWER_ON_TIMER_ENABLE_MASK ;
      SOC_IF_ERROR_RETURN (MODIFY_WC40_MAIN0_POWERr(pc->unit, pc, data, mask));
   } else if(pc->per_lane_control==TSCMOD_MISC_CTL_MAIN_TAP_ADJUST_EN_0) {
      /* 0xc254 */
      data = 0 ;
      mask = CL72_MISC2_CONTROL_MAIN_TAP_ADJUST_EN_MASK ;
      SOC_IF_ERROR_RETURN (MODIFY_WC40_CL72_MISC2_CONTROLr(pc->unit, pc, data, mask));
   } else if(pc->per_lane_control==TSCMOD_MISC_CTL_TUNING_SM_EN){
      data = DSC4B0_TUNING_SM_CTRL0_TUNING_SM_EN_MASK ;
      mask = DSC4B0_TUNING_SM_CTRL0_TUNING_SM_EN_MASK ;
      SOC_IF_ERROR_RETURN
         (MODIFY_WC40_DSC4B0_TUNING_SM_CTRL0r(pc->unit, pc, data, mask));
   } else if(pc->per_lane_control==TSCMOD_MISC_CTL_PORT_CLEAN_UP) {
      /* clear credtis 0xc100, 0xc104  */
      /* 0xc100 */
      SOC_IF_ERROR_RETURN
         (MODIFY_WC40_TX_X4_CREDIT0_CREDIT0r(pc->unit, pc, 0x0, 0xffff));
      /* 0xc104 */
      SOC_IF_ERROR_RETURN
         (MODIFY_WC40_TX_X4_CREDIT0_PCS_CLOCKCNT0r(pc->unit, pc, 0x0, 0xffff));

   } else if(pc->per_lane_control==TSCMOD_MISC_CTL_CLK_SEL_156){
      data = MAIN0_SETUP_REFCLK_SEL_clk_156p25MHz << MAIN0_SETUP_REFCLK_SEL_SHIFT ;
      mask = MAIN0_SETUP_REFCLK_SEL_MASK;
      /* 0x9000 */
      SOC_IF_ERROR_RETURN
         (MODIFY_WC40_MAIN0_SETUPr(pc -> unit, pc, data, mask));
   } else if(pc->per_lane_control==TSCMOD_MISC_CTL_DSC_HW_RESET) {
      SOC_IF_ERROR_RETURN
         (MODIFY_WC40_MAIN0_RESETr
          (pc->unit, pc, MAIN0_RESET_RX_DSC_RESET_MASK,
           MAIN0_RESET_RX_DSC_RESET_MASK)) ;
   } else if(pc->per_lane_control==TSCMOD_MISC_CTL_DSC_HW_ENABLE) {
      SOC_IF_ERROR_RETURN
         (MODIFY_WC40_MAIN0_RESETr
          (pc->unit, pc, 0,
           MAIN0_RESET_RX_DSC_RESET_MASK)) ;
   } else if(pc->per_lane_control==TSCMOD_MISC_CTL_LFCK_BYPASS_0) {
      /* 0x909a lfck_bypass*/
      data = 0 ;
      mask = XGXSBLK1_LANETEST_LFCK_BYPASS_MASK ;
      SOC_IF_ERROR_RETURN
         (MODIFY_WC40_XGXSBLK1_LANETESTr(pc->unit, pc, data, mask));
   } else if(pc->per_lane_control==TSCMOD_MISC_CTL_TEMP_DEFAULT){
      /* 0xf014:  40-C */
      data =
         (0x8 << UC_INFO_B1_TEMPERATURE_TEMP_IDX_SHIFT ) |
         (678 << UC_INFO_B1_TEMPERATURE_TEMPERATURE_SHIFT ) ;
      /* 0xf014:  25-C
      data = UC_INFO_B1_TEMPERATURE_FORCE_TEMPERATURE_MASK |
         (0xa << UC_INFO_B1_TEMPERATURE_TEMP_IDX_SHIFT ) |
         (717 << UC_INFO_B1_TEMPERATURE_TEMPERATURE_SHIFT ) ;
      */
      SOC_IF_ERROR_RETURN
         (WRITE_WC40_UC_INFO_B1_TEMPERATUREr(pc->unit, pc, data));
   } else if((pc->per_lane_control & TSCMOD_MISC_CTL_MASK)==TSCMOD_MISC_CTL_NONCE_MATCH){
      if(pc->per_lane_control>>TSCMOD_MISC_CTL_SHIFT) { /* for AN CL73 LOOPBACK */
         data = CL73_UCTRL1_CL73_NONCE_MATCH_OVER_MASK ;
         mask = CL73_UCTRL1_CL73_NONCE_MATCH_OVER_MASK|CL73_UCTRL1_CL73_NONCE_MATCH_VAL_MASK ;
         SOC_IF_ERROR_RETURN
            (MODIFY_WC40_CL73_UCTRL1r(pc->unit, pc,data, mask)) ;
      } else {
         data = 0 ;
         mask = CL73_UCTRL1_CL73_NONCE_MATCH_OVER_MASK|CL73_UCTRL1_CL73_NONCE_MATCH_VAL_MASK ;
         SOC_IF_ERROR_RETURN
            (MODIFY_WC40_CL73_UCTRL1r(pc->unit, pc,data, mask)) ;
      }
   } else if((pc->per_lane_control & TSCMOD_MISC_CTL_MASK)==TSCMOD_MISC_CTL_AN_RESTART){
      mask = AN_X4_ABILITIES_ENABLES_CL37_AN_RESTART_MASK |
             AN_X4_ABILITIES_ENABLES_CL73_AN_RESTART_MASK ;
      if((pc->an_type==TSCMOD_CL73)||(pc->an_type==TSCMOD_CL73_BAM)||(pc->an_type==TSCMOD_HPAM)){
         data = 1 << AN_X4_ABILITIES_ENABLES_CL73_AN_RESTART_SHIFT ;
      } else if((pc->an_type==TSCMOD_CL37)||(pc->an_type==TSCMOD_CL37_BAM)||(pc->an_type == TSCMOD_CL37_10G)||
                (pc->an_type==TSCMOD_CL37_SGMII)) {
         data = 1 << AN_X4_ABILITIES_ENABLES_CL37_AN_RESTART_SHIFT ;
      }
      if(data) {
         SOC_IF_ERROR_RETURN (MODIFY_WC40_AN_X4_ABILITIES_ENABLESr(pc->unit, pc, 0, mask));
         SOC_IF_ERROR_RETURN (MODIFY_WC40_AN_X4_ABILITIES_ENABLESr(pc->unit, pc, data, mask));
         SOC_IF_ERROR_RETURN (MODIFY_WC40_AN_X4_ABILITIES_ENABLESr(pc->unit, pc, 0, mask));
      }
   } else if(pc->per_lane_control==TSCMOD_MISC_CTL_PDET_COMPL){
      /* setup only one lane */
      SOC_IF_ERROR_RETURN
         (READ_WC40_AN_X4_SW_MANAGEMENT_AN_MISC_STATUSr(pc->unit, pc, &data));
      if(data&AN_X4_SW_MANAGEMENT_AN_MISC_STATUS_PD_COMPLETED_MASK) {
         pc->accData = 1 ;
      } else {
         pc->accData = 0 ;
      }
   } else if(pc->per_lane_control==TSCMOD_MISC_CTL_PDET_KX_COMPL){
      /* setup only one lane */
      SOC_IF_ERROR_RETURN
         (READ_WC40_AN_X4_SW_MANAGEMENT_AN_MISC_STATUSr(pc->unit, pc, &data));
      /* return only valid when pdetect completed */
      if(data&AN_X4_SW_MANAGEMENT_AN_MISC_STATUS_PD_HCD_KX4_OR_KX_MASK){
         pc->accData = 1 ;
      } else {
         pc->accData = 0 ;
      }
   } else {
      return SOC_E_PARAM;
   }
   return SOC_E_NONE ;
}


/*!
\brief get critical link statistics.

\param  pc handle to current TSC context (a.k.a tscmod struct)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.

\details
This function updates the TSC context struct and also displays the
following parameters.

\li 1. speed
\li 2. interface type
\li 3. half-duplex/duplex info.
\li 4. more info...

<B>this function moved to tscmod_diagnostics.</B>

<B>How to call:</B><br>
<p>Don't Call this function . Use tscmod_diag instead.</p>
*/
int tscmod_port_stat_display(tscmod_st* pc)
{
  /* fix this */
  return SOC_E_ERROR;
}

/*!
\brief Control scrambler parameters of each lane.

\param  pc handle to current TSC context (a.k.a #tscmod_st)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.

This function may be used to enable/disable the scrambler for both 8B/10B
and 64B/66B encoding.  To set the encoding in 8B/10B mode, ensure that the
#tscmod_st::port_type is set to either #TSCMOD_COMBO or #TSCMOD_INDEPENDENT and
then set #tscmod_st::per_lane_control to either

\li #SCRAMBLER_ON_8B_10B
\li #SCRAMBLER_OFF_8B_10B


TX 64B/66B scrambler settings is set bit-wise ORing the following values into
#tscmod_st::per_lane_control

\li #BYPASS_SCRAMBLER_64B_66B
\li #SCRAMBLE_ENTIRE_64B_66B
\li #SCRAMBLE_PAYLOAD_ONLY_64B_66B

and various RX 64B/66B scrambler settings is set by using the following fields

\li #BYPASS_DESCRAMBLER_64B_66B
\li #DESCRAMBLE_ENTIRE_64B_66B
\li #DESCRAMBLE_PAYLOAD_ONLY_64B_66B

<B>How to call:</B><br>
<p>Call this function directly or through the function #tscmod_tier1_selector()
with the upper-case string "SCRAMBLER_CONTROL".</p>
*/
int tscmod_scrambler_control (tscmod_st* pc)
{
  /* fix this */
  return SOC_E_ERROR;
}

/*!
\brief Set Remote loopback set in PCS bypass mode including SFP+ remote timing loop.

\param  pc handle to current TSC context (a.k.a tscmod struct)

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
<p>Call this function directly or through the function #tscmod_tier1_selector()
with the upper-case string "RX_LOOPBACK_PCS_BYPASS".</p>

*/

int tscmod_rx_loopback_pcs_bypass(tscmod_st* pc)
{
  /* fix this */
  return SOC_E_ERROR;
}

/*!
\brief set the test packet size and IPG for each lane.

\param  pc handle to current TSC context (a.k.a tscmod struct)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.

\details

<B>Currently, this function is not implemented</B>

<B>How to call:</B><br>
<p>Call this function directly or through the function #tscmod_tier1_selector()
with the upper-case string "PACKET_SIZE_IPG_SET ".</p>

*/
int tscmod_packet_size_IPG_set (tscmod_st* pc)
{
  /* fix this */
  return SOC_E_ERROR;
}

/*!
\brief check per lane receive packet count.

\param  pc handle to current TSC context (a.k.a tscmod struct)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.

<B>Currently, this function is not implemented</B>

<B>How to call:</B><br>
<p>Call this function directly or through the function
#tscmod_tier1_selector() with the upper-case string
"TX_RX_PACKETS_CHECK".</p>
*/
int tscmod_tx_rx_packets_check(tscmod_st* pc)
{
  /* fix this */
  return SOC_E_ERROR;
}

/*!
\brief Generic register read function.

\param  pc handle to current TSC context (#tscmod_st)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.

\details

This function reads the value of a TSC register, whose 32-bit address is
specified in the field #tscmod_st::accAddr.  Clause-22 or clause-45 accesses may
be specified by setting the field #tscmod_st::mdio_type to a value from the
enumerated type #tscmod_mdio_type.  The following fields must also be set
properly:  #tscmod_st::phy_ad, #tscmod_st::prt_ad, #tscmod_st::this_lane, and
#tscmod_st::unit.

The register contents are returned in the field #tscmod_st::accData and also in
the functions return value.  This function cannot identify an invalid register
address.  Consequently, the field #tscmod_st::accData will be contain an
undefined value following a read from an invalid address.  Note that the user
may determine if an address is legitimate by first writing some value to an
address and then attempting to read back from the same location.

The upper 16-bits for register accesses are specified by the contents of the
address-extension register (AER).  So the AER must be set properly prior to
calling this function.

<B>How to call:</B><br>
<p>Call this function directly or through the function #tscmod_tier1_selector()
with the upper-case string "REG_READ".</p>
*/
int tscmod_reg_read(tscmod_st* pc)
{
  /* fix this */
  return SOC_E_ERROR;
}

/*!
\brief generic register write function.

\param  pc handle to current TSC context (#tscmod_st)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.

This function reads the value of a TSC register, whose 32-bit address is
specified in the field #tscmod_st::accAddr.  Clause-22 or clause-45 accesses may
be specified by setting the field #tscmod_st::mdio_type to a value from the
enumerated type #tscmod_mdio_type.  The following fields must also be set
properly:  #tscmod_st::phy_ad, #tscmod_st::prt_ad, #tscmod_st::this_lane, and
#tscmod_st::unit.


The register contents are returned in the field #tscmod_st::accData and also in
the functions return value.  This function cannot identify an invalid register
address.  Consequently, the field #tscmod_st::accData will be contain an
undefined value following a read from an invalid address.  Note that the user
may determine if an address is legitimate by first writing some value to an
address and then attempting to read back from the same location.

The upper 16-bits for register accesses are specified by the contents of the
address-extension register (AER).  So the AER must be set properly prior to
calling this function.

Note that the use of this function when interleaved with other functions may
set the core to an illegal state and cause unintended behaviour.

<B>How to call:</B><br>
<p>Call this function directly or through the function #tscmod_tier1_selector()
with the upper-case string "REG_WRITE".</p>
*/
int tscmod_reg_write(tscmod_st* pc)
{
  /* fix this */
  return SOC_E_ERROR;
}

/*!
\brief Wait for an event to happen.

\param  pc handle to current TSC context (a.k.a tscmod struct)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.

\details
Currently it can only wait for a specific register bit to toggle. This bit is
specified in the 'yet to be defined' context field.

<B>Currently, this function is deprecated. Will not be implemented</B>

*/
int tscmod_wait_bit(tscmod_st* pc)
{
  /* fix this */
  return SOC_E_ERROR;
}

/*!
\brief Wait for specific time in micro-seconds.

\param  pc handle to current TSC context (a.k.a tscmod struct)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.

The time unit is independent on the platform.

<B>Currently, this function is deprecated. Will not be implemented</B>

<B>How to call:</B><br>
<p>Call this function directly or through the function #tscmod_tier1_selector()
with the upper-case string "WAIT_TIME".</p>

*/

int tscmod_wait_time(tscmod_st* pc)
{
  /* fix this */
  return SOC_E_ERROR;
}

/*!
\brief will set TSC in EEE pass thru mode.

\param  pc handle to current TSC context (a.k.a tscmod struct)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.

\details
#tscmod_st::per_lane_control: 1 to enable LPI bypass.  0 to disable LPI bypass.

<p>Call this function directly or through the function #tscmod_tier1_selector()
with the upper-case string "EEE_PASS_THRU_SET".</p>
*/
int tscmod_EEE_pass_thru_set(tscmod_st* pc)
{
    uint16 data, mask ;  
    if(pc->per_lane_control) { data = 1 ; } else { data = 0 ; }
    data = data << RX_X4_CONTROL0_PCS_CONTROL_0_LPI_ENABLE_SHIFT ;
    mask = RX_X4_CONTROL0_PCS_CONTROL_0_LPI_ENABLE_MASK ;
    SOC_IF_ERROR_RETURN
        (MODIFY_WC40_RX_X4_CONTROL0_PCS_CONTROL_0r(pc->unit, pc, data, mask));
    return SOC_E_NONE;
}

/*!
\brief EEE parameters set (full featured)

\param  pc handle to current TSC context (a.k.a tscmod struct)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.


\details
place holder at this time.

<B>Currently, this function is not implemented</B>

<B>How to call:</B><br>
<p>Call this function directly or through the function #tscmod_tier1_selector()
with the upper-case string "EEE_SET".</p>
*/
int tscmod_EEE_set(tscmod_st* pc)
{
  /* fix this */
  return SOC_E_ERROR;
}

/*!
\brief Carrier extension set

\param  pc handle to current TSC context (a.k.a tscmod struct)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.

\details
Only for Xenia.

<B>Currently, this function is not implemented</B>

<B>How to call:</B><br>
<p>Call this function directly or through the function #tscmod_tier1_selector()
with the upper-case string "CARRIER_EXTENSION_SET".</p>
*/
int tscmod_carrier_extension_set(tscmod_st *ws)
{
  /* fix this */
  return SOC_E_ERROR;
}

/*!
\brief Set reference clock

\param  pc handle to current TSC context (a.k.a tscmod struct)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.


\details
The reference clock is set to inform the micro. The value written into the
register is interpreted by the micro as thus.

    refclk = regVal * 2 + 100;

    Since the refclk field is 7 bits, the range is 100 - 228

<B>How to call:</B><br>
<p>Call this function directly or through the function #tscmod_tier1_selector()
with the upper-case string "REFCLK_SET".</p>
*/
int tscmod_refclk_set(tscmod_st* pc)
{
  /* fix this */
  return SOC_E_ERROR;
}

/*!
\brief Set asymmetric mode.

\param  pc handle to current TSC context (a.k.a tscmod struct)

\returns #SOC_E_NONE if no errors. #SOC_E_ERROR else.


\details

Set asymmetric mode for Xenia. This will allow 1G and 2.5G simultanesously

<B>How to call:</B><br>
<p>Call this function directly or through the function #tscmod_tier1_selector()
with the upper-case string "ASYMMETRIC_MODE_SET".</p>
*/

int tscmod_asymmetric_mode_set(tscmod_st* pc)
{
  /* fix this */
  return SOC_E_ERROR;
}

int _tscmod_asymmetric_mode(tscmod_st* pc)
{
  /* fix this */
  return SOC_E_ERROR;
}

#endif
